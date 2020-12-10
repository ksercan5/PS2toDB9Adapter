#include <PS2X_lib.h>  //for v1.6

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        13  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17

#define C64_UP         6
#define C64_DOWN       5
#define C64_LEFT       4
#define C64_RIGHT      9
#define C64_FIRE       8


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

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup(){
 
  pinMode(C64_FIRE, INPUT);
  pinMode(C64_UP, INPUT);
  pinMode(C64_DOWN, INPUT);
  pinMode(C64_RIGHT, INPUT);
  pinMode(C64_LEFT, INPUT);
    
  Serial.begin(57600);
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
	Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
//  Serial.print(ps2x.Analog(1), HEX);
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
	case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
   }
}

void loop() {
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  if(error == 1) //skip loop if no controller found
    return; 
  

  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");      
    
    if(ps2x.Analog(PSS_LY) < 30 ){
      pinMode(C64_UP, OUTPUT);
      digitalWrite(C64_UP, LOW);
      Serial.println("Analog up");
      }  else if ((!ps2x.Button(PSB_CIRCLE))&&(!ps2x.Button(PSB_PAD_UP)))
      pinMode(C64_UP, INPUT);
  
    if(ps2x.Analog(PSS_LY) > 225 ){
      pinMode(C64_DOWN, OUTPUT);
      digitalWrite(C64_DOWN, LOW);
      Serial.println("Analog down");
      }  else if (!ps2x.Button(PSB_PAD_DOWN))
      pinMode(C64_DOWN, INPUT);
    
    if(ps2x.Analog(PSS_LX) < 20 ){
      pinMode(C64_LEFT, OUTPUT);
      digitalWrite(C64_LEFT, LOW);
      Serial.println("Analog left");
      }  else if (!ps2x.Button(PSB_PAD_LEFT))
      pinMode(C64_LEFT, INPUT);
      
    if(ps2x.Analog(PSS_LX) > 235 ){
      pinMode(C64_RIGHT, OUTPUT);
      digitalWrite(C64_RIGHT, LOW);
      Serial.println("Analog right");
      }  else if (!ps2x.Button(PSB_PAD_RIGHT))
      pinMode(C64_RIGHT, INPUT);

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
    }

    if(ps2x.ButtonPressed(PSB_CIRCLE)) {              //will be TRUE if button was JUST pressed
      Serial.println("Circle just pressed");
      pinMode(C64_UP, OUTPUT);
      digitalWrite(C64_UP, LOW);
    }  
    
    if(ps2x.ButtonPressed(PSB_CROSS)) {              //will be TRUE if button was JUST pressed
      Serial.println("Cross just pressed");
      pinMode(C64_FIRE, OUTPUT);
      digitalWrite(C64_FIRE, LOW);
    }  

    if(ps2x.Button(PSB_TRIANGLE)) {              //Autofire code
      Serial.println("Autofire pressed (Square)");
      pinMode(C64_FIRE, OUTPUT);
      digitalWrite(C64_FIRE, LOW);
      delay(50);
      pinMode(C64_FIRE, INPUT);
    }  
    
    if (ps2x.ButtonPressed(PSB_PAD_UP)) {
      Serial.println("Pad Up pressed");
      pinMode(C64_UP, OUTPUT);
      digitalWrite(C64_UP, LOW);
    }
    
    if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
      Serial.println("Pad Down pressed");
      pinMode(C64_DOWN, OUTPUT);
      digitalWrite(C64_DOWN, LOW);
    }
    
    if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
      Serial.println("Pad Left pressed");
      pinMode(C64_LEFT, OUTPUT);
      digitalWrite(C64_LEFT, LOW);
    }

    if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
      Serial.println("Pad Right pressed");
      pinMode(C64_RIGHT, OUTPUT);
      digitalWrite(C64_RIGHT, LOW);
    }
  
    if(ps2x.ButtonReleased(PSB_CIRCLE)) {             //will be TRUE if button was JUST released
      Serial.println("Circle just released");     
      pinMode(C64_UP, INPUT);
    }

    if(ps2x.ButtonReleased(PSB_CROSS)) {             //will be TRUE if button was JUST released
      Serial.println("Cross just released");     
      pinMode(C64_FIRE, INPUT);
    }

    if(ps2x.ButtonReleased(PSB_TRIANGLE)) {             //will be TRUE if button was JUST released
      Serial.println("Square just released");     
      pinMode(C64_FIRE, INPUT);
    }
    
    if (ps2x.ButtonReleased(PSB_PAD_UP)) {
      Serial.println("Pad Up Released");
      pinMode(C64_UP, INPUT);
    }    

    if (ps2x.ButtonReleased(PSB_PAD_DOWN)) {
      Serial.println("Pad Down Released");
      pinMode(C64_DOWN, INPUT);
    }    
    
    if (ps2x.ButtonReleased(PSB_PAD_LEFT)) {
      Serial.println("Pad Left Released");
      pinMode(C64_LEFT, INPUT);
    }    
    
    if (ps2x.ButtonReleased(PSB_PAD_RIGHT)) {
      Serial.println("Pad Rıght Released");
      pinMode(C64_RIGHT, INPUT);
    }       
   
    
//    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
//      Serial.print("Stick Values:");
//      Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
//      Serial.print(",");
//      Serial.print(ps2x.Analog(PSS_LX), DEC); 
//      Serial.print(",");
//      Serial.print(ps2x.Analog(PSS_RY), DEC); 
//      Serial.print(",");
//      Serial.println(ps2x.Analog(PSS_RX), DEC); 
//    }     
  }
  delay(50);   //Default 50
}
