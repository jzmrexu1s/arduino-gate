#include <U8x8lib.h>

U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

int pinButton = 6;
int pinRotary = A0;

int maxPeople = 100;
float maxTemp = 36.8;

int curPeople = 567;
float curTemp = 36.5;

int stateButton = 0;
int stateModify = 0;
bool firstSet = false;

uint8_t *fontNormal = u8x8_font_chroma48medium8_r;
uint8_t *fontBold = u8x8_font_victoriabold8_r;
uint8_t *fontBig = u8x8_font_px437wyse700b_2x2_r;

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() { 
	Serial.begin(9600);
	pinMode(pinButton, OUTPUT);
	pinMode(pinRotary, INPUT);
	u8x8.begin();
	u8x8.setFlipMode(1);
}
void loop() {
	stateButton = digitalRead(pinButton);
	if (stateButton == HIGH && firstSet == false) {
		stateModify = !stateModify;
		firstSet = true;
	}
	if (stateButton == LOW) firstSet = false;

	if (!stateModify) maxPeople = map(analogRead(pinRotary), 0, 1023, 1000, 0);
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
    u8x8.setFont(fontBig);
    u8x8.print(curTemp, 1);
    u8x8.setCursor(7, 3);
    if (stateModify) u8x8.setFont(fontBold); else u8x8.setFont(fontNormal);
    u8x8.print(maxTemp);
    u8x8.print("   ");

    u8x8.refreshDisplay();
    delay(200);
}