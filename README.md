# gestureGlove
gestureGlove is a wearable keyboard device that can recognize user hand gestures as a virtual keyboard. The core of the device is the Arduino Leonardo with an ATMEGA32u4 microcontroller.

## Getting Started

These instructions will help you recreate gestureGlove for testing and personal use.
There is a User Manual below.

### Prerequisites
* [Arduino IDE](https://www.arduino.cc/) - you don't have to use this but we did
* [Capacitive Sensing Library](https://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense) 
* [Keyboard Library](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/)
* Arduino Leonardo - or any other board that uses the ATMEGA32u4 microcontroller or similar variant
* Soldering iron, some support components, etc.

### 3D printing
* We included the .stl files modeled in SketchUp to organize the many wires and provide a platform to attach a PCB to.
* These parts are attached to your hand with velcro straps.
* You do not need to print these parts but it makes quality of life and debugging better, especially with a ratsnest of wires and capacitive sensing wires.
* Please look at [glove.jpg] to see how it all comes together.

### Software Setup
```
1. Install the Arduino IDE (available for Windows, Linux, Mac)
2. Install the OLED library directly from within the IDE or git to your *Arduino/libraries* folder.
3. Download the DigiSafe source files
4. Open the .ino file
5. Modify the source to suit your hardware configuration.
6. Plug in your USB device, select the correct COM port and board.
7. Upload the code!
```
## User Manual
* Assemble your device by connecting the base module to the finger sensor array.
* By default the glove is configured for left-hand use. Right-hand use requires swapping the sensor indexes in the source code and reflashing the firmware to the Arduino. In the future, there will be a switch to swap handedness without reprogramming.
* Attach the base module to your wrist with velcro through the mounting points on the 3D printed parts.
* Attach each finger sensor by compressing the touch fabric between the velcro mounting points and finger. Depending on factors such as skin moisture, proximity to electrical noise, etc affects the reading of capacitive sensor. A custom touch threshold may need to be set in the source code and reflashed to the Arduino. In the future, capacitive autocalibration may be added, removing the need for reprogramming.
* After attaching to the hand, connect a USB cable to computer and keep your hand very still for a minute. This time is used for the calibration of the Accelerometer and Gyroscope sensors *notice how led transition time is slow*. Those LEDs represent what finger is currently being read.
* The device is ready for use, currently only supporting 3 buttons on 1 finger.
* Experiment with device...
* Debugging can be accessed by connecting via Bluetooth scannable as BTdebug. This is a serial port that outputs MPU6050 sensor values.
* DEBUGGING: on the base platform, there are 3 buttons.
* DEBUGGING: button 1: toggles debug state (sensor printouts or keyboard engine)
* DEBUGGING: button 2: switches between sensor type: RAW accelerometer, RAW gyroscope, Calibrated Gyroscope Angle with Accelerometer. (sensor printouts)
* DEBUGGING: button 3: switches between sensor axis type: X Y Z, X only, Y only, Z only. (sensor printouts)

## Authors

* **Marvin Mallari** - *Research, Base Hardware Designer, MPU i2c Multiplexer code*, repo-owner
* **Dawei Zhang** - *Research, Capacitive Hardware Designer, Hand Gesture detection code*
* **Kai He** - *Research, Hardware Research, Presentation*
