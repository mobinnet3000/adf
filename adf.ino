#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_MQTT_FONA.h>



//for esp32 use <wifi.h>, for esp8266 use <esp8266wifi.h>
// this code is for esp32 for using esp8266 you just need to change <wifi.h> into <esp8266wifi.h> and also change the relay pin 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/************************* Pin Definition *********************************/

//Relays for switching appliances
#define Relay1            D0
#define Relay2            D1
#define Relay3            D2
#define Relay4            D3
#define buzzer            D4

//buzzer to know the status of MQTT connections and can be used for any other purpose according to your project need.




/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "mmm"
#define WLAN_PASS       "asdasdasd"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "mobinfff"
#define AIO_KEY         "aio_QFau23iwXOlOyEuRPA121FUzCoGg"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Publish Light = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led2");


// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Publish temperatureFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led5");
Adafruit_MQTT_Subscribe Fan1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led2");
Adafruit_MQTT_Subscribe Light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led0");
Adafruit_MQTT_Subscribe Fan2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay4");




/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);

  delay(10);

  pinMode(2, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);


  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&Fan1);
  mqtt.subscribe(&Light2);
  mqtt.subscribe(&Fan2);
}


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here




  // خواندن داده‌های سنسور (مثال فرضی)
  float temperature = analogRead(A0); // فرض کنید سنسور دما به A0 متصل است


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Light1) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(2, Light1_State);
        if (!temperatureFeed.publish(temperature)) {
    Serial.println(F("Failed to publish temperature"));
  } else {
    Serial.println(F("Temperature published!"));
  }

      
    }
    if (subscription == &Light2) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light2.lastread);
      int Light2_State = atoi((char *)Light2.lastread);
      digitalWrite(D0, Light2_State);
        if (!temperatureFeed.publish(temperature)) {
    Serial.println(F("Failed to publish temperature"));
  } else {
    Serial.println(F("Temperature published!"));
  }

    }
    if (subscription == &Fan1) {
      Serial.print(F("Got: "));
      Serial.println((char *)Fan1.lastread);
      int Fan1_State = atoi((char *)Fan1.lastread);
      digitalWrite(Relay3, Fan1_State);
    }
    if (subscription == &Fan2) {
      Serial.print(F("Got: "));
      Serial.println((char *)Fan2.lastread);
      int Fan2_State = atoi((char *)Fan2.lastread);
      digitalWrite(Relay4, Fan2_State);
      
    }
  }



  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
}
