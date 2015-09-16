/*
	Modified version of http://dwm.suckless.org/patches/three-column originally
	created by Chris Truett. This version places more windows in the left
	column than the right.

	The intent is that windows which are less important can be moved to the left
	and will be given less screen space.
*/

/* The relative factors for the size of each column */
static const float colfact[3]			= { 0.2, 0.5, 0.3 };

static Client * mtclColumn(Monitor *m, Client *first, int count, int x, int w)
{
	Client	*c;
	int		i, y, h;
	int		wh		= m->wh;
	float	cfacts	= 0;

	if (!count || !first || !(c = nexttiled(first))) {
		return(first);
	}

	/* 1st pass to calculate the total cfact */
	for (i = 0; (i < count) && c; c = nexttiled(c->next), i++) {
		cfacts += c->cfact;
	}
	c = nexttiled(first);

	/* 2nd pass to calculate heights taking into account minh and maxh */
	y = m->wy;
	for (i = 0; (i < count) && c; c = nexttiled(c->next), i++) {
		h = ((wh / cfacts) * c->cfact) - c->bw;

		if (c->maxh && h > c->maxh) {
			c->h = c->maxh;
		} else if (c->minh && h < c->minh) {
			c->h = c->minh;
		} else {
			c->h = 0;
			continue;
		}

		cfacts -= c->cfact;
		wh -= c->h + c->bw;
	}
	c = nexttiled(first);

	/* 2nd pass to position clients */
	y = m->wy;
	for (i = 0; (i < count) && c; c = nexttiled(c->next), i++) {
		if (c->h) {
			h = c->h;
		} else {
			h = ((wh / cfacts) * c->cfact) - c->bw;
		}

		resize(c, x, y, w - c->bw, h, False);

		h = HEIGHT(c);
		if (h < m->wh) {
			y = c->y + h;
		}
	}

	return(c);
}

void mtcl(Monitor *m)
{
	int				masterw, leftw, rightw;
	unsigned int	i, leftn, rightn, mastern;
	float			colfacts;
	Client			*c, *nc, *pc;

	/*
		Reorder windows so that all windows in the left column are after those
		in the right column in the list.
	*/
	pc = nexttiled(m->clients);
	nc = nexttiled(pc ? pc->next : NULL); /* Skip the master window */
	while ((c = nc)) {
		nc = nexttiled(c->next);

		if (c->isLeft && nc && !nc->isLeft) {
			/* swap c and nc */
			c->next = nc->next;
			nc->next = c;

			if (pc) {
				pc->next = nc;
			} else {
				m->clients = nc;
			}

			c = nc;
			nc = c->next;
		}

		/* Keep track of the previous client */
		pc = c;
	}

	/* Count the windows and the client factor */
	leftn = rightn = mastern = 0;
	for (c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		if (mastern > 0) {
			if (c->isLeft) {
				leftn++;
			} else {
				rightn++;
			}
		} else {
			mastern++;
		}
	}

	if (mastern == 0) {
		return;
	}

	for (colfacts = 0, i = 0; i < 3; i++) {
		colfacts += m->colfact[i];
	}

	c		= nexttiled(m->clients);
	masterw	= (m->ww / colfacts) * m->colfact[1];

	rightw	= (m->ww / colfacts) * m->colfact[2];
	leftw	= (m->ww - masterw) - rightw;

	if (!leftn) {
		masterw	+= leftw;
		leftw	= 0;
	}
	if (!rightn) {
		masterw	+= rightw;
		rightw	= 0;
	}

	/* Master */
	c = mtclColumn(m, c, mastern, m->wx + leftw, masterw);

	/* Right column */
	c = mtclColumn(m, c, rightn, m->wx + masterw + leftw, rightw);

	/* left column */
	c = mtclColumn(m, c, leftn, m->wx, leftw);
}

/* A value >= 1.0 sets that colfact to that value - 1.0 */
void setcolfact(const Arg *arg)
{
	Client	*master;
	int		index = 1;

	if (!arg || !selmon || !selmon->lt[selmon->sellt]->arrange || !selmon->sel) {
		return;
	}

	master = nexttiled(selmon->clients);
	if (selmon->sel == master) {
		/* master */
		index = 0;
	} else if (selmon->sel->isLeft) {
		/* left */
		index = -1;
	} else {
		/* right */
		index = 1;
	}
	index++;

	if (arg->f >= 1.0) {
		selmon->colfact[index] = arg->f - 1.0;
	} else {
		/* Adjust the argument based on the selected column */
		selmon->colfact[index] += arg->f;
	}

	if (selmon->colfact[index] < 0.1) {
		selmon->colfact[index] = 0.1;
	} else if (selmon->colfact[index] > 0.9) {
		selmon->colfact[index] = 0.9;
	}
	arrange(selmon);
}

static void pushleft(const Arg *arg)
{
	if (selmon && selmon->sel) {
		selmon->sel->isLeft = !selmon->sel->isLeft;

		focus(selmon->sel);
	}
	arrange(selmon);
}

