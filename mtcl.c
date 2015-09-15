/*
	Modified version of http://dwm.suckless.org/patches/three-column originally
	created by Chris Truett. This version places more windows in the left
	column than the right.

	The intent is that windows which are less important can be moved to the left
	and will be given less screen space.
*/

/*
	Factor of client count to be placed in left column vs right column.

	A value of 0 will place all clients in the right column. A value of 1 will
	put all clients in the left column.

	Calculations are done based on the cfact value for each client, and the
	master client is not included in the calculation.
*/
static const float lfact				= 0.75;
static const unsigned int mintclcount	= 4;

void mtcl(Monitor *m)
{
	int				x, y, h, w, bw;
	int				masterw, leftw, rightw, leftn, rightn;
	unsigned int	i, n;
	float			sfacts, l, r;
	Client			*c, *cwas;

	/* Count the windows and the client factor */
	for (n = 0, sfacts = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		if (n > 0) {
			sfacts += c->cfact;
		}
	}

	if (n == 0) {
		return;
	}

	c		= nexttiled(m->clients);
	masterw	= m->mfact * m->ww;
	bw		= (2 * c->bw);
	l		= lfact;
	n--;

	/*
		Disable the left column if the total screen size is too small or if
		there are not enough clients for it to make sense.
	*/
	if (m->ww < 1920 || n < mintclcount) {
		l = 0.0;
	}

	/* Calculate the number of clients in each column */
	if (n == 0) {
		rightn	= 0;
		leftn	= 0;
	} else {
		r = (1.0 - l) * sfacts;

		cwas = c;
		sfacts = 0;
		for (i = 0; c && sfacts <= r; c = nexttiled(c->next), i++) {
			sfacts += c->cfact;
		}
		c = cwas;

		if (i > n) {
			i = n;
		}
		rightn	= i;
		leftn	= n - i;
	}

	rightw		= (m->ww - masterw) * selmon->rfact;
	leftw		= (m->ww - masterw) - rightw;

	if (!leftn) {
		rightw	= m->ww - masterw;
		leftw	= 0;
	}
	if (!rightn) {
		rightw	= 0;
		leftw	= 0;
	}

	/* Master */
	// TODO Add support for a variable number of master clients?
	x = m->wx;
	y = m->wy,
	w = m->ww;
	h = m->wh - bw;

	if (leftn) {
		x += leftw;
		w -= (leftw + bw);
	}
	if (rightn) {
		w -= (rightw + bw);
	}

	resize(c, x, y, w, h, False);
	c->column = 0; /* center column */

	if (!leftn && !rightn) {
		return;
	}

	c = nexttiled(c->next);

	/* Right column */
	if (rightn > 0) {
		sfacts = 0;

		cwas = c;
		for (i = 0; c && i < rightn; c = nexttiled(c->next), i++) {
			sfacts += c->cfact;
		}
		c = cwas;

		x = m->wx + masterw + leftw;
		y = m->wy;
		w = rightw;

		for (i = 0; c && i < rightn; c = nexttiled(c->next), i++) {
			resize(c,
					x, y,
					w - bw,
					((m->wh / sfacts) * c->cfact) + bw,
					False);
			c->column = 1; /* right column */

			h = HEIGHT(c);
			if (h < m->wh) {
				y = c->y + h;
			}
		}
	}

	/* left column */
	if (leftn > 0) {
		sfacts = 0;

		cwas = c;
		for (i = 0; c && i < leftn; c = nexttiled(c->next), i++) {
			sfacts += c->cfact;
		}
		c = cwas;

		x = m->wx;
		y = m->wy;
		w = leftw;

		for (i = 0; c && i < leftn; c = nexttiled(c->next), i++) {
			resize(c, x, y,
					w - bw,
					((m->wh / sfacts) * c->cfact) + bw,
					False);
			c->column = -1; /* left column */

			h = HEIGHT(c);
			if (h < m->wh) {
				y = c->y + h;
			}
		}
	}
}

/* A value >= 1.0 sets the rfact to that value - 1.0 */
// TODO Replace mfact entirely with an array colfact on the monitor
void setrfact(const Arg *arg)
{
	if (!arg || !selmon || !selmon->lt[selmon->sellt]->arrange || !selmon->sel) {
		return;
	}

	if (!selmon->sel || 0 == selmon->sel->column) {
		/* Master window is selected, so resize the mfact */
		setmfact(arg);
		return;
	}

	if (arg->f >= 1.0) {
		selmon->rfact = arg->f - 1.0;
	} else {
		/* Adjust the argument based on the selected column */
		selmon->rfact += (arg->f * selmon->sel->column);
	}

	if (selmon->rfact < 0.1) {
		selmon->rfact = 0.1;
	} else if (selmon->rfact > 0.9) {
		selmon->rfact = 0.9;
	}
	arrange(selmon);
}

