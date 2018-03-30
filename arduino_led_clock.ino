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
static const unsigned char font[] = {
	0b11111100111111, //0
	0b11000000110000, //1
	0b11110011001111, //2
	0b11110011111100, //3
	0b11001111110000, //4
	0b00111111111100, //5
	0b00111111111111, //6
	0b11110000110000, //7
	0b11111111111111, //8
	0b11111111111100, //9
	0b00000000000000, //10 BLANK
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
        offset = 32;
        break;
    case 4:
        offset = 46;
        break;
    }

    uint16_t line;
    line = font[number];

    for (int16_t j = 0; j < 7; j++)
    {
        if (line & 0b1)
        {
            pixels.setPixelColor(j + offset, redColor, greenColor, blueColor);
        }
        else
        {
            pixels.setPixelColor(j + offset, 0, 0, 0);
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


void setup()
{

  /* add setup code here */

}

void loop()
{

  /* add main program code here */

}
