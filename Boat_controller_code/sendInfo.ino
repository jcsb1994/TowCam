/*This function is the new sendInfo without argument: scrolls through each bit of car_flag
   to see which variables have a update to send to camera. this function is called only and everytime
   the boat sends the "ok Im listening" signal
*/

void sendInfo() {


  for (byte i = 1; i < 7; i++) {  //scroll through values from 1 to 6 (which are the ID values of variables to send)

    if (var_flag & (1 << i)) {
      //if that bit was on, send the info (all the code that is in sendInfo()
      //the new sendInfo() with that for loop should be called prepareInfoToSend() or something

      /*---------------------------------------------*/
      //DEBUGGING
     // Serial.print("info modified: #");
     // Serial.println(i);
      /*---------------------------------------------*/

      int presentData;

      switch (i) {
        case setPoint_ID:
      //    Serial.print("sending setPoint: ");
       //   Serial.println(setPoint);
          presentData = setPoint;
          break;

        case goproServo_ID:
          presentData = goproServoValue;
          break;

        case lightLevel_ID:
          presentData = lightLevel;
          break;

        case Kp_ID:
          presentData = Kp;
          break;
          
        case Ki_ID:
          presentData = Ki;
          break;
          
        case Kd_ID:
          presentData = Kd;
          break;
      }
     /* Serial.print("var_flag before: ");
      Serial.print(var_flag);
      Serial.print("with bit i erased: "); */
      var_flag &= ~(1 << i);
      
   /*  Serial.print(var_flag);
     Serial.print("data that has been changed and will be sent to submarine: ");
      Serial.println(presentData, BIN);   */

      //move value 3 bits left to leave room for the ID
      presentData = (presentData << 4);
  //    Serial.print("make room for ID, now it looks like this: ");
  //    Serial.println(presentData, BIN);

      //add the ID (the value of i will fit with a new ID on each for loop iteration)
      presentData |= (i);
   //   Serial.print("info to send with its ID: ");
   //   Serial.println(presentData, BIN);

      //Separate the integer (the value with the ID added) in 2 bytes before sending
      byte h = highByte(presentData);
      byte L = lowByte(presentData);

      //Send both bytes to the camera through softwareSerial communication
      toCameraSerial.write(h); //Important to write not print, my code was not functionning before! I solved it with a forum post
      toCameraSerial.write(L);  //  https://forum.arduino.cc/index.php?topic=573538.0
   /*   Serial.println("info separated as 2 bytes: ");
      Serial.println(h, BIN);
      Serial.println(L, BIN);
      Serial.println("--------------");  */
    }
  }

}

