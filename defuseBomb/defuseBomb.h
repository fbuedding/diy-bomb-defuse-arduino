#ifndef DEFUSE_BOMB_H
#define DEFUSE_BOMB_H

#include "SevSeg.h"
#include "Keypad.h"
#include "PinDefs.h"
#include "Time.hpp"
#include <EEPROM.h>


/**
  Holds all possible states, only one should be activated
*/
enum States
{
  startup,
  unfuzed,
  fuzed,
  defuzed,
  lost,
  win,
  debug
};


/**
  Holds all possible display modes, only one should be activated
*/
enum displayModes
{
  time,
  input,
  dLost,
  debugDsp,
  winDsp
};
//Time timeLeft;

/**
  Activates a given state, everything a state needs to be activated, should be defined here!
*/
void activateState(enum States s);
/**
  Ticks time in 10ms steps, maybe changed later 
*/
void tickTime();
/**
  Displays on the 7 Segment display, formated in given displayMode
*/
void display(enum displayModes, int = -1);
void beepTick();
void keypadEvent(KeypadEvent);
void confirmInput();
void resetInput();
void beepbeep(unsigned long ms);
#endif