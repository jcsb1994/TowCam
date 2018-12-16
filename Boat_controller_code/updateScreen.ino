void updateScreen() {
  if (buttonFlag | received_Flag) {  //? make sure works
    lcd.clear();
    float showKp = Kp;
    float showKi = Ki;
    float showKd = Kd;

    //debugging info Sent to lcd -----------------------------------
    Serial.println("----------------------------------------");
    Serial.println();
    Serial.print("voltage: ");
    Serial.println(voltage);
    Serial.println();
    Serial.print("menuPage: ");
    Serial.println(menuPage);
    Serial.println();
    Serial.print("menuState: ");
    Serial.println(menuState);
    Serial.print("Kp:");
    Serial.println(Kp);
    //-----------------------------------------------------------------


    //SCROLLING MENU -------------------------

    if (menuState == scrolling_menu) {

      switch (menuPage) {
        case setPoint_menu:
          lcd.print("Target dist: ");
          lcd.print(setPoint);
          lcd.setCursor(0, 1); //Start at character 0 on line 1
          lcd.print("Measured dist: ");
          lcd.print(receivedAltitude);
          lcd.setCursor(0, 2);
          lcd.print("SLCT to set dist");
          break;
          
        case lightLevel_menu:
          lcd.print("light: ");
          lcd.print(receivedLightLevel);
          lcd.print("%");
          lcd.setCursor(0, 1);
          lcd.print("SLCT to ctrl light");
          break;
          
        case goproServo_menu:
          lcd.print("Pressure: ");
          lcd.print(receivedDepth);
          lcd.setCursor(0, 1);
          lcd.print("Temp: ");
          lcd.print(receivedTemperature);
          lcd.setCursor(0, 2);
          lcd.print("Plane angle: ");
          lcd.print(receivedPlanesAngle);
          lcd.setCursor(0, 3);
          lcd.print("SLCT: ctrl camera");
          break;

        case Kp_menu:
          lcd.print("--> Kp: ");
          lcd.print(showKp/10);
          lcd.setCursor(0, 1);
          lcd.print("Ki: ");
          lcd.print(showKi/10);
          lcd.setCursor(0, 2);
          lcd.print("Kd: ");
          lcd.print(showKd/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT to set Kp");
          break;

        case Ki_menu:
          lcd.print("Kp: ");
          lcd.print(showKp/10);
          lcd.setCursor(0, 1);
          lcd.print("--> Ki: ");
          lcd.print(showKi/10);
          lcd.setCursor(0, 2);
          lcd.print("Kd: ");
          lcd.print(showKd/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT: set Ki");
          break;

        case Kd_menu:
          lcd.print("Kp: ");
          lcd.print(showKp/10);
          lcd.setCursor(0, 1);
          lcd.print("Ki: ");
          lcd.print(showKi/10);
          lcd.setCursor(0, 2);
          lcd.print("--> Kd: ");
          lcd.print(showKd/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT to set Kd");
          break;

        case scale_menu:
          lcd.print("PID K's scale: ");
          lcd.print(K_scale/10);
          lcd.setCursor(0, 2);
          lcd.print("SLCT to change value of");
          lcd.setCursor(0, 3);
          lcd.print("of K in/decrements");
          break;
      }
    }

    //MODIFICATION MENU ------------------------

    else {
      switch (menuPage) {
        case setPoint_menu:
          lcd.print("UP, DOWN to set");
          lcd.setCursor(0, 1);
          lcd.print("target dist: ");
          lcd.print(setPoint);
          lcd.setCursor(0, 2);
          lcd.print("SLCT to save");
          break;

        case lightLevel_menu:
          lcd.print("UP, DOWN to change");
          lcd.setCursor(0, 1);
          lcd.print("light level");
          lcd.setCursor(0, 2);
          lcd.print("SLCT to finish");
          break;

        case goproServo_menu:
          lcd.print("UP, DOWN to ctrl");
          lcd.setCursor(0, 1);
          lcd.print("camera angle");
          lcd.setCursor(0, 2);
          lcd.print("SLCT to finish");
          break;

        case Kp_menu:
          lcd.print("UP, DOWN to change");
          lcd.setCursor(0, 1);
          lcd.print("Kp: ");
          lcd.print(showKp/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT to save");
          break;

        case Ki_menu:
          lcd.print("UP, DOWN to change");
          lcd.setCursor(0, 1);
          lcd.print("Ki: ");
          lcd.print(showKi/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT to save");
          break;

        case Kd_menu:
          lcd.print("UP, DOWN to change");
          lcd.setCursor(0, 1);
          lcd.print("Kd: ");
          lcd.print(showKd/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT to save");
          break;
          
        case scale_menu:
          lcd.print("UP, DOWN to change");
          lcd.setCursor(0, 1);
          lcd.print("PID K's scale: ");
          lcd.print(K_scale/10);
          lcd.setCursor(0, 3);
          lcd.print("SLCT to save");
          break;
      }
    }

    //FLAG TO SHOW THE SCREEN HAS BEEN UPDATED
    buttonFlag = 0;
    received_Flag = 0;

  }
}



















/*SERIAL VERISON (IF NO LCD SCREEN IS AVAILABLE)*/

/*void updateScreen() {
  //received_Flag removed so the Serial monitor doesn't get flooded
  if (buttonFlag) {  
    lcd.clear();
    float showKp = Kp;
    float showKi = Ki;
    float showKd = Kd;

    //debugging info Sent to lcd -----------------------------------
    Serial.println("----------------------------------------");
    Serial.println();
    Serial.print("voltage: ");
    Serial.println(voltage);
    Serial.println();
    Serial.print("menuPage: ");
    Serial.println(menuPage);
    Serial.println();
    Serial.print("menuState: ");
    Serial.println(menuState);
    //-----------------------------------------------------------------


    //SCROLLING MENU -------------------------

    if (menuState == scrolling_menu) {

      switch (menuPage) {
        case setPoint_menu:
          Serial.print("Target dist: ");
          Serial.print(setPoint);
          Serial.println(); //Start at character 0 on line 1
          Serial.print("Measured dist: ");
          Serial.print(receivedAltitude);
          Serial.println();
          Serial.print("SLCT to set dist");
          break;
          
        case lightLevel_menu:
          Serial.print("light: ");
          Serial.print(receivedLightLevel);
          Serial.print("%");
          Serial.println();
          Serial.print("SLCT to ctrl light");
          break;
          
        case goproServo_menu:
          Serial.print("Pressure: ");
          Serial.print(receivedDepth);
          Serial.println();
          Serial.print("Temp: ");
          Serial.print(receivedTemperature);
          Serial.println();
          Serial.print("Plane angle: ");
          Serial.print(receivedPlanesAngle);
          Serial.println();
          Serial.print("SLCT: ctrl camera");
          break;

        case Kp_menu:
          Serial.print("--> Kp: ");
          Serial.print(showKp/10);
          Serial.println();
          Serial.print("Ki: ");
          Serial.print(showKi/10);
          Serial.println();
          Serial.print("Kd: ");
          Serial.print(showKd/10);
          Serial.println();
          Serial.print("SLCT to set Kp");
          break;

        case Ki_menu:
          Serial.print("Kp: ");
          Serial.print(showKp/10);
          Serial.println();
          Serial.print("--> Ki: ");
          Serial.print(showKi/10);
          Serial.println();
          Serial.print("Kd: ");
          Serial.print(showKd/10);
          Serial.println();
          Serial.print("SLCT: set Ki");
          break;

        case Kd_menu:
          Serial.print("Kp: ");
          Serial.print(showKp/10);
          Serial.println();
          Serial.print("Ki: ");
          Serial.print(showKi/10);
          Serial.println();
          Serial.print("--> Kd: ");
          Serial.print(showKd/10);
          Serial.println();
          Serial.print("SLCT to set Kd");
          break;

        case scale_menu:
          Serial.print("PID K's scale: ");
          Serial.print(K_scale/10);
          Serial.println();
          Serial.print("SLCT to change value of");
          Serial.println();
          Serial.print("of K in/decrements");
          break;
      }
    }

    //MODIFICATION MENU ------------------------

    else {
      switch (menuPage) {
        case setPoint_menu:
          Serial.print("UP, DOWN to set");
          Serial.println();
          Serial.print("target dist: ");
          Serial.print(setPoint);
          Serial.println();
          Serial.print("SLCT to save");
          break;

        case lightLevel_menu:
          Serial.print("UP, DOWN to change");
          Serial.println();
          Serial.print("light level");
          Serial.println();
          Serial.print("SLCT to finish");
          break;

        case goproServo_menu:
          Serial.print("UP, DOWN to ctrl");
          Serial.println();
          Serial.print("camera angle");
          Serial.println();
          Serial.print("SLCT to finish");
          break;

        case Kp_menu:
          Serial.print("UP, DOWN to change");
          Serial.println();
          Serial.print("Kp: ");
          Serial.print(showKp/10);
          Serial.println();
          Serial.print("SLCT to save");
          break;

        case Ki_menu:
          Serial.print("UP, DOWN to change");
          Serial.println();
          Serial.print("Ki: ");
          Serial.print(showKi/10);
          Serial.println();
          Serial.print("SLCT to save");
          break;

        case Kd_menu:
          Serial.print("UP, DOWN to change");
          Serial.println();
          Serial.print("Kd: ");
          Serial.print(showKd/10);
          Serial.println();
          Serial.print("SLCT to save");
          break;
          
        case scale_menu:
          Serial.print("UP, DOWN to change");
          Serial.println();
          Serial.print("PID K's scale: ");
          Serial.print(K_scale/10);
          Serial.println();
          Serial.print("SLCT to save");
          break;
      }
    }

    //FLAG TO SHOW THE SCREEN HAS BEEN UPDATED
    buttonFlag = 0;
    received_Flag = 0;

  }
}
*/








