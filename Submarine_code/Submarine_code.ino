/*PARTS OF THE PROJECT
   CONTROLLERS
    Arduino #1 in the boat
    Arduino #2 in the camera's enclosure
      Serial communication between 2 arduinos: requires SoftwareSerial library to clear RX and TX pins

   SENSORS (3)
    linear potentiometer
      INPUT PIN: Analog, pullup
    Pressure sensor
      INPUT PIN: requires MS5837 Arduino Library.
      3V3 LOGIC: needs a I2C Level Converter between arduino and sensor to operate http://docs.bluerobotics.com/level-converter/#introduction
    altimeter
      INPUT PIN: 5 (RX of its own softwareSerial instance)
      Aiming directly under the tow camera.
      Vertical positioning: For now, no second SONAR: will be using the GoPro to locate incoming obstacles

   ACTUATORS (3)
    linear actuator (taken from the Newton gripper)
      output PIN: digital, requires Servo library
      3V3 LOGIC: needs a I2C Level Converter between arduino and actuator to operate http://docs.bluerobotics.com/level-converter/#introduction
    Servo motor (controlling the GoPro mount angle)
      output PIN: digital, requires Servo library
    Lumen light by blue robotics
      output PIN: digital, requires Servo library

   MECANICAL PARTS
    Diving planes
    Caudal fin (stability plate at the tail of the camera)
    Waterproof enclosure
      3'' diameter with a Dome head.

   COMMUNICATION BETWEEN BOAT AND CAMERA
   Camera will send these data:
   1. planes angle (analogRead from linear pot)
   2. depth (pressure from bar30)
   3. temperature (from Bar30)
   4. light % (from light servo) and if ON or OFF
   5. the ACTUAL distance from seafloor

   The boat will show all of the above on the LCD.
   The boat will be able to send these to the camera:
   1. the DESIRED distance from seafloor
   2. moving directions for the goPRO position
   3. light control directions

   The camera will send its data every cycle.
   It will first listen to the SONAR, then listen to the boat. As soon as it does, it will send a byte to the camera telling it it has stopped listening to the sonar and is now listening for incoming directions.
   If data was waiting to be sent to the camera, it will be sent when the boat receives that signal byte. Ater this data is sent from the boat to cam, Camera reads from its other sensors.
   I then throws all that bundle of information back at the boat (including the sonar data).

   The boat is not going to send anything to the camera until it receives the special signal saying it is ready to receive, because camera is almost always listening to sonat and will miss the info otherwise.


*/

/**************************************************************************************************************************************************************/

/*GROCERY LIST
   Dome Cap 3'' BOUGHT
   1 time 5v to 3v3 logic lvl converters Still unsure?
   get 2 arduinos to check comm at home DONE
   ask if the SONAR sensor is analog or digital,PWM, I2C, etc DONE: Serial DONE


*/

/**************************************************************************************************************************************************************/

/*CODE EXPLAINED
   Libraries and vars are declared
   Setup: initialize communication with 2 Servo motors
    Initialize serial communication with the second arduino (pins RX and TX are NOT used)
    LOOP:
    0. retrieve SoftwareSerial.available (info stored in the receive buffer) that will modify camera angle and desired distance from depth (setPoint)
    1. write this information to the Servos. **The PID will need to be computed for the linear actuator Servo and therefore the info wont be written yet
    2. read SONAR,set it as input to compute PID, then compute PID (with the new setPoint if serial info sent a new one)
    3. correct planes angle by writing the PID output to the linear actuator
    4. Retrieve the planes angle: either with a linear pot or with Servo position?? We will need to map the value into an angle in degrees. That angle can be sent to serial
    5. Send the sonar value to serial
    6. read the pressure. Send data as depth to serial

  CTRL+F FOR "?" TO SEE PARTS THAT MIGHT REQUIRE ATTENTION
  CTRL+F FOR "//Serial" and replace all for ""Serial" to toggle on all debugging lines (opposite to turn them off)


*/

/**************************************************************************************************************************************************************/

/*VERSIONS LIST
   v1: can receive data from Serial V3
   V1.1 ADDED ID # to received integers
   v1.2 SoftwareSerial
   v3.1 Serial: is working
   v4: add sonar listening
   v4.1 sonar sending works, need to tell it listen afterwards
   v4.2 the sonar works and we can listen to the boat between measurements, BUT ONLY WITH AUTOMATIC VALUES, not actual pressed buttons info (works with BOAT code v3)
   v5_MAIN and v4_BOAT fix minor bugs and have cleaner code
   v5.1 pressing buttons finally sends info that reaches the submarine, but requires a delay(60) everytime we listen to boat (so submarine listens only once every 1 or 1/2 second to reduce the impact of delay)
   v5.2_MAIN and 4.2_BOAT are functionning with the sonar hooked in and comm is ok between them
   v5.3_MAIN and 4.2_BOAT code is cleaned
   v5.3.2 and v4.2 most recent
   BOAT v4.3: applied LCD instead of serial
   v5.3.3 & v4.4: 4-bit ID's to add Kp, Ki and Kd on the list
*/

/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

/*MICROCONTROLLER LIBRARIES--------------------*/
#include <Wire.h>   //helps communicate with I2C devices like the Bar30 (pressure sensore we'll be using)
#include <SoftwareSerial.h>   //In order to setup a software serial comm between the 2 Arduinos (boat and camera's)
#include <PID_v1.h>   //helps building the PID algorithm to maintain a set distance from the seafloor

/*ACTUATORS LIBRARIES--------------------*/
#include <Servo.h>   //Blue Robotics Gripper, lumen light and GoPro holder's Servo motor use this library to function

/*SENSORS LIBRARIES--------------------*/
#include <MS5837.h>   //required to communicate with the Bar30 sensor




/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/

/*MICROCONTROLLER PINS--------------------*/
#define swRX 8
#define swTX 9

/*SENSORS PINS--------------------*/
#define potPin A0
#define pressurePin 4
#define sonarRX 5 //the altimeter (SONAR) works with serial comm. It will be the default ON software serial, beccause the other software serial instance, with the boat arduino, won't be used as often

/*ACTUATORS PINS--------------------*/
#define linActPin 6   //The Blue Robotics Newton Gripper (that will be used as a linear actuator in this project) requires a PWM pin
#define goproServoPin 10
#define lumenPin 11





/**************************************************************************************************************************************************************/
////VARIABLES
/**************************************************************************************************************************************************************/

/*MICROCONTROLLER VARIABLES--------------------*/
//Serial variables
unsigned long last_serial_sent_time;
unsigned long last_ready_sent_time;


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


//PID variables(NOTE THAT THE SONAR AS A PID SENSOR MIGHT CHANGE FOR LINEAR POT, SINCE MIGHT NOT BE ABLE TO ANALOG READ
double setPoint; //desired input from the SONAR (desired depth)
double altitude = 10; //input from the SONAR. Actual debth -> setPoint - altitude = error
double PIDoutput; //value that will be written to the GRIPPER (to adjust planes' angle)

//tuning parameters (values that dict how aggressive PID functions will be when correcting error)
double Kp = 0; //P (proportional) constant
double Ki = 0; //I (integral) constant
double Kd = 0; //D (derivative) constant


/*ACTUATORS VARIABLES--------------------*/

//linear actuator (Newton gripper)

//servo controlling the gopro position
int goproPosition; //not useful? variable used to store the information received from the boat arduino concerning the GoPro's desired position, used to write to gopro Servo.

//lumen light variable
int lightIntensity = 1100;


/*SENSORS VARIABLES--------------------*/

//altimeter/sonar variables
/*Variable storing measured altitude is actually placed with PID variables because it is the process variable to compute PID*/


//bar30 variables
int camDepth;
int waterTemp;

//linear potentiometer
int linPotAngle;




/**************************************************************************************************************************************************************/
////OBJECTS
/**************************************************************************************************************************************************************/

/*MICROCONTROLLER OBJECTS--------------------*/
//SoftwareSerial for comm between the 2 Arduinos
SoftwareSerial toBoatSerial(swRX, swTX);  //toBoatSerial will communicate with boatSerial

//PID
PID planesPID(&altitude, &PIDoutput, &setPoint, Kp, Ki, Kd, REVERSE);   //The PID algorithm is set to REVERSE or DIRECT, depending on what direction the angle moves when newton moves?


/*ACTUATORS OBJECTS---------------------*/
//Servo
Servo planes;
Servo gopro;
Servo lumen;


/*SENSORS OBJECTS-----------------------*/
//Bar30
MS5837 camPressureSensor;

//Sonar altimeter
SoftwareSerial altimeter(sonarRX, 6);  //swTX won't be used with sonar, wont interfere with other softserial instance with the same TX




/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/
void setup() {

  /*MICROCONTROLLER SETUP--------------------*/
  //SoftwareSerial between Arduinos setup
  toBoatSerial.begin(9600); // begin communication on the software serial channel
  Serial.begin(9600);


  //Wire for comm with devices setup
  Wire.begin();   //initiate the wire, and no adress argument is specified, so join the I2C bus as a master

  //PID setup
  //altitude = analogRead(sonarPin); OR analogRead(potPin)????????? IS THE SONAR EVEN ANALOG?? DOUBT IT
  setPoint = 100; //either set a basic setPoint, or no value until chosen from boat arduino ?
  // planesPID.SetMode(AUTOMATIC);   //Specifies whether the PID should be on (Automatic) or off (Manual). Off by default


  /*SENSORS SETUP-------------------------*/
  //bar30 (pressure sensor) setup
  camPressureSensor.init(); //from MS5837 library, sets model of sensor used
  camPressureSensor.setFluidDensity(1029); // kg/m^3 (997 freshwater, 1029 for seawater) **maybe make a variable/changeable value?


  //Altimeter setup
  altimeter.begin(115200); //or 115200?

  /*ACTUATORS SETUP-----------------------*/
  //Servo setup
  planes.attach(linActPin);

  pinMode(goproServoPin, OUTPUT);
  gopro.attach(goproServoPin);
  //  gopro.write(120);    //start the gopro at an angle of 120 degrees

  lumen.attach(lumenPin);
}




/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/
void loop() {






  /*---------------------------------------------------------------------------------------------------*/
  /* listen to the altimeter (and bar30 while waiting on serial data to arrive)*/
  readSensors();  //only reads sonar


  waterTemp = camPressureSensor.temperature();
  camDepth = camPressureSensor.depth();









  /*---------------------------------------------------------------------------------------------------*/
  /* send the "ready I'm listening" signal to the boat and switch serial listen()ing.*/
  unsigned long ready_sent_time = millis();
  if (ready_sent_time - last_ready_sent_time > 1000) {
    toBoatSerial.listen();
    sendInfo_toBoat(readySignal_ID);
    last_ready_sent_time = ready_sent_time;
    delay(60); // it takes a delay for data to arrive. we can't just wait forever because we don't know if data was sent at all. SO FAR THAT LEAVES ENOUGH TIME! REDUCE WHEN ADDING SERVO WRITES
  }














  /*---------------------------------------------------------------------------------------------------*/
  /* Compute PID and apply on the linear actuator to ctrl dive planes*/

  //PID ACTION ON DIVING PLANES
  planesPID.Compute();      //by default runs 5 times per second max (use SetSampleTime() to change the default 200ms value)
  //AnalogWrite doesn't work on our Servo actuator, it uses Servo: can't use analogWrite(linActPin, PIDoutput);
  //map the analog reading stored as the output, then apply a writeMicroseconds on the gripper
  PIDoutput = map(PIDoutput, 0, 255, 1100, 1900); //those values WILL NEED TO BE CHANGED. Linear range of the gripper goes from 1100 (open) to 1900 (closed)
  /*write 1500 to stop, write 1470 or less to close, 1530 or more to open

  */
  planes.writeMicroseconds(PIDoutput);   //Write the new corrected output value as a desired angle for the diving planes
  readPlanesAngle();

  //gopro.write(120);

  /*
    if (lol < 180) {
    lol +=10;
    }
    gopro.write(lol);
    delay(1000); */









  /*---------------------------------------------------------------------------------------------------*/
  /* send data from the sensors to the boat*/

  unsigned long serial_sent_time = millis();
  if (serial_sent_time - last_serial_sent_time > 2000) { //Not sure if this is useful

    sendInfo_toBoat(altitude_ID);
    sendInfo_toBoat(depth_ID);
    sendInfo_toBoat(temperature_ID);
    sendInfo_toBoat(linPot_ID);
    //sendInfo_toBoat(lightLevel_ID); probably useless to send
    //Serial.println("sending setPoint");
    sendInfo_toBoat(setPoint_ID);
    //  sendInfo_toBoat(lightLevel_ID);


    /*---------------------------------------------------------------*/
    //DEBUGGING
    //Serial.println("Sending data collected underwater!"); //for debugging
    //sendInfo_toBoat(depth_ID);
    //sendInfo_toBoat(altitude_ID);
    //Serial.println(setPoint);
    /*---------------------------------------------------------------*/

    last_serial_sent_time = serial_sent_time;
  }












  /*---------------------------------------------------------------------------------------------------*/
  /* check for data that has been sent from the boat. Must be as far possible to listen() because we want to give time for data to arrive*/

  if (toBoatSerial.available() > 1) {  //we are waiting for

    Serial.println("Data from the boat:");


    /*RECEIVE TWO BYTES THAT MAKE ONE INT-----------------------------------*/
    byte h = toBoatSerial.read();       //read those two bytes back to back!
    //delay(2);
    byte L = toBoatSerial.read();

    //Serial.println("Two raw bytes received: ");
    //Serial.println(h, BIN);
    //Serial.println(L, BIN);


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

    /*---------------------------------------------------------------*/
    //DEBUGGING
    /*Serial.print("high byte received: ");
      Serial.println(h, BIN);
      Serial.print("low byte received: ");
      Serial.println(L, BIN);
      Serial.print("ID received: ");
      Serial.println(receivedID, BIN); */
    /*---------------------------------------------------------------*/


    /*IDENTIFY THE RECEIVED INTEGER------------------------------------*/
    switch (receivedID) {

      case setPoint_ID:
        setPoint = receivedData;
        Serial.print("Desired distance from seafloor: ");
        Serial.println(setPoint);
        break;

      case goproServo_ID:
        goproPosition = receivedData; //might not be needed cuz use recceivedData to write directly?
        Serial.print("servo value to the gopro holder: ");
        Serial.println(goproPosition);
        gopro.write(receivedData);   //receivedData should be between 0 and 180 to set the servo angle
        break;

      case lightLevel_ID:
        lightIntensity = receivedData; //might not be needed cuz use recceivedData to write directly?
        Serial.print("received light level to put on lights: ");
        Serial.println(lightIntensity);
        lumen.writeMicroseconds(receivedData);
        break;

      case Kp_ID:
        Kp = receivedData / 10;
        Serial.print("Kp: ");
        Serial.println(Kp);
        break;

      case Ki_ID:
        Ki = receivedData / 10;
        Serial.print("Ki: ");
        Serial.println(Ki);
        break;

      case Kd_ID:
        Kd = receivedData / 10;
        Serial.print("Kd: ");
        Serial.println(Kd);
        break;
    }

    //etc.
    /*This data will potentially modify:
      - the PID setPoint
      - PID constant values.. This will be very useful when testing in the pool
      - the GoPro mount angle
      - fluid density for the Bar30 sensor?
      -On/OFF lights: ctrls with Servo pulse */
  }



}


