#ifndef SEG7_H
#define SEG7_H 1

extern const char table_7seg[];
enum {LEFT = 0,RIGHT};
enum {BINARY = 0, SEG7};
#define DIGIT_MASK 0x80// If this bit is set, left is used, otherwise right is use.
#define DISPLAY_SIDE_MASK 0x20// If this bit is set, the left display is used ottherwise right is used
#define DISPLAY_TYPE_MASK 0x40// If this bit is set, binary is used, otherwise 7seg is used.

void clock_set_display(char display);
char clock_get_display();

void clock_set_digit(char display);
char clock_get_digit();

void clock_set_display_side(char display);
char clock_get_display_side();

void clock_write_double_digit(const char *digits);

#endif

