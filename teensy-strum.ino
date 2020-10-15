#include <TimerOne.h>

const int proximityPin = 12;
//const int smallMagnometer = 11;
const int largeMagnometer = 10;
const int doorSwitch = 9;

const int joystickPin = A0;

int downStrokePin = largeMagnometer;
//int upStrokePin = smallMagnometer;

int topStrummed = 0;
int bottomStrummed = 0;

const int channel = 1;

const int cooldownTotal = 20;
volatile bool coolingDown = false;

void setup()
{
    Timer1.initialize(cooldownTotal * 10000);
    Timer1.attachInterrupt(endCooldown);
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(downStrokePin, INPUT);
    attachInterrupt(digitalPinToInterrupt(downStrokePin), bottomSensorFalling, RISING);
    //pinMode(upStrokePin, INPUT_PULLUP);
    //attachInterrupt(digitalPinToInterrupt(upStrokePin), topSensorFalling, FALLING);
    pinMode(proximityPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(proximityPin), bottomSensorFalling, FALLING);
    pinMode(doorSwitch, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(doorSwitch), bottomSensorFalling, RISING);
    pinMode(joystickPin, INPUT);
}

void doUpStroke()
{
  usbMIDI.sendNoteOn(24, 99, channel);  // 60 = C4
  usbMIDI.sendNoteOff(24, 0, channel);
}

void doDownStroke()
{
  usbMIDI.sendNoteOn(26, 99, channel);  // 60 = C4
  usbMIDI.sendNoteOff(26, 0, channel);
}

void startCooldown()
{
  coolingDown = true;
  Timer1.start();
}

void endCooldown()
{
  coolingDown = false;
  Timer1.stop();
}

bool isCoolingDown()
{
  noInterrupts();
  bool tempCoolingDown = coolingDown;
  interrupts();
  return tempCoolingDown;
}
void topSensorFalling()
{
  if (isCoolingDown() == false)
  {
    doDownStroke();
    startCooldown();
  }
}

void bottomSensorFalling()
{
  if (isCoolingDown() == false)
  {
    doUpStroke();
    startCooldown();
  }
}

bool canStroke = true;

void loop()
{
  int joystickValue = analogRead(joystickPin);
  if (joystickValue > 450 && joystickValue < 550)
  {
    canStroke = true;
  }
  if (joystickValue > 600 && canStroke) {
    canStroke = false;
    doUpStroke();
  } else if (joystickValue < 400 && canStroke) {
    canStroke = false;
    doDownStroke();
  }
}
