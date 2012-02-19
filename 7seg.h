#ifndef SEG7_H
#define SEG7_H 1

extern const char table_7seg[];
enum {LEFT = 0,RIGHT};
enum {BINARY = 0, SEG7};
#define BINARY_SIDE_MASK 0x80// If this bit is set, top is used, otherwise bottom is use.
#define SEG7_LEFT_INH_MASK 0x80// If this bit is set, the left display is inhibited
#define SEG7_RIGHT_INH_MASK 0x10// If this bit is set, the right display is inhibited
#define DISPLAY_TYPE_MASK 0x40// If this bit is set, binary is used, otherwise 7seg is used.

void clock_set_display(char display);
char clock_get_display();

void clock_set_digit(char display);
char clock_get_digit();

void clock_set_display_side(char display);
char clock_get_display_side();

void clock_write_double_digit(const char *digits);

void update_display(char side, char val);

#endif

