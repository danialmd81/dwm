/* See LICENSE file for copyright and license details. */
// clang-format off

#include <X11/XF86keysym.h>
#include <X11/Xutil.h>

/* Helper macros for spawning commands */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
#define CMD(...)   { .v = (const char*[]){ __VA_ARGS__, NULL } }

/* appearance */
static const unsigned int borderpx       = 2;   /* border pixel of windows */
static const unsigned int snap           = 32;  /* snap pixel */
static const char autostartblocksh[]     = "autostart_blocking.sh";
static const char autostartsh[]          = "autostart.sh";
static const char dwmdir[]               = "dwm";
static const char localshare[]           = ".local/share";
static const int showbar                 = 1;   /* 0 means no bar */
static const int topbar                  = 1;   /* 0 means bottom bar */
/* Status is to be shown on: -1 (all monitors), 0 (a specific monitor by index), 'A' (active monitor) */
static const int statusmon               = 'A';
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int showsystray             = 1;   /* 0 means no systray */

/* Indicators: see patch/bar_indicators.h for options */
static int tagindicatortype              = INDICATOR_TOP_LEFT_SQUARE;
static int tiledindicatortype            = INDICATOR_NONE;
static int floatindicatortype            = INDICATOR_TOP_LEFT_SQUARE;

/* Fonts */
static const char *fonts[] = {
    "CaskaydiaCove Nerd Font:size=11:style=Bold:antialias=true:autohint=true",
    "Vazirmatn:size=11:style=Medium:antialias=true:autohint=true",
    "Font Awesome 5 Free Solid:size=10:antialias=true:autohint=true",
    "Noto Color Emoji:size=10:antialias=true:autohint=true"
};
static const char dmenufont[]            = "CaskaydiaCove NerdFont:size=12";

static char c000000[]                    = "#000000"; // placeholder value

/* Catppuccin Macchiato Color Palette */
static char normfgcolor[]                = "#cdd6f4"; /* Text */
static char normbgcolor[]                = "#1e1e2e"; /* Base background */
static char normbordercolor[]            = "#313244"; /* Surface0 inactive border */
static char normfloatcolor[]             = "#313244";

static char selfgcolor[]                 = "#1e1e2e"; /* Base text on active tag */
static char selbgcolor[]                 = "#89b4fa"; /* Blue active accent */
static char selbordercolor[]             = "#89b4fa"; /* Blue active border */
static char selfloatcolor[]              = "#89b4fa";

static char titlenormfgcolor[]           = "#cdd6f4";
static char titlenormbgcolor[]           = "#1e1e2e";
static char titlenormbordercolor[]       = "#313244";
static char titlenormfloatcolor[]        = "#313244";

static char titleselfgcolor[]            = "#89b4fa";
static char titleselbgcolor[]            = "#1e1e2e";
static char titleselbordercolor[]        = "#89b4fa";
static char titleselfloatcolor[]         = "#89b4fa";

static char tagsnormfgcolor[]            = "#cdd6f4";
static char tagsnormbgcolor[]            = "#1e1e2e";
static char tagsnormbordercolor[]        = "#313244";
static char tagsnormfloatcolor[]         = "#313244";

static char tagsselfgcolor[]             = "#1e1e2e";
static char tagsselbgcolor[]             = "#89b4fa";
static char tagsselbordercolor[]         = "#89b4fa";
static char tagsselfloatcolor[]          = "#89b4fa";

static char hidnormfgcolor[]             = "#45475a";
static char hidselfgcolor[]              = "#89b4fa";
static char hidnormbgcolor[]            = "#1e1e2e";
 static char hidselbgcolor[]              = "#1e1e2e";

static char urgfgcolor[]                 = "#1e1e2e";
static char urgbgcolor[]                 = "#f38ba8"; /* Red urgent background */
static char urgbordercolor[]             = "#f38ba8";
static char urgfloatcolor[]              = "#f38ba8";

static char *colors[][ColCount] = {
    /*                       fg                bg                border                float */
    [SchemeNorm]         = { normfgcolor,      normbgcolor,      normbordercolor,      normfloatcolor },
    [SchemeSel]          = { selfgcolor,       selbgcolor,       selbordercolor,       selfloatcolor },
    [SchemeTitleNorm]    = { titlenormfgcolor, titlenormbgcolor, titlenormbordercolor, titlenormfloatcolor },
    [SchemeTitleSel]     = { titleselfgcolor,  titleselbgcolor,  titleselbordercolor,  titleselfloatcolor },
    [SchemeTagsNorm]     = { tagsnormfgcolor,  tagsnormbgcolor,  tagsnormbordercolor,  tagsnormfloatcolor },
    [SchemeTagsSel]      = { tagsselfgcolor,   tagsselbgcolor,   tagsselbordercolor,   tagsselfloatcolor },
    [SchemeHidNorm]      = { hidnormfgcolor,   hidnormbgcolor,   c000000,              c000000 },
    [SchemeHidSel]       = { hidselfgcolor,    hidselbgcolor,    c000000,              c000000 },
    [SchemeUrg]          = { urgfgcolor,       urgbgcolor,       urgbordercolor,       urgfloatcolor },
};

/* Workspace Tags */
static char *tagicons[][NUMTAGS] =
{
    [DEFAULT_TAGS]        = { "1: Web", "2: Code", "3: Net", "4: Hub", "5", "6", "7", "8", "9" },
    [ALTERNATIVE_TAGS]    = { "A", "B", "C", "D", "E", "F", "G", "H", "I" },
    [ALT_TAGS_DECORATION] = { "<1>", "<2>", "<3>", "<4>", "<5>", "<6>", "<7>", "<8>", "<9>" },
};

/* Window Rules */
static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     *  WM_WINDOW_ROLE(STRING) = role
     *  _NET_WM_WINDOW_TYPE(ATOM) = wintype
     */
    RULE(.wintype = WTYPE "DIALOG",  .isfloating = 1)
    RULE(.wintype = WTYPE "UTILITY", .isfloating = 1)
    RULE(.wintype = WTYPE "TOOLBAR", .isfloating = 1)
    RULE(.wintype = WTYPE "SPLASH",  .isfloating = 1)

    /* Workspace assigns */
    RULE(.class = "Firefox",          .tags = 1 << 0)
    RULE(.class = "Google-chrome",    .tags = 1 << 0)

    RULE(.class = "code",             .tags = 1 << 1)
    RULE(.class = "dev.zed.Zed",      .tags = 1 << 1)
    RULE(.class = "QtCreator",        .tags = 1 << 1)

    RULE(.class = "TelegramDesktop",  .tags = 1 << 2)
    RULE(.class = "Throne",           .tags = 1 << 2)
    RULE(.class = "Nekobox",          .tags = 1 << 2)
    RULE(.class = "GUI.for.SingBox",  .tags = 1 << 2)

    RULE(.class = "copyq",            .isfloating = 1)
};

/* Bar rules */
static const BarRule barrules[] = {
    /* monitor   bar    alignment         widthfunc                 drawfunc                clickfunc                hoverfunc                name */
    { -1,        0,     BAR_ALIGN_LEFT,   width_tags,               draw_tags,              click_tags,              hover_tags,              "tags" },
    { -1,        0,     BAR_ALIGN_LEFT,   width_ltsymbol,           draw_ltsymbol,          click_ltsymbol,          NULL,                    "layout" },
    {  0,        0,     BAR_ALIGN_RIGHT,  width_systray,            draw_systray,           click_systray,           NULL,                    "systray" },
    { statusmon, 0,     BAR_ALIGN_RIGHT,  width_status2d,           draw_status2d,          click_statuscmd,         NULL,                    "status2d" },
    { -1,        0,     BAR_ALIGN_NONE,   width_wintitle,           draw_wintitle,          click_wintitle,          NULL,                    "wintitle" },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 144;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[M]",      monocle }, /* monocle / tabbed style */
    { "><>",      NULL },    /* floating behavior */
    { "[]=",      tile },    /* tiling split */
};

/* key definitions */
#define MODKEY Mod4Mask /* Super / Logo key */
/* 
 * TAGKEYS Macro: Generates 4 fundamental workspace actions for a given number key.
 * Uses bitwise left-shifts (1 << TAG) to target the specific tag bitmask.
 *
 *   1. Super + Key            -> Focus workspace
 *   2. Super + Ctrl + Key     -> Multi-view (toggle view of workspace alongside active ones)
 *   3. Super + Shift + Key    -> Move active window to workspace
 *   4. Super + Ctrl + Shift   -> Pin active window to appear on multiple workspaces
 */
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0";
static const char *dmenucmd[] = {
    "j4-dmenu-desktop",
    "--dmenu=dmenu -i -fn 'CaskaydiaCove NerdFont-12' -nf '#cdd6f4' -sb '#89b4fa' -sf '#11111b' -p 'Apps:'",
    NULL
};
static const char *termcmd[]  = { "tilix", NULL };

/* This defines the name of the executable that handles the bar (used for signalling purposes) */
#define STATUSBAR "dwmblocks"

/* Status bar click signals (statuscmd) */
static const StatusCmd statuscmds[] = {
    { "notify-send Volume$BUTTON", 1 },
	{ "notify-send CPU$BUTTON", 2 },
	{ "notify-send Battery$BUTTON", 3 },
};

static const Key keys[] = {
    /* modifier                     key            function                argument */
    /* Core Launchers & Session Actions */
    { MODKEY,                       XK_Return,     zoom,                   {0} }, /* Promote focused window to Master */
    { MODKEY|ShiftMask,             XK_Return,     spawn,                  {.v = termcmd } }, /* Launch Terminal */
    { MODKEY,                       XK_p,          spawn,                  {.v = dmenucmd } }, /* j4-dmenu launcher */
    { MODKEY,                       XK_q,          killclient,             {0} }, /* Close active window */
    { MODKEY,                       XK_b,          togglebar,              {0} }, /* Toggle status bar */
    { MODKEY|ShiftMask,             XK_q,          quit,                   {0} }, /* Exit dwm */

    /* Custom Shortcuts & Applications */
    { MODKEY,                       XK_x,          spawn,                  SHCMD("/home/danial/.local/bin/code") },
    { MODKEY,                       XK_z,          spawn,                  SHCMD("/home/danial/.local/bin/zed") },
    { MODKEY,                       XK_c,          spawn,                  SHCMD("/opt/google/chrome/chrome") },
    { MODKEY,                       XK_r,          spawn,                  SHCMD("firefox") },
    { MODKEY|ShiftMask,             XK_r,          spawn,                  SHCMD("firefox -P mk") },
    { MODKEY,                       XK_e,          spawn,                  SHCMD("/usr/bin/mailspring") },
    { MODKEY,                       XK_t,          spawn,                  SHCMD("/home/danial/.app/Throne/Throne") },
    { MODKEY,                       XK_t,          spawn,                  SHCMD("/home/danial/.app/Throne/Throne") },
    { MODKEY,                       XK_t,          spawn,                  SHCMD("/home/danial/.app/Throne/Throne") },
    { MODKEY,                       XK_v,          spawn,                  SHCMD("copyq toggle") },
    // { MODKEY,                       XK_Escape,     spawn,                  SHCMD("i3lock -c 000000") },
    { ControlMask|Mod1Mask,         XK_Delete,     spawn,                  SHCMD("~/.local/bin/powermenu") },

    /* Focus Navigation (HJKL, Arrows, Alt+Tab) */
    { MODKEY,                       XK_h,          focusstack,             {.i = -1 } },
    { MODKEY,                       XK_l,          focusstack,             {.i = +1 } },
    { MODKEY,                       XK_j,          focusstack,             {.i = +1 } },
    { MODKEY,                       XK_k,          focusstack,             {.i = -1 } },
    { MODKEY,                       XK_Left,       focusstack,             {.i = -1 } },
    { MODKEY,                       XK_Right,      focusstack,             {.i = +1 } },
    { MODKEY,                       XK_Up,         focusstack,             {.i = -1 } },
    { MODKEY,                       XK_Down,       focusstack,             {.i = +1 } },
    { Mod1Mask,                     XK_Tab,        focusstack,             {.i = +1 } },
    { Mod1Mask|ShiftMask,           XK_Tab,        focusstack,             {.i = -1 } },
    { MODKEY,                       XK_Tab,        focusstack,             {.i = +1 } },
    { MODKEY|ShiftMask,             XK_Tab,        focusstack,             {.i = -1 } },

    /* Window Reordering in Stack (movestack) */
    { MODKEY|ShiftMask,             XK_j,          movestack,              {.i = +1 } },
    { MODKEY|ShiftMask,             XK_k,          movestack,              {.i = -1 } },
    { MODKEY|ShiftMask,             XK_Down,       movestack,              {.i = +1 } },
    { MODKEY|ShiftMask,             XK_Up,         movestack,              {.i = -1 } },

    /* Master Area & Window Resizing */
    { MODKEY|ShiftMask,             XK_h,          setmfact,               {.f = -0.05} },
    { MODKEY|ShiftMask,             XK_l,          setmfact,               {.f = +0.05} },
    { MODKEY,                       XK_i,          incnmaster,             {.i = +1 } },
    { MODKEY,                       XK_d,          incnmaster,             {.i = -1 } },

    /* Layout Toggles */
    { MODKEY|ControlMask,           XK_m,          setlayout,              {.v = &layouts[0]} }, /* Monocle */
    { MODKEY|ControlMask,           XK_f,          setlayout,              {.v = &layouts[1]} }, /* Float */
    { MODKEY|ControlMask,           XK_t,          setlayout,              {.v = &layouts[2]} }, /* Tile */
    { MODKEY|ShiftMask,             XK_space,      togglefloating,         {0} },
    { MODKEY,                       XK_F11,        togglefullscreen,       {0} },

    /* Multi-Monitor Control */
    { MODKEY,                       XK_comma,      focusmon,               {.i = -1 } },
    { MODKEY,                       XK_period,     focusmon,               {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,      tagmon,                 {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period,     tagmon,                 {.i = +1 } },

    /* Media Controls & Real-Time dwmblocks Signals */
    { 0, XF86XK_AudioMute,                         spawn,                  SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle && pkill -RTMIN+4 dwmblocks") },
    { 0, XF86XK_AudioLowerVolume,                  spawn,                  SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%- && pkill -RTMIN+4 dwmblocks") },
    { 0, XF86XK_AudioRaiseVolume,                  spawn,                  SHCMD("wpctl set-volume -l 1.0 @DEFAULT_AUDIO_SINK@ 5%+ && pkill -RTMIN+4 dwmblocks") },
    { 0, XF86XK_AudioMicMute,                      spawn,                  SHCMD("wpctl set-mute @DEFAULT_AUDIO_SOURCE@ toggle") },
    { 0, XF86XK_AudioPlay,                         spawn,                  SHCMD("playerctl play-pause") },
    { 0, XF86XK_AudioPause,                        spawn,                  SHCMD("playerctl play-pause") },
    { 0, XF86XK_AudioPrev,                         spawn,                  SHCMD("playerctl previous") },
    { 0, XF86XK_AudioNext,                         spawn,                  SHCMD("playerctl next") },
    { 0, XF86XK_AudioStop,                         spawn,                  SHCMD("playerctl stop") },
    { 0, XF86XK_MonBrightnessDown,                 spawn,                  SHCMD("brightnessctl set 5%- && pkill -RTMIN+3 dwmblocks") },
    { 0, XF86XK_MonBrightnessUp,                   spawn,                  SHCMD("brightnessctl set 5%+ && pkill -RTMIN+3 dwmblocks") },
    { 0, XK_Print,                                 spawn,                  SHCMD("grim") },

    /* Workspaces (1-9) */
    TAGKEYS(                        XK_1,                                  0)
    TAGKEYS(                        XK_2,                                  1)
    TAGKEYS(                        XK_3,                                  2)
    TAGKEYS(                        XK_4,                                  3)
    TAGKEYS(                        XK_5,                                  4)
    TAGKEYS(                        XK_6,                                  5)
    TAGKEYS(                        XK_7,                                  6)
    TAGKEYS(                        XK_8,                                  7)
    TAGKEYS(                        XK_9,                                  8)
    { MODKEY,                       XK_0,          view,                   {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,          tag,                    {.ui = ~0 } },
};

/* Button Definitions */
static const Button buttons[] = {
    /* click                event mask           button          function        argument */
    { ClkLtSymbol,          0,                   Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,                   Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,                   Button1,        togglewin,      {0} },
    { ClkWinTitle,          0,                   Button3,        showhideclient, {0} },
    { ClkWinTitle,          0,                   Button2,        zoom,           {0} },
    
    /* Clickable status bar blocks for dwmblocks */
    { ClkStatusText,        0,                   Button1,        sigstatusbar,   {.i = 1} },
    { ClkStatusText,        0,                   Button2,        sigstatusbar,   {.i = 2} },
    { ClkStatusText,        0,                   Button3,        sigstatusbar,   {.i = 3} },
    { ClkStatusText,        0,                   Button4,        sigstatusbar,   {.i = 4} },
    { ClkStatusText,        0,                   Button5,        sigstatusbar,   {.i = 5} },

    /* Client window and Tagbar interaction */
    { ClkClientWin,         MODKEY,              Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,              Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,              Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,                   Button1,        view,           {0} },
    { ClkTagBar,            0,                   Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,              Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,              Button3,        toggletag,      {0} },
};
