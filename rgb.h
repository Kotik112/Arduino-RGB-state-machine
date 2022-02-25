#ifndef RGB_H
#define RGB_H


#define redPin 11
#define greenPin 10
#define bluePin 9

typedef enum {
    RED,
    GREEN,
    BLUE,
    OFF
} LED_MODE_t;

typedef enum {
    RED_GREEN,
    GREEN_BLUE,
    BLUE_RED
} FADE_STATE_t;

typedef struct {
  int r;
  int g;
  int b;
} rgb_t;

#endif
