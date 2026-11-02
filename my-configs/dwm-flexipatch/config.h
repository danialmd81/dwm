/* See LICENSE file for copyright and license details. */

#include <X11/X.h>
#include <X11/XF86keysym.h>
#include <X11/Xutil.h>

/* Helper macros for spawning commands */
#define SHCMD(cmd)                                                                                                                                                                                                                                                                                         \
  {                                                                                                                                                                                                                                                                                                        \
    .v = (const char*[])                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                      \
      "/bin/sh", "-c", cmd, NULL                                                                                                                                                                                                                                                                           \
    }                                                                                                                                                                                                                                                                                                      \
  }
#define CMD(...)                                                                                                                                                                                                                                                                                           \
  {                                                                                                                                                                                                                                                                                                        \
    .v = (const char*[])                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                      \
      __VA_ARGS__, NULL                                                                                                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                                                                                                      \
  }

/* appearance */
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const char autostartblocksh[] = "autostart_blocking.sh";
static const char autostartsh[] = "autostart.sh";
static const char dwmdir[] = "dwm";
static const char localshare[] = ".local/share";
static const int showbar = 1;    /* 0 means no bar */
static const int topbar = 1;     /* 0 means bottom bar */
static const int bar_height = 0; /* 0 means derive from font, >= 1 explicit height */
static const int focusonwheel = 0;
static int floatposgrid_x = 5; /* float grid columns */
static int floatposgrid_y = 5; /* float grid rows */
/* Status is to be shown on: -1 (all monitors), 0 (a specific monitor by index), 'A' (active monitor) */
static const int statusmon = -1;
static const unsigned int systrayspacing = 12; /* systray spacing */
static const int showsystray = 1;              /* 0 means no systray */

/* alt-tab configuration */
static const unsigned int tabmodkey = 0x40;   /* (Alt) when this key is held down the alt-tab functionality stays active. Must be the same modifier as used to run alttabstart */
static const unsigned int tabcyclekey = 0x17; /* (Tab) when this key is hit the menu moves one position forward in client stack. Must be the same key as used to run alttabstart */
static const unsigned int tabposy = 1;        /* tab position on Y axis, 0 = top, 1 = center, 2 = bottom */
static const unsigned int tabposx = 1;        /* tab position on X axis, 0 = left, 1 = center, 2 = right */
static const unsigned int maxwtab = 600;      /* tab menu width */
static const unsigned int htabitem = 40;      /* tab menu item height */

/* Indicators: see patch/bar_indicators.h for options */
static int tagindicatortype = INDICATOR_TOP_LEFT_SQUARE;
static int tiledindicatortype = INDICATOR_NONE;
static int floatindicatortype = INDICATOR_TOP_LEFT_SQUARE;

/* Fonts */
static const char* fonts[] = {
  "JetBrainsMono NFM:size=11:style=Bold:antialias=true:autohint=true",
  "CaskaydiaCove NFM:size=11:style=Bold:antialias=true:autohint=true",
  "Noto Color Emoji:size=11:style=Bold:antialias=true:autohint=true",
  "Vazirmatn:size=11:style=Bold:antialias=true:autohint=true",
};
static const char dmenufont[] = "JetBrainsMono NFM:size=11:style=Bold:antialias=true:autohint=true";

static char c000000[] = "#000000"; // placeholder value

static char normfgcolor[] = "#bbbbbb";
static char normbgcolor[] = "#222222";
static char normbordercolor[] = "#444444";
static char normfloatcolor[] = "#db8fd9";

static char selfgcolor[] = "#eeeeee";
static char selbgcolor[] = "#005577";
static char selbordercolor[] = "#005577";
static char selfloatcolor[] = "#005577";

static char titlenormfgcolor[] = "#bbbbbb";
static char titlenormbgcolor[] = "#222222";
static char titlenormbordercolor[] = "#444444";
static char titlenormfloatcolor[] = "#db8fd9";

static char titleselfgcolor[] = "#eeeeee";
static char titleselbgcolor[] = "#005577";
static char titleselbordercolor[] = "#005577";
static char titleselfloatcolor[] = "#005577";

static char tagsnormfgcolor[] = "#bbbbbb";
static char tagsnormbgcolor[] = "#222222";
static char tagsnormbordercolor[] = "#444444";
static char tagsnormfloatcolor[] = "#db8fd9";

static char tagsselfgcolor[] = "#eeeeee";
static char tagsselbgcolor[] = "#005577";
static char tagsselbordercolor[] = "#005577";
static char tagsselfloatcolor[] = "#005577";

static char hidnormfgcolor[] = "#005577";
static char hidselfgcolor[] = "#227799";
static char hidnormbgcolor[] = "#222222";
static char hidselbgcolor[] = "#222222";

static char urgfgcolor[] = "#bbbbbb";
static char urgbgcolor[] = "#222222";
static char urgbordercolor[] = "#ff0000";
static char urgfloatcolor[] = "#db8fd9";

static char normTTBbgcolor[] = "#330000";
static char normLTRbgcolor[] = "#330033";
static char normMONObgcolor[] = "#000033";
static char normGRIDbgcolor[] = "#003300";
static char normGRD1bgcolor[] = "#003300";
static char normGRD2bgcolor[] = "#003300";
static char normGRDMbgcolor[] = "#506600";
static char normHGRDbgcolor[] = "#b96600";
static char normDWDLbgcolor[] = "#003333";
static char normSPRLbgcolor[] = "#333300";
static char normfloatbgcolor[] = "#115577";
static char actTTBbgcolor[] = "#440000";
static char actLTRbgcolor[] = "#440044";
static char actMONObgcolor[] = "#000044";
static char actGRIDbgcolor[] = "#004400";
static char actGRD1bgcolor[] = "#004400";
static char actGRD2bgcolor[] = "#004400";
static char actGRDMbgcolor[] = "#507711";
static char actHGRDbgcolor[] = "#b97711";
static char actDWDLbgcolor[] = "#004444";
static char actSPRLbgcolor[] = "#444400";
static char actfloatbgcolor[] = "#116688";
static char selTTBbgcolor[] = "#550000";
static char selLTRbgcolor[] = "#550055";
static char selMONObgcolor[] = "#212171";
static char selGRIDbgcolor[] = "#005500";
static char selGRD1bgcolor[] = "#005500";
static char selGRD2bgcolor[] = "#005500";
static char selGRDMbgcolor[] = "#508822";
static char selHGRDbgcolor[] = "#b98822";
static char selDWDLbgcolor[] = "#005555";
static char selSPRLbgcolor[] = "#555500";
static char selfloatbgcolor[] = "#117799";

static char* colors[][ColCount] = {
  /*                       fg                bg                border                float */
  [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor, normfloatcolor},
  [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor, selfloatcolor},
  [SchemeTitleNorm] = {titlenormfgcolor, titlenormbgcolor, titlenormbordercolor, titlenormfloatcolor},
  [SchemeTitleSel] = {titleselfgcolor, titleselbgcolor, titleselbordercolor, titleselfloatcolor},
  [SchemeTagsNorm] = {tagsnormfgcolor, tagsnormbgcolor, tagsnormbordercolor, tagsnormfloatcolor},
  [SchemeTagsSel] = {tagsselfgcolor, tagsselbgcolor, tagsselbordercolor, tagsselfloatcolor},
  [SchemeHidNorm] = {hidnormfgcolor, hidnormbgcolor, c000000, c000000},
  [SchemeHidSel] = {hidselfgcolor, hidselbgcolor, c000000, c000000},
  [SchemeUrg] = {urgfgcolor, urgbgcolor, urgbordercolor, urgfloatcolor},
  [SchemeFlexActTTB] = {titleselfgcolor, actTTBbgcolor, actTTBbgcolor, c000000},
  [SchemeFlexActLTR] = {titleselfgcolor, actLTRbgcolor, actLTRbgcolor, c000000},
  [SchemeFlexActMONO] = {titleselfgcolor, actMONObgcolor, actMONObgcolor, c000000},
  [SchemeFlexActGRID] = {titleselfgcolor, actGRIDbgcolor, actGRIDbgcolor, c000000},
  [SchemeFlexActGRD1] = {titleselfgcolor, actGRD1bgcolor, actGRD1bgcolor, c000000},
  [SchemeFlexActGRD2] = {titleselfgcolor, actGRD2bgcolor, actGRD2bgcolor, c000000},
  [SchemeFlexActGRDM] = {titleselfgcolor, actGRDMbgcolor, actGRDMbgcolor, c000000},
  [SchemeFlexActHGRD] = {titleselfgcolor, actHGRDbgcolor, actHGRDbgcolor, c000000},
  [SchemeFlexActDWDL] = {titleselfgcolor, actDWDLbgcolor, actDWDLbgcolor, c000000},
  [SchemeFlexActSPRL] = {titleselfgcolor, actSPRLbgcolor, actSPRLbgcolor, c000000},
  [SchemeFlexActFloat] = {titleselfgcolor, actfloatbgcolor, actfloatbgcolor, c000000},
  [SchemeFlexInaTTB] = {titlenormfgcolor, normTTBbgcolor, normTTBbgcolor, c000000},
  [SchemeFlexInaLTR] = {titlenormfgcolor, normLTRbgcolor, normLTRbgcolor, c000000},
  [SchemeFlexInaMONO] = {titlenormfgcolor, normMONObgcolor, normMONObgcolor, c000000},
  [SchemeFlexInaGRID] = {titlenormfgcolor, normGRIDbgcolor, normGRIDbgcolor, c000000},
  [SchemeFlexInaGRD1] = {titlenormfgcolor, normGRD1bgcolor, normGRD1bgcolor, c000000},
  [SchemeFlexInaGRD2] = {titlenormfgcolor, normGRD2bgcolor, normGRD2bgcolor, c000000},
  [SchemeFlexInaGRDM] = {titlenormfgcolor, normGRDMbgcolor, normGRDMbgcolor, c000000},
  [SchemeFlexInaHGRD] = {titlenormfgcolor, normHGRDbgcolor, normHGRDbgcolor, c000000},
  [SchemeFlexInaDWDL] = {titlenormfgcolor, normDWDLbgcolor, normDWDLbgcolor, c000000},
  [SchemeFlexInaSPRL] = {titlenormfgcolor, normSPRLbgcolor, normSPRLbgcolor, c000000},
  [SchemeFlexInaFloat] = {titlenormfgcolor, normfloatbgcolor, normfloatbgcolor, c000000},
  [SchemeFlexSelTTB] = {titleselfgcolor, selTTBbgcolor, selTTBbgcolor, c000000},
  [SchemeFlexSelLTR] = {titleselfgcolor, selLTRbgcolor, selLTRbgcolor, c000000},
  [SchemeFlexSelMONO] = {titleselfgcolor, selMONObgcolor, selMONObgcolor, c000000},
  [SchemeFlexSelGRID] = {titleselfgcolor, selGRIDbgcolor, selGRIDbgcolor, c000000},
  [SchemeFlexSelGRD1] = {titleselfgcolor, selGRD1bgcolor, selGRD1bgcolor, c000000},
  [SchemeFlexSelGRD2] = {titleselfgcolor, selGRD2bgcolor, selGRD2bgcolor, c000000},
  [SchemeFlexSelGRDM] = {titleselfgcolor, selGRDMbgcolor, selGRDMbgcolor, c000000},
  [SchemeFlexSelHGRD] = {titleselfgcolor, selHGRDbgcolor, selHGRDbgcolor, c000000},
  [SchemeFlexSelDWDL] = {titleselfgcolor, selDWDLbgcolor, selDWDLbgcolor, c000000},
  [SchemeFlexSelSPRL] = {titleselfgcolor, selSPRLbgcolor, selSPRLbgcolor, c000000},
  [SchemeFlexSelFloat] = {titleselfgcolor, selfloatbgcolor, selfloatbgcolor, c000000},
};

/* Tags
 * In a traditional dwm the number of tags in use can be changed simply by changing the number
 * of strings in the tags array. This build does things a bit different which has some added
 * benefits. If you need to change the number of tags here then change the NUMTAGS macro in dwm.c.
 *
 * Examples:
 *
 *  1) static char *tagicons[][NUMTAGS*2] = {
 *         [DEFAULT_TAGS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I" },
 *     }
 *
 *  2) static char *tagicons[][1] = {
 *         [DEFAULT_TAGS] = { "•" },
 *     }
 *
 * The first example would result in the tags on the first monitor to be 1 through 9, while the
 * tags for the second monitor would be named A through I. A third monitor would start again at
 * 1 through 9 while the tags on a fourth monitor would also be named A through I. Note the tags
 * count of NUMTAGS*2 in the array initialiser which defines how many tag text / icon exists in
 * the array. This can be changed to *3 to add separate icons for a third monitor.
 *
 * For the second example each tag would be represented as a bullet point. Both cases work the
 * same from a technical standpoint - the icon index is derived from the tag index and the monitor
 * index. If the icon index is is greater than the number of tag icons then it will wrap around
 * until it an icon matches. Similarly if there are two tag icons then it would alternate between
 * them. This works seamlessly with alternative tags and alttagsdecoration patches.
 */
static char* tagicons[][NUMTAGS] = {
  /* 1:Web | 2:Code | 3:Net | 4:Hub | 5:Office | 6:Downloads | 7:Design | 8:Sys | 9:Misc */
  [DEFAULT_TAGS] = {"1:🌐", "2:💻", "3:💬", "4", "5", "6", "7", "8", "9"},
  [ALTERNATIVE_TAGS] = {"A", "B", "C", "D", "E", "F", "G", "H", "I"},
  [ALT_TAGS_DECORATION] = {"<1>", "<2>", "<3>", "<4>", "<5>", "<6>", "<7>", "<8>", "<9>"},
};

/* Window Rules */
static const Rule rules[] = {
  /* xprop(1):
   *	WM_CLASS(STRING) = instance, class
   *	WM_NAME(STRING) = title
   *	WM_WINDOW_ROLE(STRING) = role
   *	_NET_WM_WINDOW_TYPE(ATOM) = wintype
   */

  RULE(.wintype = WTYPE "DIALOG", .isfloating = 1, .floatpos = "-2x -2y") RULE(.wintype = WTYPE "UTILITY", .isfloating = 1, .floatpos = "-2x -2y") RULE(.wintype = WTYPE "TOOLBAR", .isfloating = 1, .floatpos = "-2x -2y") RULE(.wintype = WTYPE "SPLASH", .isfloating = 1, .floatpos = "-2x -2y")

  /* Tag 1: Web (Browsers) */
  // RULE(.class = "Firefox",          .tags = 1 << 0)
  // RULE(.class = "Google-chrome",    .tags = 1 << 0)

  /* Tag 2: Code (IDEs & Editors) */
  // RULE(.class = "code",             .tags = 1 << 1)
  // RULE(.class = "dev.zed.Zed",      .tags = 1 << 1)
  // RULE(.class = "QtCreator",        .tags = 1 << 1)
  // RULE(.class = "draw-io",          .tags = 1 << 1)

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
  {statusmon, 0, BAR_ALIGN_LEFT, width_tags, draw_tags, click_tags, hover_tags, "tags"},
  {statusmon, 0, BAR_ALIGN_RIGHT, width_systray, draw_systray, click_systray, NULL, "systray"},
  {statusmon, 0, BAR_ALIGN_LEFT, width_ltsymbol, draw_ltsymbol, click_ltsymbol, NULL, "layout"},
  {statusmon, 0, BAR_ALIGN_RIGHT, width_status2d, draw_status2d, click_statuscmd, NULL, "status2d"},
  {statusmon, 0, BAR_ALIGN_NONE, width_flexwintitle, draw_flexwintitle, click_flexwintitle, NULL, "flexwintitle"},
};

/* layout(s) */
static const float mfact = 0.55;     /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;        /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

/* mouse scroll resize */
static const int scrollsensetivity = 10; /* 1 means resize window by 1 pixel for each scroll event */
/* resizemousescroll direction argument list */
static const int scrollargs[][2] = {
  /* width change         height change */
  {+scrollsensetivity, 0},
  {-scrollsensetivity, 0},
  {0, +scrollsensetivity},
  {0, -scrollsensetivity},
};

static const Layout layouts[] = {
  /* symbol     arrange function */
  {"[M]", monocle}, /* first entry is default */
  {"><>", NULL},    /* no layout function means floating behavior */
  {"[]=", tile},
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
#define TAGKEYS(KEY, TAG) {MODKEY, KEY, view, {.ui = 1 << TAG}}, {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}}, {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* commands */
static char dmenumon[2] = "0";
static const char* dmenucmd[] = { "j4-dmenu-desktop", "--dmenu=dmenu -i -p 'Apps:'", NULL };
static const char* termcmd[] = {"tilix", NULL};
// static const char* lockcmd[] = {"xsecurelock", NULL};
static const char* powercmd[] = {"/home/danial/.local/bin/powermenu", NULL};
static const char* windowcmd[] = {"/home/danial/.local/bin/window_switcher", NULL};

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
  {MODKEY, XK_b, togglebar, {0}},
  {MODKEY, XK_j, focusstack, {.i = +2}},
  {MODKEY, XK_k, focusstack, {.i = -2}},
  {MODKEY, XK_comma, focusmon, {.i = -1}},
  {MODKEY, XK_period, focusmon, {.i = +1}},
  {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
  {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
  {MODKEY, XK_h, showhideclient, {0}},
  {MODKEY | ShiftMask, XK_q, quit, {0}},
  {MODKEY, XK_q, killclient, {0}},

  {MODKEY, XK_p, spawn, {.v = dmenucmd}},
  {MODKEY, XK_w, spawn, {.v = windowcmd}},
  {MODKEY, XK_Return, spawn, {.v = termcmd}},
  {MODKEY, XK_Delete, spawn, {.v = powercmd}},
  {MODKEY, XK_e, spawn, SHCMD("mailspring")},
  {MODKEY, XK_f, spawn, SHCMD("nautilus")},
  {MODKEY, XK_c, spawn, SHCMD("google-chrome-stable")},
  {MODKEY, XK_r, spawn, SHCMD("firefox")},
  {MODKEY | ControlMask, XK_r, spawn, SHCMD("firefox -P mk")},
  {MODKEY, XK_t, spawn, SHCMD("$HOME/.app/Throne/Throne")},
  {MODKEY | ControlMask, XK_t, spawn, SHCMD("Telegram")},
  {MODKEY, XK_x, spawn, SHCMD("code")},
  {MODKEY, XK_z, spawn, SHCMD("$HOME/.local/bin/zed")},
  {MODKEY, XK_v, spawn, SHCMD("copyq toggle")},
  {ShiftMask, XK_Print, spawn, SHCMD("flameshot full -c -p $HOME/Pictures/Screenshots")},
  {0, XK_Print, spawn, SHCMD("flameshot gui")},

  {Mod1Mask, XK_Tab, alttabstart, {.i = +2}},

  {MODKEY, XK_s, setlayout, {0}},
  {MODKEY | ControlMask | ShiftMask, XK_h, togglehorizontalmax, {0}},
  {MODKEY | ControlMask | ShiftMask, XK_l, togglehorizontalmax, {0}},
  {MODKEY | ControlMask | ShiftMask, XK_j, toggleverticalmax, {0}},
  {MODKEY | ControlMask | ShiftMask, XK_k, toggleverticalmax, {0}},
  {MODKEY, XK_m, togglemax, {0}},
  {MODKEY | ShiftMask, XK_Left, floatpos, {.v = "-20w 0h"}},
  {MODKEY | ShiftMask, XK_Right, floatpos, {.v = "20w 0h"}},
  {MODKEY | ShiftMask, XK_Down, floatpos, {.v = "0w -20h"}},
  {MODKEY | ShiftMask, XK_Up, floatpos, {.v = "0w 20h"}},

  /* Media Controls */
  {0, XF86XK_AudioMute, spawn, SHCMD("notify-vol mute")},
  {0, XF86XK_AudioLowerVolume, spawn, SHCMD("notify-vol down")},
  {0, XF86XK_AudioRaiseVolume, spawn, SHCMD("notify-vol up")},
  {0, XF86XK_AudioPlay, spawn, SHCMD("playerctl play-pause")},
  {0, XF86XK_AudioPause, spawn, SHCMD("playerctl play-pause")},
  {0, XF86XK_AudioPrev, spawn, SHCMD("playerctl previous")},
  {0, XF86XK_AudioNext, spawn, SHCMD("playerctl next")},
  {0, XF86XK_AudioStop, spawn, SHCMD("playerctl stop")},
  {0, XF86XK_MonBrightnessDown, spawn, SHCMD("notify-bright down")},
  {0, XF86XK_MonBrightnessUp, spawn, SHCMD("notify-bright up")},

  TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3) TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7) TAGKEYS(XK_9, 8)
};

/* Button Definitions */
static const Button buttons[] = {
  /* click                event mask           button          function        argument */
  {ClkLtSymbol, 0, Button1, setlayout, {0}},
  {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[1]}},
  {ClkWinTitle, 0, Button1, togglewin, {0}},
  {ClkWinTitle, 0, Button3, showhideclient, {0}},
  {ClkWinTitle, 0, Button2, zoom, {0}},

  /* Clickable status bar blocks for dwmblocks */
  {ClkStatusText, 0, Button1, sigstatusbar, {.i = 1}},
  {ClkStatusText, 0, Button2, sigstatusbar, {.i = 2}},
  {ClkStatusText, 0, Button3, sigstatusbar, {.i = 3}},
  {ClkStatusText, 0, Button4, sigstatusbar, {.i = 4}},
  {ClkStatusText, 0, Button5, sigstatusbar, {.i = 5}},

  /* Client window and Tagbar interaction */
  {ClkClientWin, MODKEY, Button1, movemouse, {0}},
  {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
  {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
  {ClkClientWin, MODKEY, Button4, resizemousescroll, {.v = &scrollargs[0]}},
  {ClkClientWin, MODKEY, Button5, resizemousescroll, {.v = &scrollargs[1]}},
  {ClkClientWin, MODKEY, Button6, resizemousescroll, {.v = &scrollargs[2]}},
  {ClkClientWin, MODKEY, Button7, resizemousescroll, {.v = &scrollargs[3]}},
  {ClkTagBar, 0, Button1, view, {0}},
  {ClkTagBar, 0, Button3, toggleview, {0}},
  {ClkTagBar, MODKEY, Button1, tag, {0}},
  {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};

