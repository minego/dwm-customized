/* See LICENSE file for copyright and license details. */

#include "push.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx		= 24;		/* gap between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int usealtbar          = 1;        /* 1 means use non-dwm status bar */
static const char *altbarclass      = "Polybar"; /* Alternate bar class name */
static const char *alttrayname      = "tray";    /* Polybar tray instance name */
static const char *altbarcmd        = "$HOME/bin/launch-polybar"; /* Alternate bar launch command */
static const double defaultopacity  = 1.0;
// static const char *fonts[]         = { "SauceCodePro Nerd Font Mono:size=10", "SauceCodePro Nerd Font:size=10", "monospace:size=10" };
static const char *fonts[]         = { "monospace:size=10", "TerminessTTF Nerd Font Mono:size=24" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_orange[]      = "#fff39d21";
static const char *colors[][3]      = {
	/*					fg				bg				border   */
	[SchemeNorm] = {	"#ffffffff",	"#00000000",	"#00000000" },
	[SchemeSel]  = {	"#ff000000",	"#ffffffff",	"#ffffffff" },
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "﬐", "", "ﱘ", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class				instance	title           tags mask	isfloating  isterminal  noswallow	opacity	monitor	isLeft */
	{ "kitty",				NULL,		NULL,           0,			False,		True,		0,			0.8,	-1,		False	},

	{ "Slack",				NULL,		NULL,			1 << 5,		False,		False,		0,			1.0,	-1,		True	},
	{ "TelegramDesktop",	NULL,		NULL,			1 << 5,		False,		False,		0,			1.0,	-1,		True	},
	{ "googlemessages",		NULL,		NULL,			1 << 5,		False,		False,		0,			1.0,	-1,		True	},

	{ "Spotify",			NULL,		NULL,			1 << 7,		False,		False,		0,			1.0,	-1,		False	},
	{ "Mail",				NULL,		NULL,			1 << 6,		False,		False,		0,			1.0,	-1,		False	},
	{ "Code",				NULL,		NULL,			1 << 1,		False,		False,		0,			1.0,	-1,		False	},
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int attachbelow = 1;    /* 1 means attach after the currently active window */

/* Variable column layout */
static const int nmastercols	= 1;	/* number of master columns for n column layout */
static const int nrightcols		= 1;	/* number of right columns for n column layout */
#include "varcol.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "=O=",      varcol },  /* first entry is default */
	{ "[M]",      monocle },
	{ "[]=",      tile },
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
static const char *termcmd[]  = { "myterm", NULL };
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]	= { "j4-dmenu-desktop", "--term=myterm", "--dmenu=~/bin/dmenu.sh", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_slash,  spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },

	{ MODKEY|ShiftMask,             XK_i,      incncols,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_d,      incncols,       {.i = -1 } },
	{ MODKEY,                       XK_h,      setcolfact,     {.f = -0.05} },
	{ MODKEY,                       XK_l,      setcolfact,     {.f = +0.05} },

	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },

	{ MODKEY|ShiftMask,				XK_j,      pushdown,       {0} },
	{ MODKEY|ShiftMask,				XK_k,      pushup,         {0} },

	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_Tab,    pushleft,       {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,						XK_w,      killclient,     {0} },
	// { MODKEY,						XK_z,      toggleswallow,  {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,						XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_space,  nextlayout,     {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_t,      togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_KP_Add, changeopacity, {.f = +0.1}},
	{ MODKEY|ShiftMask,             XK_KP_Subtract, changeopacity, {.f = -0.1}},
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
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
};

static const char *ipcsockpath = "/tmp/dwm.sock";
static IPCCommand ipccommands[] = {
  IPCCOMMAND(  view,                1,      {ARG_TYPE_UINT}   ),
  IPCCOMMAND(  toggleview,          1,      {ARG_TYPE_UINT}   ),
  IPCCOMMAND(  tag,                 1,      {ARG_TYPE_UINT}   ),
  IPCCOMMAND(  toggletag,           1,      {ARG_TYPE_UINT}   ),
  IPCCOMMAND(  tagmon,              1,      {ARG_TYPE_UINT}   ),
  IPCCOMMAND(  focusmon,            1,      {ARG_TYPE_SINT}   ),
  IPCCOMMAND(  focusstack,          1,      {ARG_TYPE_SINT}   ),
  IPCCOMMAND(  zoom,                1,      {ARG_TYPE_NONE}   ),
  IPCCOMMAND(  incnmaster,          1,      {ARG_TYPE_SINT}   ),
  IPCCOMMAND(  killclient,          1,      {ARG_TYPE_SINT}   ),
  IPCCOMMAND(  togglefloating,      1,      {ARG_TYPE_NONE}   ),
  IPCCOMMAND(  setmfact,            1,      {ARG_TYPE_FLOAT}  ),
  IPCCOMMAND(  setlayoutsafe,       1,      {ARG_TYPE_PTR}    ),
  IPCCOMMAND(  quit,                1,      {ARG_TYPE_NONE}   )
};

