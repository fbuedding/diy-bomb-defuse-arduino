#include "defuseBomb.h"

// Time maybe changed later
#define H 1
#define M 0
#define S 300

#define PIN "56843"

String pinBuffer = "     ";
String inputBuffer = "       ";

SevSeg sevseg;  // Ein sieben Segment Objekt initialisieren

const byte ROWS = 4;  // number of rows
const byte COLS = 4;  // number of columns

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { R1, R2, R3, R4 };  // row pinouts of the keypad
byte colPins[COLS] = { C1, C2, C3, C4 };  // column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);




enum States state;

enum displayModes displayMode = time;

Time timeLeft = Time(H, M, S, 0);

bool timeSet = false;
bool pinSet = false;

int cacheDisplayMode;

void setup() {
  state = startup;
  inputBuffer.reserve(7);
  inputBuffer = "";
  pinBuffer.reserve(5);
  pinBuffer = "";

  byte numDigits = 5;
  byte digitPins[] = { DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4 };
  byte segmentPins[] = { A, B, C, D, E, F, G, P };
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);

  pinMode(BUZZER, OUTPUT);

  pinMode(DIGIFUZE, INPUT_PULLUP);
  pinMode(DEBUG, INPUT_PULLUP);

  keypad.addEventListener(keypadEvent);
  Serial.begin(9600);
  Serial.println("Online");
  Time tmp;
  EEPROM.get(0, tmp);
  if(tmp.s != -1 && tmp.m != -1 && tmp.h != -1){
    timeLeft = tmp;
    delete(&tmp);
  }else {
    Serial.println("no time"); 
  }
  int tmpPin;
  EEPROM.get(sizeof(timeLeft), tmpPin);
  if(tmpPin != -1){
    Serial.println("Pin"); 
    Serial.println(tmpPin); 

    pinBuffer.concat(tmpPin);
  }else {
    Serial.println("no Pin"); 
  }
  
  if (digitalRead(DEBUG) == LOW) {
    activateState(debug);
    timeLeft = Time(1, 15, 60);
    timeLeft -= Time(0, 65, 60);
  } else if (digitalRead(DIGIFUZE) == HIGH) {
    activateState(fuzed);
  } else {
    activateState(unfuzed);
  }
}

void loop() {
  switch (state) {
    case unfuzed:
      keypad.getKey();
      display(displayMode);
      if (digitalRead(DIGIFUZE) == HIGH) {
        activateState(fuzed);
      }
      break;
    case fuzed:
      keypad.getKey();

      display(displayMode);
      break;
    case lost:
      display(displayMode);
      break;
    case debug:
      display(displayMode);
      keypad.getKey();
      break;
    case win:
      display(displayMode);
      break;
  }

  sevseg.setBrightness(100);
  sevseg.refreshDisplay();
}

//############################# FUNCTIONS ###############################
;
void activateState(enum States s) {
  if (s == state)
    return;

  switch (s) {
    case unfuzed:
      state = unfuzed;
      displayMode = debugDsp;
      break;
    case fuzed:
      beepbeep(75);
      state = fuzed;
      displayMode = time;
      if (pinBuffer.length() == 0)
        pinBuffer = PIN;
      timeLeft.setTick(1);
      setupTimer1_1000hz();
      break;
    case lost:
      stopTimer1();
      displayMode = dLost;
      state = lost;
      digitalWrite(BUZZER, HIGH);
      break;
    case debug:
      state = debug;
      displayMode = debugDsp;
      break;
    case win:
      stopTimer1();
      state = win;
      displayMode = winDsp;
      digitalWrite(BUZZER, LOW);
      beepbeep(75);
      beepbeep(75);
  }
}

void tickTime() {
  timeLeft--;
  if (timeLeft < Time()) {
    activateState(lost);
  }
}

void display(enum displayModes m, int anim) {

  char disp[] = "_______";

  switch (m) {
    case time:
      if (timeLeft < Time(0, 1)) {
        sprintf(disp, "%02d.%03d", timeLeft.s, timeLeft.ms);
      } else if (timeLeft < Time(1)) {
        sprintf(disp, "%d.%02d.%02d", timeLeft.m, timeLeft.s, timeLeft.ms / 10);
      } else {
        sprintf(disp, "%d.%02d.%02d", timeLeft.h, timeLeft.m, timeLeft.s);
      }
      sevseg.setChars(disp);
      break;

    case input:
      for (unsigned int i = 0; i < inputBuffer.length(); i++) {
        disp[INPUTSIZE - inputBuffer.length() + i] = inputBuffer.charAt(i);
      }
      sevseg.setChars(disp);
      break;

    case dLost:
      sevseg.setChars("lost");
      break;

    case debugDsp:
      sprintf(disp, "%d.%02d.%02d", timeLeft.h, timeLeft.m, timeLeft.s);
      sevseg.setChars(disp);
      break;
    case winDsp:
      sevseg.setChars("great");
      break;
    default:
      sprintf(disp, "%d.%02d.%02d", timeLeft.h, timeLeft.m, timeLeft.s);
      sevseg.setChars(disp);
      break;
  }

  free(disp);
}

void beepTick() {
  if (timeLeft.ms > 1000 - BEEPMS - 1) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

void keypadEvent(KeypadEvent key) {

  switch (keypad.getState()) {
    case RELEASED:
      if (state == unfuzed)
        beep(50);
      if (displayMode != input) {
        cacheDisplayMode = displayMode;
        displayMode = input;
      }
      if (key == '*') {
        resetInput();
      } else if (key == '#') {
        confirmInput();
      } else {
        if (inputBuffer.length() != INPUTSIZE) {
          inputBuffer += key;
          if (inputBuffer.equals(pinBuffer) == 1 && state == fuzed) {
            Serial.println("Gewonnen");
            activateState(win);
            return;
          }
        }
        if (inputBuffer.length() == INPUTSIZE && state != unfuzed) {
          delay(1000);
          inputBuffer = "";
          displayMode = cacheDisplayMode;
        }
      }
      break;
  }
}

void confirmInput() {
  switch (state) {
    case unfuzed:
      if (!pinSet) {
        pinBuffer = inputBuffer;
        EEPROM.put(sizeof(timeLeft), inputBuffer.toInt());
        pinSet = true;
      } else if (!timeSet) {
        switch (inputBuffer.length()) {
          case 0:

            break;
          case 1:
            timeLeft = Time(0, 0, inputBuffer.toInt());
            break;
          case 2:
            timeLeft = Time(0, 0, inputBuffer.toInt());
            break;
          case 3:
            {
              char m = inputBuffer.charAt(0) - '0';
              int s = atoi(inputBuffer.c_str() + 1);
              timeLeft = Time(0, m, s);

            }

            break;
          case 4:
            {
              char mArr[] = { inputBuffer.charAt(0), inputBuffer.charAt(1), '\0' };
              int m = atoi(mArr);
              int s = atoi(inputBuffer.c_str() + 2);
              timeLeft = Time(0, m, s);
            }
            break;
          case 5:
            {
              char hArr[] = { inputBuffer.charAt(0), '\0' };
              char mArr[] = { inputBuffer.charAt(1), inputBuffer.charAt(2), '\0' };
              int h = atoi(hArr);
              int m = atoi(mArr);
              int s = atoi(inputBuffer.c_str() + 3);
              timeLeft = Time(h, m, s);
            }
            break;
          
        }
        timeSet= true;
        EEPROM.put(0, timeLeft);
      }
      break;
    case fuzed:

      break;
  }
  resetInput();
}

void resetInput() {
  inputBuffer = "";
  displayMode = cacheDisplayMode;
}

void beepbeep(unsigned long ms) {
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
  delay(ms);
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
}
void beep(unsigned long ms) {
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
}

/**
 *############################# INTERRUPTS ###############################
 */

void stopTimer1() {
  noInterrupts();
  TIMSK1 &= ~(1 << OCIE1A);
  interrupts();
}

void setupTimer1_1hz() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 1 Hz (16000000/((15624+1)*1024))
  OCR1A = 15624;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}
void setupTimer1_2hz() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 2 Hz (16000000/((31249+1)*256))
  OCR1A = 31249;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 256
  TCCR1B |= (1 << CS12);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void setupTimer1_10hz() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 10 Hz (16000000/((6249+1)*256))
  OCR1A = 6249;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 256
  TCCR1B |= (1 << CS12);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void setupTimer1_100hz() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 100 Hz (16000000/((624+1)*256))
  OCR1A = 624;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 256
  TCCR1B |= (1 << CS12);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void setupTimer1_1000hz() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 1000 Hz (16000000/((249+1)*64))
  OCR1A = 249;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 64
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

ISR(TIMER1_COMPA_vect) {
  tickTime();
  beepTick();
}
