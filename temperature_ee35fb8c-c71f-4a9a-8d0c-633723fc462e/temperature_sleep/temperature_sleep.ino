// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network details
const char* ssid = "<PLACE_YOUR_SSID_HERE>";
const char* password = "<PLACE_YOUR_PASSWORD_HERE>";
const String uuid = "ee35fb8c-c71f-4a9a-8d0c-633723fc462e";
const String host = "xdevicesdev.home";
const String httpPort = "8000";

// GPIO 5 
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];

void getTemperature() {
  float tempC;
  DS18B20.requestTemperatures(); 
  tempC = DS18B20.getTempCByIndex(0);
  // Details: https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__stdlib_1ga060c998e77fb5fc0d3168b3ce8771d42.html
  dtostrf(tempC, 2, 2, temperatureCString);
  delay(100);
}

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(30);

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
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // put your main code here, to run repeatedly:

  Serial.print("connecting to ");
  Serial.println(host);

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

  Serial.println("Going into deep sleep for 15 minutes");
  ESP.deepSleep(900e6); 
  delay(500);
  
}



// runs over and over again
void loop() {
}   
