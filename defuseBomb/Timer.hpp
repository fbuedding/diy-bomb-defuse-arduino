#ifndef TIMER_HPP
#define TIMER_HPP
#include <LinkedList.h>
#include "TimerJob.hpp"
class Timer {
public:
  Timer();
  void addJob(Time time, void (*callback)());
  void tick(Time now);
private:
  Time* time;
  LinkedList<TimerJob*>* jobs;
};
#endif