![Two Sigma 8x8 blinky](https://farm9.staticflickr.com/8883/27674811764_a58d1e7645_z_d.jpg)

# Two Sigma 8x8 Blinky

The Two Sigma 8x8 Blinky is an Arduino-compatible wearable electronics platform manufactured for Two Sigma Investments by Blinkinlabs.  The 8x8 Blinky is equipped with a WiFi-enabled microcontroller platform (Espressif ESP8266) and may be used for connected device applications.

## Install
1. Install a recent release of the Arduino IDE (version 1.6.8 or newer) from [Arduino.cc](https://www.arduino.cc/en/Main/Software).
1. Launch the Arduino IDE, then access the application's Preferences panel,
1. Paste the board URL, `http://arduino.esp8266.com/stable/package_esp8266com_index.json` into the "Additional Boards Manager URLs" field.
1. Install the ESP8266 board support, via Tools > Board Manager, search for "ESP8266" and select "Install".  This is the last step.

## Getting Started
1. Within the Arduino IDE, open "MegaDemo.ino" from the "MegaDemo" folder.
1. To Upload the demo to the 8x8 Blinky, first select "Generic ESP8266 Module" from Tools > Boards,
1. Change the following items under the Tools menu as follows (unlisted items should remain unchanged):
  1. Flash Mode: QIO
  1. Flash Size: 2M (1M SPIFFS)
  1. Reset Method: nodemcu
  1. Upload Speed: 460800 (Use 215600 on Microsoft Windows systems.)
  1. Port: <varies, typically "/dev/cu.usbmodem1411" on Mac, or "COM#" on Microsoft Windows.>
1. Invoke the "Upload" function under the Sketch menu.

## Where to learn more about the Espressif ESP8266 WiFi Module on the 8x8 Blinky
The ESP8266 is popular for hobbyists building "Internet of Things" connected devices, and has tremendous potential.  Here are three good sources to learn more about what you might do with your 8x8 Blinky:

1. The [ESP8266 Arduino User Community](http://www.esp8266.com/viewforum.php?f=25&sid=d6e359cf888a34141c95f1bc94b0aabb)
2. Code examples located within the Arduino IDE under File > Examples > "Examples for Generic ESP8266 Module" (visible only when Tools > Board > "Generic ESP8266 Module" is selected)
3. The [Blinkinlabs 8x8 Documentation](https://github.com/Blinkinlabs/EightByEight/blob/master/documentation/Getting%20started%20guide.pdf)

## Contributing and Learning More About Two Sigma
We warmly welcome pull requests!  The TS 8x8 will be making appearances at a number of events attended by Two Sigma, and we hope you'll find it an inspiring platform to experiment and learn about the field of wearable technology and creative computing!

Learn more about Two Sigma Investments and our culture of creativity at (https://www.twosigma.com!

Finally, explore our other open source contributions at https://www.github.com/twosigma!


