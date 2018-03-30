#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <DS3231.h>

/*
* Define font array
*
*    11   12
*
*  10        13
*  09        14
*
*    08   07
*
*  01        06
*  02        05
*
*    03    04
*
* Mirror because we use >>
*/
static const char font[] = {
	0b1110111, //0
	0b1000100, //1
	0b1101011, //2
	0b1101110, //3
	0b1011100, //4
	0b0111110, //5
	0b0111111, //6
	0b1100100, //7
	0b1111111, //8
	0b1111110, //9
	0b0000000, //10 BLANK
};

// characters on segments
int char1 = 8;
int char2 = 8;
int char3 = 8;
int char4 = 8;

// LED Strip object definition
const int ledStripPin = 12; // pin in arduino
const int ledsInStrip = 59; // leds in strip
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(ledsInStrip, ledStripPin, NEO_GRB + NEO_KHZ800); // strip object

																							  // Strip actual color
byte redColor = 200; // strip color red
byte greenColor = 50; // strip color green
byte blueColor = 120; // strip color blue

// Time objects
DS3231 clock;
RTCDateTime dateTime;

// days from last accident
int days;

/**
 * Show @char from @font array on selected @segment with selected color in @redColor, @greenColor and @blueColor
 *
 * int number Char in font array
 * int segment Selected segment
 */
void showCharOnSegment(int16_t number, int16_t segment)
{
    int16_t offset = 0;
    switch (segment) {
    case 1:
        offset = 1;
        break;
    case 2:
        offset = 15;
        break;
    case 3:
        offset = 31;
        break;
    case 4:
        offset = 45;
        break;
    }

    uint16_t line;
    line = font[number];

    for (int16_t j = 0; j < 7; j++)
    {
        if (line & 0b1)
        {
            pixels.setPixelColor(j*2 + offset, redColor, greenColor, blueColor);
			pixels.setPixelColor(j*2 + offset + 1, redColor, greenColor, blueColor);
        }
        else
        {
            pixels.setPixelColor(j*2 + offset, 0, 0, 0);
			pixels.setPixelColor(j*2 + offset + 1, 0, 0, 0);
        }
        line >>= 1;
		
    }
}

/**
* Show or hide dot
*/
void showDot(bool yes) {
	if (yes == true) pixels.setPixelColor(31, redColor, greenColor, blueColor);
	if (yes == false) pixels.setPixelColor(31, 0, 0, 0);
}

/**
* Show or hide colon
*/
void showColon(bool yes) {
	if (yes == true) {
		pixels.setPixelColor(29, redColor, greenColor, blueColor);
		pixels.setPixelColor(30, redColor, greenColor, blueColor);
	}
	if (yes == false) {
		pixels.setPixelColor(29, 0, 0, 0);
		pixels.setPixelColor(30, 0, 0, 0);
	}
}

/**
* Clear selected @segment (leave him blank)
*/
void clearSegment(int8_t segment)
{
	showCharOnSegment(10, segment);
}

/**
* Test function - blink 5 time all pixels
*/
void testMode() {
	for (int i = 0; i<5; i++) {
		showCharOnSegment(8, 1);
		showCharOnSegment(8, 2);
		showCharOnSegment(8, 3);
		showCharOnSegment(8, 4);
		showColon(1);
		showDot(1);
		pixels.show();
		delay(500);
		clearSegment(1);
		clearSegment(2);
		clearSegment(3);
		clearSegment(4);
		showColon(0);
		showDot(0);
		pixels.show();
		delay(500);
	}
}

/**
* Convert @number to single chars. If @showZero is true then all segments on begining is 0, else is blank.
* Ex.
* @showZero = true
* @number = 99
* EXIT => 0099
*
* @showZero = false
* @number = 99
* EXIT => __99 where _ is blank segment
*
*/
void convertNumberToChars(int number, bool showZero) {

	// number not between 0-9999
	if (number < 0 || number > 9999) number = 0;

	// convert number to segment
	char1 = number / 1000;
	number = number % 1000;
	char2 = number / 100;
	number = number % 100;
	char3 = number / 10;
	char4 = number % 10;

	// if not showZero then replace 0 on begining to blank segment
	if (showZero == false) {
		if (char1 == 0) {
			char1 = 10;
			if (char2 == 0) {
				char2 = 10;
				if (char3 == 0) {
					char3 = 10;
					if (char4 == 0) {
						char4 = 10;
					}
				}
			}
		}
	}
}

void setup()
{

	Serial.begin(9600);

	// ledStrip init
	pixels.begin();
	//testMode();

	// Clock init
	//clock.begin();
	//clock.setDateTime(__DATE__, __TIME__);

	// set alarm to every 23:59:00
	//todo ustawienie na godzinê, a testy wy³¹czyæ
	//clock.setAlarm2(0, 23, 59, DS3231_MATCH_M);
	//clock.setAlarm1(0, 0, 0, 10, DS3231_MATCH_M); // ustawiona ka¿da 10 sekunda minuty do testów

	//todo odczyt z pamiêci ram zegara przy starcie
	//days = 0;

	/*showCharOnSegment(1, 1);
	showCharOnSegment(2, 2);
	showCharOnSegment(3, 3);
	showCharOnSegment(4, 4);
	pixels.show();
	delay(5000);
	showCharOnSegment(5, 1);
	showCharOnSegment(6, 2);
	showCharOnSegment(7, 3);
	showCharOnSegment(8, 4);
	pixels.show();
	delay(5000);
	showCharOnSegment(9, 1);
	showCharOnSegment(0, 2);
	showCharOnSegment(1, 3);
	showCharOnSegment(2, 4);
	pixels.show();*/

}

void loop()
{

  /* add main program code here */

}
