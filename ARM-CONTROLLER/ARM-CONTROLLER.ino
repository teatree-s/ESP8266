/*
   Copyright (c) 2022, teatree
   All rights reserved.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Servo.h>

#ifndef AP_SSID
#define AP_SSID "ap-your-ssid"
#define AP_PSK  "ap-your-psk"
#endif

/* Set these to your desired credentials. */
const char *ssid = AP_SSID;
const char *password = AP_PSK;

ESP8266WebServer server(80);

const int ROBOT_BASE = D5;
const int ROBOT_ARM1 = D6;
const int ROBOT_ARM2 = D7;
const int ROBOT_HAND = D8;

Servo baseServo;  // create servo object to control a servo
Servo arm1Servo;  // create servo object to control a servo
Servo arm2Servo;  // create servo object to control a servo
Servo handServo;  // create servo object to control a servo

const int MAX_POS = 120;
const int MIN_POS = 0;
const int MIN_HAND_POS = 85;
const int INC_POS = 2;
const int DEFAULT_BASE_POS = 60;
const int DEFAULT_ARM1_POS = 40;
const int DEFAULT_ARM2_POS = 60;
const int DEFAULT_HAND_POS = 100;

static int baseServoPos = DEFAULT_BASE_POS;
static int arm1ServoPos = DEFAULT_ARM1_POS;
static int arm2ServoPos = DEFAULT_ARM2_POS;
static int handServoPos = DEFAULT_HAND_POS;
static int baseServoLastPos = 0;
static int arm1ServoLastPos = 0;
static int arm2ServoLastPos = 0;
static int handServoLastPos = 0;

static int baseServoState = 0;
static int arm1ServoState = 0;
static int arm2ServoState = 0;
static int handServoState = 0;
static bool resetServoPos = false;

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

void btn() {
  Serial.println("call btn()");
  server.send(200);
  
  String paramState;
  String paramKind;

  if (server.hasArg("state")) {
    paramState = server.arg("state");
  }
  if (server.hasArg("kind")) {
    paramKind = server.arg("kind");
  }

  if (paramState.equals("on")) {
    if (paramKind.equals("top")) {
      arm1ServoState = 1;
    } else if (paramKind.equals("bottom")) {
      arm1ServoState = 2;
    } else if (paramKind.equals("left")) {
      baseServoState = 1;
    } else if (paramKind.equals("right")) {
      baseServoState = 2;
    } else if (paramKind.equals("r-top")) {
      arm2ServoState = 1;
    } else if (paramKind.equals("r-bottom")) {
      arm2ServoState = 2;
    } else if (paramKind.equals("r-left")) {
      handServoState = 1;
    } else if (paramKind.equals("r-right")) {
      handServoState = 2;
    } else if (paramKind.equals("reset")) {
      resetServoPos = true;
    } 
  } else if (paramState.equals("off")) {
    if (paramKind.equals("top")) {
      arm1ServoState = 0;
    } else if (paramKind.equals("bottom")) {
      arm1ServoState = 0;
    } else if (paramKind.equals("left")) {
      baseServoState = 0;
    } else if (paramKind.equals("right")) {
      baseServoState = 0;
    } else if (paramKind.equals("r-top")) {
      arm2ServoState = 0;
    } else if (paramKind.equals("r-bottom")) {
      arm2ServoState = 0;
    } else if (paramKind.equals("r-left")) {
      handServoState = 0;
    } else if (paramKind.equals("r-right")) {
      handServoState = 0;
    }
  }
}

void changePosition() {
  // BASE
  if (baseServoState == 1) {
    baseServoPos = min(baseServoPos+INC_POS, MAX_POS);
  } else if (baseServoState == 2) {
    baseServoPos = max(baseServoPos-INC_POS, MIN_POS);
  } 
  // ARM1
  if (arm1ServoState == 1) {
    arm1ServoPos = min(arm1ServoPos+INC_POS, MAX_POS);
  } else if (arm1ServoState == 2) {
    arm1ServoPos = max(arm1ServoPos-INC_POS, MIN_POS);
  } 
  // ARM2
  if (arm2ServoState == 1) {
    arm2ServoPos = min(arm2ServoPos+INC_POS, MAX_POS);
  } else if (arm2ServoState == 2) {
    arm2ServoPos = max(arm2ServoPos-INC_POS, MIN_POS);
  } 
  // HAND
  if (handServoState == 1) {
    handServoPos = min(handServoPos+INC_POS, MAX_POS);
  } else if (handServoState == 2) {
    handServoPos = max(handServoPos-INC_POS, MIN_HAND_POS);
  }
  if (resetServoPos) {
    baseServoPos = DEFAULT_BASE_POS;
    arm1ServoPos = DEFAULT_ARM1_POS;
    arm2ServoPos = DEFAULT_ARM2_POS;
    handServoPos = DEFAULT_HAND_POS;
    resetServoPos = false;
  }
}

void move() {
  if (baseServoPos != baseServoLastPos) {
    baseServo.write(baseServoPos);
    Serial.print("baseServoPos:");
    Serial.println(baseServoPos);
    baseServoLastPos = baseServoPos;
  }
  if (arm1ServoPos != arm1ServoLastPos) {
    arm1Servo.write(arm1ServoPos);
    Serial.print("arm1ServoPos:");
    Serial.println(arm1ServoPos);
    arm1ServoLastPos = arm1ServoPos;
  }
  if (arm2ServoPos != arm2ServoLastPos) {
    arm2Servo.write(arm2ServoPos);
    Serial.print("arm2ServoPos:");
    Serial.println(arm2ServoPos);
    arm2ServoLastPos = arm2ServoPos;
  }
  if (handServoPos != handServoLastPos) {
    handServo.write(handServoPos);
    Serial.print("handServoPos:");
    Serial.println(handServoPos);
    handServoLastPos = handServoPos;
  }
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
  server.on("/btn", btn);
  server.begin();

  pinMode(D4, OUTPUT);
  digitalWrite(D4, true);

  baseServo.attach(ROBOT_BASE);
  arm1Servo.attach(ROBOT_ARM1);
  arm2Servo.attach(ROBOT_ARM2);
  handServo.attach(ROBOT_HAND);

  LittleFS.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  changePosition();
  move();
  delay(50);
}
