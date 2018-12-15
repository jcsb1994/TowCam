/*Might need to turn LCD off in the interrupt if takes a while to
   come back to beginning of loop and refresh LCD screen
*/

void buttonsActions() {

  unsigned long interrupt_time = millis(); //saves time at which this interrupt starts
  if (interrupt_time - last_interrupt_time > 200)  //compares last time interrupt was finished to now. We give 200 millis to ignore rebounds
  {

    voltage = analogRead(voltagePin);
    buttonFlag = 1; //tell the arduino it needs to refresh the LCD screen



    /**************************************************************************************************************************************************************/
    //UP BUTTON IS PRESSED
    if (minUp <= voltage && voltage <= maxUp) {


      /*case 1: on scrolling menu, scroll UP */
      if (menuState == scrolling_menu) {
        menuPage++;     //move up the cursor between menu pages

        if (menuPage > max_menuPage) {
          menuPage = 0;    //if we passed above the top menu page, move back to the lowest menu page (which is 0)
        }
      }

      /*case 2: on modification menu, INCREMENT selected variable*/
      else {
        switch (menuPage) {
          case setPoint_menu:
            setPoint += 10;
            break;

          case goproServo_menu:
            if (goproServoValue < 180) {
              goproServoValue += 10;
              var_flag |= 1 << goproServo_ID;
            }
            break;

          case lightLevel_menu:
            if (lightLevel < 1900) {
              lightLevel += 100;
              var_flag |= 1 << lightLevel_ID;
            }
            break;

          case Kp_menu:
            if (Kp <= 1000) {
              Kp += K_scale;
              var_flag |= 1 << Kp_ID;
            }
            break;

          case Ki_menu:

            if (Ki <= 1000) {
              Ki += K_scale;
              var_flag |= 1 << Ki_ID;
            }
            break;

          case Kd_menu:

            if (Kd <= 1000) {
              Kd += K_scale;
              var_flag |= 1 << Kd_ID;
            }
            break;

          case scale_menu:

            if (K_scale < 1000) {
              K_scale *= 10;
            }
            break;

        }
      }
    }

    /**************************************************************************************************************************************************************/
    //DOWN BUTTON IS PRESSED
    if (minDown <= voltage && voltage <= maxDown) {


      /*case 1: on scrolling menu, scroll DOWN */
      if (menuState == scrolling_menu) {



        if (menuPage == 0) {           //if we scrolled down to the lowest page of the menu, move back to the top
          menuPage = max_menuPage;
        }

        else {   //if we are not at the lowest page, scroll down  (note that we can't use if(menuPage<0 because it's a byte variable, so no negative number is allowed)
          menuPage--;
        }

      }


      /*case 2: on modification menu, DECREMENT selected variable*/
      else {
        switch (menuPage) {

          case setPoint_menu:
            if (setPoint > 0) {
              setPoint -= 10;
            }
            break;

          case lightLevel_menu:
            if (lightLevel > 1100) {
              lightLevel -= 100;
            }
            var_flag |= 1 << lightLevel_ID;

            break;

          case goproServo_menu:   //If we are on the gopro angle page, decrement angle of the gopro when button pressed
            if (goproServoValue > 80) {  //the angle cannot go below 80 degrees
              goproServoValue -= 10;
            }
            var_flag |= 1 << goproServo_ID;

            break;

          case Kp_menu:
            if (Kp > 0) {
              Kp -= K_scale;
              var_flag |= 1 << Kp_ID;
            }
            break;

          case Ki_menu:

            if (Ki > 0) {
              Ki -= K_scale;
              var_flag |= 1 << Ki_ID;
            }
            break;

          case Kd_menu:

            if (Kd > 0) {
              Kd -= K_scale;
              var_flag |= 1 << Kd_ID;
            }
            break;
          case scale_menu:

            if (K_scale > 1) {
              K_scale /= 10;
            }
            break;
        }
      }
    }


    /**************************************************************************************************************************************************************/
    //SELECT BUTTON IS PRESSED
    if (minSelect <= voltage && voltage <= maxSelect) {

      /*case 1: we are selecting the variable to modify, so we are entering the modification menu*/
      menuState ^= 1;  //alternate between scrolling and modification menues. value 1 lets you set the selected variable, value 0 lets you scroll through LCD menu pages

      /*case 2: we are pressing SELECT button to save the changes we have done to a variable. We are leaving the modification menu and sending the new variable value to the camera*/
      if (menuState == 0) {  //IF we just left the modification menu
        switch (menuPage) {
          case setPoint_menu:
            var_flag |= 1 << setPoint_ID;
          case Kp_menu:
            var_flag |= 1 << Kp_ID;
          case Ki_menu:
            var_flag |= 1 << Ki_ID;
          case Kd_menu:
            var_flag |= 1 << Kd_ID;
            /* those are not really useful cause we want to send them every time the UP/DOWN are pressed
                      case lightLevel_menu:
                        var_flag |= 1 << lightLevel_ID;
                        break;

                      case goproServo_menu:
                        var_flag |= 1 << goproServo_ID;
                        break; */
        }
      }
      /**************************************************************************************************************************************************************/


    }

    //SAVE WHEN THE INTERRUPT OCCURED TO PREVENT BOUNCES FROM TRIGGERING MULTIPLE INTERRUPTS
    last_interrupt_time = interrupt_time;
  }
}

