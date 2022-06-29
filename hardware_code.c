#include <ESP8266HTTPClient.h>
#include "ESP8266WiFi.h"
#include <DHT.h>
#include <DHT_U.h>  
DHT dht2(2,DHT11);  
int value=0;
int motorPin = 3; //motor transistor is connected to pin 3
int measurePin=A0;//dust sensor to A0
int ledPower=4;//dust sensor led to D2
int samplingTime=280;
int deltaTime=40;
int sleepTime=9680;
int prediction=0;
float voMeasured=0;
float calcVoltage=0;
float dustDensity=0;


#include <ArduinoJson.h>
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
String key = "YOUR_API_KEY";
//int status = WL_IDLE_STATUS;
String jsonString = "{\n";

double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;
int more_text = 1; // set to 1 for more debug output

#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "project-fadd1-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "6iLCUSCDmI4pzCJO85UYy4LcalA4kG9BvnduCvlM"
#define WIFI_SSID "ketak"
#define WIFI_PASSWORD "12345678"

void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(ledPower,OUTPUT);
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  digitalWrite(ledPower,LOW);//led power on
  delayMicroseconds(samplingTime);

  voMeasured=analogRead(measurePin);//read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);//led off
  delayMicroseconds(sleepTime);
  calcVoltage=voMeasured*(5.0/1024.0);
  dustDensity=170*calcVoltage-0.1;
  
  float temp=dht2.readTemperature( );
  float hum=dht2.readHumidity();

//code for fetching location of user

  char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");

    if (more_text) {
      // Print out the formatted json...
      Serial.println("{");
      Serial.println("\"homeMobileCountryCode\": 234,");  // this is a real UK MCC
      Serial.println("\"homeMobileNetworkCode\": 27,");   // and a real UK MNC
      Serial.println("\"radioType\": \"gsm\",");          // for gsm
      Serial.println("\"carrier\": \"Vodafone\",");       // associated with Vodafone
      //Serial.println("\"cellTowers\": [");                // I'm not reporting any cell towers
      //Serial.println("],");
      Serial.println("\"wifiAccessPoints\": [");
      for (int i = 0; i < n; ++i)
      {
        Serial.println("{");
        Serial.print("\"macAddress\" : \"");
        Serial.print(WiFi.BSSIDstr(i));
        Serial.println("\",");
        Serial.print("\"signalStrength\": ");
        Serial.println(WiFi.RSSI(i));
        if (i < n - 1)
        {
          Serial.println("},");
        }
        else
        {
          Serial.println("}");
        }
      }
      Serial.println("]");
      Serial.println("}");
    }
    Serial.println(" ");
  }
  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"homeMobileCountryCode\": 234,\n"; // this is a real UK MCC
  jsonString += "\"homeMobileNetworkCode\": 27,\n";  // and a real UK MNC
  jsonString += "\"radioType\": \"gsm\",\n";         // for gsm
  jsonString += "\"carrier\": \"Vodafone\",\n";      // associated with Vodafone
  jsonString += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"macAddress\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signalStrength\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  //--------------------------------------------------------------------

  Serial.println("");

  WiFiClientSecure client;

  //Connect to the client and make the api call
  Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)Host + thisPage + "AIzaSyCYNXIYINPmTNIdusMjJloS4_BXSOff1_g");
  Serial.println(" ");
  if (client.connect(Host, 443)) {
    Serial.println("Connected");
    client.println("POST " + thisPage + key + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }

  //Read and parse all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (more_text) {
      Serial.print(line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["location"]["lat"];
      longitude   = root["location"]["lng"];
      accuracy   = root["accuracy"];
       value = (int)(latitude * 100 + .5);
       latitude= (float)value/100;
       value = (int)(longitude * 100 + .5);
       longitude= (float)value/100;
       

    }
  }

  Serial.println("closing connection");
  Serial.println();
  client.stop();

  //printing out all sensed and fetched value

   Serial.print("Latitude = ");
   Serial.println(latitude, 3);
   Serial.print("Longitude = ");
   Serial.println(longitude, 3);
   Serial.print("Accuracy = ");
   Serial.println(accuracy);
  
   Serial.println("Temperature in C:");  
   Serial.println(temp);  
   Serial.println("Humidity in C:");  
   Serial.println(hum);
   Serial.println("AQI: ");
   Serial.println(dustDensity);  
   delay(1000);  
   
  // sending these values to cloud database 
  
  Firebase.setFloat("sensorData/Temperature", temp);
  Firebase.setFloat("sensorData/Humidity", hum);
  Firebase.setFloat("sensorData/Latitude",latitude);
  // handle error
  if (Firebase.failed()) {
      Serial.print("sending sensor data  failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  
  
  prediction=Firebase.getFloat("AllergyPrediction");
  
  // get value 
  Serial.print("AllergyPrediction: ");
  Serial.println(prediction);
  delay(1000);
  if(prediction)
  {
    digitalWrite(motorPin, HIGH); //vibrate
  }
  else
  {
    digitalWrite(motorPin, LOW); //don't vibrate  
  }


  // append a new value to /logs
  String name = Firebase.pushInt("logs", n++);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed: /logs/");
  Serial.println(name);
  delay(1000);
}s