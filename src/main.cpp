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

Display display{SCREEN_WIDTH, SCREEN_HEIGHT};

XBee xbee{2, RESET_PIN, &display};

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

	xbee.connect();
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
		if (actionButton.isPressed()) xbee.connect();
	}
	
	display.update(xbee.isConnected(), currentChannel, currentBandwidth, firmwareVersion, selectedChannel, selectedBandwidth);
}

// // https://docs.digi.com//resources/documentation/digidocs/90002273/default.htm#concepts/c_90002273_start.htm?TocPath=Digi%2520XBee%25C2%25AE%25203%2520802.15.4%2520RF%2520Module%257C_____0
// // "C:\Users\Elliott\AppData\Local\Digi\XCTU-NG\radio_firmwares\XB3-24A\XB3-24A_2014-th.gbl" -- firmware location



//   pinMode(RESET_PIN, OUTPUT);

//   resetXbee();   


// bool tryUpdateFirmware = true;

//   // if (tryUpdateFirmware) {
//   //   if (firmwareVersion[1] != LATEST_FIRMWARE[1] || firmwareVersion[2] != LATEST_FIRMWARE[2] || firmwareVersion[3] != LATEST_FIRMWARE[3]) {
//   //     updateFirmware();
//   //   }
//   // }
// }

// // 
// // 
// // 
// // 
// // 
// // 

// // 
// // 
// // 
// // 
// // 
// // 
// // 
// // 
// // 



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