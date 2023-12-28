#include <Stream.h>
#include "rtc_timer.h"

#define MAX_TIMERS 10
timer_cb_t latencyCb(void*);
timer_cb_t blinkerCb(void*);
timer_cb_t blinkerSlowCb(void*);
timer_cb_t oneShotCb(void*);

Timer latency(5, false, latencyCb), blinker(100, false, blinkerCb),
    blinkerSlow(500, false, blinkerSlowCb), oneShot(5000, true, oneShotCb);
timer_cb_t latencyCb(void* data) {
  digitalWrite(PIN_PD5, HIGH);
  digitalWrite(PIN_PD5, LOW);
}

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

timer_cb_t lambda(void* data) {
  Serial.print("Timer #");
  Serial.println(*(uint16_t*)data);
}

// std::vector<Timer *> collection;
Timer* timers[MAX_TIMERS];
uint16_t data[MAX_TIMERS];

void setup() {
  pinMode(PIN_PD7, OUTPUT);
  digitalWrite(PIN_PD5, HIGH);  // LED

  pinMode(PIN_PD6, OUTPUT);  // LED
  digitalWrite(PIN_PD6, HIGH);

  pinMode(PIN_PD5, OUTPUT);
  digitalWrite(PIN_PD5, LOW);

  Serial.begin(115200);
  Serial.println("Reboot OK");
  Serial.print("OSCHFCTRLA: 0x");
  Serial.println(CLKCTRL.OSCHFCTRLA, HEX);

  Timer::init(false, false, Serial);
  for (int i = 0; i < MAX_TIMERS; i++) {
    data[i] = i;
    timers[i] = new Timer((i + 1) * 1000, false, lambda, &data[i]);
    timers[i]->timerStart();
    delay(5);
  }
  latency.timerStart();
  blinker.timerStart();
  blinkerSlow.timerStart();
}

void loop() {
  Timer::timerLoop();
}
