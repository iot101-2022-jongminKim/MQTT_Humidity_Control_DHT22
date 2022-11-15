// DHT22
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

#define RELAY 15
int val = 0;

const char*   ssid="533-2.4G-4";
const char*   password = "kpu123456!";
const char*   mqttServer = "iotlab101.tosshub.co";
const int     mqttPort = 1883;

unsigned long interval = 3000;
unsigned long lastPublished = - interval;

DHTesp dht;
int DHT_interval = 2000;
unsigned lastDHTReadMillis = 0;
float humidity = 0;
float temperature = 0;



WiFiClient espClient;
PubSubClient client(espClient);

void pubDHtLight();
void callback(char* topic, byte* payload, unsigned int length);

void readDHT22()
{
  unsigned long currentMillis = millis();

  if (currentMillis - lastDHTReadMillis >= DHT_interval)
  {
    lastDHTReadMillis = currentMillis;
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    
  }
}

void setup() {
  Serial.begin(115200);
  dht.setup(14, DHTesp::DHT22);
  pinMode(RELAY, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while( WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while(!client.connected()){
    Serial.println("Connecting to MQTT...");

    if (client.connect("jongminKim_DHT22")){
      Serial.println("connected");
    } else {
      Serial.print("failed with state"); Serial.println(client.state());
      delay(2000);
    }
  }

  // client.subscribe("id/jongminKim/sensor/cmd/humidity");
  // client.subscribe("id/jongminKim/sensor/cmd/temperature");
  // client.subscribe("id/jongminKim/sensor/cmd/light");
  
  digitalWrite(RELAY, LOW);
}

void loop() {
  client.loop();
  // readDHT22();///
  unsigned long currentMillis = millis();
  if(currentMillis - lastPublished >= interval){
    lastPublished = currentMillis;
    pubDHtLight();
  }
}

void pubDHtLight() {
  char buf[10];
  char buf1[10];
  char buf2[10];

  readDHT22();
  sprintf(buf, "%2.1f", humidity);
  client.publish("id/jongminKim/sensor/evt/humidity", buf);
  // sprintf(buf1, "%2.1f", temperature);
  // client.publish("id/jongminKim/sensor/evt/temperature", buf1);

  // val = analogRead(0);
  // client.publish("id/jongminKim/sensor/evt/light", buf2);
  // sprintf(buf2, "%2d", val);
  Serial.printf("%d\t", buf); //Serial.printf("temperature: %d\t", buf1); Serial.printf("light: %d\n", buf2);
}



void callback(char* topic, byte* payload, unsigned int length){
  char msgBuffer[20];

  int i;
  for(i = 0; i < (int)length; i++){
    msgBuffer[i] = payload[i];
  }

  msgBuffer[i] = '\0';
  Serial.printf("\n%s -> %s\n", topic, msgBuffer);
  Serial.print("humidity:"); Serial.println(humidity);
  pubDHtLight();

}