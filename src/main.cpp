#include <Arduino.h>
#include <Wire.h>

#include "Display.h"
#include "XBee.h"
#include "Debounce.h"
#include "ToggleSwitch.h"
#include "Selections.h"

#define i2c_ADDRESS 0x3c // This specific display uses this address
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define ACTION_PIN 32 // The pin to connect the button that both connects to the XBee and programs it with the selected settings
#define CHANNEL_C_PIN 27 // Toggles the channel selection to 'C' 
#define CHANNEL_F_PIN 26 // Toggles the channel selection to 'F' 
#define BANDWITDH_555_PIN 25 // Toggles the bandwitdh selection to '555' 
#define BANDWIDTH_3332_PIN 33 // Toggles the bandwitdh selection to '3332' 

#define RESET_PIN 7


#define DEBOUNCE_DELAY 40 // This is the delay for the debounce for the three inputs. Higher value means more time required to pass before the program will allow the button to be pressed again. It's in ms

Display display{SCREEN_WIDTH, SCREEN_HEIGHT};

XBee xbee{2, RESET_PIN, &display};

ToggleSwitch<Channel::Selections> channelSwitch{CHANNEL_C_PIN, CHANNEL_F_PIN, Channel::Selections::C, Channel::Selections::F, Channel::Selections::Null};
ToggleSwitch<Bandwidth::Selections> bandwidthSwitch{BANDWITDH_555_PIN, BANDWIDTH_3332_PIN, Bandwidth::Selections::B555, Bandwidth::Selections::B3332, Bandwidth::Selections::Null};
Debounce actionButton{ACTION_PIN, DEBOUNCE_DELAY};

std::string currentChannel = "C";
std::string currentBandwidth = "555";
std::string firmwareVersion = "2014";

Channel::Selections selectedChannel = Channel::Selections::C;
Bandwidth::Selections selectedBandwidth = Bandwidth::Selections::B555;

void setup()
{
	Serial.begin(9600);
	while (!Serial) {}

	Serial.println("Loading XBee Programmer");
	#if not(FIRMWARE_TEST_MODE)
	Serial.println("Will automatically update any XBee that does not have a compatible (20xx) firmware version");
	Serial.println("The most recent firmware version as of development is " LATEST_FIRMWARE_VERSION);
	#else
	Serial.println("Test mode enabled. Will automatically downgrade any XBee that does not have the 1014 firmware version");
	#endif

	xbee.begin(9600);
	while (!xbee) {}

	if (!display.initiliaze(i2c_ADDRESS)) throw "Display did not initialize properly";

	xbee.connect();
}

void loop()
{
	if (xbee.isConnected())
	{
		selectedChannel = channelSwitch.GetValueFromSwitch();
		selectedBandwidth = bandwidthSwitch.GetValueFromSwitch();
		
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

// "C:\Users\Elliott\AppData\Local\Digi\XCTU-NG\radio_firmwares\XB3-24Z\XB3-24Z_1014-th.gbl" -- older firmware location, only for testing the update firmware function, not compatible with 802.15.4