#include <Arduino.h>
#include <Wire.h>

#include "Display.h"
#include "XBee.h"
#include "Debounce.h"

#define i2c_ADDRESS 0x3c // This specific display uses this address
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define CHANNEL_IN_PIN 32 // The pin to connect the switch that toggles between channel 'C' and 'F'
#define BANDWIDTH_IN_PIN 33 // The pin to connect the switch that toggles between bandwidth '555' and '3332'
#define ACTION_PIN 25 // The pin to connect the button that both connects to the XBee and programs it with the selected settings


#define RESET_PIN 7


#define DEBOUNCE_DELAY 40 // This is the delay for the debounce for the three inputs. Higher value means more time required to pass before the program will allow the button to be pressed again. It's in ms

XBee xbee{2, RESET_PIN};

Display display{SCREEN_WIDTH, SCREEN_HEIGHT};

Debounce channelButton{CHANNEL_IN_PIN, DEBOUNCE_DELAY};
Debounce bandwidthButton{BANDWIDTH_IN_PIN, DEBOUNCE_DELAY};
Debounce actionButton{ACTION_PIN, DEBOUNCE_DELAY};

std::string currentChannel = "C";
std::string currentBandwidth = "555";
std::string firmwareVersion = "2014";

ChannelSelections selectedChannel = ChannelSelections::C;
BandwidthSelections selectedBandwidth = BandwidthSelections::B555;

void setup()
{
	Serial.begin(9600);
	while (!Serial) {}

	xbee.begin(9600);
	while (!xbee) {}

	if (!display.initiliaze(i2c_ADDRESS)) throw "Display did not initialize properly";

	xbee.connect(&display);
}

void loop()
{
	if (xbee.isConnected())
	{
		if (channelButton.isPressed())
		{
			if (selectedChannel == ChannelSelections::C) selectedChannel = ChannelSelections::F;
			else selectedChannel = ChannelSelections::C;
		}
		if (bandwidthButton.isPressed())
		{
			if (selectedBandwidth == BandwidthSelections::B555) selectedBandwidth = BandwidthSelections::B3332;
			else selectedBandwidth = BandwidthSelections::B555;
		}
		if (actionButton.isPressed())
		{
			xbee.program(selectedChannel, selectedBandwidth);
		}

		std::vector<std::string> pingResults = xbee.ping();
		currentChannel = pingResults[0];
		currentBandwidth = pingResults[1];
		firmwareVersion = pingResults[2];
	}
	else
	{
		if (actionButton.isPressed()) xbee.connect(&display);
	}
	
	display.update(xbee.isConnected(), currentChannel, currentBandwidth, firmwareVersion, selectedChannel, selectedBandwidth);
}

// // https://docs.digi.com//resources/documentation/digidocs/90002273/default.htm#concepts/c_90002273_start.htm?TocPath=Digi%2520XBee%25C2%25AE%25203%2520802.15.4%2520RF%2520Module%257C_____0
// // "C:\Users\Elliott\AppData\Local\Digi\XCTU-NG\radio_firmwares\XB3-24A\XB3-24A_2014-th.gbl" -- firmware location



//   pinMode(RESET_PIN, OUTPUT);

//   resetXbee();   


// bool tryUpdateFirmware = true;


// unsigned long previousTime;

// void displayDots() {
//   previousTime = millis();
//   while ((millis() - previousTime ) < 1100) {
//     if (millis() % 200 == 0) {
//       display.print(F("."));
//       display.display();
//     }
//   }
// }

// void connectToXBee() {
//   // // Clears the buffer - probably not needed, but ensures nothing in the way of the "OK" signal
//   // while (xbee.available()) xbee.read();
//   // display.setCursor(0, 0);
//   // display.clearDisplay();
//   // normalColor();
//   // display.print(F("Initializing XBee"));
//   // display.display();
//   // // Waits the one second before (plus a little extra) and displays dots to show something is happening
//   // displayDots();
//   // // Sends the command character three times to the XBee
//   // xbee.write("+++", 3);
//   // // Waits the one second after (plus a little extra) and displays dots to show something is happening
//   // displayDots();

//   // display.clearDisplay();
//   // display.setCursor(0, 0);
//   // if (xbee.available()) {
//   //   if (xbee.find("OK\r")) {
//   //     xbeeFound = true;
//   //     display.println(F("Xbee has successfully entered Command Mode"));
//   //   }
//   //   else {
//   //     display.println(F("Xbee did not confirm entering Command Mode"));
//   //   }
//   // }
//   // else {
//   //   display.println(F("Xbee was not found"));
//   // }
//   // display.display();
//   // delay(1000);


//   // if (tryUpdateFirmware) {
//   //   if (firmwareVersion[1] != LATEST_FIRMWARE[1] || firmwareVersion[2] != LATEST_FIRMWARE[2] || firmwareVersion[3] != LATEST_FIRMWARE[3]) {
//   //     updateFirmware();
//   //   }
//   // }
// }


// // #define SOH 0x01
// // #define EOT 0x04
// // #define ACK 0x06
// // #define NAK 0x15
// // #define CAN 0x18
// // #define CRC_REQUEST 'C'

// // uint16_t crc16(const uint8_t *buf, uint16_t len) {
// //   uint16_t crc = 0;
// //   while (len--) {
// //     crc ^= (uint16_t)*buf++ << 8;
// //     for (uint8_t i = 0; i < 8; i++)
// //       crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
// //   }
// //   return crc;
// // }

// // bool sendXmodemCRC(Stream &serial, File &firmware) {
// //   uint8_t packet[128];
// //   uint8_t packetNum = 1;

// //   // 1️⃣ Wait for bootloader to send 'C'
// //   unsigned long start = millis();
// //   while (millis() - start < 5000) {
// //     if (serial.available() && serial.read() == 'C') break;
// //   }

// //   while (true) {
// //     int read = firmware.read(packet, 128);

// //     if (read <= 0) {
// //       // 2️⃣ Send EOT
// //       serial.write(EOT);
// //       while (serial.read() != ACK);
// //       return true;
// //     }

// //     // pad short packet
// //     for (int i = read; i < 128; i++) packet[i] = 0x1A;

// //     // 3️⃣ Build and send packet
// //     serial.write(SOH);
// //     serial.write(packetNum);
// //     serial.write(~packetNum);

// //     uint16_t crc = crc16(packet, 128);
// //     serial.write(crc >> 8);
// //     serial.write(crc & 0xFF);

// //     // 4️⃣ Wait for ACK
// //     int r = -1;
// //     unsigned long t = millis();
// //     while (millis() - t < 2000) {
// //       if (serial.available()) {
// //         r = serial.read();
// //         break;
// //       }
// //     }
// //     if (r != ACK) return false;

// //     packetNum++;
// //   }
// // }

// void updateFirmware() {
//   // while (true) {
      
//   //   if (channelButton.GetState() == LOW || bandwidthButton.GetState() == LOW) {
//   //     tryUpdateFirmware = !tryUpdateFirmware;
//   //   }
    
//   //   if (actionButton.GetState() == LOW) break;

    
//   //   display.clearDisplay();
//   //   normalColor();
//   //   display.setCursor(0, 0);
//   //   display.println(F("   Update firmware?   "));

//   //   display.print("     ");
//   //   setSelectedColor(true, tryUpdateFirmware);
//   //   display.print(F(" Yes "));

//   //   normalColor();
//   //   display.print(F("    "));

//   //   setSelectedColor(false, tryUpdateFirmware);
//   //   display.println(F(" No "));

//   //   display.display();
    
//   //   sendATCommand(CHANNEL_AT_CMD); // feeds the command timeout
//   // }

//   // if (!tryUpdateFirmware) return;

//   // firmwareXbee.begin(115200);
//   // delay(10);
//   // sendATCommand(INVOKE_BOOTLOADER_AT_CMD);
//   // xbee.end();

//   // firmwareXbee.write('1');
//   // firmwareXbee.flush();

//   // firmwareXbee.end();
//   // xbee.begin(9600);

// }


// void resetXbee() {
//   digitalWrite(RESET_PIN, LOW);
//   delay(10);
//   digitalWrite(RESET_PIN, HIGH);
// }