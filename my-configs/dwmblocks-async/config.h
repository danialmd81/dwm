#ifndef CONFIG_H
#define CONFIG_H

// String used to delimit block outputs in the status.
#define DELIMITER " "

// Maximum number of Unicode characters that a block can output.
#define MAX_BLOCK_OUTPUT_LENGTH 128

// Control whether blocks are clickable.
#define CLICKABLE_BLOCKS 1

// Control whether a leading delimiter should be prepended to the status.
#define LEADING_DELIMITER 0

// Control whether a trailing delimiter should be appended to the status.
#define TRAILING_DELIMITER 0

// Define blocks for the status feed as X(icon, cmd, interval, signal).
#define BLOCKS(X)                                                             \
    X("", "/home/danial/.local/bin/statusbar/sb-netspeed", 1, 8) /* Net */    \
    X("", "/home/danial/.local/bin/statusbar/sb-memory", 5, 6)   /* Ram */    \
    X("", "/home/danial/.local/bin/statusbar/sb-cpu", 5, 7)      /* CPU */    \
    X("", "/home/danial/.local/bin/statusbar/sb-battery", 5, 5) /* Battery */ \
    X("", "/home/danial/.local/bin/statusbar/sb-date", 0, 4)    /* Date */    \
    X("", "/home/danial/.local/bin/statusbar/sb-clock", 60, 1)  /* Clock */

#endif  // CONFIG_H

// X("", "/home/danial/.local/bin/statusbar/sb-brightness", 0, 2)      /*
// Brightness */ X("", "/home/danial/.local/bin/statusbar/sb-volume", 0, 3) /*
// Volume */ X("", "/home/danial/.local/bin/statusbar/sb-keyboard", 0, 9) /*
// Keyboard Layout */
