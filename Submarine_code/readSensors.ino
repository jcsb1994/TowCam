void readSensors() {

  /*  delay(90); // debug only
     altitude = 123;  // debug only */

  boolean bar30_counter = 0;

  const int BUFF_SIZE = 8;  // Set buffer size for data from Ping. 8 is the number of bytes to read from the Altimeter on each reading
  char ASCII_Altitude[BUFF_SIZE];  //this is the buffer to store incoming bytes in with readBytesUntil(). 8 bytes of data come from altimeter as ASCII.

  //unsigned long SonarReadingStart = millis();   // debug only

  altimeter.listen();


/* WAITING DOESNT SEEM TO BE IMPORTANT. GETS THE DATA ANYWAY WHEN TESTES (just make sure?)

read Bar30 while waiting on altimeter data to arrive------------------------------------*/

 /*  while (altimeter.available() < 1) {    //MASK IF THE SONAR ISN'T CONNECTED BECAUSE MCU WILL WAIT FOREVER.
  if (bar30_counter == 0) {
    waterTemp = camPressureSensor.temperature();
    camDepth = camPressureSensor.depth();

//those readings are possible too
//camPressureSensor.altitude(); 
//camPressureSensor.pressure(); 
    
    bar30_counter ++;
  }
}    */


if (altimeter.available() > 0) {

  altimeter.readBytesUntil('\n', ASCII_Altitude, BUFF_SIZE); //Variable types:   readBytesUntil(char, char[] or byte[], int); '\n' means: LF (meaning line feed) char at end
  altitude = atoi(ASCII_Altitude); //We want to convert the string ASCII_Altitude into integer, to store it in the altitude variable and send it to the other Arduino via Serial
}









/*---------------------------------------------*/
/* //DEBUGGING

  unsigned long delaySonarReading = millis() - SonarReadingStart;
  Serial.print("time taken to sonar: ");
  Serial.println(delaySonarReading);

   Serial.print("ASCII_Altitude: ");  //debugging only
   Serial.println(ASCII_Altitude);
   Serial.print("altitude: ");
   Serial.println(altitude); */
/*---------------------------------------------*/
}
