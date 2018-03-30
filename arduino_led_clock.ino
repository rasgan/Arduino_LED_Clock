#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <DS3231.h>
#include <Bounce2.h>

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

// Buttons declaration
const int plusButtonPin = 2;
const int minusButtonPin = 3;
const int modeButtonPin = 4;

Bounce bouncePlusButton = Bounce();
Bounce bounceMinusButton = Bounce();
Bounce bounceModeButton = Bounce();

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

/*
 * Actual and prev mode
 * 1 - time
 * 2 - date
 * 3 - temp
 * 4 - days from accident
*/
int prevMode = 0;
int actualMode = 1;

// time help variables
String prevTime = "";
String prevDate = "";
String actualTime = "";
String actualDate = "";

// number showed on display
int display;

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
* Test function - blink @blinks time all pixels
*/
void testMode(int blinks) {
	for (int i = 0; i<blinks; i++) {
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


//todo zrobiæ w convert opcje do wyboru w konfiguracji czy true czy false
/**
 * Convert @number to single chars. Then send it to segments.
 * If @showZero is true then all segments on begining is 0, else is blank.
 *
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
void showDisplay(int number, bool showZero) {
	
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
	showCharOnSegment(char1, 1);
	showCharOnSegment(char2, 2);
	showCharOnSegment(char3, 3);
	showCharOnSegment(char4, 4);
}

/*
 * convert time and convert it to dipslay
*/
int convertTimeToDipslay() {
	int time = 0;
	actualTime = clock.dateFormat("i", dateTime);
	time = actualTime.toInt();
	actualTime = clock.dateFormat("H", dateTime);
	time = time + actualTime.toInt()*100;
	showColon(true);
	showDot(false);
	showMinus(false);
	return time;
}

/*
* Convert date and convert it to dipslay
*/
int convertDateToDipslay() {
	int date = 0;
	actualDate = clock.dateFormat("m", dateTime);
	date = actualDate.toInt();
	actualDate = clock.dateFormat("d", dateTime);
	date = date + actualDate.toInt() * 100;
	showColon(false);
	showDot(true);
	showMinus(false);
	return date;
}

//todo zrobiæ
void showMinus(bool show) {}

//todo komentarz
void setup()
{

	Serial.begin(9600);

	// ledStrip init
	pixels.begin();
	testMode(2);

	// Clock init
	clock.begin();
	clock.setDateTime(__DATE__, __TIME__);

	// set alarm to every 23:59:00
	clock.setAlarm2(0, 23, 59, DS3231_MATCH_M);
	
	//todo ustawienie na godzinê, a testy wy³¹czyæ
	//clock.setAlarm1(0, 0, 0, 10, DS3231_MATCH_S); // ustawiona ka¿da 10 sekunda minuty do testów

	//todo odczyt z pamiêci ram zegara przy starcie
	days = 0;
	//todo odczyt z pamiêci
	actualMode = 2;

}

//todo komentarz
void loop()
{
	// read actual date and time
	dateTime = clock.getDateTime();
	clock.forceConversion();

	Serial.println(clock.dateFormat("d-m-Y H:i:s - l", dateTime));

	// check if 23:59 then increment days
	if (clock.isAlarm2()) {
		days++;
		clock.clearAlarm2();
		//todo zapis dni do pamiêci
	}

	switch (actualMode)
	{
		// clock mode
	case 1: {
		// get actual time
		actualTime = clock.dateFormat("H:i", dateTime);
		// check is time or mode changed from last display
		if ((actualTime != prevTime) || (actualMode != prevMode)) {
			prevTime = actualTime;
			prevMode = actualMode;
			display = convertTimeToDipslay();
		}
		break; 
	}
	
	// date mode
	case 2: {
		// get actual time
		actualDate = clock.dateFormat("m-d", dateTime);
		// check is date or mode changed from last display
		if ((actualDate != prevDate) || (actualMode != prevMode)) {
			prevDate = actualDate;
			prevMode = actualMode;
			display = convertDateToDipslay();
		}
		break;
	}
	
	// cdays mode
	case 4: {
		display = days;
		break;
	}

	// dafault mode
	default:
		actualMode = 1;
		break;
	}

	// show sended chars
	showDisplay(display, true);
	pixels.show();
	delay(1000);

	


}

//todo kasowanie zegara
//todo ustawienie zegara na ileœ dni