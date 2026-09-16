/* See LICENSE file for copyright and license details. */

#include <X11/X.h>
#include <X11/XF86keysym.h>
#include <X11/Xutil.h>

/* Helper macros for spawning commands */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
#define CMD(...)   { .v = (const char*[]){ __VA_ARGS__, NULL } }

/* appearance */
static const unsigned int borderpx       = 2; /* border pixel of windows */
static const unsigned int snap           = 32; /* snap pixel */
static const char autostartblocksh[]     = "autostart_blocking.sh";
static const char autostartsh[]          = "autostart.sh";
static const char dwmdir[]               = "dwm";
static const char localshare[]           = ".local/share";
static const int showbar                 = 1; /* 0 means no bar */
static const int topbar                  = 1; /* 0 means bottom bar */
static const int bar_height              = 24; /* 0 means derive from font, >= 1 explicit height */
static const int focusonwheel            = 0;
static int floatposgrid_x                = 5; /* float grid columns */
static int floatposgrid_y                = 5; /* float grid rows */

/* Status is to be shown on: -1 (all monitors), 0 (a specific monitor by index), 'A' (active monitor) */
static const int statusmon               = -1;
static const unsigned int systrayspacing = 12;   /* systray spacing */
static const int showsystray             = 1;   /* 0 means no systray */

/* alt-tab configuration */
static const unsigned int tabmodkey        = 0x40; /* (Alt) when this key is held down the alt-tab functionality stays active. Must be the same modifier as used to run alttabstart */
static const unsigned int tabcyclekey      = 0x17; /* (Tab) when this key is hit the menu moves one position forward in client stack. Must be the same key as used to run alttabstart */
static const unsigned int tabposy          = 1;    /* tab position on Y axis, 0 = top, 1 = center, 2 = bottom */
static const unsigned int tabposx          = 1;    /* tab position on X axis, 0 = left, 1 = center, 2 = right */
static const unsigned int maxwtab          = 600;  /* tab menu width */
static const unsigned int htabitem         = 40;   /* tab menu item height */

/* Indicators: see patch/bar_indicators.h for options */
static int tagindicatortype              = INDICATOR_TOP_LEFT_SQUARE;
static int tiledindicatortype            = INDICATOR_NONE;
static int floatindicatortype            = INDICATOR_TOP_LEFT_SQUARE;

/* Fonts */
static const char *fonts[] = {
    "JetBrainsMono NFM:size=11:style=Bold:antialias=true:autohint=true",
    "CaskaydiaCove NFM:size=11:style=Bold:antialias=true:autohint=true",
    "Vazirmatn:size=11:style=Bold:antialias=true:autohint=true",
    "Noto Color Emoji:size=11:antialias=true:autohint=true",
};

static char c000000[]                    = "#000000"; /* Placeholder / unused black value */

/* Palette Definitions */
/* --- Global Default / Unfocused Window Scheme --- */
static char normfgcolor[]                = "#bbbbbb"; /* Foreground (text) color for status bar and general elements */
static char normbgcolor[]                = "#222222"; /* Background color for status bar and unfocused elements */
static char normbordercolor[]            = "#555555"; /* Border color for standard unfocused tiled windows */
static char normfloatcolor[]             = "#db8fd9"; /* Border color for unfocused floating windows */

/* --- Global Selected / Focused Window Scheme --- */
static char selfgcolor[]                 = "#eeeeee"; /* Foreground (text) color for focused elements */
static char selbgcolor[]                 = "#005577"; /* Accent background color for selected items */
static char selbordercolor[]             = "#005577"; /* Border color for focused tiled windows */
static char selfloatcolor[]              = "#005577"; /* Border color for focused floating windows */

/* --- Window Title Bar (Normal / Unfocused Window) --- */
static char titlenormfgcolor[]           = "#bbbbbb"; /* Text color of inactive window title in top bar */
static char titlenormbgcolor[]           = "#222222"; /* Background color behind inactive window title in bar */
static char titlenormbordercolor[]       = "#555555"; /* Border color for inactive window title bar section */
static char titlenormfloatcolor[]        = "#db8fd9"; /* Border color for inactive floating window title section */

/* --- Window Title Bar (Selected / Focused Window) --- */
static char titleselfgcolor[]            = "#eeeeee"; /* Text color of active window title in top bar */
static char titleselbgcolor[]            = "#222222"; /* Background color behind active window title in bar */
static char titleselbordercolor[]        = "#555555"; /* Border color for active window title bar section */
static char titleselfloatcolor[]         = "#db8fd9"; /* Border color for active floating window title section */

/* --- Tag Bar (Normal / Unfocused Workspace Tags) --- */
static char tagsnormfgcolor[]            = "#bbbbbb"; /* Text/icon color for inactive workspace tags */
static char tagsnormbgcolor[]            = "#222222"; /* Background color for inactive workspace tags */
static char tagsnormbordercolor[]        = "#555555"; /* Border color around inactive workspace tags */
static char tagsnormfloatcolor[]         = "#db8fd9"; /* Alternative indicator color for inactive floating tags */

/* --- Tag Bar (Selected / Active Workspace Tags) --- */
static char tagsselfgcolor[]             = "#eeeeee"; /* Text/icon color for currently active workspace tag */
static char tagsselbgcolor[]             = "#005577"; /* Highlight background color for active workspace tag */
static char tagsselbordercolor[]         = "#555555"; /* Border color around active workspace tag */
static char tagsselfloatcolor[]          = "#db8fd9"; /* Alternative indicator color for active floating tags */

/* --- Hidden Windows (awesomebar / awesome-tab patches) --- */
static char hidnormfgcolor[]             = "#005577"; /* Text color for hidden/minimized windows (unfocused) */
static char hidselfgcolor[]              = "#227799"; /* Text color for hidden/minimized windows (focused) */
static char hidnormbgcolor[]             = "#222222"; /* Background color for hidden window entries */
static char hidselbgcolor[]              = "#222222"; /* Background color for selected hidden window entries */

/* --- Urgent Windows (Urgency Hint / WM_HINT) --- */
static char urgfgcolor[]                 = "#bbbbbb"; /* Text color for tags/windows requesting urgent attention */
static char urgbgcolor[]                 = "#222222"; /* Background color for tags/windows with urgent notifications */
static char urgbordercolor[]             = "#ff0000"; /* High-visibility border color for urgent windows */
static char urgfloatcolor[]              = "#db8fd9"; /* Border color for urgent floating windows */

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
    /* 1:Web | 2:Code | 3:Net | 4:Hub | 5:Office | 6:Downloads | 7:Design | 8:Sys | 9:Misc */
    [DEFAULT_TAGS]        = { "1:🌐", "2:💻", "3:💬", "4", "5", "6", "7", "8", "9" },
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

    RULE(.wintype = WTYPE "DIALOG",        .isfloating = 1, .floatpos = "-1x -1y")
    RULE(.wintype = WTYPE "UTILITY",       .isfloating = 1, .floatpos = "-1x -1y")
    RULE(.wintype = WTYPE "TOOLBAR",       .isfloating = 1, .floatpos = "-1x -1y")
    RULE(.wintype = WTYPE "SPLASH",        .isfloating = 1, .floatpos = "-1x -1y")

    /* Tag 1: Web (Browsers) */
    // RULE(.class = "Firefox",          .tags = 1 << 0)
    RULE(.class = "Google-chrome",    .tags = 1 << 0)

    /* Tag 2: Code (IDEs & Editors) */
    // RULE(.class = "code",             .tags = 1 << 1)
    // RULE(.class = "dev.zed.Zed",      .tags = 1 << 1)
    // RULE(.class = "QtCreator",        .tags = 1 << 1)
    // RULE(.class = "draw-io",           .tags = 1 << 1)

    /* Tag 3: Net & Chat (Mail, Messages, Proxies & VPNs) */
    // RULE(.class = "Mailspring",       .tags = 1 << 2)
    // RULE(.class = "TelegramDesktop",  .tags = 1 << 2)
    // RULE(.class = "Throne",           .tags = 1 << 2)
    // RULE(.class = "nekobox",          .tags = 1 << 2)
    // RULE(.class = "GUI.for.SingBox",  .tags = 1 << 2)
    // RULE(.class = "v2rayN",           .tags = 1 << 2)

    /* Floating Utilities */
    RULE(.class = "copyq", .isfloating = 1, .floatpos = "1400X 2X 520W 500H")
};

/* Bar rules */
static const BarRule barrules[] = {
	/* monitor   bar    alignment         widthfunc                 drawfunc                clickfunc                hoverfunc                name */
	{ statusmon, 0,     BAR_ALIGN_LEFT,   width_tags,               draw_tags,              click_tags,              hover_tags,              "tags" },
	{ statusmon, 0,     BAR_ALIGN_RIGHT,  width_systray,            draw_systray,           click_systray,           NULL,                    "systray" },
	{ statusmon, 0,     BAR_ALIGN_LEFT,   width_ltsymbol,           draw_ltsymbol,          click_ltsymbol,          NULL,                    "layout" },
	{ statusmon, 0,     BAR_ALIGN_RIGHT,  width_status2d,           draw_status2d,          click_statuscmd,         NULL,                    "status2d" },
	{ statusmon, 0,     BAR_ALIGN_NONE,   width_wintitle,           draw_wintitle,          click_wintitle,          NULL,                    "wintitle" },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

/* mouse scroll resize */
static const int scrollsensetivity = 10; /* 1 means resize window by 1 pixel for each scroll event */
/* resizemousescroll direction argument list */
static const int scrollargs[][2] = {
	/* width change         height change */
	{ -scrollsensetivity,	0 },
	{ +scrollsensetivity,	0 },
	{ 0, 				  	-scrollsensetivity },
	{ 0, 					+scrollsensetivity },
};

static const Layout layouts[] = {
	/* symbol     arrange function */
    { "[M]",      monocle }, /* monocle / tabbed style */
    { "><>",      NULL },    /* floating behavior */
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
    { MODKEY,                       KEY,      view,                 {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,           {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,                  {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,            {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0";
static const char *dmenucmd[] = { "j4-dmenu-desktop", "--dmenu=dmenu -i -p 'Apps:'", NULL };
static const char *termcmd[] = { "kitty", NULL };
static const char *lockcmd[] = { "xsecurelock", NULL };
static const char *powercmd[] = { "/home/danial/.local/bin/powermenu", NULL };
static const char *windowcmd[] = { "/home/danial/.local/bin/window_switcher", NULL };

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
    { MODKEY,                       XK_b,          togglebar,              {0} },
    { MODKEY,                       XK_j,          focusstack,             {.i = +2 } },
    { MODKEY,                       XK_k,          focusstack,             {.i = -2 } },
    // { MODKEY,                       XK_Return,     zoom,                   {0} },
    // { MODKEY,                       XK_Tab,        view,                   {0} },
    // { MODKEY,                       XK_0,          view,                   {.ui = ~0 } },
    // { MODKEY|ShiftMask,             XK_0,          tag,                    {.ui = ~0 } },
    { MODKEY,                       XK_comma,      focusmon,               {.i = -1 } },
    { MODKEY,                       XK_period,     focusmon,               {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,      tagmon,                 {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period,     tagmon,                 {.i = +1 } },
    { MODKEY,                       XK_h,          showhideclient,         {0} },
    { MODKEY|ShiftMask,             XK_q,          quit,                   {0} },
    { MODKEY,                       XK_q,          killclient,             {0} },
    
    { MODKEY,                       XK_p,          spawn,                  {.v = dmenucmd } },
    { MODKEY,                       XK_w,          spawn,                  {.v = windowcmd } },
    { MODKEY,                       XK_Return,     spawn,                  {.v = termcmd } },
    { MODKEY,                       XK_Escape,     spawn,                  {.v = lockcmd } },
    { MODKEY,                       XK_Delete,     spawn,                  {.v = powercmd } },
    { MODKEY,                       XK_e,          spawn,                  SHCMD("mailspring --password-store=\"gnome-libsecret\"") },
    { MODKEY,                       XK_f,          spawn,                  SHCMD("nautilus") },
    { MODKEY,                       XK_c,          spawn,                  SHCMD("google-chrome-stable") },
    { MODKEY,                       XK_r,          spawn,                  SHCMD("firefox") },
    { MODKEY|ControlMask,           XK_r,          spawn,                  SHCMD("firefox -P mk") },
    { MODKEY,                       XK_t,          spawn,                  SHCMD("$HOME/.app/Throne/Throne") },
    { MODKEY|ControlMask,           XK_t,          spawn,                  SHCMD("Telegram") },
    { MODKEY,                       XK_x,          spawn,                  SHCMD("code") },
    { MODKEY,                       XK_z,          spawn,                  SHCMD("$HOME/.local/bin/zed") },
    { MODKEY,                       XK_v,          spawn,                  SHCMD("copyq toggle") },
    { ShiftMask,                    XK_Print,      spawn,                  SHCMD("flameshot full -c -p $HOME/Pictures/Screenshots") },
    { 0,                            XK_Print,      spawn,                  SHCMD("flameshot gui") }, 

    { Mod1Mask,                     XK_Tab,        alttabstart,            {.i = +2 } },

    // { Mod1Mask,                     XK_Tab,        focusstack,             {.i = +2 } },
    // { Mod1Mask|ShiftMask,           XK_Tab,        focusstack,             {.i = -2 } },

    { MODKEY,                       XK_s,          setlayout,              {0} },
    { MODKEY|ControlMask|ShiftMask, XK_h,          togglehorizontalmax,    {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_l,          togglehorizontalmax,    {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_j,          toggleverticalmax,      {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_k,          toggleverticalmax,      {0} },
    { MODKEY,                       XK_m,          togglemax,              {0} },
    { MODKEY|ShiftMask,             XK_Left,       floatpos,               {.v = "-20w 0h"} },
    { MODKEY|ShiftMask,             XK_Right,      floatpos,               {.v = "20w 0h"} },
    { MODKEY|ShiftMask,             XK_Down,       floatpos,               {.v = "0w -20h"} },
    { MODKEY|ShiftMask,             XK_Up,         floatpos,               {.v = "0w 20h"} },

    /* Media Controls */
    { 0, XF86XK_AudioMute,                         spawn,                  SHCMD("notify-vol mute") },
    { 0, XF86XK_AudioLowerVolume,                  spawn,                  SHCMD("notify-vol down") },
    { 0, XF86XK_AudioRaiseVolume,                  spawn,                  SHCMD("notify-vol up") },
    { 0, XF86XK_AudioPlay,                         spawn,                  SHCMD("playerctl play-pause") },
    { 0, XF86XK_AudioPause,                        spawn,                  SHCMD("playerctl play-pause") },
    { 0, XF86XK_AudioPrev,                         spawn,                  SHCMD("playerctl previous") },
    { 0, XF86XK_AudioNext,                         spawn,                  SHCMD("playerctl next") },
    { 0, XF86XK_AudioStop,                         spawn,                  SHCMD("playerctl stop") },
    { 0, XF86XK_MonBrightnessDown,                 spawn,                  SHCMD("notify-bright down") },
    { 0, XF86XK_MonBrightnessUp,                   spawn,                  SHCMD("notify-bright up") },

    TAGKEYS(                        XK_1,                                  0)
    TAGKEYS(                        XK_2,                                  1)
    TAGKEYS(                        XK_3,                                  2)
    TAGKEYS(                        XK_4,                                  3)
    TAGKEYS(                        XK_5,                                  4)
    TAGKEYS(                        XK_6,                                  5)
    TAGKEYS(                        XK_7,                                  6)
    TAGKEYS(                        XK_8,                                  7)
    TAGKEYS(                        XK_9,                                  8)
};

/* Button Definitions */
static const Button buttons[] = {
    /* click                event mask           button          function               argument */
    { ClkLtSymbol,          0,                   Button1,        setlayout,             {0} },
    { ClkLtSymbol,          0,                   Button3,        setlayout,             {.v = &layouts[1]} },
    { ClkWinTitle,          0,                   Button1,        togglewin,             {0} },
    { ClkWinTitle,          0,                   Button3,        showhideclient,        {0} },
    { ClkWinTitle,          0,                   Button2,        zoom,                  {0} },
    
    /* Clickable status bar blocks for dwmblocks */
    { ClkStatusText,        0,                   Button1,        sigstatusbar,          {.i = 1} },
    { ClkStatusText,        0,                   Button2,        sigstatusbar,          {.i = 2} },
    { ClkStatusText,        0,                   Button3,        sigstatusbar,          {.i = 3} },
    { ClkStatusText,        0,                   Button4,        sigstatusbar,          {.i = 4} },
    { ClkStatusText,        0,                   Button5,        sigstatusbar,          {.i = 5} },

    /* Client window and Tagbar interaction */
    { ClkClientWin,         MODKEY,              Button1,        movemouse,             {0} },
    { ClkClientWin,         MODKEY,              Button2,        togglefloating,        {0} },
    { ClkClientWin,         MODKEY,              Button3,        resizemouse,           {0} },
    { ClkClientWin,         MODKEY,              Button4,        resizemousescroll,     {.v = &scrollargs[0]} },
	{ ClkClientWin,         MODKEY,              Button5,        resizemousescroll,     {.v = &scrollargs[1]} },
	{ ClkClientWin,         MODKEY,              Button6,        resizemousescroll,     {.v = &scrollargs[2]} },
	{ ClkClientWin,         MODKEY,              Button7,        resizemousescroll,     {.v = &scrollargs[3]} },
    { ClkTagBar,            0,                   Button1,        view,                  {0} },
    { ClkTagBar,            0,                   Button3,        toggleview,            {0} },
    { ClkTagBar,            MODKEY,              Button1,        tag,                   {0} },
    { ClkTagBar,            MODKEY,              Button3,        toggletag,             {0} },
};
