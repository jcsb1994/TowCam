# Underwater Automated TowCam
This project has been developped in collaboration with the EEL Laboratory, affiliated with California State University, Monterey Bay.
Its main purpose is to increase seafloor mapping efficiency by combining the speed of a boat to the cheaper price of a camera 
compared to an ROV.

This project uses custom 3D printed parts, custom fabricated parts, and stock products from manufacturer BlueRobotics (https://www.bluerobotics.com).

# Electronics Components

   <b>MICROCONTROLLERS (2)</b><br>
    1. Arduino in the boat to be used as a controller, connected via serial to the other Arduino.
    2. Arduino in the submarine's enclosure.
      Serial communication between 2 arduinos: SoftwareSerial and AltSoftSerial libraries were used to clear RX and TX pins for debugging

   <b>SENSORS (3)</b><br>
    1. linear potentiometer<br>
      INPUT PIN: Analog, pullup<br>
    2. Pressure sensor<br>
      INPUT PIN: requires MS5837 Arduino Library.
      3V3 LOGIC: needs a I2C Level Converter between arduino and sensor to operate http://docs.bluerobotics.com/level-converter/#introduction<br>
    3. altimeter<br>
      INPUT PIN: 5 (RX of its own softwareSerial instance)
      Aiming directly under the tow camera.
      Vertical positioning: For now, no second SONAR: will be using the GoPro to locate incoming obstacles

   <b>ACTUATORS (3)</b>
    1. linear actuator (taken from the Newton gripper)<br>
      output PIN: digital, requires Servo library
      3V3 LOGIC: needs a I2C Level Converter between arduino and actuator to operate http://docs.bluerobotics.com/level-converter/#introduction<br>
    2. Servo motor (controlling the GoPro mount angle)<br>
      output PIN: digital, requires Servo library<br>
    3. Lumen light by blue robotics<br>
      output PIN: digital, requires Servo library
    


# Mechanical Components

<b>BlueRobotics products</b><br>
https://www.bluerobotics.com/store/watertight-enclosures/3-series/wte3-asm-r1/#configuration
https://www.bluerobotics.com/store/sensors-sonars-cameras/sensors/bar30-sensor-r1/
https://www.bluerobotics.com/store/thrusters/grippers/newton-gripper-asm-r1-rp/
https://www.bluerobotics.com/store/thrusters/lights/lumen-r2-rp/   <br>
SONAR Altimeter: <b> not in stocks (yet) </b>

<b>3D printed parts</b><br>
*All 3D printed parts are available in the stl_files folder

