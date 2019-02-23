#include <WiFi.h>
#include <PubSubClient.h>
#include <Credentials/Credentials.h>

const int DONE_PIN = 13;
int battery_voltage = 0; // raw reading
float valtage_devider_ratio = 0.0;

//--------------------------------WIFI----------------------------------------------------
WiFiClient espClient;

//--------------------------------MQTT----------------------------------------------------
PubSubClient client(espClient);

//--------------------------------E-Paper----------------------------------------------------

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one
#include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r

#include "test.h"
//#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\espressif\esp32\variants\lolin32\pins_arduino.h
GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4  i have used 14 instead 4


//--------------------------------Sensor----------------------------------------------------

#include <SparkFunHTU21D.h>

HTU21D sensor;
float humd = 0.00;
float temp = 0.00;
String humidity = "0.00";
String temperature = "0.00";

void setup()
{
	pinMode(DONE_PIN, OUTPUT);
	digitalWrite(DONE_PIN, LOW);
	Serial.begin(115200);
	sensor.begin();
	humd = sensor.readHumidity();
	temp = sensor.readTemperature();
	humidity = String(humd, 1);
	temperature = String(temp, 1);
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);
	delay(3000);
	connectToBroker();
	//getDataFromServer();
	display.init(115200);
	drawScreen1();
	showFontr("FreeMonoBold12pt7b");
	Serial.println(millis());
	digitalWrite(DONE_PIN, LOW);	
}


//void getDataFromServer() {
//	http.begin(SERVER_IP, HTTP_OTA_PORT); //Specify the URL
//}

void loop()
{

}

void drawScreen1()
{
	display.drawExamplePicture(BitmapWaveshare_black, BitmapWaveshare_red, sizeof(BitmapWaveshare_black), sizeof(BitmapWaveshare_red));
}

void showFontr(const char name[])
{
	display.setTextColor(GxEPD_BLACK);
	display.setFont(&FreeMonoBold12pt7b);
	display.setCursor(0, 0);
	display.println();
	display.println(name);
	display.println(" !\"#$%&'()*+,-./");
	display.println("0123456789:;<=>?");
	display.println("@ABCDEFGHIJKLMNO");
	display.println("PQRSTUVWXYZ[\\]^_");
	display.println("`abcdefghijklmno");
	display.println("pqrstuvwxyz{|}~ ");
	display.update();
	delay(5000);
}

//
//#if defined(_GxGDEW029Z10_H_)
//#define HAS_RED_COLOR
//void showBitmapExample()
//{
//	display.drawPicture(BitmapWaveshare_black, BitmapWaveshare_red, sizeof(BitmapWaveshare_black), sizeof(BitmapWaveshare_red));
//	delay(5000);
//display.drawExamplePicture(BitmapExample1, BitmapExample2, sizeof(BitmapExample1), sizeof(BitmapExample2));
//	delay(5000);
//	display.drawExamplePicture(BitmapExample3, BitmapExample4, sizeof(BitmapExample3), sizeof(BitmapExample4));
//	delay(5000);
//	display.drawExampleBitmap(BitmapWaveshare_black, sizeof(BitmapWaveshare_black));
//	delay(2000);
//	// example bitmaps for b/w/r are normal on b/w, but inverted on red
//	display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1));
//	delay(2000);
//	display.drawExampleBitmap(BitmapExample2, sizeof(BitmapExample2), GxEPD::bm_invert);
//	delay(2000);
//	display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
//	display.update();
//
//	delay(30000);
//	display.fillScreen(GxEPD_WHITE);
//	display.update();
//
//}
//#endif
//
//
//void showFontCallback()
//{
//	const char* name = "FreeMonoBold9pt7b";
//	const GFXfont* f = &FreeMonoBold9pt7b;
//	display.fillScreen(GxEPD_WHITE);
//	display.setTextColor(GxEPD_BLACK);
//	display.setFont(f);
//	display.setCursor(0, 0);
//	display.println();
//	display.println(name);
//	display.println(" !\"#$%&'()*+,-./");
//	display.println("0123456789:;<=>?");
//	display.println("@ABCDEFGHIJKLMNO");
//	display.println("PQRSTUVWXYZ[\\]^_");
//#if defined(HAS_RED_COLOR)
//	display.setTextColor(GxEPD_RED);
//#endif
//	display.println("`abcdefghijklmno");
//	display.println("pqrstuvwxyz{|}~ ");
//}
//
//void drawCornerTest()
//{
//	display.drawCornerTest();
//	delay(5000);
//	uint8_t rotation = display.getRotation();
//	for (uint16_t r = 0; r < 4; r++)
//	{
//		display.setRotation(r);
//		display.fillScreen(GxEPD_WHITE);
//		display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
//		display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
//		display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
//		display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
//		display.update();
//		delay(5000);
//	}
//	display.setRotation(rotation); // restore
//}



//Connection to MQTT broker
void connectToBroker() {
	client.setServer(SERVER_IP, MQTT_SERVER_PORT);
	if (client.connect("ESP32_E_Paper_Screen")) {
		client.publish("ESP32_E_Paper_Screen/temperature", temperature.c_str());
		client.publish("ESP32_E_Paper_Screen/humidity", humidity.c_str());
	}
}

