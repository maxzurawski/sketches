// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network details
const char* ssid = "<PLACE_YOUR_SSID_HERE>";
const char* password = "<PLACE_YOUR_WIFI_PASSWORD_HERE>";
const String uuid = "4896a44a-4338-4c23-b828-e279a3368d94";
const String host = "xdevicesdev.home";
const String httpPort = "8000";

// GPIO 5 
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];

const unsigned long SECOND = 1000;
const unsigned long TEN_SECONDS = 10*SECOND;

void getTemperature() {
  float tempC;
  DS18B20.requestTemperatures(); 
  tempC = DS18B20.getTempCByIndex(0);
  dtostrf(tempC, 2, 2, temperatureCString);
}

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.println("ESP8266 in waking up mode");
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial){}

  DS18B20.begin(); 
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// runs over and over again
void loop() {
  Serial.println("ESP8266 in da loop");

  HTTPClient client;

  Serial.println("Getting temperature");
  getTemperature();
  String url = "/api/dispatcher/temperature/" + uuid + "/" + temperatureCString;
  String requestURL = "http://" + host + ":" + httpPort + url;
  Serial.println(requestURL);
  client.begin(requestURL);
  int httpResponseCode = client.POST(0, 0);

  if(httpResponseCode > 0) {
    String response = client.getString();
    Serial.println(httpResponseCode);
    Serial.print(response);
  } else {
    Serial.print("ERROR on sending POST: ");
    Serial.println(httpResponseCode);
  }
  client.end();
  delay(TEN_SECONDS);
}   
