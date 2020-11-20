#include <PS2X_lib.h>  //for v1.6

/*Data from PS2-Mouse:
 * 
 * data0[2:1:0]  ->  middle,right, left mousebutton
 * data1         ->  x from -127 to + 127   signed int
 * data2         ->  y from -127 to + 127   signed int
 * 
 * 
 * amiga mouse-port, looking at the amiga:
 * 
 * 9 PIN D-SUB MALE
 * 
 *   _________________________
 *   \                       /
 *    \  1   2   3   4   5  /
 *     \                   /
 *      \  6   7   8   9  /
 *       \_______________/
 * 
 * 1  V-pulse               
 * 2  H-pulse
 * 3  VQ-pulse
 * 4  HQ-pulse
 * 5  BUTTON 3 (Middle)
 * 6  BUTTON 1 (Lefl)
 * 7  +5V
 * 8  GND
 * 9  BUTTON 2 (Right)
 */

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

/*                  ARDUINO PIN SETUP                             */
/*                  Joy Pins | Mouse Pins                         */

#define C64_UP         6     //   V-Pulse
#define C64_DOWN       5     //   H-Pulse
#define C64_LEFT       4     //   VQ-Pulse
#define C64_RIGHT      9     //   HQ-Pulse
#define C64_FIRE       8     //   LMB
#define RMB            7     //   RMB

#define ADELAY         200
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

// quadrature encoding used by the amiga mouse protocol***
//
uint8_t H[4]  = { LOW, LOW, HIGH, HIGH};
uint8_t HQ[4] = { LOW, HIGH, HIGH, LOW};

uint8_t QX = 3;
uint8_t QY = 3;

uint8_t XSTEPS;
uint8_t YSTEPS;
uint8_t XSIGN;
uint8_t YSIGN;
int X;
int Y;
/*********************************************************/

const byte ANALOG_IDLE_VALUE = 128;


// x/y handling*******************************************
//
void AMIGAHorizontalMove() {
    pinMode(C64_DOWN, OUTPUT);
    pinMode(C64_RIGHT, OUTPUT);
    // set bits acc. to curr. position in quadr. sequence
    digitalWrite(C64_DOWN, H[QX]);
    delayMicroseconds(ADELAY);
    digitalWrite(C64_RIGHT, HQ[QX]);
//    pinMode(C64_DOWN, INPUT);
//    pinMode(C64_RIGHT, INPUT);
}


void AMIGAVerticalMove() {
    pinMode(C64_UP, OUTPUT);
    pinMode(C64_LEFT, OUTPUT);
    digitalWrite(C64_UP, H[QY]);
    delayMicroseconds(ADELAY);
    digitalWrite(C64_LEFT, HQ[QY]);
//    pinMode(C64_UP, INPUT);
//    pinMode(C64_LEFT, INPUT);
}


void AMIGA_Left() {
    
    // do a move by setting the port
    AMIGAHorizontalMove();

    // advance in the quadr. sequence
    QX = (QX >= 3) ? 0 : ++QX;
    
}

void AMIGA_Right() {
    AMIGAHorizontalMove();
    QX = (QX <= 0) ? 3 : --QX;
}

void AMIGA_Down() {
    AMIGAVerticalMove();
    QY = QY <= 0 ? 3 : --QY;
}

void AMIGA_Up() {
    AMIGAVerticalMove();
    QY = QY >= 3 ? 0 : ++QY;
}

/****************************************************************************/

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

   if(ps2x.Button(PSB_L2) || ps2x.Button(PSB_R2)) { //print stick values if either is TRUE
    

    
    
    /******Mouse Movement**************/
    // calc x/y movement
    //
    X = ps2x.Analog(PSS_RX)-ANALOG_IDLE_VALUE;
    X = (X == -128 ? -127 : X);
    Y = ps2x.Analog(PSS_RY)-ANALOG_IDLE_VALUE;
    Y = (Y == -128 ? -127 : Y);
    
    XSTEPS = abs(X);
    YSTEPS = abs(Y);
    XSIGN = (X > 0 ? 1 : 0) ;
    YSIGN = (Y > 0 ? 1 : 0) ;

    
    // handle x/y movement 
    //
    while ((XSTEPS | YSTEPS) != 0) {

        // steps left?
        if (XSTEPS != 0) {
          
            // direction
            if (XSIGN) {
                AMIGA_Right();
                Serial.println("Mouse Right!");
            }
            else {
                AMIGA_Left();
                Serial.println("Mouse Left!");
            }
            // decrease steps    
            XSTEPS--;
        }

        
        if (YSTEPS != 0) {
            if (YSIGN){
                AMIGA_Down(); 
                Serial.println("Mouse Down!");
            }
            else {
               AMIGA_Up();
               Serial.println("Mouse Up!");
            }
            YSTEPS--;
        }
        
    }    

  }

    
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

    if(ps2x.ButtonPressed(PSB_SQUARE)) {              //will be TRUE if button was JUST pressed
      Serial.println("Square just pressed");
      pinMode(RMB, OUTPUT);
      digitalWrite(RMB, LOW);
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

    if(ps2x.ButtonReleased(PSB_SQUARE)) {             //will be TRUE if button was JUST released
      Serial.println("Square just released");     
      pinMode(RMB, INPUT);
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
