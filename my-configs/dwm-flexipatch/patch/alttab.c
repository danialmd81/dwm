int alttabn;          /* move that many clients forward */
int ntabs;            /* number of active clients in tag */
int isalt;
Client **altsnext;    /* array of all clients in the tag */
Window alttabwin;

void
alttab()
{
    Monitor *m = selmon;

    if (!alttabwin || !altsnext || ntabs <= 0)
        return;

    /* Move selection index forward */
    alttabn++;
    if (alttabn >= ntabs)
        alttabn = 0;

    /* Update selection reference for drawing without modifying m->stack */
    if (altsnext[alttabn])
        m->sel = altsnext[alttabn];

    /* Redraw tab overlay */
    XRaiseWindow(dpy, alttabwin);
    drawalttab(ntabs, 0, m);
}

void
alttabend()
{
    Monitor *m = selmon;
    Client *c = NULL;

    if (!isalt)
        return;

    if (altsnext && ntabs > 0)
        c = altsnext[alttabn];

    /* Free client array */
    if (altsnext) {
        free(altsnext);
        altsnext = NULL;
    }

    /* Destroy popup overlay */
    isalt = 0;
    ntabs = 0;
    if (alttabwin) {
        XUnmapWindow(dpy, alttabwin);
        XDestroyWindow(dpy, alttabwin);
        alttabwin = 0;
    }

    /* Reorder dwm's MRU stack list once and focus */
    if (c) {
        detachstack(c);
        attachstack(c);
        focus(c);
        restack(m);
    }
}

void
drawalttab(int nwins, int first, Monitor *m)
{
    Client *c;
    int i, h;
    int y = 0;
    int totalheight;
    int px = m->mx;
    int py = m->my;

    if (first) {
        XSetWindowAttributes wa = {
            .override_redirect = True,
            .background_pixmap = ParentRelative,
            .event_mask = ButtonPressMask|ExposureMask
        };

        /* calculate total height based on number of items */
        totalheight = ntabs * htabitem;

        /* decide position of tabwin */
        if (tabposx == 1)
            px = m->mx + (m->mw / 2) - (maxwtab / 2);
        else if (tabposx == 2)
            px = m->mx + m->mw - maxwtab;

        if (tabposy == 1)
            py = m->my + (m->mh / 2) - (totalheight / 2);
        else if (tabposy == 2)
            py = m->my + m->mh - totalheight;

        alttabwin = XCreateWindow(dpy, root, px, py, maxwtab, totalheight, 2, DefaultDepth(dpy, screen),
                                     CopyFromParent, DefaultVisual(dpy, screen),
                                     CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);

        XDefineCursor(dpy, alttabwin, cursor[CurNormal]->cursor);
        XMapRaised(dpy, alttabwin);
    }

    if (!alttabwin)
        return;

    /* Clear the window before drawing */
    XClearWindow(dpy, alttabwin);

    if (ntabs > 0) {
        h = htabitem;  /* use htabitem for each item height */

        for (i = 0; i < ntabs; i++) {
            c = altsnext[i];
            if (!c || !ISVISIBLE(c))
                continue;

            /* highlight the currently selected client */
            drw_setscheme(drw, scheme[c == m->sel ? SchemeSel : SchemeNorm]);
            drw_text(drw, 0, y, maxwtab, h, 0, c->name, 0, 0);
            y += h;
        }
    }

    drw_setscheme(drw, scheme[SchemeNorm]);
    drw_map(drw, alttabwin, 0, 0, maxwtab, ntabs * htabitem);
}

void
alttabstart(const Arg *arg)
{
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

    /* Count visible clients */
    ntabs = 0;
    for (c = m->clients; c; c = c->next) {
        if (!ISVISIBLE(c))
            continue;
        ++ntabs;
    }

    if (ntabs <= 1)
        return;

    /* Allocate client array */
    altsnext = (Client **) malloc(ntabs * sizeof(Client *));
    if (!altsnext)
        return;

    /* Snapshot current MRU stack order into array */
    for (i = 0, c = m->stack; c; c = c->snext) {
        if (!ISVISIBLE(c))
            continue;
        altsnext[i++] = c;
    }

    isalt = 1;
    /* Automatically select the second MRU window (index 1) */
    alttabn = 1;
    m->sel = altsnext[alttabn];

    /* Create and draw the alttab window */
    drawalttab(ntabs, 1, m);

    /* Grab keyboard */
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 };
    grabbed = 1;
    for (i = 0; i < 1000; i++) {
        if (XGrabKeyboard(dpy, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
            break;
        nanosleep(&ts, NULL);
        if (i == 1000 - 1)
            grabbed = 0;
    }

    if (grabbed == 0) {
        alttabend();
        return;
    }

    /* Event loop */
    XEvent event;
    while (grabbed) {
        XNextEvent(dpy, &event);
        
        if (event.type == KeyPress || event.type == KeyRelease) {
            /* Release Alt key to exit and apply selection */
            if (event.type == KeyRelease && event.xkey.keycode == tabmodkey) {
                break;
            }

            /* Press Tab key to cycle further */
            if (event.type == KeyPress && event.xkey.keycode == tabcyclekey) {
                alttab();
            }
        }
    }

    /* Cleanup & finalize focus/stack */
    XUngrabKeyboard(dpy, CurrentTime);
    alttabend();
}
