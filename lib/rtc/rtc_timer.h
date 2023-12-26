#include "rtc.h"
#include <ArduinoSTL.h>

#define DEBUG(input)   { if (_serial) _serial->print(input);   }
#define DEBUGLN(input) { if (_serial) _serial->println(input); }

#pragma once
typedef void (*timer_cb_t)(void *);

inline uint16_t roundUpToNearestFive(uint16_t);

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

struct Timer {
	Timer(uint16_t num_ticks, bool one_shot, timer_cb_t cb) : 
		_num_ticks(roundUpToNearestFive(num_ticks)), _one_shot(one_shot), _cb(cb) {
		_capture_tick = 0;
		_running = false;
		_stopping = false;
		timers.push_back(this);
	}

	Timer(uint16_t num_ticks, bool one_shot, timer_cb_t cb, void *data) : 
		_num_ticks(roundUpToNearestFive(num_ticks)), _one_shot(one_shot), _cb(cb), _data(data) {
		_capture_tick = 0;
		_running = 0;
		_stopping = 0;
		timers.push_back(this);
	}

	~Timer() {
		// Remove this instance from the collection
    auto it = std::find(timers.begin(), timers.end(), this);
    if (it != timers.end()) {
      timers.erase(it);
    }
	}

	// Instance methods
	void timerStart();
	void timerStop();

	// Class methods
	inline static void CMP_ovf_cb();
  inline static void init(bool, bool, Stream &);
	inline static void timerLoop();

	uint16_t _num_ticks, _capture_tick;
	bool _one_shot, _running, _stopping, _fired;
	void *_data;
	timer_cb_t _cb;
	// Global vars in class' namespace
  static Stream *_serial;	
	static std::vector<Timer *> timers;
	static bool _trueIRQ;
};

// Initialize static members
static std::vector<Timer*> Timer::timers;
static bool Timer::_trueIRQ = false;
static Stream* Timer::_serial = NULL;	

inline static void Timer::CMP_ovf_cb () {
uint16_t now, time_until_next_expiration, delta; 
bool have_active_timers = false;

	rtcDisableCMPInterrupt();
	now = rtcReadCounter()  - 1;	// Seem to overshoot by one
	
	// DEBUGLN(now);
	// Note all expired timers 
	for (auto *timer : timers) {
		if (timer->_capture_tick == now && timer->_running) {
			timer->_fired = true;
			timer->_capture_tick = roundUpToNearestFive(now + timer->_num_ticks);
			if (_trueIRQ) {
				(*timer->_cb)(timer->_data);
				timer->_fired = false;
				if (timer->_stopping) {		// Let timer fire one last time
					timer->_running = false;
					timer->_stopping = false;
				}
			}
			if (timer->_one_shot) {
				timer->_running = false;
			}
		}
	}

  // Set the next Compare 
	time_until_next_expiration = 0xffff; // largest delta between now and next timer expiration
	for (auto *timer : timers) {
		// Ignore stopping or stopped timers when scheduling next Compare
		if (!timer -> _running || timer->_stopping) {
			continue;
		}
		have_active_timers = true;
		delta = timer->_capture_tick - now;
		if (delta < time_until_next_expiration) {
			time_until_next_expiration = delta;
		}
	}
	if (have_active_timers) {
		rtcEnableCMPInterrupt();
  	rtcWriteCMP(now + time_until_next_expiration);
	}
	else {
		DEBUG("No actives at "); DEBUGLN(now);
		rtcDisableCMPInterrupt();
	}
}

inline void Timer::timerStart () {
uint16_t tick, time_until_next_expiration, delta;
	tick = roundUpToNearestFive(rtcReadCounter());
  _capture_tick = roundUpToNearestFive(tick + _num_ticks); // account for uint overflow when tick + _num_ticks > 65535
	_running = true;

	time_until_next_expiration = 65535;
	for (auto *timer : timers) {
		if (timer->_running) {
			delta = timer->_capture_tick - tick;
			if (delta < time_until_next_expiration)
				time_until_next_expiration = delta;
		}
  }
	rtcWriteCMP(tick + time_until_next_expiration);
	rtcEnableCMPInterrupt();
}

inline void Timer::timerStop () {
	_stopping = true;
}

inline static void Timer::init(bool highSpeed, bool trueIRQ, Stream &thisSerial) {
	_serial = &thisSerial;
	_trueIRQ = trueIRQ;
  rtcInitialize(highSpeed);
  rtcSetCMPIsrCallback(CMP_ovf_cb);
	rtcEnableCMPInterrupt();
  rtcStart();
	DEBUGLN("Timer::init() OK");
}

// Run timer callbacks for all timers that have fred
inline static void Timer::timerLoop() {
	if (!_trueIRQ) {
		for (auto *timer : timers) {
			if (timer->_fired) {
				timer->_fired = false;
				(*timer->_cb)(timer->_data);
				if (timer->_stopping) {
					timer->_running = false;
					timer->_stopping = false;
				}
			}
		}
	}
}

inline uint16_t roundUpToNearestFive(uint16_t n) {
  return n % 5 == 0 ? n : n + 5 - n % 5;
}
