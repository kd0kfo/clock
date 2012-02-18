#ifndef INPUT_H
#define INPUT_H 1

extern char accumulator;

extern char poll_input();
char should_do_command();

// commands
enum {
  MOVELWH = 0/* Move literal to upper nibble of accumulator */,
  MOVELWL/* Move literal to lower nibble of accumulator */,
  USE_HEX,
  USE_BINARY,
  SET_HOUR,
  SET_MINUTES,
  SET_SECONDS,
  SET_MONTH,
  SET_DAY,
  SET_YEAR
};

#define RUN_COMMAND_MASK 0x80
#define MOVELWH_MASK 0x40
#define MOVELWL_MASK 0x20

#endif
