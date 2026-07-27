/* See LICENSE file for copyright and license details. */
// clang-format off

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
static int floatposgrid_x                = 5; /* float grid columns */
static int floatposgrid_y                = 5; /* float grid rows */

/* Status is to be shown on: -1 (all monitors), 0 (a specific monitor by index), 'A' (active monitor) */
static const int statusmon               = -1; 
static const unsigned int systrayspacing = 2; /* systray spacing */
static const int showsystray             = 1; /* 0 means no systray */

/* Indicators: see patch/bar_indicators.h for options */
static int tagindicatortype              = INDICATOR_TOP_LEFT_SQUARE;
static int tiledindicatortype            = INDICATOR_NONE;
static int floatindicatortype            = INDICATOR_TOP_LEFT_SQUARE;

/* Fonts */
static const char *fonts[] = {
    "CaskaydiaCove Nerd Font:size=12:style=Bold:antialias=true:autohint=true",
    "Vazirmatn:size=12:style=Medium:antialias=true:autohint=true",
	"JetBrainsMono Nerd Font Mono:size=12:antialias=true:autohint=true",
    "Font Awesome 5 Free Solid:size=11:antialias=true:autohint=true",
    "Noto Color Emoji:size=11:antialias=true:autohint=true"
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
    [DEFAULT_TAGS]        = { "1:🌐", "2:💻", "3:💬", "4:🔀", "5:📄", "6:📥", "7:🎨", "8:⚙️", "9:📌" },
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
    RULE(.wintype = WTYPE "DIALOG", .isfloating = 1, .floatpos = "50% 50%")
    RULE(.wintype = WTYPE "UTILITY", .isfloating = 1)
    RULE(.wintype = WTYPE "TOOLBAR", .isfloating = 1)
    RULE(.wintype = WTYPE "SPLASH",  .isfloating = 1)

    /* Tag 1: Web (Browsers) */
    RULE(.class = "Firefox",          .tags = 1 << 0)
    RULE(.class = "Google-chrome",    .tags = 1 << 0)

    /* Tag 2: Code (IDEs & Editors) */
    RULE(.class = "code",             .tags = 1 << 1)
    RULE(.class = "dev.zed.Zed",      .tags = 1 << 1)
    RULE(.class = "QtCreator",        .tags = 1 << 1)
    RULE(.class = "draw-io",           .tags = 1 << 1)

    /* Tag 3: Net & Chat (Mail, Messages, Proxies & VPNs) */
    RULE(.class = "Mailspring",       .tags = 1 << 2)
    RULE(.class = "TelegramDesktop",  .tags = 1 << 2)
    RULE(.class = "Throne",           .tags = 1 << 2)
    RULE(.class = "nekobox",          .tags = 1 << 2)
    RULE(.class = "GUI.for.SingBox",  .tags = 1 << 2)
    RULE(.class = "v2rayN",           .tags = 1 << 2)

    /* Tag 4: Hub, Media & Gaming (Files, Media Players & Games) */
    RULE(.class = "org.gnome.Nautilus",       .tags = 1 << 3)
    RULE(.class = "Top.jtmonster.jhentai",    .tags = 1 << 3)
    RULE(.class = "smplayer",                 .tags = 1 << 3)
    RULE(.class = "Audacious",                .tags = 1 << 3)
    RULE(.class = "vlc",                      .tags = 1 << 3)
    RULE(.class = "steam",                    .tags = 1 << 3)
    RULE(.class = "net.lutris.Lutris",        .tags = 1 << 3)

    /* Tag 5: Office (Documents & Productivity Tools) */
    RULE(.class = "libreoffice",                     .tags = 1 << 4)
    RULE(.class = "soffice.bin",                     .tags = 1 << 4)
    RULE(.instance = "libreoffice-startcenter",       .tags = 1 << 4)
    RULE(.instance = "libreoffice-writer",            .tags = 1 << 4)
    RULE(.instance = "libreoffice-calc",              .tags = 1 << 4)
    RULE(.instance = "libreoffice-impress",           .tags = 1 << 4)
    RULE(.instance = "libreoffice-draw",              .tags = 1 << 4)
    RULE(.instance = "libreoffice-math",              .tags = 1 << 4)
    RULE(.instance = "libreoffice-base",              .tags = 1 << 4)

    /* Tag 6: Downloads */
    RULE(.class = "com-abdownloadmanager-desktop-AppKt", .tags = 1 << 5)
    RULE(.class = "org.remmina.Remmina",                 .tags = 1 << 5)

    /* Tag 7: Design & Graphics */
    RULE(.class = "Gimp",             .tags = 1 << 6)
    RULE(.class = "Inkscape",         .tags = 1 << 6)

    /* Floating Utilities */
    RULE(.class = "copyq", .isfloating = 1, .floatpos = "600W 400H 50% 50%")
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

/* mouse scroll resize */
static const int scrollsensetivity = 30; /* 1 means resize window by 1 pixel for each scroll event */
/* resizemousescroll direction argument list */
static const int scrollargs[][2] = {
	/* width change         height change */
	{ +scrollsensetivity,	0 },
	{ -scrollsensetivity,	0 },
	{ 0, 				  	+scrollsensetivity },
	{ 0, 					-scrollsensetivity },
};

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
    "--dmenu=dmenu -i -p 'Apps:'",
    NULL
};
// static const char *termcmd[]  = { "tilix", NULL };
static const char *termcmd[]  = { "st", NULL };

static const char *slockcmd[] = { "slock", NULL };

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
    
    /* Dwm default keybindings */
    /* Core Launchers & Command Menus */
    { MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
    { MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
    
    /* Bar Visibility */
    // { MODKEY,                       XK_b,      togglebar,      {0} },

    /* Window Focus Navigation (Stack) */
    { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },

    /* Master Area Window Count */
    { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
    { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
    
    /* Master Area Size Adjustment (HJKL) */
    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },

    /* Window Operations & Workspace Toggle */
    { MODKEY,                       XK_Return, zoom,           {0} },
    { MODKEY,                       XK_Tab,    view,           {0} },
    // { MODKEY|ShiftMask,             XK_c,      killclient,     {0} },

    /* Layout Toggles */
    // { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
    // { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
    // { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
    // { MODKEY,                       XK_space,  setlayout,      {0} },

    /* Floating Window Toggle */
    { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },

    /* Tag/Workspace Operations (All Tags & Quit) */
    { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    
    /* Multi-Monitor Control */
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    
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

    /* My custom keybindings */
    /* Custom Shortcuts & Applications */
    { MODKEY,                       XK_c,      spawn,                  SHCMD("/opt/google/chrome/chrome") },
    { MODKEY,                       XK_e,      spawn,                  SHCMD("/usr/bin/mailspring") },
    { MODKEY,                       XK_f,      spawn,                  SHCMD("nautilus") },
    { MODKEY,                       XK_r,      spawn,                  SHCMD("firefox") },
    { MODKEY|ShiftMask,             XK_r,      spawn,                  SHCMD("firefox -P mk") },
    { MODKEY,                       XK_t,      spawn,                  SHCMD("/home/danial/.app/Throne/Throne") },
    { MODKEY|ShiftMask,             XK_t,      spawn,                  SHCMD("Telegram") },
    { MODKEY,                       XK_x,      spawn,                  SHCMD("/home/danial/.local/bin/code") },
    { MODKEY,                       XK_z,      spawn,                  SHCMD("/home/danial/.local/bin/zed") },
    
    /* Lock Screen */
    { MODKEY,                       XK_Escape, spawn,                  {.v = slockcmd } },

    /* Clipboard Manager Toggle */
    { MODKEY,                       XK_v,      spawn,                  SHCMD("copyq toggle") },

    /* System Power Menu (Ctrl+Alt+Del) */
    { ControlMask|Mod1Mask,         XK_Delete, spawn,                  SHCMD("~/.local/bin/powermenu") },

    /* Close Focused Window */
    { MODKEY,                       XK_q,      killclient,             {0} },

    /* Focus Navigation (Arrows, Alt+Tab) */
    { MODKEY,                       XK_Down,   focusstack,             {.i = +1 } },
    { MODKEY,                       XK_Up,     focusstack,             {.i = -1 } },
    { Mod1Mask,                     XK_Tab,    focusstack,             {.i = +1 } },
    { Mod1Mask|ShiftMask,           XK_Tab,    focusstack,             {.i = -1 } },

    /* Master Area Size Adjustment (Arrows) */
    { MODKEY,                       XK_Left,   setmfact,               {.f = -0.05} },
    { MODKEY,                       XK_Right,  setmfact,               {.f = +0.05} },

    /* Window Reordering in Stack (movestack) */
    { MODKEY|ShiftMask,             XK_j,      movestack,              {.i = +1 } },
    { MODKEY|ShiftMask,             XK_k,      movestack,              {.i = -1 } },
    { MODKEY|ShiftMask,             XK_Down,   movestack,              {.i = +1 } },
    { MODKEY|ShiftMask,             XK_Up,     movestack,              {.i = -1 } },

    /* Layout Toggles */
    { MODKEY|ControlMask,           XK_m,      setlayout,              {.v = &layouts[0]} }, /* Monocle */
    { MODKEY|ControlMask,           XK_f,      setlayout,              {.v = &layouts[1]} }, /* Float */
    { MODKEY|ControlMask,           XK_t,      setlayout,              {.v = &layouts[2]} }, /* Tile */
    { MODKEY,                       XK_F11,    togglefullscreen,       {0} },

/* Media Controls & Real-Time dwmblocks Signals */
    // { 0, XF86XK_AudioMute,                     spawn,                  SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle && pkill -RTMIN+3 dwmblocks") },
    // { 0, XF86XK_AudioMute,                     spawn,                  SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle && pkill -RTMIN+3 dwmblocks") },
    // { 0, XF86XK_AudioLowerVolume,              spawn,                  SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%- && pkill -RTMIN+3 dwmblocks") },
    // { 0, XF86XK_AudioRaiseVolume,              spawn,                  SHCMD("wpctl set-volume -l 1.0 @DEFAULT_AUDIO_SINK@ 5%+ && pkill -RTMIN+3 dwmblocks") },
    // { 0, XF86XK_AudioMicMute,                  spawn,                  SHCMD("wpctl set-mute @DEFAULT_AUDIO_SOURCE@ toggle") },
    { 0, XF86XK_AudioMute,                     spawn,                  SHCMD("vol-notify mute") },
    { 0, XF86XK_AudioLowerVolume,              spawn,                  SHCMD("vol-notify down") },
    { 0, XF86XK_AudioRaiseVolume,              spawn,                  SHCMD("vol-notify up") },
    { 0, XF86XK_AudioPlay,                     spawn,                  SHCMD("playerctl play-pause") },
    { 0, XF86XK_AudioPause,                    spawn,                  SHCMD("playerctl play-pause") },
    { 0, XF86XK_AudioPrev,                     spawn,                  SHCMD("playerctl previous") },
    { 0, XF86XK_AudioNext,                     spawn,                  SHCMD("playerctl next") },
    { 0, XF86XK_AudioStop,                     spawn,                  SHCMD("playerctl stop") },
    // { 0, XF86XK_MonBrightnessDown,             spawn,                  SHCMD("brightnessctl set 5%- && pkill -RTMIN+2 dwmblocks") },
    // { 0, XF86XK_MonBrightnessUp,               spawn,                  SHCMD("brightnessctl set 5%+ && pkill -RTMIN+2 dwmblocks") },
    { 0, XF86XK_MonBrightnessDown,             spawn,                  SHCMD("bright-notify down") },
    { 0, XF86XK_MonBrightnessUp,               spawn,                  SHCMD("bright-notify up") },
    { 0, XK_Print,                             spawn,                  SHCMD("grim") },
    { MODKEY, XK_space,                        spawn,                  SHCMD("kbd-notify") },
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
    { ClkClientWin,         MODKEY,              Button4,        resizemousescroll, {.v = &scrollargs[0]} },
	{ ClkClientWin,         MODKEY,              Button5,        resizemousescroll, {.v = &scrollargs[1]} },
	{ ClkClientWin,         MODKEY,              Button6,        resizemousescroll, {.v = &scrollargs[2]} },
	{ ClkClientWin,         MODKEY,              Button7,        resizemousescroll, {.v = &scrollargs[3]} },
    { ClkTagBar,            0,                   Button1,        view,           {0} },
    { ClkTagBar,            0,                   Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,              Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,              Button3,        toggletag,      {0} },
};
