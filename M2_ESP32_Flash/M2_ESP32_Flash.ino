
#include "Arduino.h"
#include <stdint.h>
#include "Debounce.h"
#include <variant.h>

// ************************************************************************************************************ //
//										Button Denounce Defines													//
// ************************************************************************************************************ //
debounce_t M2_Button1;
uint8_t Debounce_Delay = 20;	// this is the number of milli seconds delay before checking the button again in Debounce

uint32_t Button_endMillis;
uint32_t Button_startMillis;

bool led = 0;
bool Red_Led_State = 0;
bool toggleBootMode = false;
bool passThrough = false;
bool pipe = false;
bool loopThrough = false;

const int Red_Led = DS2;	// Red LED 32
const int XB_Reset = XBEE_RST;
const int XB_Program = XBEE_MULT4;
const int ESP2_Button = XBEE_MULT4;
const int LED_YELLOW = DS3;

int doDelay = 0;
uint32_t passThroughLastEvent = 0;

/*\brief Interrupt Service Routine for Button 1 on M2 Processor Board
*	Sets the variable M2Button1.Pressed=TRUE
*	to indicate button has been pressed
*\param NIL
*\return NIL
*/
void ISR_Button1(){
	M2_Button1.Pressed = true;
}

bool Button_Debounce(debounce_t *Button){
	// read the state of the switch into a local variable:
	Button->reading = Button->Pressed;
	Button->Pressed = false;

	// check to see if you just pressed the button
	// (i.e. the input went from HIGH to LOW),  and you've waited
	// long enough since the last press to ignore any noise:

	// If the switch changed, due to noise or pressing:
	if(Button->reading != Button->lastButtonState){
		// reset the debouncing timer
		Button->lastDebounceTime = millis();
	}

	if((millis() - Button->lastDebounceTime) > Debounce_Delay){
		// whatever the reading is at, it's been there for longer
		// than the debounce delay, so take it as the actual current state:

		// if the button state has changed:
		if(Button->reading != Button->buttonState){

			// only toggle Pressed if the new button state is LOW
			if(Button->buttonState == LOW){
				Button->Pressed = true;
			}
			Button->buttonState = Button->reading;
		}
	}

	// save the reading.  Next time through the loop,
	// it'll be the lastButtonState:
	Button->lastButtonState = Button->reading;

	return(Button->Pressed);
}

void Flash_Red_Led(){   // Flash the RED led
	digitalWrite(Red_Led, Red_Led_State ? HIGH : LOW);
	Red_Led_State = Red_Led_State ? 0 : 1;
	delay(10);
	digitalWrite(Red_Led, Red_Led_State ? HIGH : LOW);
}

void setup(){
	M2_Button1.M2_Button = Button1;
	M2_Button1.lastButtonState = LOW;
	M2_Button1.buttonState = LOW;
	M2_Button1.Pressed = false;
	M2_Button1.FlipFlop = false;
	pinMode(Button1, INPUT);	/* Set Button 1 as input */
	digitalWrite(Button1, HIGH);
	attachInterrupt(Button1, ISR_Button1, LOW);	/* ISR Button 1 */

	SerialUSB.begin(115200);
	while(!SerialUSB);
	Serial.begin(115200);
	while(!Serial);

	SerialUSB.println("System Reset");
	SerialUSB.println("> simple menu");
	SerialUSB.print("\nr = Reset\nb = Boot Mode\np = Pass Through\nl = Wifi Module Loopback Mode\n\n");

	pinMode(XB_Reset, OUTPUT);
	digitalWrite(XB_Reset, HIGH);

	pinMode(XB_Program, OUTPUT);
	digitalWrite(XB_Program, HIGH);

	pinMode(LED_YELLOW, OUTPUT);
	digitalWrite(LED_YELLOW, LOW);

	pinMode(Red_Led, OUTPUT);
	digitalWrite(Red_Led, HIGH);

	// reboot ESP32 on startup
	digitalWrite(XB_Reset, LOW);

	toggleBootMode = true;
	doDelay = 500;
	M2_Button1.Pressed = false;
	Red_Led_State = 1;
}

void loop(){

	if(M2_Button1.Pressed){
		Button_Debounce(&M2_Button1);
		M2_Button1.FlipFlop = !M2_Button1.FlipFlop;
		Red_Led_State = 0;
		Flash_Red_Led();
		if(toggleBootMode || passThrough){
			passThroughLastEvent = millis();
            digitalWrite(XB_Reset, LOW);
		} else{
			if(loopThrough){
				loopThrough = false;
				setReset();
			}
		}
	}

	if(doDelay > 0){
		SerialUSB.print("Delay: ");
		SerialUSB.print(doDelay);

		delay(doDelay);

		SerialUSB.print(".\n> ");

		doDelay = 0;
	}

	if(passThroughLastEvent > 0 && millis() - passThroughLastEvent > 3000){
		SerialUSB.print(millis());
		SerialUSB.print(" - ");
		SerialUSB.print(passThroughLastEvent);
		SerialUSB.print(" = ");
		SerialUSB.println(millis() - passThroughLastEvent);
		passThroughLastEvent = 0;
		passThrough = false;
		digitalWrite(XB_Program, HIGH);

		setReset();
	}

	if(toggleBootMode){
		digitalWrite(XB_Reset, HIGH);
		toggleBootMode = false;
	}

	if(SerialUSB.available()){

		serialEventUSB();

	}
}

void setReset(){
	SerialUSB.println("\n\nReseting ESP32");

	digitalWrite(XB_Reset, LOW);

	toggleBootMode = true;
	doDelay = 500;
}

void serialEvent() {
	// 'activity' light
	digitalWrite(LED_YELLOW, led ? HIGH : LOW);
	led = led ? 0 : 1;

	// Serial data from XB_ can just be passed straight through
	while(Serial.available()){
		SerialUSB.write((uint8_t) Serial.read());
		SerialUSB.flush();
	}
}

void serialEventUSB() {
  // 'activity' light
  digitalWrite(LED_YELLOW, led ? HIGH : LOW);
  led = led ? 0 : 1;

  while(SerialUSB.available()){
	  uint8_t c = SerialUSB.read();

	  if(pipe && c == 0x03){
		  pipe = false;
		  SerialUSB.println("Ending pipe");
		  return;
	  }

	  // if already in flash mode just write and stop
	  if(pipe || passThrough){
		  Serial.write(c);
		  Serial.flush();
		  return;
	  }

	  // proper data coming back from ESP32 can be passed back
	  SerialUSB.write(c);
	  SerialUSB.flush();

	  // simple command set
	  // b: enter boot mode
	  // r: reset

	  // both b and r lower nRST
	  // and toggle bootmode (back up in 500ms)
	  if(c == 'b' || c == 'r'){
		  SerialUSB.println("\nReseting ESP32");

		  digitalWrite(XB_Reset, LOW);

		  toggleBootMode = true;
		  doDelay = 500;
	  }

	  // b also lowers GPIO0 and enables pass-through
	  if(c == 'b'){
		  Serial.begin(115200);

		  digitalWrite(XB_Program, LOW);

		  passThrough = true;
	  } else if(c == 'p'){
		  SerialUSB.println("\nPiping data to ESP32");
		  pipe = true;
	  } else if(c == '\n'){
		  SerialUSB.print("> ");
	  }
  }
}
