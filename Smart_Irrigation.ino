#include <Arduino.h>
#include <WiFi.h>             
#include <FirebaseESP32.h>
#include <DHT11.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define DHT_SENSOR_PIN  21 // ESP32 pin GPIO21 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11
#define WIFI_SSID "realme 6"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyAEkVA9Vf5n1uaOSkVUHaNwDedPjWL9j08"
#define DATABASE_URL "https://smartirrigation-86ac9-default-rtdb.firebaseio.com/" 
#define soil 33
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;  
int val;
int cal;
DHT11 dht_sensor(DHT_SENSOR_PIN);

void setup() {
  
   Serial.begin(115200);
   pinMode(soil,INPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

   if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
   Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  //dht11_sensor.begin(); // initialize the DHT sensor
}

void loop() {
    /*int humi  = dht_sensor.readHumidity();
  int tempC = dht_sensor.readTemperature();
   val = analogRead(soil);
   cal = (val/4095)  *100;
   */
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
       int humi  = dht_sensor.readHumidity();
  int tempC = dht_sensor.readTemperature();
   val = analogRead(soil);
   cal = (val/4095)  *100;
    if (Firebase.RTDB.setInt(&fbdo, "/hum", humi)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
   if (Firebase.RTDB.setInt(&fbdo, "/temp", tempC)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
     if (Firebase.RTDB.setInt(&fbdo, "/moist", cal)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

}
