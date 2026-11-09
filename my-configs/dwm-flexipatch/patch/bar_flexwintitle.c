/* Flexwintitle properties, you can override these in your config.h if you want. */
#ifndef FLEXWINTITLE_BORDERS
#define FLEXWINTITLE_BORDERS 0 // 0 = off, 1 = on
#endif
#ifndef FLEXWINTITLE_SHOWFLOATING
#define FLEXWINTITLE_SHOWFLOATING 0 // whether to show titles for floating windows, hidden clients are always shown
#endif
#ifndef FLEXWINTITLE_MASTERWEIGHT
#define FLEXWINTITLE_MASTERWEIGHT 9 // master weight compared to stack, hidden and floating window titles
#endif
#ifndef FLEXWINTITLE_STACKWEIGHT
#define FLEXWINTITLE_STACKWEIGHT 3 // stack weight compared to master, hidden and floating window titles
#endif
#ifndef FLEXWINTITLE_HIDDENWEIGHT
#define FLEXWINTITLE_HIDDENWEIGHT 1 // hidden window title weight
#endif
#ifndef FLEXWINTITLE_FLOATWEIGHT
#define FLEXWINTITLE_FLOATWEIGHT 1 // floating window title weight, set to 0 to not show floating windows
#endif

#define SCHEMEFOR(c) getschemefor(m, c, groupactive == c)

enum
{
  GRP_NOSELECTION,
  GRP_MASTER,
  GRP_STACK1,
  GRP_STACK2,
  GRP_FLOAT,
  GRP_HIDDEN
};

int width_flexwintitle(Bar* bar, BarArg* a)
{
  return a->w;
}

int draw_flexwintitle(Bar* bar, BarArg* a)
{
  drw_rect(drw, a->x, a->y, a->w, a->h, 1, 1);
  return flextitlecalculate(bar->mon, a->x, a->w, -1, flextitledraw, NULL, a);
}

int click_flexwintitle(Bar* bar, Arg* arg, BarArg* a)
{
  flextitlecalculate(bar->mon, 0, a->w, a->x, flextitleclick, arg, a);
  return ClkWinTitle;
}

Client* flextitledrawarea(Monitor* m, Client* c, int x, int r, int w, int max_clients, int scheme, int draw_tiled, int draw_hidden, int draw_floating, int passx, void (*tabfn)(Monitor*, Client*, int, int, int, int, Arg* arg, BarArg* barg), Arg* arg, BarArg* barg)
{
  int i;
  for (i = 0; c && i < max_clients; c = c->snext)
  {
    if (ISVISIBLE(c) && ((draw_tiled && !c->isfloating && !HIDDEN(c)) || (draw_floating && c->isfloating && !HIDDEN(c)) || (draw_hidden && HIDDEN(c))))
    {
      tabfn(m, c, passx, x, w + (i < r ? 1 : 0), scheme, arg, barg);
      x += w + (i < r ? 1 : 0);
      i++;
    }
  }
  return c;
}

int getschemefor(Monitor* m, int group, int activegroup)
{
  if (group == GRP_HIDDEN)
    return SchemeHidNorm;

  return SchemeTitleNorm;
}

int getselschemefor(int scheme)
{
  return SchemeTitleSel;
}

void flextitledraw(Monitor* m, Client* c, int unused, int x, int w, int tabscheme, Arg* arg, BarArg* a)
{
  if (!c)
    return;
  int i, nclienttags = 0, nviewtags = 0;
  int tpad = lrpad / 2;
  int tx = x;
  int tw = w;

  int clientscheme = (c == selmon->sel && HIDDEN(c) ? SchemeHidSel : HIDDEN(c) ? SchemeHidNorm : c == selmon->sel ? SchemeTitleSel : c->isurgent ? SchemeUrg : SchemeTitleNorm);

  drw_setscheme(drw, scheme[clientscheme]);
  XSetWindowBorder(dpy, c->win, scheme[clientscheme][ColBorder].pixel);

  if (w <= TEXTW("A") - lrpad + tpad) // reduce text padding if wintitle is too small
    tpad = (w - TEXTW("A") + lrpad < 0 ? 0 : (w - TEXTW("A") + lrpad) / 2);

  XSetForeground(drw->dpy, drw->gc, drw->scheme[ColBg].pixel);
  XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, a->y, w, a->h);

  tx += tpad;
  tw -= lrpad;

  drw_text(drw, tx, a->y, tw, a->h, 0, c->name, 0, False);
  drawstateindicator(m, c, 1, x + 2, a->y, w, a->h, 0, 0, 0);

  /* Draw vertical separator on inactive windows only */
  if (c != selmon->sel)
  {
    XSetForeground(drw->dpy, drw->gc, scheme[SchemeTitleNorm][ColBorder].pixel);
    XFillRectangle(drw->dpy, drw->drawable, drw->gc, x + w - 1, a->y + 3, 2, a->h - 6);
  }

  /* Optional tags icons */
  for (i = 0; i < NUMTAGS; i++)
  {
    if ((m->tagset[m->seltags] >> i) & 1)
      nviewtags++;
    if ((c->tags >> i) & 1)
      nclienttags++;
  }

  if (TAGSINDICATOR == 2 || nclienttags > 1 || nviewtags > 1)
    drawindicator(m, c, 1, x, a->y, w, a->h, 0, 0, 0, INDICATOR_RIGHT_TAGS);
}

#ifndef HIDDEN
#define HIDDEN(C) 0
#endif

void flextitleclick(Monitor* m, Client* c, int passx, int x, int w, int unused, Arg* arg, BarArg* barg)
{
  if (passx >= x && passx <= x + w)
    arg->v = c;
}

int flextitlecalculate(Monitor* m, int offx, int tabw, int passx, void (*tabfn)(Monitor*, Client*, int, int, int, int, Arg* arg, BarArg* barg), Arg* arg, BarArg* barg)
{
  Client* c;
  int n, center = 0, mirror = 0, fixed = 0; // layout configuration
  int clientsnmaster = 0, clientsnstack = 0, clientsnfloating = 0, clientsnhidden = 0;
  int i, w, r, num = 0, den, fulllayout = 0;
  int clientsnstack2 = 0;
  int groupactive = 0;
  int selidx = 0;
  int dualstack = 0;
  int rw, rr;

  int mas_x = offx, st1_x = offx, st2_x = offx, hid_x = offx, flt_x = offx;
  int mas_w, st1_w, st2_w, hid_w;

  for (i = 0, c = m->stack; c; c = c->snext)
  {
    if (!ISVISIBLE(c))
      continue;
    if (HIDDEN(c))
    {
      if (FLEXWINTITLE_HIDDENWEIGHT)
        clientsnhidden++;
      continue;
    }

    if (c->isfloating)
    {
      if (FLEXWINTITLE_FLOATWEIGHT)
        clientsnfloating++;
      continue;
    }

    if (m->sel == c)
      selidx = i;

    if (i < m->nmaster)
      clientsnmaster++;
    else if ((i - m->nmaster) % 2)
      clientsnstack2++;
    else
      clientsnstack++;
    i++;
  }

  if (!m->sel)
    groupactive = GRP_NOSELECTION;
  else if (HIDDEN(m->sel))
    groupactive = GRP_HIDDEN;
  else if (m->sel->isfloating)
    groupactive = GRP_FLOAT;
  else if (selidx < clientsnmaster)
    groupactive = GRP_MASTER;
  else if (selidx < clientsnmaster + clientsnstack)
    groupactive = GRP_STACK1;
  else if (selidx < clientsnmaster + clientsnstack + clientsnstack2)
    groupactive = GRP_STACK2;

  n = clientsnmaster + clientsnstack + clientsnstack2 + clientsnfloating + clientsnhidden;
  if (n == 0)
    return 0;

  /* Certain layouts have no master / stack areas */
  if (!m->lt[m->sellt]->arrange                            // floating layout
      || (!n || (!fixed && m->nmaster && n <= m->nmaster)) // no master
      || m->lt[m->sellt]->arrange == &monocle)
    fulllayout = 1;

  num = tabw;
  c = m->stack;

  /* floating mode */
  if ((fulllayout && FLEXWINTITLE_FLOATWEIGHT > 0) || clientsnmaster + clientsnstack == 0 || !m->lt[m->sellt]->arrange)
  {
    den = clientsnmaster + clientsnstack + clientsnstack2 + clientsnfloating + clientsnhidden;
    w = num / den;
    r = num % den;
    c = flextitledrawarea(m, c, mas_x, r, w, den, SchemeTitleNorm, 1, FLEXWINTITLE_HIDDENWEIGHT, FLEXWINTITLE_FLOATWEIGHT, passx, tabfn, arg, barg);
  }
  else if (fulllayout)
  {
    den = clientsnmaster + clientsnstack + clientsnstack2 + clientsnhidden;
    w = num / den;
    r = num % den;
    c = flextitledrawarea(m, c, mas_x, r, w, den, SchemeTitleNorm, 1, FLEXWINTITLE_HIDDENWEIGHT, 0, passx, tabfn, arg, barg);
  }
  else
  {
    den = clientsnmaster * FLEXWINTITLE_MASTERWEIGHT + (clientsnstack + clientsnstack2) * FLEXWINTITLE_STACKWEIGHT + clientsnfloating * FLEXWINTITLE_FLOATWEIGHT + clientsnhidden * FLEXWINTITLE_HIDDENWEIGHT;
    w = num / den; // weight width per client
    r = num % den; // weight rest width
    rw = r / n;    // rest incr per client
    rr = r % n;    // rest rest
    if ((!center && !dualstack) || (center && n <= m->nmaster + 1))
    {
      clientsnstack += clientsnstack2;
      clientsnstack2 = 0;
      if (groupactive == GRP_STACK2)
        groupactive = GRP_STACK1;
    }

    mas_w = clientsnmaster * rw + w * clientsnmaster * FLEXWINTITLE_MASTERWEIGHT + (rr > 0 ? MIN(rr, clientsnmaster) : 0);
    rr -= clientsnmaster;
    st1_w = clientsnstack * (rw + w * FLEXWINTITLE_STACKWEIGHT) + (rr > 0 ? MIN(rr, clientsnstack) : 0);
    rr -= clientsnstack;
    st2_w = clientsnstack2 * (rw + w * FLEXWINTITLE_STACKWEIGHT) + (rr > 0 ? MIN(rr, clientsnstack2) : 0);
    rr -= clientsnstack2;
    hid_w = clientsnhidden * (rw + w * FLEXWINTITLE_HIDDENWEIGHT) + (rr > 0 ? MIN(rr, clientsnhidden) : 0);
    rr -= clientsnhidden;
    rr = r % n;

    if (mirror)
    {
      if (center && clientsnstack2)
      {
        mas_x = st1_x + st1_w;
        st2_x = mas_x + mas_w;
        hid_x = st2_x + st2_w;
      }
      else
      {
        if (clientsnstack2)
        {
          st2_x = st1_x + st1_w;
          mas_x = st2_x + st2_w;
        }
        else
          mas_x = st1_x + st1_w;
        hid_x = mas_x + mas_w;
      }
    }
    else
    {
      if (center && clientsnstack2)
      {
        mas_x = st2_x + st2_w;
        st1_x = mas_x + mas_w;
        hid_x = st1_x + st1_w;
      }
      else
      {
        st1_x = mas_x + mas_w;
        if (clientsnstack2)
        {
          st2_x = st1_x + st1_w;
          hid_x = st2_x + st2_w;
        }
        else
          hid_x = st1_x + st1_w;
      }
    }

    flt_x = hid_x + hid_w;
    c = flextitledrawarea(m, c, mas_x, rr, w * FLEXWINTITLE_MASTERWEIGHT + rw, clientsnmaster, SchemeTitleNorm, 1, 0, 0, passx, tabfn, arg, barg);
    rr -= clientsnmaster;
    c = flextitledrawarea(m, c, st1_x, rr, w * FLEXWINTITLE_STACKWEIGHT + rw, clientsnstack, SchemeTitleNorm, 1, 0, 0, passx, tabfn, arg, barg);
    rr -= clientsnstack;
    if (clientsnstack2)
    {
      c = flextitledrawarea(m, c, st2_x, rr, w * FLEXWINTITLE_STACKWEIGHT + rw, clientsnstack2, SchemeTitleNorm, 1, 0, 0, passx, tabfn, arg, barg);
      rr -= clientsnstack2;
    }
    c = flextitledrawarea(m, m->stack, hid_x, rr, w * FLEXWINTITLE_HIDDENWEIGHT + rw, clientsnhidden, SCHEMEFOR(GRP_HIDDEN), 0, 1, 0, passx, tabfn, arg, barg);
    rr -= clientsnhidden;
    c = flextitledrawarea(m, m->stack, flt_x, rr, w * FLEXWINTITLE_FLOATWEIGHT + rw, clientsnfloating, SchemeTitleNorm, 0, 0, 1, passx, tabfn, arg, barg);
  }
  return 1;
}
