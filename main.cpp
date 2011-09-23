//**************************************************************//
//  Name    : Personality Test Interactive
//  Author  : Michael Edwards, American Museum of Natural History Exhibit Media Dept.
//  Date    : 7 Jul, 2011
//  Version : 0.1
//  Notes   :
//****************************************************************

#include <WProgram.h>
#include <BigDigitDriver.h>
#include <stdlib.h>

extern "C" void __cxa_pure_virtual()
{
  cli();
  for (;;);
}

__extension__ typedef int __guard __attribute__((mode (__DI__)));

const int PINS = 6;
const unsigned long TIMEOUT_INPUT = 10000;
const unsigned long TIMEOUT_SCORE = 10000;

const int analogPins[] = {A0,A1,A2,A3,A4,A5};

int rawScoreValues[PINS];
int scoreValues[PINS];
int rawSum;
int score;
bool isInput;
bool isScored;
unsigned long inputMillis;
unsigned long scoreMillis;
int buttonPin = 8;
int ledPin = 9;

void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}

void * operator new[](size_t size)
{
    return malloc(size);
}

void operator delete[](void * ptr)
{
	if (ptr)
		free(ptr);
}

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}
void __cxa_guard_release (__guard *g) {*(char *)g = 1;}
void __cxa_guard_abort (__guard *) {}

BigDigitDriver bdd(10,11,12,A0,false,true);

int hello[] = {10,10,10,12,11,13,15,14,10,13,16,1,5,1,5,10,2,0,1,1};

int main(void) {
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

void setup() {
	Serial.begin(9600);
	pinMode(buttonPin, INPUT);
	pinMode(ledPin, OUTPUT);
	bdd.startup();
	for (int i = 0; i < PINS; i++)
	{
		rawScoreValues[i] = 0;
		scoreValues[i] = 0;
	}
	rawSum = 0;
	Serial.println("STARTUP...");
	score = 35;
	bdd.digitArray[0] = 0;
	bdd.digitArray[1] = 1;
	bdd.digitArray[2] = 2;
	bdd.digitArray[3] = 3;
	bdd.digitArray[4] = 4;
	bdd.digitArray[5] = 5;
	bdd.digitArray[6] = 0;
	bdd.digitArray[7] = 0;
	bdd.digitArray[8] = 8;
	bdd.updateDisplays();
//	bdd.updateDisplays();
}

void loop() {
	//Serial.println("heartbeat...");

	/*
	 * 50-day reset
	 */
	if (scoreMillis > millis())
	{
		scoreMillis = millis();
	}
	if (inputMillis > millis())
	{
		inputMillis = millis();
	}

	/*
	 * check for changes in input or button
	 */
	rawSum = 0;
	score=0;
	for (int i = 0; i < PINS; i++)
	{
		rawScoreValues[i] = 1023-analogRead(analogPins[i]);
		int temp = constrain((rawScoreValues[i] + 128)/255,0,1023);
		if (scoreValues[i] != temp)
		{
			Serial.print("RAW: ");
			Serial.print(rawScoreValues[i]);
			Serial.print(" -> INPUT ON PIN ");
			Serial.print(i);
			Serial.print(" ");
			Serial.print(temp);
			Serial.print(" vs ");
			Serial.println(scoreValues[i]);
			inputMillis = millis();
			isInput = true;
			isScored = false;
		}
		scoreValues[i] = temp;
		score += scoreValues[i];
	}
	score += PINS;

	if (digitalRead(buttonPin) == HIGH)
	{
		isScored = true;
		scoreMillis = millis();
	}

	/*
	 * display outputs
	 */
	if (isScored)
	{
		if (score/10 >= 1)
			bdd.digitArray[6] = score/10;
		else
			bdd.digitArray[6] = 10;
		bdd.digitArray[7] = score%10;
		bdd.updateDisplays();
		digitalWrite(ledPin,LOW);
	}
	else
	{
		if (isInput)
		{
			bdd.digitArray[6] = DISP_DASH;
			bdd.digitArray[7] = DISP_DASH;
			bdd.updateDisplays();
			//TODO: added blinking
			digitalWrite(ledPin,HIGH);
		}
		else
		{
			bdd.digitArray[6] = 10;
			bdd.digitArray[7] = 10;
			bdd.updateDisplays();
			digitalWrite(ledPin,LOW);
		}
	}

	/*
	 * reset timeouts
	 */
	if (millis() - scoreMillis > TIMEOUT_SCORE)
	{
		isScored = false;
	}

	if (millis() - inputMillis > TIMEOUT_INPUT)
	{
		isInput = false;
	}

}

