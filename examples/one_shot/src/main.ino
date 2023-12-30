#include <Stream.h>
#include "rtc_timer.h"

timer_cb_t blinkerCb(void*);
timer_cb_t blinkerSlowCb(void*);
timer_cb_t oneShotCb(void*);

Timer blinker(100, false, blinkerCb), blinkerSlow(500, false, blinkerSlowCb),
    oneShot(5000, true, oneShotCb);

timer_cb_t blinkerSlowCb(void* data) {
  static bool state = LOW;
  digitalWrite(PIN_PD6, state);
  state = !state;
}

timer_cb_t oneShotCb(void* data) {
  blinker.timerStart();
}

timer_cb_t blinkerCb(void* data) {
  static bool state = LOW;
  static uint8_t cnt = 0;

  digitalWrite(PIN_PD7, state);
  state = !state;

  if (cnt++ > 20) {
    blinker.timerStop();
    oneShot.timerStart();
    cnt = 0;
  }
}

void setup() {
  pinMode(PIN_PD7, OUTPUT);
  digitalWrite(PIN_PD5, HIGH);  // LED #1

  pinMode(PIN_PD6, OUTPUT);  // LED #2
  digitalWrite(PIN_PD6, HIGH);

  Serial.begin(115200);

  Timer::init(false, false, Serial);
  blinker.timerStart();
  blinkerSlow.timerStart();
}

void loop() {
  Timer::timerLoop();
}
