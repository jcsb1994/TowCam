/*
  This code is a prototype for a controller, communicating via Serial to a arduino-based underwater camera

  INTERRUPT
  the controller works with an interrupt service routine on INT0 (D2). different values of resistors create different output values on A0, which is read
  when the interrupt on INT0 occurs.
  Buttons:
  UP
  DOWN
  SELECT

  LCD
  A 4x20 LCD Screen with I2C is used as a user-camera interface.
  This code uses F. Malpartida's NewLiquidCrystal library and Wire llibrary to control the I2C LCD
  The I2C adress of the LCD has been found using the I2C adress scanner, which is easily found on Arduino website. If using a different LCD in later versions,
  re-run the I2C scanner to determine the new adress.

  COMMUNICATION
  The boat arduino uses the AltSoftSerial library, and the Submarine Arduino uses SoftwareSerial, because they offer different advantages. This has been found to be
  the best combination for this project, as AltSoftSerial makes the boat arduino much more efficient at transmitting Serial Data, and Submarine arduino must use

  ~brief description of Serial libraries used in this project~

  AltSoftSerial - Can simultaneously transmit and receive. Minimal interference with simultaneous use of HardwareSerial and other libraries.
  Consumes a 16 bit timer (and will not work with any libraries which need that timer) and disables some PWM pins. Can be sensitive to interrupt usage by other libraries.
  **THIS IS TIMER1 (16 bits): you could use any one of the analog pins as an input for the 16 bit timer.. to replace pins 8 and 9, used as RX and TX respectively

  AltSoftSerial always uses these pins (it can have only one instance, but is much more efficient):
               Transmit  Receive   PWM Unusable
  Arduino Uno        9         8         10

  SoftwareSerial(formerly "NewSoftSerial") - Can have multiple instances on almost any pins, but only 1 can be active at a time. Can not simultaneously transmit and receive.
  Can interfere with other libraries or HardwareSerial if used at slower baud rates. Can be sensitive to interrupt usage by other libraries.
  This is the library used for the other arduino in this project, the one located in the submarine, because it uses servo library, which is incompatible with AltSoftSerial,
  and Submarine arduino also needs 2 instances of software serial (which is also a AltSoftSerial limitation):
  one for the serial communication with the boat, and another for the vertical SONAR altimeter.
  The hardware serial port is kept for debugging purposes, but it could be used for a second SONAR altimeter, targetting forward to detect incoming obstacles.


*/
/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/
#include <AltSoftSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library, to use with I2C LCDs



/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/

#define buttonsPin  2
#define voltagePin  PC0 //everytime button interrupt occurs, voltage pin is read to see which button was pressed

//LCD PINS
//PCF8574 LCD to I2C addapter (a different and cheaper adapter than the sunfounder adapter) pin assignments to LCD connections
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7



/**************************************************************************************************************************************************************/
////VARIABLES
/**************************************************************************************************************************************************************/

/*DATA VARIABLES ------------------------------------------*/
//variables to serial send as info to the camera
int setPoint;
int goproServoValue = 120; //in degrees: will be able to go down to 80, up to a max of 180.
int lightLevel = 1100;
int Kp;  //When the boat receives it, needs to divide by 10 and store it as long.      JSUIS rendu à corriger buttons action, 
int Ki;
int Kd;
float K_scale = 10;

//variables to store received info from camera sensors
int receivedLightLevel;
int receivedAltitude;
int receivedDepth;
int receivedTemperature;
int receivedPlanesAngle;



/*CONSTANT UNIQUE IDs FOR EACH SERIALLY TRANSMITTED VARIABLE------*/
//IDs for variables to SEND to the camera
#define setPoint_ID   1   //0B0001 
#define goproServo_ID   2   //0B0010    
#define lightLevel_ID   3   //0B0011  
#define Kp_ID   4
#define Ki_ID   5
#define Kd_ID   6

//IDs for variables RECEIVED from the boat
#define readySignal_ID   0
//light level will be received as feed back too, with the same ID
#define altitude_ID   7
#define depth_ID   8
#define temperature_ID   9
#define linPot_ID   10



volatile boolean received_Flag; //flag for marking when a new value for continuously changing variable has been received, to update the LCD screen (ex: depth)



/*INTERRUPT VARIABLES---------------------------------------------*/
static unsigned long last_interrupt_time = 0;

volatile boolean buttonFlag = 1; //Tells us if an interrupt has been triggered on last loop
volatile byte voltage; //IMPORTANT: might need to be an INT if we have DIFF RESISTORS WITH VOLTAGE VALUES ABOVE 255, because analogRead goes up to 1024.
volatile byte var_flag = 0; //flag to use Serial OUTSIDE of interrupts because serial fails if info is sent during interrupt

//constants storing button voltage ranges
/* MY VALUES
/* values are set depending on resistor values. Common resistor is 10k
#define minUp  175 //175 to 185 based on a 4.7k resistor value for UP arrow button
#define maxUp  184
#define minDown  189 //189 to 220 based on a 1k resistor value for DOWN arrow button
#define maxDown  220
#define minSelect  120 //120 to 166 based on a 10k resistor value for SELECT button
#define maxSelect  166
*/

/*STEVE RESISTORS' VALUES
/* values are set depending on resistor values. Common resistor is 10k
#define minUp  195 //175 to 185 based on a 4.7k resistor value for UP arrow button
#define maxUp  205
#define minDown  210 //189 to 220 based on a 1k resistor value for DOWN arrow button
#define maxDown  220
#define minSelect  175 //120 to 166 based on a 10k resistor value for SELECT button
#define maxSelect  189*/

/*GAMEBOY RESISTORS VALUES
/* values are set depending on resistor values. Common resistor is 10k*/
#define minUp  185 //175 to 185 based on a 4.7k resistor value for UP arrow button
#define maxUp  195
#define minDown  200 //189 to 220 based on a 1k resistor value for DOWN arrow button
#define maxDown  220
#define minSelect  165 //120 to 166 based on a 10k resistor value for SELECT button
#define maxSelect  175

/*LCD VARIABLES--------------------------------------------------------*/
#define I2C_ADDR    0x27  // Define I2C Address for the PCF8574T
//Constants used to turn LED backboard of the LCD screen on and off (just to help make the code easier to understand)
#define  LED_OFF  1
#define  LED_ON  0

//LCD menu contants
/*different menu pages (help giving name to each menu the user can scroll through when controlling the camera from the boat)*/
volatile byte menuPage = 0;
#define setPoint_menu 0
#define lightLevel_menu 1
#define goproServo_menu 2
#define Kd_menu 3
#define Ki_menu 4
#define Kp_menu 5
#define scale_menu 6

#define max_menuPage 6  //this constant is helpful to tell the number of pages we have. This should be EQUAL to to highest value of all your something_menu constants

/*menu states (if true, the menu page is selected, and UP/DOWN buttons will change the variable value when pressed. We will call this the modification menu
  if false, UP and DOWN wont change the variable, but scroll through menu pages. We will call this the scrolling menu) */
volatile boolean menuState = false;
#define scrolling_menu 0
#define modification_menu 1



/**************************************************************************************************************************************************************/
////OBJECTS
/**************************************************************************************************************************************************************/
//AltSoftSerial object
AltSoftSerial toCameraSerial;

//LCD object
LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);


/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {



  //interrupt setup
    DDRC &= ~(1 << PC0); //voltagePin as input
  DDRD &= ~(1 << buttonsPin); //buttonPin as input
  PORTD |= (1 << buttonsPin);  //activate pullup resistor on buttonPin
  attachInterrupt(0, buttonsActions, FALLING);

  //serial setup
  Serial.begin(9600);

  //LCD setup
  lcd.begin (20, 4); // initialize the lcd
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.backlight();  //Backlight ON


  //AltSoftSerial setup
  toCameraSerial.begin(9600);
  // fakeAltimeter.begin(9600); //debugging only
}


/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/

void loop() {


  /*UPDATE SCREEN IF CHANGES WERE MADE---------------------------*/
  updateScreen(); //this function does nothing if no button was pressed
  //need to add OR if receivedData



  /*CHECK THE SERIAL PORT TO COMPILE NEW RECEIVED INFO-----------*/

  while (toCameraSerial.available() > 1) {
    //    Serial.println("receiving something from underwater!");


    /*RECEIVE TWO BYTES THAT MAKE ONE INT-----------------------------------*/
    byte h = toCameraSerial.read();       //read those two bytes back to back!
    //    delay(2);
    byte L = toCameraSerial.read();

    /*   Serial.println("Two raw bytes received: ");
       Serial.println(h, BIN);
       Serial.println(L, BIN);  */

    /*PUT THOSE BYTES TOGETHER AND RETRIEVE REAL VALUE
      BY REMOVING THE 3 LSB (least significant bits) THAT MAKE UP THE ID-------------------------------*/
    int receivedData = (h << 8) + L;
    //Serial.print("Two bytes together before shifting ");
    //Serial.println(receivedData, BIN);

    receivedData = (receivedData >> 4);

    //Serial.print("Real received value: ");
    Serial.println(receivedData);

    /*ISOLATE THE ID FROM THE RECEIVED LOW BYTE------------------------------*/
    byte receivedID = (L << 4);
    receivedID = (receivedID >> 4);
    //  Serial.print("only ID left: ");
    Serial.println(receivedID, BIN);

    /*Serial.print("high b: ");
      Serial.println(h, BIN);
      Serial.print("low b: ");
      Serial.println(L, BIN);
      Serial.print("ID: ");
      Serial.println(receivedID, BIN);*/

    /*IDENTIFY THE RECEIVED INTEGER------------------------------------*/
    switch (receivedID) {

      case readySignal_ID:
        //Serial.println("ready_signal received, send the new info");
        //toCameraSerial.write(0B11111001);
        //toCameraSerial.write(0B11111001);
        sendInfo();

      case lightLevel_ID:
        receivedLightLevel = receivedData;
        receivedLightLevel = (receivedLightLevel - 1100) / 8; //turn the light level from a servo pulse (1100 to 1900) into a %.
        /*//DEBUGGING
          /*Serial.print("light Level: ");
          Serial.println(receivedData);*/
        break;

      case altitude_ID:
        receivedAltitude = receivedData;
        /*//DEBUGGING
          Serial.print("altitude: ");
          Serial.println(receivedData);*/
        received_Flag++;
        break;

      case depth_ID:
        receivedDepth = receivedData;
        /*//DEBUGGING
          Serial.print("depth: ");
          Serial.println(receivedData);*/
        received_Flag++;
        break;

      case temperature_ID:
        receivedTemperature = receivedData;
        /*//DEBUGGING
          Serial.print("temp: ");
          Serial.println(receivedData);*/
        received_Flag++;
        break;

      case linPot_ID:
        receivedPlanesAngle = receivedData;
        /*//DEBUGGING
          Serial.print("angle: ");
          Serial.println(receivedData);*/
        received_Flag++;
        break;

      case setPoint_ID: //for debugging only, so we are just seeing what is received, and receivedData is not stored anywhere
        /*//DEBUGGING
          Serial.print("setPoint: ");
          Serial.println(receivedData);*/
        break;
    }
  }

}







