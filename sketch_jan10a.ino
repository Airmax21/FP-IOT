#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "Astra Barokah Lantai 1";
const char* password = "barokah140c";
const char* mqttServer = "broker.hivemq.com";
const char ultrasonik1[2] = {D3,D4};
const char ultrasonik2[2] = {D5,D6};
const char sanyo = D7;
const char solenoid = D8;
const char* id = "fpiotamikom-";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Terhubung ke ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi Terhubung");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan Masuk [");
  Serial.print(topic);
  Serial.print("]");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(D2, LOW);
  } else {
    digitalWrite(D2, HIGH);
  }

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Koneksi ke broker MQTT...");
    String clientId = "WEMOSD1Mini-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung");
      client.publish(id+"LEDTEST2", "hello world");
      client.subscribe(id+"LEDTEST1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

int hitungJarak(char trig,char echo){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long durasi = pulseIn(echo, HIGH);
  int jarak = durasi * 0.034 / 2;
  Serial.print("Jarak: ");
  Serial.println(jarak);
  return jarak;
}

void setup() {
  pinMode(D2, OUTPUT);
  Serial.begin(115200);
  setupWifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 300) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "Hallo Sayangku Zulfa Ke-%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("LEDTEST2", msg);
  }
}