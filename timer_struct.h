#ifndef TIMER_H
#define TIMER_H

#define DEBOUNCER 20

typedef struct {
  unsigned long last_event;
  unsigned long interval;
} timer_t;

#endif
