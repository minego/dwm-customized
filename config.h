/* See LICENSE file for copyright and license details. */

#include "gaplessgrid.c"
#include "push.c"
#include "forcetile.c"
#include "mtcl.c"

/* appearance */
static const char *fonts[] = {
	// "terminus:size=16",
	"ProggyTinyTT:antialias=false:autohint=false",
    "Sans:size=10.5",
    "VL Gothic:size=10.5",
    "WenQuanYi Micro Hei:size=10.5",
};
#define dmenufont					"-*-terminus-medium-r-*-*-16-*-*-*-*-*-*-*"
#define normbordercolor				"#444444"
#define normbgcolor					"#222222"
#define normfgcolor					"#bbbbbb"
#define selbordercolor				"#ff0000"
#define selbgcolor					"#555555"
#define selfgcolor					"#eeeeee"

static const unsigned int borderpx	= 1;        /* border pixel of windows */
static const unsigned int gappx		= 6;		/* gap between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const Bool systraypinningfailfirst = True;   /* True: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const Bool showsystray       = True;     /* False means no systray */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */
/*   Display modes of the tab bar: never shown, always shown, shown only in */
/*   monocle mode in presence of several windows.                           */
/*   Modes after showtab_nmodes are disabled                                */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab            = showtab_auto; /* Default tab bar show mode */
static const Bool toptab            = False;    /* False means bottom tab bar */



/* tagging */
static char tags[][MAX_TAGLEN] = { "www", "code", "3", "4", "macaw", "chat", "mail", "8", "9" };

static const Rule rules[] = {
	/* class			instance	title			tags mask	isfloating	isterminal,		monitor	isLeft cfact	opacity	NoSwallow */

	/* Default rule: (Must be first) */
	{ NULL,				NULL,		NULL,			0,			False,		False,			-1,		False, 0.00,	-1,		False },

	{ "xvkbd",			NULL,		NULL,			0,			True,		False,			-1,		False, 0.00,	0.6,	True  },
	{ "Pidgin",			NULL,		NULL,			1 << 5,		False,		False,			-1,		True,  0.50,	0.8,	False },
	{ "chromium",		NULL,		"Macaw",		1 << 4,		False,		False,			-1,		True,  0.75,	0.8,	False },
	{ "st-256color",	NULL,		NULL,			0,			False,		True,			-1,		False, 0.00,	0.8,	False },

	{ "Firefox",		NULL,		NULL,			1 << 0,		False,		False,			-1,		False, 0.00,	-1,		False },
	{ "Chromium",		NULL,		NULL,			0,			False,		False,			-1,		False, 0.00,	-1,		False },
	{ "Google-chrome",	NULL,		NULL,			0,			False,		False,			-1,		False, 0.00,	-1,		False },
	{ "mpv",			NULL,		NULL,			0,			False,		False,			-1,		False, 0.00,	-1,		False }
};

/* layout(s) */
static const float mfact      = 0.60; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = False; /* True means respect size hints in tiled resizals */

static const Layout layouts[] = {		/* First entry is default */
	/* symbol		arrange function */
    { "=O=",		mtcl			},
	{ "[]=",		tile			},
	// { "><>",		NULL			},
	{ "[M]",		monocle			},
	{ "###",		gaplessgrid		},
	{ NULL,			NULL			}
};

#include "nextlayout.c"

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      comboview,      { .i = TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     { .ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      combotag,       { .ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      { .ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
// static const char *dmenucmd[] = { "dmenu_run", "-fn", FONTSTR, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
// static const char *dmenucmd[] = { "j4-dmenu-desktop", "--display-binary", "--term=myterm", "--dmenu=\"-fn " FONTSTR  "-nb " normbgcolor " -nf " normfgcolor " -sb " selbgcolor " -sf " selfgcolor, NULL };

static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]	= { "myterm", NULL };
static const char *menucmd[]	= {
									"j4-dmenu-desktop",
									"--display-binary",
									"--term=myterm",
									"--dmenu=~/bin/dmenu.sh",
									NULL
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = menucmd } },
	{ MODKEY,                       XK_slash,  spawn,          {.v = menucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_t,      tabmode,        {-1} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setcolfact,     {.f = -0.05} },
	{ MODKEY,                       XK_l,      setcolfact,     {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY|ShiftMask,				XK_j,      pushdown,       {0} },
	{ MODKEY|ShiftMask,				XK_k,      pushup,         {0} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    comboview,      {-1} },
	{ MODKEY|ShiftMask,             XK_Tab,    pushleft,       {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,						XK_w,      killclient,     {0} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  nextlayout,     {0} },
	{ MODKEY,						XK_t,      forcetile,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_n,      nametag,        {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} }
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} },
};

