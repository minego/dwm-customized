#define LEFT_PORTION		3

/*
	Modified version of http://dwm.suckless.org/patches/three-column originally
	created by Chris Truett. This version places more windows in the left
	column than the right.

	The intent is that windows which are less important can be moved to the left
	and will be given less screen space.
*/

void mtcl(Monitor *m)
{
	int				x, y, h, w, mw, sw, bw, leftcount, rightcount;
	unsigned int	i, n;
	Client			*c;

	/* Count the windows */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		;
	}

	if (n == 0) {
		return;
	}

	c  = nexttiled(m->clients);
	mw = m->mfact * m->ww;
	sw = (m->ww - mw) / 2;
	// bw = (2 * c->bw);
	bw = c->bw;

	/* Calculate the number of clients in each column */
	n--;
	if (n == 0) {
		rightcount	= 0;
		leftcount	= 0;
	} else if (n < LEFT_PORTION) {
		rightcount	= n;
		leftcount	= 0;
	} else {
		rightcount	= n / LEFT_PORTION;
		if ((n % LEFT_PORTION) > 1) {
			/* Round up */
			rightcount++;
		}

		leftcount	= n - rightcount;
	}

	/* Master */
	x = m->wx;
	y = m->wy,
	w = m->ww;
	h = m->wh - bw;

	if (leftcount) {
		x += sw;
		w -= (sw + bw);
	}
	if (rightcount) {
		w -= (sw + bw);
	}

	resize(c, x, y, w, h, False);

	if (!leftcount && !rightcount) {
		return;
	}

	w = (m->ww - mw) / ((n > 1) + 1);
	c = nexttiled(c->next);

	/* Right column */
	if (rightcount > 0) {
		x = m->wx + mw + sw;
		y = m->wy;
		h = m->wh / rightcount;

		if (h < bh) {
			h = m->wh;
		}

		for (i = 0; c && i < rightcount; c = nexttiled(c->next), i++) {
			resize(c,
					x, y,
					w - bw, h - bw,
					False);

			if (h != m->wh) {
				y = c->y + HEIGHT(c);
			}
		}
	}

	/* left column */
	if (leftcount > 0) {
		x = m->wx;
		y = m->wy;
		h = m->wh / leftcount;

		if (h < bh) {
			h = m->wh;
		}

		for (i = 0; c && i < leftcount; c = nexttiled(c->next), i++) {
			resize(c, x, y,
					w - bw, h - bw,
					False);

			if (h != m->wh) {
				y = c->y + HEIGHT(c);
			}
		}
	}
}

