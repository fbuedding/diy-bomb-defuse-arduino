#ifndef TIMER_Job_HPP
#define TIMER_Job_HPP

#include "Time.hpp"


class TimerJob {
public:
  TimerJob(Time startTime, Time duration, void (*callback)()){this->endTime = startTime + duration; }
  TimerJob(Time t, void (*callback)()) {this->endTime = t; this->callback = callback;}
  TimerJob();
  void (*callback)();
  Time endTime;
private:

};
#endif