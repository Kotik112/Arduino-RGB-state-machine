#define key1 8
#define key2 12
#define key1_interrupt 2


#include "rgb.h"
#include "timer_struct.h"
#include "state_machine.h"
#include <ctype.h>

//0 = RED, 1 = GREEN, 2 = BLUE, 3, OFF
int rgb_state = 0;

//keeps track of button states to check for changes
int last_key1_state = LOW;
int last_key2_state = LOW;

rgb_t rgb_values = {0,255,255};  //Default RED

timer_t debouncer = {millis(), DEBOUNCER};  //Handles debouncing
timer_t fade_delay = {millis(), 20UL}; // 20 ms is default
timer_t test = {millis(), 300UL};

//Rainbow mode variables
int fade_counter = 0;
int fade_state = RED_GREEN;

//Machine state
int machine_state = 0;
state_struct_t machine_state1 = {RGB, RAINBOW}; //Default value

int sensor_value;
int last_sensor_value;

void setup() {
  Serial.begin(9600);

  pinMode(key1, INPUT);
  pinMode(key2, INPUT);
  pinMode(redPin, INPUT);
  pinMode(greenPin, INPUT);
  pinMode(bluePin, INPUT);
}


void loop() {
  
  if(potentiometer_change()) {
    sensor_value = analogRead(A0);
  }
  
  if (!Serial.available()) {
    key1_function(sensor_value);
    key2_function(sensor_value);
    state_executer(machine_state, sensor_value);
  }
  if (Serial.available()) {
  menu_handler();
  //state_executer(machine_state, sensor_value);
  }
  
}

bool potentiometer_change() {
  int sensor_now = analogRead(A0);
  if (last_sensor_value != sensor_now || last_sensor_value != sensor_now+1 || last_sensor_value != sensor_now-1) {
    Serial.println("True");
    return true;
  }
  last_sensor_value = sensor_now;
  return false;
}

void key1_function(int sensor_value) {
  int key_state = digitalRead(key1);
  if (time_checker(&debouncer) && key_state != last_key1_state) {
    last_key1_state = key_state;
    if(key_state == HIGH) {     
      rgb_state++;
      Serial.println(rgb_state);
      if(rgb_state > 2) {
        rgb_state = 0;
        machine_state = 0;
      }
      
    }
    else if (key_state == LOW) {
      //Serial.println("Button 1 Released");
    }
  }
}

void key2_function(int sensor_value) {
  int key_state = digitalRead(key2);
  if (time_checker(&debouncer)) {
    if( key_state != last_key2_state) {
      last_key2_state = key_state;
      if(key_state == HIGH) {
        machine_state = RAINBOW;
        //Serial.println("Button 2 Pressed");
      }
      else {
        //Serial.println("Button 2 Released");
        
      }
    }
  }
}

bool time_checker(timer_t* time_struct) {
  unsigned long now = millis();
  if (now - time_struct->last_event >= time_struct->interval) {
    time_struct->last_event = now;
    return true;
  }
  return false;
}

void change_rgb(const int sensor_value) {
  int brightness = sensor_value / 4;
  switch(rgb_state){
    case RED:
      analogWrite(redPin, brightness);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 255);
      break;

     case GREEN:
      analogWrite(redPin, 255);
      analogWrite(greenPin, brightness);
      analogWrite(bluePin, 255);
      break;

     case BLUE:
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, brightness);
      break;

     case OFF:
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 255);
      break;

    default:
      rgb_state = 0;
  }
}

void rgb(rgb_t rgb_values) {
  analogWrite(redPin, rgb_values.r);
  analogWrite(greenPin, rgb_values.g);
  analogWrite(bluePin, rgb_values.b);
}

void rainbow(const int sensor_value) {
  //FOR DEBUGGING PURPOSES
  /*if(time_checker(&test)) {
    Serial.println("RGB");
    Serial.println(rgb_values.r);
    Serial.println(rgb_values.g);
    Serial.println(rgb_values.b);
  }*/

  int rainbow_speed = sensor_value / 25;
  fade_delay.interval = rainbow_speed;
  if(time_checker(&fade_delay) && fade_counter < 255) {
    if(fade_state == RED_GREEN) {
      //Serial.println(rgb_values.r);
      rgb_values.r--;
      rgb_values.g++;
      analogWrite(redPin, 255 - rgb_values.r);
      analogWrite(greenPin, 255 - rgb_values.g);
      analogWrite(bluePin, 255);
      fade_counter++;
      if (fade_counter == 255) {
        fade_counter = 0;
        fade_state = GREEN_BLUE;
      }
    }
    if (fade_state == GREEN_BLUE) {
      rgb_values.g--;
      rgb_values.b++;
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255 - rgb_values.g);
      analogWrite(bluePin, 255 - rgb_values.b);
      fade_counter++;
      if (fade_counter == 255) {
        fade_counter = 0;
        fade_state = BLUE_RED;
      }
    }
    if (fade_state == BLUE_RED) {
      rgb_values.b--;
      rgb_values.r++;
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255 - rgb_values.g);
      analogWrite(bluePin, 255 - rgb_values.b);
      fade_counter++;
      if (fade_counter == 255) {
        fade_counter = 0;
        fade_state = RED_GREEN;
      }
    }
  }
}

int state_executer(int state, int sensor_value) {
    switch (state) {
      case RGB:
        change_rgb(sensor_value);
        break;

      case RAINBOW:
        rainbow(sensor_value);
        break;

      default:
        machine_state = 0;
    }
}

void print_menu(void) {
  Serial.println("|****************************|");
  Serial.println("|**|Ardnoid Configuration |**|");
  Serial.println("|**|        Menu          |**|");
  Serial.println("|****************************|");
  Serial.println("");
  Serial.println("Type one of the following options:");
  Serial.println("red / green / blue / rainbow or off");
  Serial.println("red / green / blue / rainbow or off.");
  Serial.println("To use the potentiometer, use 'pot' followed by a value (0-9).");
  Serial.println("For example 'pot1' or 'pot9'.");
}

void menu_handler() {
  Serial.println("Type 'menu' for available commands:");
    
    String menu_choice = Serial.readString();
    if(menu_choice.compareTo("red") == 0) {
      Serial.println("LED light RED");
      rgb_state = RED;
      machine_state = RGB;
    }
    else if(menu_choice.compareTo("green") == 0) {
      Serial.println("LED light GREEN");
      rgb_state = GREEN;
      machine_state = RGB;
    }
    else if(menu_choice.compareTo("blue") == 0) {
      Serial.println(" LED light BLUE");
      rgb_state = BLUE;
      machine_state = RGB;
    }
    else if(menu_choice.compareTo("rainbow") == 0) {
      Serial.println("RAINBOW MODE ON");
      machine_state = RAINBOW;
    }
    else if(menu_choice.compareTo("off") == 0) {
      rgb_state = 3;
      machine_state = 0;
    }
    else if(menu_choice.compareTo("menu") == 0) {
      print_menu();
    }
    else if(menu_choice[0] == 'p' && menu_choice[1] == 'o' && menu_choice[2] == 't') {
      char buffer[15]; 
      menu_choice.toCharArray(buffer, 15);
      char value = buffer[3];
      
      if(!isDigit(value)) {
        Serial.println("Value must be numerical.");
      }
      else {
        sensor_value = (value - '0') * 10; //multiply by 10 to incease the difference
        Serial.println(sensor_value);
      }

    }
 }
  
