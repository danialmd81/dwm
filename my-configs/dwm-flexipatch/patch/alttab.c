int alttabn; /* move that many clients forward */
int ntabs;   /* number of active clients in tag */
int isalt;
Client **altsnext; /* array of all clients in the tag */
Window alttabwin;

void alttab() {
  Monitor *m = selmon;

  /* Cycle selection index forward without changing actual window focus */
  if (ntabs > 0) {
    alttabn++;
    if (alttabn >= ntabs)
      alttabn = 0; /* reset index */
  }

  /* redraw tab */
  XRaiseWindow(dpy, alttabwin);
  drawalttab(ntabs, 0, m);
}

void alttabend() {
  Monitor *m = selmon;
  Client *buff;
  int i;

  if (!isalt)
    return;

  /* Re-order client stack so chosen window moves to front position */
  if (ntabs > 1) {
    if (alttabn != 0) { /* if user picked original client do nothing */
      buff = altsnext[alttabn];
      if (alttabn > 1) {
        for (i = alttabn; i > 0; i--)
          altsnext[i] = altsnext[i - 1];
      } else {
        altsnext[alttabn] = altsnext[0];
      }
      altsnext[0] = buff;
    }

    /* Unhide chosen window if currently hidden */
    if (HIDDEN(altsnext[0]))
      show(altsnext[0]);

    /* Restack clients and focus selected window */
    for (i = ntabs - 1; i >= 0; i--) {
      focus(altsnext[i]);
      restack(m);
    }

    free(altsnext); /* free list of clients */
  } else if (ntabs == 1) {
    if (HIDDEN(altsnext[0]))
      show(altsnext[0]);
    focus(altsnext[0]);
    restack(m);
    free(altsnext);
  }

  /* Destroy popup switcher window */
  isalt = 0;
  ntabs = 0;
  XUnmapWindow(dpy, alttabwin);
  XDestroyWindow(dpy, alttabwin);
}

void drawalttab(int nwins, int first, Monitor *m) {
  Client *c;
  int i, h;
  int y = 0;
  int px = m->mx;
  int py = m->my;

  if (first) {
    XSetWindowAttributes wa = {.override_redirect = True,
                               .background_pixmap = ParentRelative,
                               .event_mask = ButtonPressMask | ExposureMask};

    /* decide position of tabwin */
    if (tabposx == 1)
      px = m->mx + (m->mw / 2) - (maxwtab / 2);
    else if (tabposx == 2)
      px = m->mx + m->mw - maxwtab;

    if (tabposy == 1)
      py = m->my + (m->mh / 2) - (maxhtab / 2);
    else if (tabposy == 2)
      py = m->my + m->mh - maxhtab;

    h = maxhtab;

    alttabwin = XCreateWindow(
        dpy, root, px, py, maxwtab, maxhtab, 2, DefaultDepth(dpy, screen),
        CopyFromParent, DefaultVisual(dpy, screen),
        CWOverrideRedirect | CWBackPixmap | CWEventMask, &wa);

    XDefineCursor(dpy, alttabwin, cursor[CurNormal]->cursor);
    XMapRaised(dpy, alttabwin);
  }

  h = maxhtab / ntabs;
  for (i = 0; i < ntabs; i++) { /* draw all clients into tabwin */
    c = altsnext[i];
    if (!ISVISIBLE(c))
      continue;

    /* Highlight index based on alttabn rather than active window focus */
    int is_selected = (i == alttabn);

    /* Apply distinct scheme for hidden vs visible windows */
    if (HIDDEN(c)) {
      drw_setscheme(drw, scheme[is_selected ? SchemeHidSel : SchemeHidNorm]);
    } else {
      drw_setscheme(drw, scheme[is_selected ? SchemeSel : SchemeNorm]);
    }

    drw_text(drw, 0, y, maxwtab, h, 0, c->name, 0, 0);
    y += h;
  }

  drw_setscheme(drw, scheme[SchemeNorm]);
  drw_map(drw, alttabwin, 0, 0, maxwtab, maxhtab);
}

void alttabstart(const Arg *arg) {
  Client *c;
  Monitor *m = selmon;
  int grabbed;
  int i;

  altsnext = NULL;
  if (alttabwin)
    alttabend();

  if (isalt == 1) {
    alttabend();
    return;
  }

  isalt = 1;
  alttabn = 0;
  ntabs = 0;

  /* Count all visible and hidden windows on current tag */
  for (c = m->clients; c; c = c->next) {
    if (!ISVISIBLE(c))
      continue;

    ++ntabs;
  }

  if (!ntabs) {
    alttabend();
    return;
  }

  altsnext = (Client **)malloc(ntabs * sizeof(Client *));

  /* Build client stack (includes hidden windows) */
  for (i = 0, c = m->stack; c; c = c->snext) {
    if (!ISVISIBLE(c))
      continue;

    altsnext[i] = c;
    i++;
  }

  /* Draw initial overlay */
  drawalttab(ntabs, 1, m);

  /* Move selection highlight forward to next client (+1) */
  if (ntabs > 1) {
    alttabn = 1;
    drawalttab(ntabs, 0, m);
  }

  struct timespec ts = {.tv_sec = 0, .tv_nsec = 1000000};

  /* Grab keyboard input */
  grabbed = 1;
  for (i = 0; i < 1000; i++) {
    if (XGrabKeyboard(dpy, root, True, GrabModeAsync, GrabModeAsync,
                      CurrentTime) == GrabSuccess)
      break;
    nanosleep(&ts, NULL);
    if (i == 1000 - 1)
      grabbed = 0;
  }

  if (grabbed == 0) {
    alttabend();
    return;
  }

  XEvent event;
  while (grabbed) {
    XNextEvent(dpy, &event);
    if (event.type == KeyPress || event.type == KeyRelease) {
      /* Release modifier key -> confirm selection & switch focus */
      if (event.type == KeyRelease && event.xkey.keycode == tabmodkey)
        break;

      /* Press Tab key -> cycle selection visually inside menu */
      if (event.type == KeyPress && event.xkey.keycode == tabcyclekey) {
        alttab();
      }
    }
  }

  /* Perform focus switch, unhide if necessary, and tear down overlay window */
  alttabend();
  XUngrabKeyboard(dpy, CurrentTime);
}