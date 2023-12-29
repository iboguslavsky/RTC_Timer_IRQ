# Interrupts-based Timer library for AVR Dx product family of MCUs
Unlinke other common timer libraries, this one doesn't have a limitation on the number of timers supported. The number of available timers is only limited by the amount of the available RAM. See [100_Timers](https://github.com/iboguslavsky/RTC_Timer_IRQ/edit/main/README.md) for an exmple running on 2Kb RAM MCU.

## Features
This library makes use of the newly-available [RTC peripheral](https://onlinedocs.microchip.com/oxy/GUID-8CE4FE13-3B15-43FE-A86C-FC8177202CD3-en-US-6/GUID-5EFC8FBF-DD40-43CB-898A-D0EAD386D90D.html) This allows to:
- Avoid use of more commonly used TCA/TCB/TCD timers, freeing them up for more common uses withint apps
- Exceptionally low power consumption (~800 nA when running from internal 32KHz oscillator)
- 1ms accuracy (when clocked by 1.024KHz clock derived from from OSC32K)

The library allows selection of fast (32KHz) and slow (1.024Khz) clock to be used. When longer timer period is needed, use slow clock option (up to ~64s timer period). When more accuracy is required, use the fast clock (~30ms timer resolution) 

## Basic usage

## Limitations
The library is using CMP mode of the RTC timer to dive individual timers. Snce RTC domain clock is asynchronous to the main clock, it takes up to 3 RTC lcock cycles to syncronize CMP register changes. BEcause of this limitation, timer granularity is forced in code to a minimu of 5 timer ticks. WHen slow clock timer is used, this results in ~5ms tier granularity. The library will round up the tiimer period to the nearest 5ms boundary (ie, setting timer to 22ms period will result in a 25ms period).
