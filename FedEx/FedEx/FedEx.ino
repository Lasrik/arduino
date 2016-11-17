/*
 Name:		FedEx.ino
 Created:	17.11.2016 16:45:56
 Author:	kages
*/

// the setup function runs once when you press reset or power the board
#include <MeMCore.h>

MeRGBLed led(0, 30);

MeDCMotor motor1(M1); //Motor1 is Left Motor
MeDCMotor motor2(M2); //Motor2 is Right Motor

MeBuzzer buzzer;

MeLineFollower lineFinder(PORT_2);

MeLightSensor lightSensor(PORT_8);

MeUltrasonicSensor ultrasonic(PORT_3);

MeIR ir;

int runSpeed = 50;

#define M_FORWARD 1
#define M_BACKWARD 2
#define M_STOP 0

uint8_t direction = M_STOP;

#define cornerQuotient 2

int lineFollowFlag = 10;

int minFallbackSpeed = 205;

#define NOTE_c 261 
#define NOTE_d 294
#define NOTE_e 329
#define NOTE_f 349 
#define NOTE_g 392 
#define NOTE_a 440 
#define NOTE_b 493
#define NOTE_C 523
#define NOTE_D 587
#define NOTE_E 659
// Define a special note, 'R', to represent a rest
#define NOTE_R 0

int speedupQuotient = 0;


void ov1812() {
	int eighth = 125;

	buzzer.tone(NOTE_R, eighth);
	buzzer.tone(NOTE_g, eighth);
	buzzer.tone(NOTE_C, eighth);
	buzzer.tone(NOTE_D, eighth);
	buzzer.tone(NOTE_E, eighth);
	buzzer.tone(NOTE_D, eighth);
	buzzer.tone(NOTE_C, eighth);
	buzzer.tone(NOTE_D, eighth);
	buzzer.tone(NOTE_E, 2 * eighth);
	buzzer.tone(NOTE_C, 2 * eighth);
	buzzer.tone(NOTE_C, 4 * eighth);

}

void setup() {
	led.setpin(13);
	Serial.begin(9600);
	pinMode(7, INPUT); //Define button pin as input
	ir.begin();
	led.setColorAt(0, 255, 0, 0);
	led.setColorAt(1, 255, 0, 0);
	led.show();
	ov1812();
}


// the loop function runs over and over again until power down or reset
void loop() {

	startButton();

	while (true) {
		//ledExample();
		//irMovement();
		
		followTheLine();
		//doNotCrash();
	}

	//buttonExample();
	//lineFinderExample();
  
}



void ledExample()
{
	led.setColor(255, 255, 255); //Set both LED to White
	led.show();					//Must use .show() to make new colour take effect.
	delay(500);

	led.setColorAt(0, 255, 0, 0); //Set LED0 (RGBLED1) (RightSide) to Red
	led.setColorAt(1, 0, 0, 255); //Set LED1 (RGBLED2) (LeftSide)  to Blue
	led.show();
	delay(500);

	led.setColorAt(0, 0, 0, 255); //Set LED0 (RGBLED1) (RightSide) to Blue
	led.setColorAt(1, 255, 0, 0); //Set LED1 (RGBLED2) (LeftSide)  to Red
	led.show();
	delay(500);
}

void motorExample()
{

	//motor.run() maximum speed is 255 to -255, 0 is stop
	motor1.run(-100); //Motor1 (Left)  forward is -negative
	motor2.run(100);	//Motor2 (Right) forward is +positive
	delay(500);

	motor1.run(100);	//Motor1 (Left)  backward is +positive
	motor2.run(-100); //Motor2 (Right) backward is -negative
	delay(500);

	motor1.stop(); //Stop Motor1 
	motor2.stop(); //Stop Motor1 
	delay(500);
}

void buzzerExample() {
	buzzer.tone(600, 1000);	//Buzzer sounds 600Hz for 1000ms
	delay(2000);			//Pause for 2000ms, Buzzer no sound
	buzzer.tone(1200, 1000);	//Buzzer sounds 1200Hz for 1000ms
	delay(2000);	//Pause for 2000ms, Buzzer no sound
}

void lineFinderExample()
{
	int sensorState = lineFinder.readSensors();
	switch (sensorState)
	{
	case S1_IN_S2_IN:	Serial.println("S1_IN_S2_IN"); break;
	case S1_IN_S2_OUT:	Serial.println("S1_IN_S2_OUT"); break;
	case S1_OUT_S2_IN:	Serial.println("S1_OUT_S2_IN"); break;
	case S1_OUT_S2_OUT:	Serial.println("S1_OUT_S2_OUT"); break;
	default: break;
	}
	delay(200);
}

void lightSensorExample()
{
	Serial.print("value = ");		// Print the results to the serial monitor
	Serial.println(lightSensor.read());	// Brightness value from 0-1023
	delay(50);						// Wait 50 milliseconds before next measurement
}

void ultrasonicExample()
{
	Serial.print("distance(cm) = ");	// Print the results to the serial monitor
	Serial.println(ultrasonic.distanceCm());	// Distance value from 3cm - 400cm
	delay(50);			// Wait 50 milliseconds before next measurement
}

void buttonExample() {
	if (analogRead(7) < 100) {
		Serial.println("Button Pressed");
	}
	else {
		Serial.println("Not Pressed");
	}
	delay(50);
}

void irExample()
{
	if (ir.decode())
	{
		uint32_t value = ir.value;
		Serial.print("Raw Value: ");
		Serial.println(value);
		value = value >> 16 & 0xff;
		Serial.print("Button Code: ");
		Serial.println(value);
		Serial.print("Button: ");
		switch (value)
		{
		case IR_BUTTON_A: Serial.println("A"); break;
		case IR_BUTTON_B: Serial.println("B"); break;
		case IR_BUTTON_C: Serial.println("C"); break;
		case IR_BUTTON_D: Serial.println("D"); break;
		case IR_BUTTON_E: Serial.println("E"); break;
		case IR_BUTTON_F: Serial.println("F"); break;
		case IR_BUTTON_SETTING: Serial.println("Setting"); break;
		case IR_BUTTON_LEFT: Serial.println("Left"); break;
		case IR_BUTTON_RIGHT: Serial.println("Right"); break;
		case IR_BUTTON_UP: Serial.println("Up"); break;
		case IR_BUTTON_DOWN: Serial.println("Down"); break;
		case IR_BUTTON_0: Serial.println("0"); break;
		case IR_BUTTON_1: Serial.println("1"); break;
		case IR_BUTTON_2: Serial.println("2"); break;
		case IR_BUTTON_3: Serial.println("3"); break;
		case IR_BUTTON_4: Serial.println("4"); break;
		case IR_BUTTON_5: Serial.println("5"); break;
		case IR_BUTTON_6: Serial.println("6"); break;
		case IR_BUTTON_7: Serial.println("7"); break;
		case IR_BUTTON_8: Serial.println("8"); break;
		case IR_BUTTON_9: Serial.println("9"); break;
		default:break;
		}
	}
}

void irMovement()
{
	if (ir.decode())
	{
		uint32_t value = ir.value;
		value = value >> 16 & 0xff;
		switch (value)
		{

		case IR_BUTTON_LEFT:
			left();
			break;
		case IR_BUTTON_RIGHT:
			right();
			break;
		case IR_BUTTON_UP:
			forward();
			break;
		case IR_BUTTON_DOWN:
			backward();
			break;
		case IR_BUTTON_SETTING:
			stop();
			break;
		case IR_BUTTON_0:
			runSpeed = 255 * 1 / 10;
			break;
		case IR_BUTTON_1:
			runSpeed = 255 * 2 / 10;
			break;
		case IR_BUTTON_2:
			runSpeed = 255 * 3 / 10;
			break;
		case IR_BUTTON_3:
			runSpeed = 255 * 4 / 10;
			break;
		case IR_BUTTON_4:
			runSpeed = 255 * 5 / 10;
			break;
		case IR_BUTTON_5:
			runSpeed = 255 * 6 / 10;
			break;
		case IR_BUTTON_6:
			runSpeed = 255 * 7 / 10;
			break;
		case IR_BUTTON_7:
			runSpeed = 255 * 8 / 10;
			break;
		case IR_BUTTON_8:
			runSpeed = 255 * 9 / 10;
			break;
		case IR_BUTTON_9:
			runSpeed = 255;
			break;
		default:break;
		}
	}
}

void forward()
{
	motor1.run(-runSpeed);
	motor2.run(runSpeed);
	direction = M_FORWARD;
}

void backward()
{
	motor1.run(runSpeed);
	motor2.run(-runSpeed);
	direction = M_BACKWARD;
}

void stop()
{
	motor1.run(0);
	motor2.run(0);
	direction = M_STOP;
}

void right()
{
		motor1.run(-runSpeed);
		motor2.run(-int(runSpeed / cornerQuotient));
}

void left()
{
		motor1.run(int(runSpeed / cornerQuotient));
		motor2.run(runSpeed);
}

void doNotCrash()
{
	if (direction == M_FORWARD && ultrasonic.distanceCm() < 7)
	{
		stop();
		left();
		buzzer.tone(1000, 200);	//Buzzer sounds 1200Hz for 1000ms
		backward();
		stop();
		delay(100);
		right();
		buzzer.tone(1000, 200);	//Buzzer sounds 1200Hz for 1000ms
		stop();
	}
}

void startButton() {

	while (analogRead(7) > 100) {
		delay(50);
	}

	for (int i = 0; i< 5; i++){
		led.setColorAt(0, 0, 255, 0);
		led.setColorAt(1, 0, 255, 0);
		led.show();
		delay(800);
		led.setColorAt(0, 0, 0, 0);
		led.setColorAt(1, 0, 0, 0);
		led.show();
		buzzer.tone(1000, 200);
	}

	led.setColorAt(0, 0, 0, 255);
	led.setColorAt(1, 0, 0, 255);
	led.show();
}


void followTheLine()
{
	if (runSpeed < 255) {
		if (speedupQuotient % 4 == 0){
			runSpeed++;
			speedupQuotient = 0;
		}
		else {
			speedupQuotient++;
		}
	}

	uint8_t val = lineFinder.readSensors();
	switch (val) {

	case S1_IN_S2_IN:
		forward();
		lineFollowFlag = 10;
		break;

	case S1_IN_S2_OUT:
		forward();
		if (lineFollowFlag > 1) lineFollowFlag--;
		break;

	case S1_OUT_S2_IN:
		forward();
		if (lineFollowFlag < 20) lineFollowFlag++;
		break;

	case S1_OUT_S2_OUT:
		runSpeed = minFallbackSpeed;
		if (lineFollowFlag == 10) backward();
		if (lineFollowFlag < 10) left();
		if (lineFollowFlag > 10) right();
		break;
	}
}