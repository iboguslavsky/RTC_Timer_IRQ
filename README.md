# Interrupts-based Timer library for AVR Dx product family of MCUs
Unlinke other common timer libraries, this one doesn't have a limitation on the number of available timers. The number of timers is only limited by the amount of the available RAM. See [100_Timers](https://github.com/iboguslavsky/RTC_Timer_IRQ/edit/main/README.md) for an example running on 2Kb RAM MCU.

Since timers are hardware-based and interrupt-driven, it allows for accurate triggering of timers - independently of any blocking code in the app. See [accuracy](https://github.com/iboguslavsky/RTC_Timer_IRQ/tree/main/examples/accuracy) example.

## Features
This library makes use of the [RTC peripheral](https://onlinedocs.microchip.com/oxy/GUID-8CE4FE13-3B15-43FE-A86C-FC8177202CD3-en-US-6/GUID-5EFC8FBF-DD40-43CB-898A-D0EAD386D90D.html) available on newer parts, Like Dx series and series 1 AVR parts. This allows for:
- Avoiding the use of more commonly used TCA/TCB/TCD timers
- Exceptionally low power consumption (~800 nA when running from internal 32KHz oscillator)
- 1ms accuracy (when clocked by 1.024KHz clock derived from from OSC32K)

The library allows selection of fast (32KHz) and slow (1.024Khz) clock to be used. When longer timer period is needed, use slow clock option (up to ~64s timer period). When more accuracy is required, use the fast clock (~30ms timer resolution).
The library provides support for continuos (auto-reloading) and one-shot timers. 

Upon timer expiration, the timer will invoke a user-provided callback function. There is an optional input parameter of arbitrary type, that can be provided to the callback. See code examples for the input parameter use, including how to dereference the input parameter.

## Basic usage
### Initialize RTC Counter
```
Timer::init(fastCLock, trueIRQ, Serial);
```
The first boolean parameter selects fast or slow clock. The second one provides selection of "true interrupt" usage pattern, by avoiding using the main loop() in an Arduino app alltogether. This is acceptable when some blocking code is expected to be executed as part of main loop(). The preferred method though is to set trueIRQ to __false__, requiring the use of _Timer::timerLoop()_ funtion inside the main loop() to **poll* for expiring timers, which is more lightweight and aligns with the polling nature of Arduino's loop() function.

The last parameter allows for passing of Serial (or any other stream) to help with debugging of the library by enabling Serial.print() funtionality inside library functions.

### Instantiate Timer class
```
Timer latency(period, isSingleShot, callback);
```
Period is expressed in a number of RTC counter's "ticks". For a **slow** clock, it's roughly 1ms (1,000/1,024 ms, to be precise). For a **fast** clock, it is ~30ms (1 / 32678 s). Keep in mind that the period value is rounded up to the nearest 5 ticks (see [Limitations](https://github.com/iboguslavsky/RTC_Timer_IRQ/blob/main/README.md#limitations) section below)

Timer class can be instantiated [statically](https://github.com/iboguslavsky/RTC_Timer_IRQ/tree/main/examples/one_shot) (as a global variable, for example) - or it could be allocated dynamically during runtime using [**new**](https://github.com/iboguslavsky/RTC_Timer_IRQ/edit/main/README.md). The advantage of having instance data pre-allocated during compile time is more predictability around memory consumption.

You can pass an arbitrary data structure to the callback function. See [100_Timers](https://github.com/iboguslavsky/RTC_Timer_IRQ/edit/main/README.md) for sample implementation.

### Start timer
The timer, once allocated, remains paused until started. To start a timer, or to restart a paused timer, simply do:
```
latencyTimer.startTimer();
```

### Stop timer
Any non-single shot timer can be paused. _Note_: the timer will always execute its last run before being stopped. To stop (pause) a timer, run:
```
latencyTimer.stopTimer();
```

### How to handle timer polling
For  Arduino "native" (ie, non-true IRQ timers) handling, do not forget to include this in the loop() funtion (anywhere):
```
Timer::timerLoop()
```
This will check for expired timers and run corresponsding callback functions defined during timer's instantiation. Keep in mind that if loop() gets blocked, your timer will be inaccurate. If you need to keep the accuracy, use *trueIRQ* mode in _Timer::init()_.

## Limitations
The library is using CMP mode of the RTC timer to dive individual timers. Snce RTC domain clock is asynchronous to the main clock, it takes up to 3 RTC lcock cycles to syncronize CMP register changes. BEcause of this limitation, timer granularity is forced in code to a minimu of 5 timer ticks. WHen slow clock timer is used, this results in ~5ms tier granularity. The library will round up the tiimer period to the nearest 5ms boundary (ie, setting timer to 22ms period will result in a 25ms period).

For that same reason, when starting timers in a rapid succession, allow for 5 ticks (5ms for slow clock) of delay between each timerStart() run.
