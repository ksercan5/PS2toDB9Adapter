// Version Update
// 1.0 Based on Code 2.1 for single adapter
// 27.02.2021
//
// Double Board for C64 with Arduino Nano
// by Sercan Kocabaş


#include <PS2X_lib.h>  //for v1.6

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT_2        2  //14    
#define PS2_CMD_2        3  //15
#define PS2_SEL_2        4  //16
#define PS2_CLK_2        5  //17

#define C64_UP_2         8
#define C64_DOWN_2       9
#define C64_LEFT_2       10
#define C64_RIGHT_2      11
#define C64_FIRE_2       12

#define PS2_DAT_1        1  //14    
#define PS2_CMD_1        0  //15
#define PS2_SEL_1        A5  //16
#define PS2_CLK_1        A4  //17

#define C64_UP_1         A3
#define C64_DOWN_1       A2
#define C64_LEFT_1       A1
#define C64_RIGHT_1      A0
#define C64_FIRE_1       13

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false

PS2X ps2x_1; // create PS2 Controller Class for Joy1
PS2X ps2x_2; // create PS2 Controller Class for Joy2


//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

byte error_1 = 0;
byte vibrate_1 = 0;

byte error_2 = 0;
byte vibrate_2 = 0;

void setup(){


  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(C64_FIRE_1, INPUT);
  pinMode(C64_UP_1, INPUT);
  pinMode(C64_DOWN_1, INPUT);
  pinMode(C64_RIGHT_1, INPUT);
  pinMode(C64_LEFT_1, INPUT);


  pinMode(C64_FIRE_2, INPUT);
  pinMode(C64_UP_2, INPUT);
  pinMode(C64_DOWN_2, INPUT);
  pinMode(C64_RIGHT_2, INPUT);
  pinMode(C64_LEFT_2, INPUT);
   
  
  
  //Serial.begin(57600); //Enable this to enable serial communication for troubleshooting, in this case JOY1 will not be available. 
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************

  Serial.println("Dualshock2 to DB9 (Commodore 64 --- Double Board ---) Adapter Software Ver 1.0");
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error_1 = ps2x_1.config_gamepad(PS2_CLK_1, PS2_CMD_1, PS2_SEL_1, PS2_DAT_1, pressures, rumble);
  
//  if(error_1 == 0){
//    Serial.print("Found Controller 1, configured successful ");
//    Serial.print("pressures = ");
//	if (pressures)
//	  Serial.println("true ");
//	else
//	  Serial.println("false");
//	Serial.print("rumble = ");
//	if (rumble)
//	  Serial.println("true)");
//	else
//	  Serial.println("false");
//    Serial.println("Note: Go to https://github.com/ksercan5/PS2toDB9Adapter for updates and to report bugs.");
//  }  
//  else if(error_1 == 1)
//    Serial.println("No controller 1 found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
//   
//  else if(error_1 == 2)
//    Serial.println("Controller 1 found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
//
//  else if(error_1 == 3)
//    Serial.println("Controller 1 refusing to enter Pressures mode, may not support it. ");
  
error_2 = ps2x_2.config_gamepad(PS2_CLK_2, PS2_CMD_2, PS2_SEL_2, PS2_DAT_2, pressures, rumble);
  
//  if(error_2 == 0){
//    Serial.print("Found Controller 2, configured successful ");
//    Serial.print("pressures = ");
//  if (pressures)
//    Serial.println("true ");
//  else
//    Serial.println("false");
//  Serial.print("rumble = ");
//  if (rumble)
//    Serial.println("true)");
//  else
//    Serial.println("false");
//    Serial.println("Note: Go to https://github.com/ksercan5/PS2toDB9Adapter for updates and to report bugs.");
//  }  
//  else if(error_2 == 1)
//    Serial.println("No controller 2 found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
//   
//  else if(error_2 == 2)
//    Serial.println("Controller 2 found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
//
//  else if(error_2 == 3)
//    Serial.println("Controller 2 refusing to enter Pressures mode, may not support it. ");
//  
//
}


void check_joy1() {
  ps2x_1.read_gamepad(false, vibrate_1);
  // Analog left for Joy 1
    if(ps2x_1.Analog(PSS_LY) < 30 ){
      pinMode(C64_UP_1, OUTPUT);
      digitalWrite(C64_UP_1, LOW);
//      Serial.println("Analog up");
      }  else if ((!ps2x_1.Button(PSB_CIRCLE))&&(!ps2x_1.Button(PSB_PAD_UP)))
      pinMode(C64_UP_1, INPUT);
  
    if(ps2x_1.Analog(PSS_LY) > 225 ){
      pinMode(C64_DOWN_1, OUTPUT);
      digitalWrite(C64_DOWN_1, LOW);
//      Serial.println("Analog down");
      }  else if (!ps2x_1.Button(PSB_PAD_DOWN))
      pinMode(C64_DOWN_1, INPUT);
    
    if(ps2x_1.Analog(PSS_LX) < 20 ){
      pinMode(C64_LEFT_1, OUTPUT);
      digitalWrite(C64_LEFT_1, LOW);
//      Serial.println("Analog left");
      }  else if (!ps2x_1.Button(PSB_PAD_LEFT))
      pinMode(C64_LEFT_1, INPUT);
      
    if(ps2x_1.Analog(PSS_LX) > 235 ){
      pinMode(C64_RIGHT_1, OUTPUT);
      digitalWrite(C64_RIGHT_1, LOW);
//      Serial.println("Analog right");
      }  else if (!ps2x_1.Button(PSB_PAD_RIGHT))
      pinMode(C64_RIGHT_1, INPUT);

    if(ps2x_1.ButtonPressed(PSB_CIRCLE)) {              //will be TRUE if button was JUST pressed
//      Serial.println("Circle just pressed");
      pinMode(C64_UP_1, OUTPUT);
      digitalWrite(C64_UP_1, LOW);
    }  
     if(ps2x_1.ButtonPressed(PSB_CROSS)) {              //will be TRUE if button was JUST pressed
//      Serial.println("Cross just pressed");
      pinMode(C64_FIRE_1, OUTPUT);
      digitalWrite(C64_FIRE_1, LOW);
    }  
    if(ps2x_1.Button(PSB_TRIANGLE)) {              //Autofire code
//      Serial.println("Autofire pressed (Square)");
      pinMode(C64_FIRE_1, OUTPUT);
      digitalWrite(C64_FIRE_1, LOW);
      delay(50);
      pinMode(C64_FIRE_1, INPUT);
    }  
    
    if (ps2x_1.ButtonPressed(PSB_PAD_UP)) {
//      Serial.println("Pad Up pressed");
      pinMode(C64_UP_1, OUTPUT);
      digitalWrite(C64_UP_1, LOW);
    }    
    if (ps2x_1.ButtonPressed(PSB_PAD_DOWN)) {
//      Serial.println("Pad Down pressed");
      pinMode(C64_DOWN_1, OUTPUT);
      digitalWrite(C64_DOWN_1, LOW);
    }
      if (ps2x_1.ButtonPressed(PSB_PAD_LEFT)) {
//      Serial.println("Pad Left pressed");
      pinMode(C64_LEFT_1, OUTPUT);
      digitalWrite(C64_LEFT_1, LOW);
    }

    if (ps2x_1.ButtonPressed(PSB_PAD_RIGHT)) {
//      Serial.println("Pad Right pressed");
      pinMode(C64_RIGHT_1, OUTPUT);
      digitalWrite(C64_RIGHT_1, LOW);
    }   

    //Button released part Joy 1
 
    if(ps2x_1.ButtonReleased(PSB_CIRCLE)) {             //will be TRUE if button was JUST released
//      Serial.println("Circle just released");     
      pinMode(C64_UP_1, INPUT);
    }

    if(ps2x_1.ButtonReleased(PSB_CROSS)) {             //will be TRUE if button was JUST released
//      Serial.println("Cross just released");     
      pinMode(C64_FIRE_1, INPUT);
    }

    if(ps2x_1.ButtonReleased(PSB_TRIANGLE)) {             //will be TRUE if button was JUST released
//      Serial.println("Square just released");     
      pinMode(C64_FIRE_1, INPUT);
    }
    
    if (ps2x_1.ButtonReleased(PSB_PAD_UP)) {
//      Serial.println("Pad Up Released");
      pinMode(C64_UP_1, INPUT);
    }    

    if (ps2x_1.ButtonReleased(PSB_PAD_DOWN)) {
//      Serial.println("Pad Down Released");
      pinMode(C64_DOWN_1, INPUT);
    }    
    
    if (ps2x_1.ButtonReleased(PSB_PAD_LEFT)) {
//      Serial.println("Pad Left Released");
      pinMode(C64_LEFT_1, INPUT);
    }    
    
    if (ps2x_1.ButtonReleased(PSB_PAD_RIGHT)) {
//      Serial.println("Pad Rıght Released");
      pinMode(C64_RIGHT_1, INPUT);
    }  
}


void check_joy2() {
  ps2x_2.read_gamepad(false, vibrate_2);
// Analog left for Joy 2

    if(ps2x_2.Analog(PSS_LY) < 30 ){
      pinMode(C64_UP_2, OUTPUT);
      digitalWrite(C64_UP_2, LOW);
//      Serial.println("Analog up");
      }  else if ((!ps2x_2.Button(PSB_CIRCLE))&&(!ps2x_2.Button(PSB_PAD_UP)))
      pinMode(C64_UP_2, INPUT);
  
    if(ps2x_2.Analog(PSS_LY) > 225 ){
      pinMode(C64_DOWN_2, OUTPUT);
      digitalWrite(C64_DOWN_2, LOW);
 //     Serial.println("Analog down");
      }  else if (!ps2x_2.Button(PSB_PAD_DOWN))
      pinMode(C64_DOWN_2, INPUT);
    
    if(ps2x_2.Analog(PSS_LX) < 20 ){
      pinMode(C64_LEFT_2, OUTPUT);
      digitalWrite(C64_LEFT_2, LOW);
//      Serial.println("Analog left");
      }  else if (!ps2x_2.Button(PSB_PAD_LEFT))
      pinMode(C64_LEFT_2, INPUT);
      
    if(ps2x_2.Analog(PSS_LX) > 235 ){
      pinMode(C64_RIGHT_2, OUTPUT);
      digitalWrite(C64_RIGHT_2, LOW);
//      Serial.println("Analog right");
      }  else if (!ps2x_2.Button(PSB_PAD_RIGHT))
      pinMode(C64_RIGHT_2, INPUT);

    if(ps2x_2.ButtonPressed(PSB_CIRCLE)) {              //will be TRUE if button was JUST pressed
//      Serial.println("Circle just pressed");
      pinMode(C64_UP_2, OUTPUT);
      digitalWrite(C64_UP_2, LOW);
    }  
    if(ps2x_2.ButtonPressed(PSB_CROSS)) {              //will be TRUE if button was JUST pressed
//      Serial.println("Cross just pressed");
      pinMode(C64_FIRE_2, OUTPUT);
      digitalWrite(C64_FIRE_2, LOW);
    } 

    if(ps2x_2.Button(PSB_TRIANGLE)) {              //Autofire code
//      Serial.println("Autofire pressed (Square)");
      pinMode(C64_FIRE_2, OUTPUT);
      digitalWrite(C64_FIRE_2, LOW);
      delay(50);
      pinMode(C64_FIRE_2, INPUT);
    }  
     if (ps2x_2.ButtonPressed(PSB_PAD_UP)) {
//      Serial.println("Pad Up pressed");
      pinMode(C64_UP_2, OUTPUT);
      digitalWrite(C64_UP_2, LOW);
    }
    if (ps2x_2.ButtonPressed(PSB_PAD_DOWN)) {
//      Serial.println("Pad Down pressed");
      pinMode(C64_DOWN_2, OUTPUT);
      digitalWrite(C64_DOWN_2, LOW);
    }
    if (ps2x_2.ButtonPressed(PSB_PAD_LEFT)) {
//      Serial.println("Pad Left pressed");
      pinMode(C64_LEFT_2, OUTPUT);
      digitalWrite(C64_LEFT_2, LOW);
    }
    if (ps2x_2.ButtonPressed(PSB_PAD_RIGHT)) {
//      Serial.println("Pad Right pressed");
      pinMode(C64_RIGHT_2, OUTPUT);
      digitalWrite(C64_RIGHT_2, LOW);
    }   

     //Button released part Joy 2

    if(ps2x_2.ButtonReleased(PSB_CIRCLE)) {             //will be TRUE if button was JUST released
//      Serial.println("Circle just released");     
      pinMode(C64_UP_2, INPUT);
    }

    if(ps2x_2.ButtonReleased(PSB_CROSS)) {             //will be TRUE if button was JUST released
//      Serial.println("Cross just released");     
      pinMode(C64_FIRE_2, INPUT);
    }

    if(ps2x_2.ButtonReleased(PSB_TRIANGLE)) {             //will be TRUE if button was JUST released
//      Serial.println("Square just released");     
      pinMode(C64_FIRE_2, INPUT);
    }
    
    if (ps2x_2.ButtonReleased(PSB_PAD_UP)) {
//      Serial.println("Pad Up Released");
      pinMode(C64_UP_2, INPUT);
    }    

    if (ps2x_2.ButtonReleased(PSB_PAD_DOWN)) {
//      Serial.println("Pad Down Released");
      pinMode(C64_DOWN_2, INPUT);
    }    
    
    if (ps2x_2.ButtonReleased(PSB_PAD_LEFT)) {
//      Serial.println("Pad Left Released");
      pinMode(C64_LEFT_2, INPUT);
    }    
    
    if (ps2x_2.ButtonReleased(PSB_PAD_RIGHT)) {
//      Serial.println("Pad Rıght Released");
      pinMode(C64_RIGHT_2, INPUT);
    }   
}

void loop() {
  if (error_1 == 0) check_joy1();
  if (error_2 == 0) check_joy2();
  delay(25);   //Default 50
}
