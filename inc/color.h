#ifndef _COLOR_H_
#define _COLOR_H_

#define ESC(c) "\e[" #c

#define FG_RESET        ESC(0m)
#define FG_BLACK	ESC(30m)
#define FG_RED		ESC(31m)
#define FG_GREEN	ESC(32m)
#define FG_YELLOW	ESC(33m)
#define FG_BLUE		ESC(34m)
#define FG_MAGENTA	ESC(35m)
#define FG_CYAN		ESC(36m)
#define FG_WHITE	ESC(37m)

#define SP_RESET	ESC(0m)
#define SP_BLINK	ESC(5m)
#define SP_BOLD		ESC(1m)

#endif
