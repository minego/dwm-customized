#include <math.h>

/*
	Heavily modified version of the 3 column layout patch, originally created by
	Chris Truett:
		http://dwm.suckless.org/patches/three-column

	- Treat the left column as special and allow clients to be pushed into it
	- Allow a variable number of master columns
	- Allow a variable number of right columns
	- Supports nmaster
*/

/* The relative factors for the size of each column */
static const float colfact[3]			= { 0.1, 0.6, 0.3 };

static int isleft(Client *c)
{
	if (c == NULL) {
		return 0;
	}

	if (c->mon != NULL && c->mon->mw <= 2000) {
		/* The left column is not worth using on a small monitor */
		return 0;
	}

	return c->isLeft;
}

/* Return non-zero if the currently selected client is in a master column */
static int ismaster(void)
{
	Client	*c;
	int		i;

	if (!selmon || !selmon->lt[selmon->sellt]->arrange || !selmon->sel) {
		return 0;
	}

	for (i = 0, c = nexttiled(selmon->clients); c; c = nexttiled(c->next), i++) {
		if (selmon->sel == c) {
			/* c is the selected client, and is index i */
			if (i < selmon->nmaster) {
				return 1;
			} else {
				return 0;
			}
		}
	}

	return 0;
}

/* A value >= 1.0 sets that colfact to that value - 1.0 */
void setcolfact(const Arg *arg)
{
	int		index = 1;

	if (!arg || !selmon || !selmon->lt[selmon->sellt]->arrange || !selmon->sel) {
		return;
	}

	if (ismaster()) {
		index = 0;
		/* master */
		index = 0;
	} else if (isleft(selmon->sel)) {
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

static Client * placeColumn(Monitor *m, Client *first, int count, int *x, int w)
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

		resize(c, *x, y, cw, ch, False);

		ch = HEIGHT(c);
		if (ch < m->wh) {
			y = c->y + ch;
		}
	}

	(*x) += w;

	return(c);
}

/*
	variable column layout

	This layout has a variable number of columns, in 3 categories.
		0-1 small left columns, containing clients that have been "pushed" left
		1-n master columns
		0-n right columns
*/
void varcol(Monitor *m)
{
	int				masterw, leftw, rightw, x;
	unsigned int	i, leftn, rightn, mastern, coln;
	float			colfacts;
	Client			*c, **end, *next;
	Client			*left_clients	= NULL;
	int				nmastercols		= m->nmastercols;
	int				nrightcols		= m->nrightcols;

	/*
		Pull off all of the windows which have been pushed to the left, and
		reattach them at the end of the list.
	*/
	end = &left_clients;
	next = nexttiled(m->clients);

	for (i = 0; (c = next); i++) {
		/* Calculate the next one here in case we detach */
		next = nexttiled(c->next);

		if (i < m->nmaster) {
			/* Master */
			;
		} else if (isleft(c)) {
			/* Left; Detach and put in the left list */
			detach(c);
			c->next = NULL;

			*end = c;
			end = &c->next;
		}
	}

	/*
		Find the end of the client list so that the left windows can be
		reattached at the end.
	*/
	end = &m->clients;
	while (end && *end) {
		end = &((*end)->next);
	}
	*end = left_clients;


	/* Count the windows for each column type */
	leftn = rightn = mastern = 0;
	for (c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		if (mastern < m->nmaster) {
			mastern++;
		} else if (isleft(c)) {
			leftn++;
		} else {
			rightn++;
		}
	}

	nmastercols	= MAX(MIN(mastern, nmastercols), 1);
	nrightcols	= MAX(MIN(rightn, nrightcols), 1);

	if (mastern == 0) {
		return;
	}


	/* Calculate the total colfacts value */
	colfacts = 0;

	/* Left column */
	if (leftn > 0) {
		colfacts += m->colfact[0];
	}

	/* Center column(s) */
	for (i = 0; i < nmastercols; i++) {
		colfacts += m->colfact[1];
	}

	/* Right column(s) */
	if (rightn > 0) {
		for (i = 0; i < nrightcols; i++) {
			colfacts += m->colfact[2];
		}
	}

	/* Calculate the width for each column type */
	leftw	= (m->ww / colfacts) * m->colfact[0];
	masterw	= (m->ww / colfacts) * m->colfact[1];
	rightw	= (m->ww / colfacts) * m->colfact[2];


	/* Master columns; start to the right of the "left" column */
	c = nexttiled(m->clients);
	coln = floor(mastern / nmastercols);
	x = m->wx;
	if (leftn > 0) {
		x += leftw;
	}

	for (i = 0; i < nmastercols; i++) {
		if (i < nmastercols - 1) {
			c = placeColumn(m, c, coln, &x, masterw);
			mastern -= coln;
		} else {
			/* Remainder goes into the last master column */
			c = placeColumn(m, c, mastern, &x, masterw);
		}
	}

	/* Right columns; start directly to the right of the last master column */
	if (rightn > 0) {
		coln = floor(rightn / nrightcols);

		for (i = 0; i < nrightcols; i++) {
			if (i < nrightcols - 1) {
				c = placeColumn(m, c, coln, &x, rightw);
				rightn -= coln;
			} else {
				/* Remainder goes into the last right column */
				c = placeColumn(m, c, rightn, &x, rightw);
			}
		}
	}

	/* left column */
	if (leftn > 0) {
		x = m->wx;
		c = placeColumn(m, c, leftn, &x, leftw);
	}
}

/*
	Modify either the right or master column count
*/
void incncols(const Arg *arg)
{
	if (!arg || !selmon || !selmon->lt[selmon->sellt]->arrange || !selmon->sel) {
		return;
	}

	if (ismaster()) {
		/* master */
		selmon->nmastercols = MAX(selmon->nmastercols + arg->i, 0);

		/* Auto adjust nmaster as well */
		selmon->nmaster = MAX(selmon->nmaster, selmon->nmastercols);
	} else if (isleft(selmon->sel)) {
		/* left */
		;
	} else {
		/* right */
		selmon->nrightcols = MAX(selmon->nrightcols + arg->i, 0);
	}

	arrange(selmon);
}

