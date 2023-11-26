#ifdef ESP32
#include <WiFi.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#endif

//Library for DS18B20 temp. sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>
#define ONE_WIRE_BUS 2 //GPIO 2

//Firebase libarary
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//FirebaseConfig FIREBASE_HOST;
//FirebaseAuth FIREBASE_AUTH;

#define FIREBASE_HOST "https://taal2-b082a-default-rtdb.asia-southeast1.firebasedatabase.app" 
#define FIREBASE_AUTH "AIzaSyDwkuJwJO5mDSbqA2TkX7YigkTbk_jC_OY" // Replace with your Firebase database secret

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Replace with your network credentials
const char* ssid = "oninsama";
const char* password = "123qweasd";
const char * ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 8 * 60 * 60; // GMT +8
const int daylightOffset_sec = 0;

// Create AsyncWebServer object on port 80
//AsyncWebServer server(80);


//GlobaL variable & set up for PH SENSOR
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;
float ph_act;
float calibration_value = 20.24 - 0.7; // 21.34 - 0.7


//Global variables & setup for DS18B20 temp. sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


//Global variables & setup for Turbidity Sensor
const int Turbidity_Sensor_Pin = 34;
float Turbidity_Sensor_Voltage;
int samples = 600;
float ntu; // Nephelometric Turbidity Units

String dateTime;
String getTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  dateTime = String(asctime(&timeinfo));
  return dateTime;
}

void readPhLevel() {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(35);
    delay(30);
  }

  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];

  float volt = (float)avgval * 3.3 / 4096.0 / 6;
  ph_act = -5.70 * volt + calibration_value;

  if (isnan(ph_act)) {
    Serial.println("Failed to read from Ph Sensor!");
  }
  else {
    Serial.print("pH Val: ");
   String datetime = getTime();
   // Create a FirebaseJson object
   FirebaseJson json;
   json.add("DateTime", datetime.c_str());
   json.add("Sensor", "PH");
   json.add("Reading", ph_act);

   if (Firebase.RTDB.set(&firebaseData, "/readings/data/ph", &json)) {
     Serial.println("PUSHED DATA SUCCESSFULLY");
   } else {
     Serial.println("ERROR PUSHING DATA");
     Serial.println(firebaseData.errorReason());
   }
    Serial.println(ph_act);
    
    // return String(ph_act);
  }

}//END


void readTemperatureValue() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);

    String datetime = getTime();

    // Create a FirebaseJson object
    FirebaseJson json;
    json.add("DateTime", datetime.c_str());
    json.add("Sensor", "Temperature");
    json.add("Reading", tempC);

    if (Firebase.RTDB.set(&firebaseData, "/readings/data/temperature", &json)) {
      Serial.println("PUSHED DATA SUCCESSFULLY");
    } else {
      Serial.println("ERROR PUSHING DATA");
      Serial.println(firebaseData.errorReason());
    }

    // return String(tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
    // return "";
  }
}

//end


void readTurbidityValue(){
  Turbidity_Sensor_Voltage = 0;
  for (int i = 0; i < samples; i++)
  {
    Turbidity_Sensor_Voltage += ((float)analogRead(Turbidity_Sensor_Pin) / 4095.0) * 3.3;
  }

  Turbidity_Sensor_Voltage = Turbidity_Sensor_Voltage / samples;

  Turbidity_Sensor_Voltage = round_to_dp(Turbidity_Sensor_Voltage, 2);
  ntu = -1120.4 * pow(Turbidity_Sensor_Voltage, 2) + 5742.3 * Turbidity_Sensor_Voltage - 4352.9;
  Serial.print("NTU: "); Serial.println(ntu);

   String datetime = getTime();
   // Create a FirebaseJson object
   FirebaseJson json;
   json.add("DateTime", datetime);
   json.add("Sensor", "Turbidity");
   json.add("Reading", ntu);

   if (Firebase.RTDB.set(&firebaseData, "/readings/data/turbidity", &json)) {
     Serial.println("PUSHED DATA SUCCESSFULLY");
   } else {
     Serial.println("ERROR PUSHING DATA");
     Serial.println(firebaseData.errorReason());
   }
  // Serial.print(datetime);
  // return String(ntu);
}//end


float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  
  return in_value;
}

bool timeIsSet() {
  time_t now;
  time(&now);
  return now > 8 * 3600 * 2; // Check if the time is later than 2AM on Jan 2, 1970
}

void setup() 
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  sensors.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // config time
  while (!timeIsSet()) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(1000); // Wait for a second before trying again
  }
  
  // Firebase setup
  config.database_url = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH; 

  auth.user.email = "kokoybaldofordawin@gmail.com";
  auth.user.password = "taalproj2";
  // auth.token.uid = "firebase-adminsdk-zsxy9@taal-d9a33.iam.gserviceaccount.com";

  /* Assign the callback function for the long running token generation task */ 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Add this line to refresh the token every 60 secs
  config.signer.preRefreshSeconds = 60;

  Firebase.begin(&config, &auth);
  if (Firebase.ready()) {
    Serial.println("Firebase connected.");
  } else {
    Serial.println("Firebase connection failed!");
  }
  Firebase.reconnectWiFi(true);
}


void loop() {
    readPhLevel();
    readTemperatureValue();
    readTurbidityValue();
    yield(); // yield control to background tasks
    delay(2000); // wait for a second
    yield();
}