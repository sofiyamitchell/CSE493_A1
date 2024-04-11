#include <Servo.h> 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int DELAY_MS = 20; // delay in ms between changing colors
const int MAX_COLOR_VALUE = 255;
const int PHOTOCELL_INPUT_PIN = A0;
const int MIN_PHOTOCELL_VAL = 200; 
const int MAX_PHOTOCELL_VAL = 800; 
const int POTENTIOMETER_PIN = A1;
const int BUTTON_INPUT_PIN = 2; 
const int SERVO_PIN = 9;
const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
const int rotationInterval = 1000; //Rotate light in creative mode once a second
const int rotationAmount = 30; //Rotate 30 degrees each second

int mode = 1;     
int buttonState = 1; 
int lastButtonState = 1;
Servo myServo; 
int pos = 0; 
bool isLightOn = false;
unsigned long previousMillis = 0;
bool rotateServo = false;


enum RGB{
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};

int _rgbLedValues[] = {255, 0, 0};
enum RGB _curFadingUpColor = GREEN;
enum RGB _curFadingDownColor = RED;
const int FADE_STEP = 5;  

void setup() {
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);  
  pinMode(POTENTIOMETER_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  Serial.begin(9600); 
}

void loop() {
  buttonState = digitalRead(BUTTON_INPUT_PIN);
  int photocellVal = analogRead(PHOTOCELL_INPUT_PIN);
  int ledVal = map(photocellVal, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 255);
  int potVal = analogRead(POTENTIOMETER_PIN);
  
  ledVal = constrain(ledVal, 0, 255);

  //Button press logic generated by ChatGPT, and adjusted to pull-down resistor logic
  if (buttonState != lastButtonState) {
    delay(100);  // Basic debounce delay
    if (buttonState == LOW) {
      mode++;
      if(mode > 3){
        mode = 1;
      }
      Serial.print("Switched to mode ");
      Serial.println(mode);
    }
  }
  lastButtonState = buttonState;

  switch (mode) {
    case 1:
      crossFade_photoresistor(ledVal);
      break;
    case 2:
      creative();
      break;
    case 3:
      if(potVal > 1000){
        setColor(64, 224, 208); 
      }
      else if(potVal > 900){
        setColor(255, 255, 50);
      } else {
        setColor(128, 0, 128);
      }
      break;
  }
  delay(DELAY_MS);
}

//Crossfading logic from the RGB crossfade lesson (https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade.html)
void crossFade_photoresistor(int ledVal)
{
  _rgbLedValues[_curFadingUpColor] += FADE_STEP;
        _rgbLedValues[_curFadingDownColor] -= FADE_STEP;

        if(_rgbLedValues[_curFadingUpColor] > MAX_COLOR_VALUE){
          _rgbLedValues[_curFadingUpColor] = MAX_COLOR_VALUE;
          _curFadingUpColor = (RGB)((int)_curFadingUpColor + 1);

          if(_curFadingUpColor > (int)BLUE){
            _curFadingUpColor = RED;
          }
        }

        if(_rgbLedValues[_curFadingDownColor] < 0){
          _rgbLedValues[_curFadingDownColor] = 0;
          _curFadingDownColor = (RGB)((int)_curFadingDownColor + 1);

          if(_curFadingDownColor > (int)BLUE){
            _curFadingDownColor = RED;
          }
        }
        //Scale the brightness of the final LED output by a value based on the amount of light sensed 
        //by the photoresistor
        float scale = (float)ledVal / MAX_COLOR_VALUE;
        setColor(_rgbLedValues[RED] * scale, _rgbLedValues[GREEN] * scale, _rgbLedValues[BLUE] * scale);
}

//Helper function to set RGB LED color to new color
void setColor(int red, int green, int blue)
{
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}

void creative()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= rotationInterval) {
    previousMillis = currentMillis;
    rotateServo = !rotateServo;
    isLightOn = !isLightOn;

    if (rotateServo) {
      if (pos == 180) {
        pos = 0;
      } else {
        pos += rotationAmount;
      }
      myServo.write(pos);
    }

    if (isLightOn) {
      setColor(MAX_COLOR_VALUE, 100, 0);
    } else {
      setColor(0, 0, 0);
    }
  }
}


