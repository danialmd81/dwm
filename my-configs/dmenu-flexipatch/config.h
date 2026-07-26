/* See LICENSE file for copyright and license details. */
/* Default settings; can be overriden by command line. */

// clang-format off

static int topbar = 1;                      /* -b  option; if 0, dmenu appears at bottom */
static int fuzzy = 1;                       /* -F  option; if 0, dmenu doesn't use fuzzy matching */
/* -fn option overrides fonts[0]; default X11 font or font set */
static const char *fonts[] = {
	"CaskaydiaCove Nerd Font Mono:size=12:antialias=true:autohint=true",
	"JetBrainsMono Nerd Font Mono:size=12:antialias=true:autohint=true",
	"monospace:size=10"
};
static const char *prompt      = NULL;      /* -p  option; prompt to the left of input field */

static
const char *colors[][2] = {
    /*                         fg (text)    bg (background) */
    
    /* Standard unselected items in the match list */
    [SchemeNorm]          = { "#bbbbbb",    "#222222" }, 

    /* Currently selected / focused item */
    [SchemeSel]           = { "#eeeeee",    "#005577" }, 

    /* Multi-select output highlight (when selecting multiple items via Ctrl+Enter) */
    [SchemeOut]           = { "#000000",    "#00ffff" }, 

    /* Mid-priority items or non-exact match highlights (center-patch / grid-patch) */
    [SchemeMid]           = { "#bbbbbb",    "#222222" }, 

    /* Character matching highlight color inside the SELECTED item (fuzzy / exact search) */
    [SchemeSelHighlight]  = { "#ffc978",    "#005577" }, 

    /* Character matching highlight color inside UNSELECTED items (fuzzy / exact search) */
    [SchemeNormHighlight] = { "#ffc978",    "#222222" }, 

    /* High-priority items (highlight-prompt or hp-patch for pinned/frequent entries) */
    [SchemeHp]            = { "#bbbbbb",    "#333333" }, 
};
/* -l option; if nonzero, dmenu uses vertical list with given number of lines */
static unsigned int lines      = 0;

/*
 * Characters not considered part of a word while deleting words
 * for example: " /?\"&[]"
 */
static const char worddelimiters[] = " ";

