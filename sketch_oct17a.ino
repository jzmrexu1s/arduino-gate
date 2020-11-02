#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int pinButton = 6;
int pinRotary = A0;
// int pinSensor = D4;
int pinRed0 = 7;
int pinRed1 = 8;
int pinBuzzer = 5;

int pinInterrupt = 0;

int maxPeople = 100;
float maxTemp = 39.0;

int curPeople = 0;

int stateButton = 0;
bool stateModify = false;
bool before = false;
bool firstSet = false;

bool state0 = false;
bool state1 = false;
bool prevState0 = true;
bool prevState1 = true;
bool firstPassed = false;

uint32_t t = 0;

uint8_t *fontNormal = u8x8_font_chroma48medium8_r;
uint8_t *fontBold = u8x8_font_victoriabold8_r;
uint8_t *fontBig = u8x8_font_px437wyse700b_2x2_r;

void interruptHandler() { 
    t = millis();
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void stateChange() {
    before = stateModify;
    stateModify = !stateModify;
}

void setup() { 
	Serial.begin(9600);
	pinMode(pinButton, OUTPUT);
	pinMode(pinRotary, INPUT);
    // pinMode(pinSensor, OUTPUT);
    pinMode(pinRed0, INPUT);
    pinMode(pinRed1, INPUT);
    u8x8.setBusClock(1);
	u8x8.begin();
	u8x8.setFlipMode(1);
    mlx.begin();
    attachInterrupt(pinInterrupt, interruptHandler, FALLING);
}
void loop() {
	/*stateButton = digitalRead(pinButton);
	if (stateButton == HIGH && firstSet == false) {
		stateModify = !stateModify;
		firstSet = true;
	}*/

    if ((digitalRead(pinButton) == HIGH) && (millis() - t > 300)) {
        t = millis();
        stateModify = !stateModify;
    }

	if (stateButton == LOW) firstSet = false;

	if (!stateModify) maxPeople = map(analogRead(pinRotary), 0, 1023, 1000, 0);
    
    int curRed0 = digitalRead(pinRed0);
    int curRed1 = digitalRead(pinRed1);

    if (state0 == false && curRed0 == LOW) {
        state0 = true;
    }
    if (state0 == true && curRed0 == HIGH) {
        firstPassed = !firstPassed;
        state0 = false;
    }
    if (state1 == false && curRed1 == LOW) {
        state1 = true;
    }
    if (state1 == true && curRed1 == HIGH && firstPassed == true) {
        curPeople += 1;
        firstPassed = false;
    }
    if (state1 == true && curRed1 == HIGH) {
        state1 = false;
    }


    float curTemp = mlx.readObjectTempC();
    if (curPeople > maxPeople || curTemp > maxTemp) {
        analogWrite(pinBuzzer, 1);
    } else analogWrite(pinBuzzer, 0);
    // Serial.println(curTemp);
    // float curAmbTemp = mlx.readAmbientTempC();
    // Serial.println(curRed0);
    // Serial.println(curRed1);
    u8x8.setFont(fontNormal);
    u8x8.setCursor(0, 0);
    u8x8.print("PEOPLE");
    u8x8.setCursor(0, 1);
    u8x8.setFont(fontBig);
    u8x8.print(curPeople);
    u8x8.setCursor(0, 3);
    if (!stateModify) u8x8.setFont(fontBold); else u8x8.setFont(fontNormal);
    u8x8.print(maxPeople);
    u8x8.print("   ");

    if (stateModify) maxTemp = mapfloat(analogRead(pinRotary), 0.0, 1023.0, 42.0, 32.0);
    u8x8.setFont(fontNormal);
    u8x8.setCursor(7, 0);
    u8x8.print("TEMP");
    u8x8.setCursor(7, 1);
    
    if (!firstPassed) {
        u8x8.print("WAIT     ");
        u8x8.setCursor(7, 2);
        u8x8.print("         ");
    }
    if (firstPassed && curTemp < 34) {
        u8x8.print("CLOSE    ");
        u8x8.setCursor(7, 2);
        u8x8.print("         ");
    }
    if (firstPassed && curTemp > 42) {
        u8x8.print("FAR      ");
        u8x8.setCursor(7, 2);
        u8x8.print("         ");
    }
    if (firstPassed && curTemp > 34 && curTemp < 40) {
        u8x8.setFont(fontBig);
        u8x8.print(curTemp, 1);
    }
    u8x8.setCursor(7, 3);
    if (stateModify) u8x8.setFont(fontBold); else u8x8.setFont(fontNormal);
    u8x8.print(maxTemp);
    u8x8.print("   ");

    u8x8.refreshDisplay();
}