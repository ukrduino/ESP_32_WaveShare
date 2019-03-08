#include <WiFi.h>
#include <Credentials/Credentials.h>
#include <ArduinoJson.h>    //https://github.com/bblanchon/ArduinoJson



//--------------------------------Open weather map---------------------------------------------------
int weatherID = 0;
char* servername = "api.openweathermap.org";  // remote server we will connect to
char* servername2 = "http://forex.1forge.com";  // remote server we will connect to
String result;
String weatherDescription = "";
String weatherLocation = "";
String CityID = "706448"; //Sparta, Greece
String APIKEY = "85caa036010dce285793ea9a1a494fea";



//--------------------------------WIFI----------------------------------------------------
WiFiClient espClient;

//----------------------------------Currency---------------------------------------------
//https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
//https://1forge.com/forex-data-api/api-documentation
#include <HTTPClient.h>
HTTPClient http;

void setup()
{
	Serial.begin(115200);
	setup_wifi();
	getWeatherData();
	//getCurrencyData();
}

void setup_wifi() {
	// We start by connecting to a WiFi network

	Serial.print(F("Connecting to "));
	Serial.println(SSID);
	WiFi.begin(SSID, PASSWORD);
	delay(1000);

	if (WiFi.waitForConnectResult() != WL_CONNECTED) {

		Serial.println(F("Connection Failed!"));
		return;
	}
}


void loop()
{

}




void getWeatherData() //client function to send/receive GET request data.
{
	String payload;
	http.begin("http://forex.1forge.com/1.0.3/convert?from=EUR&to=USD&quantity=1&api_key=C0PpsLfvczArzUfXOZdK7pkrskTmRP0q");
	int httpCode = http.GET();
	if (httpCode > 0) { //Check for the returning code

		payload = http.getString();
		Serial.println(httpCode);
		Serial.println(payload);
	}

	else {
		Serial.println("Error on HTTP request");
	}
	http.end();
	StaticJsonBuffer<1024> json_buf;
	JsonObject &root = json_buf.parseObject(payload);
	if (!root.success())
	{
		Serial.println("parseObject() failed");
	}

	String value = root["value"];
	String text = root["text"];
	String timestamp = root["timestamp"];
	Serial.println(value);
	Serial.println(text);
	Serial.println(timestamp);
	//String result = "";
	//const int httpPort = 80;
	//if (!espClient.connect(servername2, httpPort)) {
	//	return;
	//}
	//// We now create a URI for the request
	//String url = "http://forex.1forge.com/1.0.3/convert?from=EUR&to=USD&quantity=1&api_key=C0PpsLfvczArzUfXOZdK7pkrskTmRP0q";

	//// This will send the request to the server
	//espClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
	//	"Host: " + servername2 + "\r\n" +
	//	"Connection: close\r\n\r\n");
	//unsigned long timeout = millis();
	//while (espClient.available() == 0) {
	//	if (millis() - timeout > 5000) {
	//		espClient.stop();
	//		return;
	//	}
	//}

	//// Read all the lines of the reply from server
	//while (espClient.available()) {
	//	result = espClient.readStringUntil('\r');
	//}

	//result.replace('[', ' ');
	//result.replace(']', ' ');

	//char jsonArray[result.length() + 1];
	//result.toCharArray(jsonArray, sizeof(jsonArray));
	//jsonArray[result.length() + 1] = '\0';

	//StaticJsonBuffer<1024> json_buf;
	//JsonObject &root = json_buf.parseObject(jsonArray);
	//if (!root.success())
	//{
	//	Serial.println("parseObject() failed");
	//}

	//String location = root["city"]["name"];
	//String temperature = root["list"]["main"]["temp"];
	//String weather = root["list"]["weather"]["main"];
	//String description = root["list"]["weather"]["description"];
	//String idString = root["list"]["weather"]["id"];
	//String timeS = root["list"]["dt_txt"];

	//weatherID = idString.toInt();
	//Serial.print("WeatherID: ");
	//Serial.println(weatherID);
	//Serial.println(location);
	//Serial.println(temperature);
	//Serial.println(weather);
	//Serial.println(description);
	//Serial.println(timeS);
	Serial.println(result);
}

void getCurrencyData() //client function to send/receive GET request data.
{
	String result = "";
	const int httpPort = 80;
	if (!espClient.connect(servername, httpPort)) {
		return;
	}
	// We now create a URI for the request
	String url = "/data/2.5/forecast?id=" + CityID + "&units=metric&cnt=1&APPID=" + APIKEY;

	// This will send the request to the server
	espClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + servername + "\r\n" +
		"Connection: close\r\n\r\n");
	unsigned long timeout = millis();
	while (espClient.available() == 0) {
		if (millis() - timeout > 5000) {
			espClient.stop();
			return;
		}
	}

	// Read all the lines of the reply from server
	while (espClient.available()) {
		result = espClient.readStringUntil('\r');
	}

	result.replace('[', ' ');
	result.replace(']', ' ');

	char jsonArray[result.length() + 1];
	result.toCharArray(jsonArray, sizeof(jsonArray));
	jsonArray[result.length() + 1] = '\0';

	StaticJsonBuffer<1024> json_buf;
	JsonObject &root = json_buf.parseObject(jsonArray);
	if (!root.success())
	{
		Serial.println("parseObject() failed");
	}

	String location = root["city"]["name"];
	String temperature = root["list"]["main"]["temp"];
	String weather = root["list"]["weather"]["main"];
	String description = root["list"]["weather"]["description"];
	String idString = root["list"]["weather"]["id"];
	String timeS = root["list"]["dt_txt"];

	weatherID = idString.toInt();
	Serial.print("WeatherID: ");
	Serial.println(weatherID);
	Serial.println(location);
	Serial.println(temperature);
	Serial.println(weather);
	Serial.println(description);
	Serial.println(timeS);
	Serial.println(result);
}

