/*
	Turn off floating for a client even if it is fixed. The default behavior for
	DWM is for a fixed size client to always be floated.

	This may not play well with some applications.

	To use change bind a key to the function forcetile and include forcetile.c
	in your config.h.
*/
static void forcetile(const Arg *arg)
{
	if (!selmon->sel) {
		return;
	}

	selmon->sel->isfloating = False;
	arrange(selmon);
}


