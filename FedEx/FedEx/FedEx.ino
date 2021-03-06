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

int runSpeed = 70;

#define M_FORWARD 1
#define M_BACKWARD 2
#define M_STOP 0

uint8_t direction = M_STOP;

#define cornerQuotient 1

int lineFollowFlag = 10;

int maxRunSpeed = 200;
int minFallbackSpeed = 150;
int minRunSpeed = 70;
bool crashed = false;


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

int fuzzyAmount = 1;
int fuzzyCounter = 0;

int turnCounter = 0;

boolean automatic = true;

#define ir_cornerQuotient 2.5

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
  led.setColorAt(0, 255, 0, 0);
  led.setColorAt(1, 255, 0, 0);
  led.show();
  ov1812();
}


// the loop function runs over and over again until power down or reset
void loop() {

  startButton();

  while (automatic) {

    if (!crashed) {
      followTheLine();
    } else {
      evade();
    }
	  doNotCrash();
	  checkButton();
  }

  runSpeed = 150;
  stop();

  direction = M_STOP;

  while(true){
	ir_irMovement();
	ir_doNotCrash();
  }

}

void checkButton() {
	if (analogRead(7) < 100) {
		automatic = false;
		led.setColorAt(0, 0, 255, 0);
		led.setColorAt(1, 0, 255, 0);
		led.show();
		buzzer.tone(1000, 200);
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
      default: break;
    }
  }
}

void forward()
{
  motor1.run(-runSpeed);
  motor2.run(runSpeed);
  direction = M_FORWARD;
  turnCounter = 0;
}

void backward()
{
  motor1.run(minRunSpeed);
  motor2.run(-minRunSpeed);
  direction = M_BACKWARD;
  runSpeed = minRunSpeed;
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
  turnCounter++;
}

void left()
{
  motor1.run(int(runSpeed / cornerQuotient));
  motor2.run(runSpeed);
  turnCounter++;
}

void doNotCrash()
{
  if (direction == M_FORWARD && ultrasonic.distanceCm() < 10)
  {
    crashed = true;
  }
}


void startButton() {
  while (analogRead(7) > 100) {
    delay(50);
  }

  for (int i = 0; i < 5; i++) {
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
	if (runSpeed < maxRunSpeed) {
		if (speedupQuotient % 16 == 0){
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
	
		/*
		if (fuzzyCounter < fuzzyAmount) {
			fuzzyCounter++;
			return;
		} 
		*/

      fuzzyCounter = 0;
      runSpeed = minFallbackSpeed;

	  
		  left();
		

      if (lineFollowFlag == 10) backward();
	  
	  if (turnCounter < 220) {
			if (lineFollowFlag < 10) left();
			if (lineFollowFlag > 10) right();
			delay(5);
	  }
	  else {
		  turnCounter = -turnCounter;
		  lineFollowFlag = 20 - lineFollowFlag;
		  stop();
	  }
      break;
  }
}

void evadeleft() {
  motor1.run(90);
  motor2.run(125);
/*
  motor1.run(int(runSpeed / cornerQuotient));
  motor2.run(runSpeed);
*/ 
  delay(150);
}

void evaderight() {
  motor1.run(-90);
  motor2.run(-90);
  delay(250);
}

void evadeforward() {
  motor1.run(-125);
  motor2.run(125);
  direction = M_FORWARD;
  delay(250);
}

void evade() {
 stop();
 while (crashed) {
    while(ultrasonic.distanceCm() < 7) {
      evaderight();
    }
    evaderight();
    stop();
    while (true) {
      evadeforward();
      evadeleft();
      if (ultrasonic.distanceCm() < 7) {
        break;
      }

      switch (lineFinder.readSensors()) {
        case S1_IN_S2_IN:
        case S1_IN_S2_OUT:
        case S1_OUT_S2_IN:
          stop();
          crashed = false;
          return;
      }
    }
  }
}



void ir_irMovement()
{
  if(turnCounter == 4800) 
  {
    ir_forward();
  }
  
	if (ir.decode())
	{
		uint32_t value = ir.value;
		value = value >> 16 & 0xff;
		switch (value)
		{

		case IR_BUTTON_LEFT:
			ir_left();
			break;
		case IR_BUTTON_RIGHT:
			ir_right();
			break;
		case IR_BUTTON_UP:
			ir_forward();
			break;
		case IR_BUTTON_DOWN:
			ir_backward();
			break;
		case IR_BUTTON_SETTING:
			ir_stop();
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

void ir_forward()
{
	motor1.run(-runSpeed);
	motor2.run(runSpeed);
	direction = M_FORWARD;
  turnCounter = 0;
}

void ir_backward()
{
	motor1.run(runSpeed);
	motor2.run(-runSpeed);
	direction = M_BACKWARD;
  turnCounter = 0;
}

void ir_stop()
{
	motor1.run(0);
	motor2.run(0);
	direction = M_STOP;
  turnCounter = 0;
}

void ir_right()
{
	if (direction == M_FORWARD)
	{
		motor1.run(-runSpeed);
		motor2.run(int(runSpeed / ir_cornerQuotient));
		return;
	}
	if (direction == M_BACKWARD)
	{
		motor1.run(runSpeed);
		motor2.run(-int(runSpeed / ir_cornerQuotient));
		return;
	}
	if (direction == M_STOP)
	{
		motor1.run(-runSpeed / 2);
		motor2.run(-runSpeed / 2);
		return;
	}
 turnCounter++;
}

void ir_left()
{
	if (direction == M_FORWARD)
	{
		motor1.run(-int(runSpeed / ir_cornerQuotient));
		motor2.run(runSpeed);
		return;
	}

	if (direction == M_BACKWARD)
	{
		motor1.run(int(runSpeed / ir_cornerQuotient));
		motor2.run(-runSpeed);
		return;
	}
	if (direction == M_STOP)
	{
		motor1.run(runSpeed / 2);
		motor2.run(runSpeed / 2);
		return;
	}
 turnCounter++;
}

void ir_doNotCrash()
{
	if (direction == M_FORWARD && ultrasonic.distanceCm() < 5)
	{
		stop();
		buzzer.tone(1000, 200);	//Buzzer sounds 1200Hz for 1000ms
		delay(100);
		buzzer.tone(1000, 200);	//Buzzer sounds 1200Hz for 1000ms
	}
}
