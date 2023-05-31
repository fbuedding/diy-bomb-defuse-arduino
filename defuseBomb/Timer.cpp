#include "Timer.hpp"
Timer::Timer() {
  jobs = new LinkedList<TimerJob*>();
  }
void Timer::tick(Time now) {
  int end = jobs->size()-1;
  for(int i = end; i > 0; --i){
    TimerJob j = *jobs->get(i);
    if(j.endTime < now){
      j.callback();
      jobs->remove(i);
      delete(&j);
    }
  }

} 

void Timer::addJob(Time time, void (*callback)()){
  jobs->add(&TimerJob(*(this->time) + time, callback));
}

