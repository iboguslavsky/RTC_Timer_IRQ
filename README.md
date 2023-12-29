# Interrupts-based Timer library for AVR Dx product family of MCUs
Unlinke other common timer libraries, this one doesn't have a limitation on the number of timers supported. The number of available timers is only limited by the amount of the available RAM. See [100_Timers](https://github.com/iboguslavsky/RTC_Timer_IRQ/edit/main/README.md) for an exmple running on 2Kb RAM MCU.

Since timers are hardware-based and interrupt-driven, it allows for accurate triggering of timers - independently of any blocking code in the app.

## Features
This library makes use of the newly-available on Dx series [RTC peripheral](https://onlinedocs.microchip.com/oxy/GUID-8CE4FE13-3B15-43FE-A86C-FC8177202CD3-en-US-6/GUID-5EFC8FBF-DD40-43CB-898A-D0EAD386D90D.html). This allows for:
- Avoiding the use of more commonly used TCA/TCB/TCD timers
- Exceptionally low power consumption (~800 nA when running from internal 32KHz oscillator)
- 1ms accuracy (when clocked by 1.024KHz clock derived from from OSC32K)

The library allows selection of fast (32KHz) and slow (1.024Khz) clock to be used. When longer timer period is needed, use slow clock option (up to ~64s timer period). When more accuracy is required, use the fast clock (~30ms timer resolution).
The library provides support for continuos (auto-reloading) and one-shot timers. 

Upon timer expiration, the timer will invoke a user-provided callback function. There is an optional input parameter of arbitrary type, that can be provided to the callback. See code examples for the input parameter use, including how to dereference the input parameter.

## Basic usage
### Initialize Timer subsystem
```
Timer::init(fastCLock, trueIRQ, Serial);
```
The first boolean parameter selects fast or slow clock. The second one provides selection of "true Interrupt" use (ie, avoids using the main loop() in an Arduino app. This is preferred when some blocking code is expected to be executed as part of loop()). The preferred method is set trueIRQ to __false__, reuiring the use of _ _Timer::timerLoop()_ _ funtion insode the main loop() to **poll* for expiring timers, which is more lightweight and aligns with the polling nature of Arduino's loop() function.

The last parameter allows for passing of Serial (or any other) stream parameter to help with debugging of the library by allowing Serial.print() funtionality to be used inside library functions.

### Declare individual timer instances

### Start timer

### Stop timer

## Limitations
The library is using CMP mode of the RTC timer to dive individual timers. Snce RTC domain clock is asynchronous to the main clock, it takes up to 3 RTC lcock cycles to syncronize CMP register changes. BEcause of this limitation, timer granularity is forced in code to a minimu of 5 timer ticks. WHen slow clock timer is used, this results in ~5ms tier granularity. The library will round up the tiimer period to the nearest 5ms boundary (ie, setting timer to 22ms period will result in a 25ms period).
