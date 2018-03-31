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

/*
 * Colors code to weeks
 */
static const byte colors[] = {
	255,0,0,
	255,6,0,
	255,13,0,
	255,21,0,
	255,31,0,
	255,41,0,
	255,52,0,
	255,64,0,
	255,76,0,
	255,89,0,
	255,101,0,
	255,114,0,
	255,128,0,
	255,141,0,
	255,154,0,
	255,167,0,
	255,180,0,
	255,193,0,
	255,204,0,
	255,215,0,
	255,225,0,
	255,234,0,
	255,242,0,
	255,250,0,
	254,255,0,
	249,255,0,
	243,255,0,
	236,255,0,
	228,255,0,
	219,255,0,
	209,255,0,
	200,255,0,
	190,255,0,
	179,255,0,
	168,255,0,
	156,255,0,
	144,255,0,
	134,255,0,
	121,255,0,
	109,255,0,
	98,255,0,
	87,255,0,
	75,255,0,
	65,255,0,
	54,255,0,
	45,255,0,
	35,255,0,
	27,255,0,
	18,255,0,
	12,255,0,
	6,255,0,
	0,255,0
};

// Buttons declaration
const int plusButtonPin = 7;
const int minusButtonPin = 5;
const int modeButtonPin = 6;

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
const int ledsInStrip = 62; // leds in strip
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(ledsInStrip, ledStripPin, NEO_GRB + NEO_KHZ800); // strip object

// Strip actual color
byte redColor = 200; // strip color red
byte greenColor = 10; // strip color green
byte blueColor = 10; // strip color blue

// help variable isBlinking
boolean isBlinking = false;

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
String prevTemp = "";
String actualTime = "";
String actualDate = "";
String actualTemp = "";

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
* Show or hide minus
*/
void showMinus(bool yes) {
	if (yes == true) {
		pixels.setPixelColor(0, redColor, greenColor, blueColor);
	}
	if (yes == false) {
		pixels.setPixelColor(0, 0, 0, 0);
	}
}

/**
* Show or hide celsius
*/
void showCelsius(bool yes) {
	if (yes == true) {
		pixels.setPixelColor(60, redColor, greenColor, blueColor);
	}
	if (yes == false) {
		pixels.setPixelColor(60, 0, 0, 0);
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
		showCelsius(1);
		pixels.show();
		delay(500);
		clearSegment(1);
		clearSegment(2);
		clearSegment(3);
		clearSegment(4);
		showColon(0);
		showDot(0);
		showCelsius(0);
		pixels.show();
		delay(500);
	}
}

/**
 * Convert @number to single chars. Then send it to segments.
 * If @showZero is true then all segments on begining is 0, else is blank.
 * If @blink is 1 then show digits, else digits are cleared
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
void showDisplay(int number, bool showZero, bool blink) {
	
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
	if (blink == 0) {
		showCharOnSegment(char1, 1);
		showCharOnSegment(char2, 2);
		showCharOnSegment(char3, 3);
		showCharOnSegment(char4, 4);
	}
	else {
		clearSegment(1);
		clearSegment(2);
		clearSegment(3);
		clearSegment(4);
	}
	
}

/*
 * Get time and convert it to dipslay
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
	showCelsius(false);
	return time;
}

/*
* Get date and convert it to dipslay
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
	showCelsius(false);
	return date;
}

/*
* Get temperature and convert it to dipslay
*/
int convertTempToDipslay() {
	int temp = 0;
	temp = actualTemp.toFloat() * 100.00;
	showMinus(false);
	if (temp < 0) {
		showMinus(true);
	}
	showCelsius(true);
	showColon(false);
	showDot(true);
	return temp;
}

/*
 * Change color on every week
 */
void changeRGBColors() {
	int color;
	color = days / 7;
	color = 3 * color;
	redColor = colors[color];
	greenColor = colors[color+1];
	blueColor = colors[color+2];
}

/*
 * Setup
*/
void setup()
{

	Serial.begin(9600);

	// ledStrip init
	pixels.begin();
	testMode(2);
	showCelsius(false);
	showColon(false);
	showDot(false);
	showMinus(false);

	// Clock init
	clock.begin();
	clock.setDateTime(__DATE__, __TIME__);

	//todo read from ds3231 clock memory on start
	days = 0;
	//todo read from ds3231 clock memory on start
	actualMode = 3;

	// Set buttons mode
	pinMode(plusButtonPin, INPUT_PULLUP);
	pinMode(minusButtonPin, INPUT_PULLUP);
	pinMode(modeButtonPin, INPUT_PULLUP);
	bouncePlusButton.attach(plusButtonPin);
	bouncePlusButton.interval(50);
	bounceMinusButton.attach(minusButtonPin);
	bounceMinusButton.interval(50);
	bounceModeButton.attach(modeButtonPin);
	bounceModeButton.interval(50);

}

/*
 * Mian function
*/
void loop()
{
	// read actual date and time
	dateTime = clock.getDateTime();
	clock.forceConversion();
	actualTemp = clock.readTemperature();

	// check if 23:59 then increment days
	if (clock.dateFormat("H:i", dateTime) == "23:59") {
		days++;
		//todo save to ds3231 memory
	}

	//change RGB colors
	changeRGBColors();

	// update buttons
	bouncePlusButton.update();
	bounceMinusButton.update();
	bounceModeButton.update();

	// change actualMode
	if (bounceModeButton.risingEdge()) {
		actualMode++;
		// normal mode display
		if (actualMode > 4 && actualMode < 99) actualMode = 1;
	}

	// enter the options mode
	if (bounceModeButton.read() == LOW && bouncePlusButton.risingEdge()) {
		actualMode = actualMode * 100;
		if (actualMode > 1000) actualMode = 100;
	}
	
	// Show display selected by actualMode
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

	// temp mode
	case 3: {
		if ((actualTemp != prevTemp) || (actualMode != prevMode)) {
			prevTemp = actualTemp;
			prevMode = actualMode;
			display = convertTempToDipslay();
		}
		break;
	}
	
	// days mode
	case 4: {
		display = days;
		showCelsius(false);
		showColon(false);
		showDot(false);
		showMinus(false);
		if (bounceModeButton.read() == LOW && bounceMinusButton.read() == LOW) {
			days = 0;
		}
		break;
	}

	// days options
	case 400: {
		display = days;
		if (isBlinking == true) {
			isBlinking = false;
		}
		else {
			isBlinking = true;
		}
	}

	// days options - change number of days
	case 401: {
		display = days;
		if (isBlinking == true) {
			isBlinking = false;
		}
		else {
			isBlinking = true;
		}
		// increment days
		if (bouncePlusButton.risingEdge()) {
			days++;
			if (days > 9999) days = 0;
		}
		// decrement days
		if (bounceMinusButton.risingEdge()) {
			days--;
			if (days <0) days = 9999;
		}
		// back to display mode
		if (bounceModeButton.fallingEdge()) {
			actualMode = 4;
			isBlinking = false;
			//todo save days to ds3231 memory
		}
	}

	// dafault mode
	default:
		//actualMode = 1; //fixme ??
		break;
	}

	// show sended chars
	showDisplay(display, true, isBlinking);
	pixels.show();
}

//todo modify mechanical project, buttons on down right
//TODO save settings to memmory
//todo add option to change collors on time
//todo add option to change collors on date
//todo add diff colors to temp
//todo in config mode select option to show following zero or not @showDisplay