void horizgrid(Monitor *m)
{
	Client			*c;
	unsigned int	n, i;
	int				w = 0, h = 0;
	int				ntop, nbottom = 0;

	/* Count windows */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		;
	}

	if (n == 0) {
		return;
	}

	if (n == 1) {
		/* Just fill the whole screen */
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, m->ww - (2*c->bw), m->wh - (2*c->bw), False);
	} else if (n == 2) {
		/* Choose which way to split based on the monitor orientation */
		if (m->wh < m->ww) {
			/* landscape */
			w = m->ww / 2;

			c = nexttiled(m->clients);
			resize(c, m->wx, m->wy, w - (2 * c->bw), m->wh - (2 * c->bw), False);
			c = nexttiled(c->next);
			resize(c, m->wx + w, m->wy, w - (2 * c->bw), m->wh - (2 * c->bw), False);
		} else {
			/* portrait */
			h = m->wh / 2;

			c = nexttiled(m->clients);
			resize(c, m->wx, m->wy, m->ww - (2 * c->bw), h - (2 * c->bw), False);
			c = nexttiled(c->next);
			resize(c, m->wx, m->wy + h, m->ww - (2 * c->bw), h - (2 * c->bw), False);
		}
	} else {
		ntop = n / 2;
		nbottom = n - ntop;

		for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
			if (i < ntop) {
				resize(c, m->wx + i * m->ww / ntop, m->wy, m->ww / ntop - (2*c->bw), m->wh / 2 - (2*c->bw), False);
			} else {
				resize(c, m->wx + (i - ntop) * m->ww / nbottom, m->wy + m->wh / 2, m->ww / nbottom - (2*c->bw), m->wh / 2 - (2*c->bw), False);
			}
		}
	}
}
