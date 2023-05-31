#include "Time.hpp"

Time::Time(int h, int m, int s, int ms) {
  this->h = h;
  this->m = m;
  this->s = s;
  this->ms = ms;
  this->normalize();
}

void Time::setTick(int tick) {
  this->tick = tick;
}

void Time::normalize() {
  while (ms < 0) {
    ms = 1000 + ms;
    s--;
  }
  while (ms > 999) {
    ms = ms - 1000;
    s++;
  }
  while (s < 0) {
    s = 60 + s;
    m--;
  }
  while (s > 59) {
    s = s - 60;
    m++;
  }
  while (m > 59) {
    m = m - 60;
    h++;
  }
  while (m < 0) {
    m = 60 + m;
    h--;
  }
}

bool Time::operator<(const Time &r) const {
  if (h < r.h) return true;
  if (h > r.h) return false;
  if (m < r.m) return true;
  if (m > r.m) return false;
  if (s < r.s) return true;
  if (s > r.s) return false;
  if (ms < r.ms) return true;
  if (ms > r.ms) return false;
  return false;  //Times have to be equal
}

bool Time::operator>(const Time &r) const {
  return r < *this;
}

bool Time::operator<=(const Time &r) const {
  return !(r > *this);
}
bool Time::operator>=(const Time &r) const {
  return !(r < *this);
}

Time& Time::operator+=(const Time &rhs){
  ms = ms + rhs.ms;
  s = s + rhs.s;
  m = m + rhs.m;
  h = h + rhs.h;
  this->normalize();
  return *this;
}
Time operator+(Time lhs,const Time& rhs){
  lhs += rhs;
  return lhs;
}
Time& Time::operator-=(const Time &rhs){
  ms = ms - rhs.ms;
  s = s - rhs.s;
  m = m - rhs.m;
  h = h - rhs.h;
  this->normalize();
  return *this;
}
Time operator-(Time lhs,const Time& rhs){
  lhs -= rhs;
  return lhs;
}

Time& Time::operator--() {
  ms -= tick;
  normalize();
  return *this;
}
Time &Time::operator++() {
  ms += tick;
  normalize();
  return *this;
}