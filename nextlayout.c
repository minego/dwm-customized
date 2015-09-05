static void nextlayout(const Arg *arg) {

    Layout *l;

    for (l = (Layout *)layouts; l != selmon->lt[selmon->sellt]; l++) {
		;
	}

    if (l->symbol && (l + 1)->symbol) {
        setlayout(&((Arg) { .v = (l + 1) }));
    } else {
        setlayout(&((Arg) { .v = layouts }));
	}
}
