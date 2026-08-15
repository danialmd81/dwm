int alttabn;          /* move that many clients forward */
int ntabs;            /* number of active clients in tag */
int isalt;
Client **altsnext;    /* array of all clients in the tag */
Window alttabwin;

void
alttab()
{
	Monitor *m = selmon;

	XRaiseWindow(dpy, alttabwin);

	/* move to next window */
	if (m->sel && m->sel->snext) {
		alttabn++;
		if (alttabn >= ntabs)
			alttabn = 0;

		focus(altsnext[alttabn]);
	}
}

void
alttabend()
{
	Monitor *m = selmon;
	Client *buff;
	int i;

	if (!isalt)
		return;

	/* Reorder the MRU list in memory – no visual updates */
	if (ntabs > 1) {
		if (alttabn != 0) {
			buff = altsnext[alttabn];
			if (alttabn > 1)
				for (i = alttabn; i > 0; i--)
					altsnext[i] = altsnext[i - 1];
			else
				altsnext[alttabn] = altsnext[0];
			altsnext[0] = buff;
		}
		free(altsnext);
	}

	/* destroy the popup window */
	isalt = 0;
	ntabs = 0;
	XUnmapWindow(dpy, alttabwin);
	XDestroyWindow(dpy, alttabwin);

	/* Focus the selected window and restack ONCE – this is the only restack */
	focus(m->sel);
	restack(m);
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

	isalt = 1;
	alttabn = 0;
	ntabs = 0;

	for (c = m->clients; c; c = c->next) {
		if (!ISVISIBLE(c))
			continue;
		++ntabs;
	}

	if (!ntabs) {
		alttabend();
		return;
	}

	altsnext = (Client **) malloc(ntabs * sizeof(Client *));

	for (i = 0, c = m->stack; c; c = c->snext) {
		if (!ISVISIBLE(c))
			continue;
		altsnext[i] = c;
		i++;
	}

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 };

	/* grab keyboard */
	grabbed = 1;
	for (i = 0; i < 1000; i++) {
		if (XGrabKeyboard(dpy, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
			break;
		nanosleep(&ts, NULL);
		if (i == 1000 - 1)
			grabbed = 0;
	}

	XEvent event;
	alttab();

	if (grabbed == 0) {
		alttabend();
		return;
	}

	while (grabbed) {
		XNextEvent(dpy, &event);
		if (event.type == KeyPress || event.type == KeyRelease) {
			if (event.type == KeyRelease && event.xkey.keycode == tabmodkey)
				break;

			if (event.type == KeyPress) {
				if (event.xkey.keycode == tabcyclekey) {
					alttab();
				}
			}
		}
	}

	/* FINAL FIX: alttabend() already focuses and restacks once.
	   Do NOT call focus(c) or restack(m) here – they cause flicker. */
	alttabend();
	XUngrabKeyboard(dpy, CurrentTime);
	/* focus(c); restack(m);   <-- REMOVED */
}