/*This function is called whenever the camera sends data it gathered to us.
   It takes ID as an argument to tell which variable it will send:
   just use the IDs predefined in the main code
   Example: sendInfo(pressure_ID); will send the pressure (depth) value to the camera
*/
void sendInfo_toBoat(byte ID) {


  /*---------------------------------------------*/
  //DEBUGGING
  //Serial.print("A new info is being sent, with ID: ");
  //Serial.println(ID);
  /*---------------------------------------------*/


  int presentData;

  switch (ID) {

    case setPoint_ID:             //this data is for debugging only. no need to send the setPoint to the boat
      presentData = setPoint;
      break;

    case readySignal_ID:
      presentData = 0;
      break;

    case lightLevel_ID:       //not needed? depends if transmission is too slow for fast clicking
      presentData = (lightIntensity << 4);
      break;

    case altitude_ID:
      //Serial.print("before turning altitude from doub to int: ");
      //Serial.println(altitude, BIN);
      presentData = altitude;
      //Serial.print("after: ");
      //Serial.println(altitude, BIN);
      presentData = (presentData << 4);
      break;

    case depth_ID:
      //Serial.println("depth is being sent");
      presentData = (camDepth << 4);
      break;

    case temperature_ID:
      presentData = (waterTemp << 4);
      break;

    case linPot_ID:
      presentData = (linPotAngle << 4);
      break;
  }


  presentData |= (ID);

  //SEPARATE AS 2 BYTES AND SEND TO SOFTSERIAL
  byte h = highByte(presentData);
  byte l = lowByte(presentData);

  toBoatSerial.write(h); //Important to write not print, my code was not functionning before! I solved it with a forum post
  toBoatSerial.write(l);  //  https://forum.arduino.cc/index.php?topic=573538.0


  /*---------------------------------------------*/
  //DEBUGGING
  /*Serial.print("info to send with its ID: ");
  Serial.println(presentData, BIN);
  Serial.println("info separated as 2 bytes (with ID added): ");
  Serial.println(h, BIN);
  Serial.println(l, BIN);
  Serial.println("--------------"); */
  /*---------------------------------------------*/

}

