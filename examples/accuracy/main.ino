#include <Stream.h>
#include "rtc_timer.h"

timer_cb_t latencyCb(void* data) {
  digitalWriteFast(PIN_PD5, HIGH);
  digitalWriteFast(PIN_PD5, LOW);
}

Timer latency(5, false, latencyCb);

void setup() {
  pinMode(PIN_PD5, OUTPUT);
  digitalWrite(PIN_PD5, LOW);

  Serial.begin(115200);

  // Use trueIRQ for best accuracy (pure interrupt-driven callback)
  Timer::init(false, true, Serial);
  latency.timerStart();
}

void loop() {}
