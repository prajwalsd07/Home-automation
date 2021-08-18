#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "Sagar" // Enter Wifi_Network_Name
#define WIFI_PASS "@sagar5401@" // Enter Wifi_Network_Password

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "prajwalsd" // io-Adafruit username
#define MQTT_PASS "aio_SyTn57ytFNeQYjk5YJ7MPPO5ypt7" // Adafruit AIO Key

int led = 13; // LED Positive pin, POSITIVE - D7, NEGETIVE - GND

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/OnOff");
Adafruit_MQTT_Publish LightsStatus = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/f/LightStatus");


void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);


  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  digitalWrite(LED_BUILTIN, LOW);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }

  Serial.println("OK!");

  //Subscribe to the onoff topic
  mqtt.subscribe(&onoff);

  pinMode(led, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(led, LOW);

}

void loop()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &onoff)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) onoff.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) onoff.lastread, "ON"))
      {
        //active low logic
        digitalWrite(led, HIGH);
        LightsStatus.publish("ON");
      }
      else if (!strcmp((char*) onoff.lastread, "OFF"))
      {
        digitalWrite(led, LOW);
        LightsStatus.publish("OFF");

      }
      else
      {
        LightsStatus.publish("ERROR");
      }
    }
    else
    {
      //LightsStatus.publish("ERROR");
    }
  }
}


void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while (((ret = mqtt.connect()) != Serial.println(mqtt.connectErrorString(ret))))
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
    Serial.println("MQTT Connected!");
}
