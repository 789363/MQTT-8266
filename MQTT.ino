#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>
const int trigPin = 2;     //D4
const int echoPin = 0;     //D3
const int trig_2Pin = 12;  //D6
const int echo_2Pin = 14;  //D5
long duration;
long duration2;
int distance;
int distance2;
int fsrData;
int lastfsrData;
const int fsrPin = A0;
WebSocketsClient webSocket;
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  //初始化針腳
  pinMode(trigPin, OUTPUT);    // 将trigPin设置为输出
  pinMode(trig_2Pin, OUTPUT);  // 将trigPin2设置为输出
  pinMode(echoPin, INPUT);     // 将echoPin设置为输入
  pinMode(echo_2Pin, INPUT);   // 将echoPin2设置为输入
  // 初始化 WiFi 和 webSocket。
  Serial.begin(115200);
  WiFi.begin("Yuwei_1", "a78936333");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  // Connect to MQTT server
  client.setServer("o.tcp.jp.ngrok.io", 18232);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT server...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT server");
    } else {
      delay(1000);
    }
  }
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  //将trigPin设置为HIGH状态10微秒
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  digitalWrite(trig_2Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_2Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_2Pin, LOW);
  duration2 = pulseIn(echo_2Pin, HIGH);
  distance2 = duration2 * 0.034 / 2;
  //查看超音波結果
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Distance2: ");
  Serial.println(distance2);
  //連線MQTT
  if (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266 Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker. Reason: ");
      Serial.println(client.state());
      return;
    }
  }
  fsrData = analogRead(fsrPin); // 將讀取到的數值轉為 0~1023 之間的數字
  Serial.print("FSR value is:");
  Serial.println(fsrData);

  //判斷垃圾桶是否滿了 並推送給MQTT server
  if (distance <= 5) {
    
    client.publish("esp8266/can1/status", "1");
    Serial.println("Message published1滿");
  }
    if (fsrData > lastfsrData+50) {
    Serial.println("Message published1重量");
        client.publish("esp8266/can1/value", "500");
  }
    if (distance <= 5 &  lastfsrData > fsrData+80 ) {
    
    client.publish("esp8266/can1/music", "1");
    Serial.println("Message published1丟");
        client.publish("esp8266/can1/value", "1250");
  }
  if (distance > 5) {
    client.publish("esp8266/can1/status", "0");

    Serial.println("Message published");
  }
  if (distance2 <= 5) {
    client.publish("esp8266/can2/status", "1");
    Serial.println("Message published2滿");

  }
  if (distance2 > 5) {
    client.publish("esp8266/can2/status", "0");
    Serial.println("Message published");
  }
  lastfsrData = fsrData;
}
