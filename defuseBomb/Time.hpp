#ifndef TIME_HPP
#define TIME_HPP
class Time {
public:
  int h;
  int m;
  int s;
  int ms;

  Time(int h=0, int m=0, int s=0, int ms=0);

  void setTick(int);

  bool Time::operator<(const Time&)const;
  bool Time::operator>(const Time& r)const;
  bool Time::operator<=(const Time& r)const;
  bool Time::operator>=(const Time& r)const;

  Time& Time::operator--();
  Time& Time::operator++();

  Time& Time::operator+=(const Time& rhs);
  friend Time operator+(Time lhs, const Time& rhs);
  Time& Time::operator-=(const Time& rhs);
  friend Time operator-(Time lhs, const Time& rhs);

private:
  int tick = 1;

  void normalize();
};
#endif