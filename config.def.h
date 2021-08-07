/* See LICENSE file for copyright and license details. */

/* for audio and brightness keys */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 0; /* border pixel of windows */
static const unsigned int snap     = 8; /* snap pixel */
static const int showbar           = 1; /* 0 means no bar */
static const int topbar            = 1; /* 0 means bottom bar */
static const char *fonts[]         = { "Roboto Mono:size=9" };
static const char dmenufont[]      = "Rovoto Mono:size=9";
static const char gray1[]          = "#373737";
static const char gray2[]          = "#000000";
static const char gray3[]          = "#c8c8c8";
static const char gray4[]          = "#ffffff";
static const char black[]          = "#000000";
static const char *colors[][3]     = {
	/*               fg      bg      border */
	[SchemeNorm] = { gray3,  gray1,  gray2 },
	[SchemeSel]  = { gray4,  black,  black  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* class  instance  title  tags  floating  monitor */
	{ NULL,   NULL,     NULL,  0,    False,    -1 },
};

/* layout(s) */
static const float mfact        = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster        = 1;    /* number of clients in master area */
static const int resizehints    = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1;    /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol   arrange  function */
	{ "[]=",    tile },  /* first entry is default */
	{ "><>",    NULL },  /* no layout function means floating behavior */
	{ "[M]",    monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                        KEY,  view,        {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,            KEY,  toggleview,  {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,              KEY,  tag,         {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask,  KEY,  toggletag,   {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]   = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", gray1, "-nf", gray3, "-sb", black, "-sf", gray4, NULL };
static const char *termcmd[]    = { "kitty", NULL };
static const char *firefoxcmd[] = { "firefox", NULL };
static const char *discordcmd[] = { "dbus-launch", "flatpak", "run", "com.discordapp.Discord", NULL };
static const char *spotifycmd[] = { "dbus-launch", "flatpak", "run", "com.spotify.Client", NULL };
static const char *unitycmd[]   = { "dbus-launch", "flatpak", "run", "com.unity.UnityHub", NULL };
static const char *prtscrncmd[] = { "sh", "-c", "import png:- | xclip -selection clipboard -t image/png", NULL };

/* for audio and brightness keys */
static const char *volup[]     = { "sh", "-c", "pactl set-sink-volume @DEFAULT_SINK@ +5%; pkill -RTMIN+1 dwmblocks", NULL };
static const char *voldown[]   = { "sh", "-c", "pactl set-sink-volume @DEFAULT_SINK@ -5%; pkill -RTMIN+1 dwmblocks", NULL };
static const char *volmute[]   = { "sh", "-c", "pactl set-sink-mute @DEFAULT_SINK@ toggle; pkill -RTMIN+1 dwmblocks", NULL };
static const char *lightup[]   = { "sh", "-c", "xbacklight -inc 5 -time 0; pkill -RTMIN+2 dwmblocks", NULL };
static const char *lightdown[] = { "sh", "-c", "xbacklight -dec 5 -time 0; pkill -RTMIN+2 dwmblocks", NULL };

static Key keys[] = {
	/* modifier          key         function         argument */
	{ MODKEY,            XK_p,       spawn,           {.v = dmenucmd } },
	{ MODKEY|ShiftMask,  XK_Return,  spawn,           {.v = termcmd } },
	{ MODKEY|ShiftMask,  XK_f,       spawn,           {.v = firefoxcmd } },
	{ MODKEY|ShiftMask,  XK_d,       spawn,           {.v = discordcmd } },
	{ MODKEY|ShiftMask,  XK_s,       spawn,           {.v = spotifycmd } },
	{ MODKEY|ShiftMask,  XK_u,       spawn,           {.v = unitycmd } },
 	{ MODKEY|ShiftMask,  XK_p,       spawn,           {.v = prtscrncmd } },
	{ MODKEY,            XK_b,       togglebar,       {0} },
	{ MODKEY,            XK_j,       focusstack,      {.i = +1 } },
	{ MODKEY,            XK_k,       focusstack,      {.i = -1 } },
	{ MODKEY,            XK_i,       incnmaster,      {.i = +1 } },
	{ MODKEY,            XK_d,       incnmaster,      {.i = -1 } },
	{ MODKEY,            XK_h,       setmfact,        {.f = -0.05} },
	{ MODKEY,            XK_l,       setmfact,        {.f = +0.05} },
	{ MODKEY,            XK_Return,  zoom,            {0} },
	{ MODKEY,            XK_Tab,     view,            {0} },
	{ MODKEY|ShiftMask,  XK_c,       killclient,      {0} },
	{ MODKEY,            XK_t,       setlayout,       {.v = &layouts[0]} },
	{ MODKEY,            XK_f,       setlayout,       {.v = &layouts[1]} },
	{ MODKEY,            XK_m,       setlayout,       {.v = &layouts[2]} },
	{ MODKEY,            XK_space,   setlayout,       {0} },
	{ MODKEY|ShiftMask,  XK_space,   togglefloating,  {0} },
	{ MODKEY,            XK_0,       view,            {.ui = ~0 } },
	{ MODKEY|ShiftMask,  XK_0,       tag,             {.ui = ~0 } },
	{ MODKEY,            XK_comma,   focusmon,        {.i = -1 } },
	{ MODKEY,            XK_period,  focusmon,        {.i = +1 } },
	{ MODKEY|ShiftMask,  XK_comma,   tagmon,          {.i = -1 } },
	{ MODKEY|ShiftMask,  XK_period,  tagmon,          {.i = +1 } },
	TAGKEYS(             XK_1,       0)
	TAGKEYS(             XK_2,       1)
	TAGKEYS(             XK_3,       2)
	TAGKEYS(             XK_4,       3)
	TAGKEYS(             XK_5,       4)
	TAGKEYS(             XK_6,       5)
	TAGKEYS(             XK_7,       6)
	TAGKEYS(             XK_8,       7)
	TAGKEYS(             XK_9,       8)
	{ MODKEY|ShiftMask,  XK_q,                      quit,   {0} },
	{ 0,                 XF86XK_AudioRaiseVolume,   spawn,  {.v = volup   } },
	{ 0,                 XF86XK_AudioLowerVolume,   spawn,  {.v = voldown } },
	{ 0,                 XF86XK_AudioMute,          spawn,  {.v = volmute } },
	{ 0,                 XF86XK_MonBrightnessUp,    spawn,  {.v = lightup} },
	{ 0,                 XF86XK_MonBrightnessDown,  spawn,  {.v = lightdown} },
};

/* button definitions */
static Button buttons[] = {
	/* click          event    button    function         argument */
	{ ClkLtSymbol,    0,       Button1,  setlayout,       {0} },
	{ ClkLtSymbol,    0,       Button3,  setlayout,       {.v = &layouts[2]} },
	{ ClkWinTitle,    0,       Button2,  zoom,            {0} },
	{ ClkStatusText,  0,       Button2,  spawn,           {.v = termcmd } },
	{ ClkClientWin,   MODKEY,  Button1,  movemouse,       {0} },
	{ ClkClientWin,   MODKEY,  Button2,  togglefloating,  {0} },
	{ ClkClientWin,   MODKEY,  Button3,  resizemouse,     {0} },
	{ ClkTagBar,      0,       Button1,  view,            {0} },
	{ ClkTagBar,      0,       Button3,  toggleview,      {0} },
	{ ClkTagBar,      MODKEY,  Button1,  tag,             {0} },
	{ ClkTagBar,      MODKEY,  Button3,  toggletag,       {0} },
};

