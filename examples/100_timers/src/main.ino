#include <Stream.h>
#include "rtc_timer.h"

#define MAX_TIMERS 100

timer_cb_t callback(void* data) {
  Serial.print("Timer #"); Serial.println(*(uint16_t*)data);
}

Timer* timers[MAX_TIMERS];
uint16_t data[MAX_TIMERS];

void setup() {
  Serial.begin(115200);
  Timer::init(false, false, Serial);
  for (int i = 0; i < MAX_TIMERS; i++) {
    data[i] = i;
    timers[i] = new Timer((i + 1) * 100, false, callback, &data[i]);
    timers[i]->timerStart();
    delay(5);
  }
}

void loop() {
  Timer::timerLoop();
}
