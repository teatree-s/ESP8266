/*
   Copyright (c) 2022, teatree
   All rights reserved.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#ifndef AP_SSID
#define AP_SSID "ap-your-ssid"
#define AP_PSK  "ap-your-psk"
#endif

/* Set these to your desired credentials. */
const char *ssid = AP_SSID;
const char *password = AP_PSK;

ESP8266WebServer server(80);

const int INT1 = D5;
const int INT2 = D6;
const int INT3 = D7;
const int INT4 = D8;

static bool ledOut = true;

/* Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void home() {
  Serial.println("call home()");
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  }
  else {
    Serial.println("index.html is not in data folder.");
    server.send(200, "text/html", "<h1>You are connected!</h1><h2>But, index.html is not in data folder.</h2>");
  }
}

void led() {
  Serial.println("call led()");
  server.send(200);
  ledOut = !ledOut;
  digitalWrite(D4, ledOut);
}

void front() {
  Serial.println("call front()");
  server.send(200);
  digitalWrite(INT1, HIGH); // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, LOW);  // 正回転
  digitalWrite(INT3, LOW);  // 
  digitalWrite(INT4, HIGH); // 正回転
}

void left() {
  Serial.println("call left()");
  server.send(200);
  digitalWrite(INT1, HIGH); // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, LOW);  // 正回転
  digitalWrite(INT3, HIGH); // 
  digitalWrite(INT4, LOW);  // 逆回転
}

void right() {
  Serial.println("call right()");
  server.send(200);
  digitalWrite(INT1, LOW);  // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, HIGH); // 逆回転
  digitalWrite(INT3, LOW);  // 
  digitalWrite(INT4, HIGH); // 正回転
}

void back() {
  Serial.println("call back()");
  server.send(200);
  digitalWrite(INT1, LOW);  // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, HIGH); // 逆回転
  digitalWrite(INT3, HIGH); // 
  digitalWrite(INT4, LOW);  // 逆回転
}

void stop() {
  Serial.println("call stop()");
  server.send(200);
  digitalWrite(INT1, LOW);  // モーター停止
  digitalWrite(INT2, LOW);
  digitalWrite(INT3, LOW);
  digitalWrite(INT4, LOW);
}

void setup() {
  delay(1000);
  
  Serial.begin(115200);

  Serial.println();
  Serial.print("Configuring access point...");

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("PASSWORD: ");
  Serial.println(password);

  server.on("/", home);
  server.on("/command", led);
  server.on("/command1", front);
  server.on("/command2", left);
  server.on("/command3", right);
  server.on("/command4", back);
  server.on("/command5", stop);
  server.begin();

  pinMode(D4, OUTPUT);
  digitalWrite(D4, true);

  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  stop();

  LittleFS.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
