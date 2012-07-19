#include <BigDigitDriver.h>



//**************************************************************//
//  Name    : Personality Test Interactive
//  Author  : Michael Edwards, American Museum of Natural History Exhibit Media Dept.
//  Date Authored   : 7 Jul, 2011
//  Updated : Cameron Browning and Katie
//  Updated : 7-19-2012 
//  Version : 1.1b
//  Notes   : requires libBigDigitDriver: https://github.com/camb416/libBigDigitDriver
/*
#include <WProgram.h>
#include <stdlib.h>
*/

const int PINS = 6;
const static unsigned long TIMEOUT_INPUT = 5000;
const static unsigned long TIMEOUT_SCORE = 5000;
const static unsigned long TIMEOUT_BLINK = 250;

const static int analogPins[] = {A0,A1,A2,A3,A4,A5};

int rawScoreValues[PINS];
int scoreValues[PINS];
int rawSum;
int score;
bool isInput;
bool isScored;
unsigned long inputMillis;
unsigned long scoreMillis;
static int buttonPin = 8;
static int ledPin = 9;

BigDigitDriver bdd(10,11,12,A0,false,true);

void setup() {
	Serial.begin(9600);
	pinMode(buttonPin, INPUT);
	pinMode(ledPin, OUTPUT);
	bdd.startup();
	for (int i = 0; i < PINS; i++) {
		rawScoreValues[i] = 0;
		scoreValues[i] = 0;
	}
	rawSum = 0;
	Serial.println("STARTUP...");
	score = 35;
	bdd.digitArray[7] = 7;
	bdd.digitArray[8] = 8;
	bdd.updateDisplays();
}

void loop() {
	/*
	 * 50-day reset: makes sure program functions if on for 50+ days, corrects for millis()
         * rolling over to 0
	 */
	if (scoreMillis > millis())
		scoreMillis = millis();
	if (inputMillis > millis())
		inputMillis = millis();

	/*
	 * Check for changes in input
	 */
	rawSum = 0;
	score=0;
	for (int i = 0; i < PINS; i++) {
		rawScoreValues[i] = 1023-analogRead(analogPins[i]);
		int temp = constrain((rawScoreValues[i] + 128)/255,0,1023);
		if (scoreValues[i] != temp)  {                                // if temp doesn't match old value
                        scoreValues[i] = temp;                               // update the scores array
			inputMillis = millis();
			isInput = true;                                      // set bools that keep track of the changes
			isScored = false;
		}
	}
        
        /*
         * Sum the scores
         */
        for (int i = 0; i < PINS; i++)
          score += scoreValues[i];
          
	score += PINS;                      // dials read from 0 to 5, we want scores to be from 0 to 6
                                            // adding PINS corrects this
        /*
         * Checks for button press
         */
	if (digitalRead(buttonPin) == HIGH) {
                Serial.println("pressed");
		isScored = true;
		scoreMillis = millis();
	}

	/*
	 * Displays output
	 */
	if (isScored)                                              // if the button's been pressed...
	{
		if ((millis()/TIMEOUT_BLINK)%2 == 0) {             // creates blinking effect
			bdd.digitArray[7] = 10;
			bdd.digitArray[8] = 10;
		}

		else {
			if (score/10 >= 1)                        // score >= 10, make left digit the 10's digit
				bdd.digitArray[7] = score/10;
			else
				bdd.digitArray[7] = 10;          // score < 10, make left digit blank
                        //Serial.println(score%10);
			bdd.digitArray[8] = score%10;            // either way, make right digit the 1's digit
		}

		bdd.updateDisplays();
		digitalWrite(ledPin,LOW);
	}
	else {                                                    // blank display if the button wasn't pressed
		bdd.digitArray[7] = 10;
		bdd.digitArray[8] = 10;
		bdd.updateDisplays();

		if (isInput)
			digitalWrite(ledPin,HIGH);
		else
			digitalWrite(ledPin,LOW);
	}
	/*
	 * Reset timeouts
	 */
	if (millis() - scoreMillis > TIMEOUT_SCORE)
		isScored = false;

	if (millis() - inputMillis > TIMEOUT_INPUT)
		isInput = false;
}


