/*
   Copyright (c) 2022, teatree
   All rights reserved.
*/

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClient.h>

#ifndef AP_SSID
#define AP_SSID "ap-your-ssid"
#define AP_PSK "ap-your-psk"
#endif

/* Set these to your desired credentials. */
const char *ssid = AP_SSID;
const char *password = AP_PSK;

ESP8266WebServer server(80);

const int ONBOARD_LED = D4;
const int INT1 = D5;
const int INT2 = D6;
const int INT3 = D7;
const int INT4 = D8;
const int EchoPin = D2;
const int TrigPin = D3;

enum class Command {
  None,
  Front,
  Left,
  Right,
  Back,
  Stop,
  Auto,
  GetDistance,
};

enum class Motor {
  Stop,
  Front,
  Back,
};

static Command command = Command::None;
static bool ledOut = true;
static Motor motor = Motor::Stop;
static double distance = 0.0;
static bool isDriveAuto = false;
static bool driveAvoid = false;
static int driveAvoidPhase = 0;
const static double driveAvoidDistance = 25.0;

/* Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void home() {
  Serial.println("call home()");
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    Serial.println("index.html is not in data folder.");
    server.send(200, "text/html",
                "<h1>You are connected!</h1><h2>But, index.html is not in data "
                "folder.</h2>");
  }
}

/* モーター制御 */
void motorFront() {
  digitalWrite(INT1, HIGH);  // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, LOW);   //
  digitalWrite(INT3, LOW);   //
  digitalWrite(INT4, HIGH);  //
  motor = Motor::Front;
}

void motorLeft() {
  digitalWrite(INT1, HIGH);  // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, LOW);   //
  digitalWrite(INT3, HIGH);  //
  digitalWrite(INT4, LOW);   //
}

void motorRight() {
  digitalWrite(INT1, LOW);   // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, HIGH);  //
  digitalWrite(INT3, LOW);   //
  digitalWrite(INT4, HIGH);  //
}

void motorBack() {
  digitalWrite(INT1, LOW);   // HIGH LOWの組み合わせでモーター回転
  digitalWrite(INT2, HIGH);  //
  digitalWrite(INT3, HIGH);  //
  digitalWrite(INT4, LOW);   //
  motor = Motor::Back;
}

void motorStop() {
  digitalWrite(INT1, LOW);  // モーター停止
  digitalWrite(INT2, LOW);
  digitalWrite(INT3, LOW);
  digitalWrite(INT4, LOW);
  motor = Motor::Stop;
}

/* コマンド */
void CommandLED() {
  Serial.println("call CommandLED()");
  server.send(200);
  ledOut = !ledOut;
  digitalWrite(ONBOARD_LED, ledOut);
}

void CommandFront() {
  Serial.println("call CommandFront()");
  server.send(200);
  command = Command::Front;
}

void CommandLeft() {
  Serial.println("call CommandLeft()");
  server.send(200);
  command = Command::Left;
}

void CommandRight() {
  Serial.println("call CommandRight()");
  server.send(200);
  command = Command::Right;
}

void CommandBack() {
  Serial.println("call CommandBack()");
  server.send(200);
  command = Command::Back;
}

void CommandStop() {
  Serial.println("call CommandStop()");
  server.send(200);
  command = Command::Stop;
}

void CommandAuto() {
  Serial.println("call CommandAuto()");
  server.send(200);
  command = Command::Auto;
}

void CommandGetDistance() {
  Serial.println("call CommandGetDistance()");
  server.send(200, "text/plain", String(distance, 2));
}

/* 距離計算 */
void calcDistance() {
  static long last_time = 0;
  long now_time = millis();
  if ((now_time - last_time) < 100) {
    return;
  }
  last_time = now_time;

  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  double duration = pulseIn(EchoPin, HIGH);  // μS
  if (duration > 0) {
    duration = duration / 2;  //往復距離なので半分の時間
    distance = duration * 340 * 100 / 1000000;  // 距離（cm）を計算
    Serial.print(distance);
    Serial.println("cm");
  } else {
    Serial.println("no echo");
  }
}

/* 操作 */
void driveLR(Command command) {
  if (command == Command::Left) {
    motorLeft();
  } else {
    motorRight();
  }
  delay(300);

  if (motor == Motor::Front) {
    motorFront();
  } else if (motor == Motor::Back) {
    motorBack();
  }
}

void driveAutoStart() {
  Serial.println("call Drive::driveAutoStart()");
  motorFront();
  isDriveAuto = true;
};

void driveAutoStop() {
  Serial.println("call Drive::driveAutoStop()");
  isDriveAuto = false;
  driveAvoid = false;
};

void driveAuto() {
  if (isDriveAuto) {
    if (driveAvoid) {
      static long last_time = 0;
      long now_time = millis();

      if (driveAvoidPhase == 0) {
        Serial.println("call Drive::isDriveAuto() 0");
        last_time = now_time;
        motorStop();
        motorRight();
        driveAvoidPhase++;
      } else if (driveAvoidPhase == 1) {
        if ((now_time - last_time) > 300) {
          Serial.println("call Drive::isDriveAuto() 1");
          motorStop();
          driveAvoidPhase++;
        }
      } else if (driveAvoidPhase == 2) {
        if ((now_time - last_time) > 800) {
          Serial.println("call Drive::isDriveAuto() 2");
          driveAvoid = false;
        }
      }
    } else {
      if (distance > driveAvoidDistance) {
        if (motor != Motor::Front) {
          motorFront();
        }
      } else {
        driveAvoid = true;
        driveAvoidPhase = 0;
      }
    }
  }
}

/* コマンド実行 */
void executeCommand() {
  switch (command) {
    case Command::Front:
      motorFront();
      break;
    case Command::Left:
    case Command::Right:
      driveLR(command);
      break;
    case Command::Back:
      motorBack();
      break;
    case Command::Stop:
      motorStop();
      driveAutoStop();
      break;
    case Command::Auto:
      driveAutoStart();
      break;
    case Command::None:
    default:
      driveAuto();
      break;
  }
  command = Command::None;
}

// put your setup code here, to run once:
void setup() {
  delay(500);

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
  server.on("/command/led", CommandLED);
  server.on("/command/front", CommandFront);
  server.on("/command/left", CommandLeft);
  server.on("/command/right", CommandRight);
  server.on("/command/back", CommandBack);
  server.on("/command/stop", CommandStop);
  server.on("/command/auto", CommandAuto);
  server.on("/command/distance", CommandGetDistance);
  server.begin();

  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, true);

  pinMode(INT1, OUTPUT);
  pinMode(INT2, OUTPUT);
  pinMode(INT3, OUTPUT);
  pinMode(INT4, OUTPUT);
  motorStop();

  LittleFS.begin();

  pinMode(EchoPin, INPUT);
  pinMode(TrigPin, OUTPUT);

  Serial.println("HTTP server started");
}

// put your main code here, to run repeatedly:
void loop() {
  calcDistance();
  executeCommand();
  server.handleClient();
}
