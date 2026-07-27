/* user and group to drop privileges to */
#include <stddef.h>

static const char *user  = "nobody";
static const char *group = "nogroup"; /* Changed for Void Linux */

static const char *colorname[NUMCOLS] = {
    [INIT]   = "#000000",   /* after initialization (solid black) */
    [INPUT]  = "#005577",   /* during input (dwm blue) */
    [FAILED] = "#CC3333",   /* wrong password (red) */
    [CAPS]   = "#E5C07B",   /* CapsLock on (amber/yellow) */
};

/* default message */
static const char * message = "Type password to unlock...";

/* text color */
static const char * text_color = "#ffffff";

/* text size (X11 bitmap font: "fixed", "6x10", "9x15") */
static const char * font_name = "9x15";

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 1;

/* time in seconds before the monitor shuts down (DPMS) */
static int monitortime = 5;
