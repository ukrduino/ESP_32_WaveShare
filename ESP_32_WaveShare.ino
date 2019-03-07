#include <WiFi.h>
#include <PubSubClient.h>
#include <Credentials/Credentials.h>


const int DONE_PIN = 13;
double battery_voltage = 0.0;
double voltage_devider_ratio = 2.2106;
bool wifiConnected = false;
bool sensorReadingOk = true;

//-------------------------------NTP---------------------------------------------------
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>    // Built-in

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String time_str, current_hour, current_minute, current_day, current_month, current_year;

//--------------------------------WIFI----------------------------------------------------
WiFiClient espClient;

//--------------------------------MQTT----------------------------------------------------
PubSubClient client(espClient);

//--------------------------------E-Paper----------------------------------------------------

#include <GxEPD.h>
#include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r 128/296 vertical

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\espressif\esp32\variants\lolin32\pins_arduino.h
GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4  i have used 14 instead 4 ***128/296


//--------------------------------Sensor----------------------------------------------------

#include <SparkFunHTU21D.h>
HTU21D sensor;
float humd = 0.00;
float temp = 0.00;
String humidity = "0.00";
String temperature = "0.00";

void setup()
{
	Serial.begin(115200);
	pinMode(DONE_PIN, OUTPUT);
	digitalWrite(DONE_PIN, LOW);
	//battery_voltage = ReadVoltage(36) * voltage_devider_ratio;
	getSensorData();
	if (sensorReadingOk)
	{
		startWiFi();
		if (wifiConnected)
		{
			startTime();
			delay(1000);
			connectToBroker();
			delay(1000);
		}
		stopWiFi();
		display.init(115200);
		drawDisplay();
	}
	digitalWrite(DONE_PIN, HIGH);
}

void loop()
{

}

void getSensorData() {
	sensor.begin();
	sensor.setResolution(0b10000001);
	humd = sensor.readHumidity();
	temp = sensor.readTemperature();
	if (temp > 50 || humd > 100)
	{
		delay(500);
		temp = sensor.readTemperature();
		humd = sensor.readHumidity();
	}
	if (temp > 50 || humd > 100)
	{
		sensorReadingOk = false;
	}
	else
	{
		humidity = String(humd, 1);
		temperature = String(temp, 1);
		sensorReadingOk = true;
	}
}

//Connection to MQTT broker
void connectToBroker() {
	client.setServer(SERVER_IP, MQTT_SERVER_PORT);
	if (client.connect("ESP32_E_Paper_Screen")) {
		client.publish("ESP32_E_Paper_Screen/temperature", temperature.c_str());
		delay(1000);
		client.publish("ESP32_E_Paper_Screen/humidity", humidity.c_str());
	}
}


void drawDisplay()
{
	display.setRotation(1);
	display.fillScreen(GxEPD_WHITE);
	display.setTextColor(GxEPD_BLACK);
	/*display.drawBitmap(Olga_Sergey, 0, 100, 300, 200, GxEPD_BLACK);*/
	display.fillRect(0, 108, 296, 2, GxEPD_RED);

	display.setFont(&FreeSans24pt7b);
	display.setCursor(20, 80);
	display.print(temperature);
	display.setCursor(170, 80);
	display.print(humidity);
	display.setTextColor(GxEPD_RED);
	display.setFont(&FreeSans18pt7b);
	display.setCursor(115, 80);
	display.print("C");
	display.setCursor(260, 80);
	display.print("%");


	display.setTextColor(GxEPD_BLACK);
	display.setFont(&FreeSansBold9pt7b);
	display.setCursor(260, 126);
	display.print(battery_voltage);

	display.setCursor(0, 126);
	display.print(time_str);
	display.update();
}

double ReadVoltage(byte pin) {
	double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
	if (reading < 1 || reading > 4095) return 0;
	// return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
	return -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
}


void startTime() {
	configTime(2 * 3600, 0, "0.uk.pool.ntp.org", "time.nist.gov"); // long gmtOffset_sec, int daylightOffset_sec (летнее иди зимнее время)
	UpdateLocalTime();
}

void UpdateLocalTime() {
	struct tm timeinfo;
	while (!getLocalTime(&timeinfo)) {
		Serial.println("Failed to obtain time");
	}
	//See http://www.cplusplus.com/reference/ctime/strftime/
	Serial.println(&timeinfo, "%a %b %d %Y   %H:%M:%S"); // Displays: Saturday, June 24 2017 14:05:49
	char output[50];
	strftime(output, 50, "%a %d-%b-%y  (%H:%M:%S)", &timeinfo);
	time_str = output;
}


void startWiFi() {
	int connAttempts = 0;
	Serial.print(F("\r\nConnecting to: ")); Serial.println(String(SSID));
	WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500); Serial.print(".");
		if (connAttempts > 20) {
			WiFi.disconnect();
			Serial.println("Cant connect WiFi :(");
			wifiConnected = false;
			return;
		}
		connAttempts++;
	}
	Serial.println("WiFi connected");
	wifiConnected = true;
}

void stopWiFi() {
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	wifiConnected = false;
}