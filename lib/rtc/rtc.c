#include "rtc.h"

rtcCb (*rtcCMPIsrCb)(void) = NULL;

int8_t rtcInitialize(bool highSpeed) {
	// Wait for all register to be synchronized 
	while (RTC.STATUS > 0) {};
	// Reset the counter after reboot
	RTC.CNT = 0x00;
  // High Speed: 32Khz clock, ~2 secs max period, ~30us granularity (min period 10 ticks)
  // Norm speed: 1024Hz clock, ~64 secs max period, ~1ms granularity (min period 10 ticks)
	RTC.CLKSEL = highSpeed ? 0x00 : 0x01;
	return 0;
}

void rtcStart(void) {
	while (RTC.STATUS > 0) {};
  RTC.CTRLA |= RTC_RTCEN_bm;
}

void rtcStop(void) {
	while (RTC.STATUS > 0) {};
  RTC.CTRLA &= ~RTC_RTCEN_bm;
}

void rtcSetCMPIsrCallback(rtcCb cb) {
  rtcCMPIsrCb = cb;
}

ISR(RTC_CNT_vect) {
	if (RTC.INTFLAGS & RTC_CMP_bm ) {
		if (rtcCMPIsrCb != NULL) {
			(*rtcCMPIsrCb)();
		} 
	}  
	RTC.INTFLAGS = RTC_CMP_bm;
}

inline uint16_t rtcReadCounter(void) {
  return RTC.CNT;
}

inline void rtcEnableCMPInterrupt(void) {
  RTC.INTCTRL |= RTC_CMP_bm;
}

inline void rtcDisableCMPInterrupt(void) {
  RTC.INTCTRL &= ~RTC_CMP_bm; 
}

inline void rtcWriteCMP(uint16_t ticks) {
  while (RTC.STATUS & RTC_CMPBUSY_bm);
  RTC.CMP = ticks;
}

inline void rtcReadCMP(void) {
	return RTC.CMP;
}
