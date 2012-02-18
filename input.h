#ifndef INPUT_H
#define INPUT_H 1

extern char accumulator;

extern char poll_input();
char should_do_command();

// commands
#define USE_HEX 0
#define USE_BINARY 1
#define SET_HOUR 2
#define SET_MINUTES 3
#define SET_SECONDS 4
#define SET_MONTH 5
#define SET_DAY 6
#define SET_YEAR 7
#define MOVELWL 0x20// Move literal to lower nibble of accumulator
#define MOVELWH 0x40// Move literal to upper nibble of accumulator


#define RUN_COMMAND_MASK 0x80
#define MOVELWH_MASK MOVELWH
#define MOVELWL_MASK MOVELWL


#endif
