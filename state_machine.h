#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    RGB,
    RAINBOW,
    SERIAL_COM,
    LED_OFF
} FSM_STATES_t;

typedef struct {
  FSM_STATES_t state;
  FSM_STATES_t next_state;  
} state_struct_t;

int state_executer(int state, int sensor_value);

#endif
