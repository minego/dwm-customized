/*
	Modified version of http://dwm.suckless.org/patches/three-column originally
	created by Chris Truett. This version places more windows in the left
	column than the right.

	The intent is that windows which are less important can be moved to the left
	and will be given less screen space.
*/

/* The relative factors for the size of each column */
static const float colfact[3]			= { 0.1, 0.6, 0.3 };

static Client * mtclColumn(Monitor *m, Client *first, int count, int x, int w)
{
	Client	*c;
	int		i, y;
	int		cw, ch;
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
		ch = ((wh / cfacts) * c->cfact) - (2 * c->bw);

		if (c->maxh && ch > c->maxh) {
			c->h = c->maxh;
		} else if (c->minh && ch < c->minh) {
			c->h = c->minh;
		} else {
			c->h = 0;
			continue;
		}

		cfacts -= c->cfact;
		wh -= c->h + (2 * c->bw);
	}
	c = nexttiled(first);

	/* 2nd pass to position clients */
	y = m->wy;
	for (i = 0; (i < count) && c; c = nexttiled(c->next), i++) {
		if (c->h) {
			ch = c->h;
		} else {
			ch = ((wh / cfacts) * c->cfact) - (2 * c->bw);
		}

		cw = w - (2 * c->bw);
		if (c->maxw && cw > c->maxw) {
			/* Don't make a window bigger than it's max hint size */
			cw = c->maxw;
		} else if (c->minw && cw < c->minw) {
			cw = c->minw;
		}

		resize(c, x, y, cw, ch, False);

		ch = HEIGHT(c);
		if (ch < m->wh) {
			y = c->y + ch;
		}
	}

	return(c);
}

void mtcl(Monitor *m)
{
	int				masterw, leftw, rightw;
	unsigned int	i, leftn, rightn, mastern;
	float			colfacts;
#if 1 // MNG, fixing order for master on the right
	Client			*master_c		= nexttiled(m->clients);
	Client			*c, *next, **end_c;
	Client			*left_clients	= NULL;
	Client			**left_end		= &left_clients;
	Client			*right_clients	= NULL;
	Client			**right_end		= &right_clients;

int ready = 1;
while (0 == ready) {
	;
}

	/*
		Enumerate all tiled windows except the master and place them into either
		the right or left list.
	*/
	next = nexttiled(master_c ? master_c->next : NULL);
	while ((c = next)) {
		next = nexttiled(c->next);

		detach(c);
		c->next = NULL;

		if (c->isLeft) {
			*left_end = c;
			left_end = &c->next;
		} else {
			*right_end = c;
			right_end = &c->next;
		}
	}

	/*
		Reattach the windows in the appropriate order, keeping in mind that the
		"right" list is actually in the center and the master is actually on the
		right of the screen.
	*/
	end_c = &m->clients;
	while (end_c && *end_c) {
		end_c = &((*end_c)->next);
	}

	*right_end	= left_clients;
	*end_c		= right_clients;

#else

	Client			*c, *nc, *pc;
	Client			*lc, **lp;

// TODO MNG Automatically move windows in and out of the left column so that
//		the most recently used windows are kept on the master and right column
//		and all others are on the left.

	/*
		Reorder windows so that all windows in the left column are after those
		in the right column in the list.
	*/
	pc = nexttiled(m->clients);
	nc = nexttiled(pc ? pc->next : NULL); /* Skip the master window */
	lc = NULL;
	lp = &lc;

	while ((c = nc)) {
		nc = nexttiled(c->next);

		if (c->isLeft) {
			detach(c);
			c->next = NULL;

			*lp = c;
			lp = &c->next;
		} else {
			/* Keep track of the last client */
			pc = c;
		}
	}

	if (lc && pc) {
		/* Reattach the list of left clients to the end */
		*lp = pc->next;
		pc->next = lc;
	}
#endif

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

#if 1
	/* Master */
	c = mtclColumn(m, c, mastern, m->wx + leftw, masterw);

	/* Right column */
	c = mtclColumn(m, c, rightn, m->wx + masterw + leftw, rightw);

	/* left column */
	c = mtclColumn(m, c, leftn, m->wx, leftw);
#else
	// MNG Swap the right and master columns

	/* Master */
	c = mtclColumn(m, c, mastern, m->wx + leftw + rightw, masterw);

	/* Right column */
	c = mtclColumn(m, c, rightn, m->wx + leftw, rightw);

	/* left column */
	c = mtclColumn(m, c, leftn, m->wx, leftw);


#endif
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

