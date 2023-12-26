#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rtcCb)(void);

void rtcSetCMPIsrCallback(rtcCb cb);
int8_t rtcInitialize(bool);
void rtcStart(void);
void rtcStop(void);
uint16_t rtcReadCounter(void);
void rtcEnableCMPInterrupt(void);
void rtcDisableCMPInterrupt(void);
void rtcWriteCMP(uint16_t ticks);
void rtcReadCMP(void);

#ifdef __cplusplus
}
#endif
