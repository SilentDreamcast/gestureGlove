# gestureGlove
gestureGlove is a wearable keyboard device that can recognize user hand gestures as a virtual keyboard. The core of the device is the Arduino Leonardo with an ATMEGA32u4 microcontroller.

## Getting Started

These instructions will help you recreate gestureGlove for testing and personal use.

### Prerequisites
* [Arduino IDE](https://www.arduino.cc/) - you don't have to use this but we did
* [Capacitive Sensing Library](https://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense) 
* [Keyboard Library](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/)
* Arduino Leonardo - or any other board that uses the ATMEGA32u4 microcontroller or similar variant
* Soldering iron, some support components, etc.

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

## Authors

* **Marvin Mallari** - *Research, Base Hardware Designer, MPU i2c Multiplexer code*, repo-owner
* **Dawei Zhang** - *Research, Capacitive Hardware Designer, Hand Gesture detection code*
* **Kai He** - *Research, Hardware Research, Presentation*
