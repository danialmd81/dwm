#ifndef CONFIG_H
#define CONFIG_H

// String used to delimit block outputs in the status.
#define DELIMITER "  "

// Maximum number of Unicode characters that a block can output.
#define MAX_BLOCK_OUTPUT_LENGTH 45

// Control whether blocks are clickable.
#define CLICKABLE_BLOCKS 1

// Control whether a leading delimiter should be prepended to the status.
#define LEADING_DELIMITER 0

// Control whether a trailing delimiter should be appended to the status.
#define TRAILING_DELIMITER 0

// Define blocks for the status feed as X(icon, cmd, interval, signal).
#define BLOCKS(X)                                                              \
  X("", "$HOME/.local/bin/statusbar/sb-nettraf", 1, 1)    /* Net */            \
  X("", "$HOME/.local/bin/statusbar/sb-cpu", 5, 2)        /* CPU */            \
  X("", "$HOME/.local/bin/statusbar/sb-memory", 5, 3)     /* Ram */            \
  X("", "$HOME/.local/bin/statusbar/sb-disk", 30, 4)      /* Disk */           \
  X("", "$HOME/.local/bin/statusbar/sb-volume", 0, 5)     /* Volume (Sig) */   \
  X("", "$HOME/.local/bin/statusbar/sb-brightness", 0, 6) /* Brightness */     \
  X("", "$HOME/.local/bin/statusbar/sb-battery", 5, 7)    /* Battery */        \
  X("", "$HOME/.local/bin/statusbar/sb-clock", 30, 8)     /* Clock */

#endif // CONFIG_H
