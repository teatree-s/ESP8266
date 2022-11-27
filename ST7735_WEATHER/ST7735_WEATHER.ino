
/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>
#include <WiFiClient.h>

// TFT
#define TFT_CS D1
#define TFT_RST D0
#define TFT_DC D2
#define SD_CS 4  // Chip select line for SD card

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// WiFi
#define STASSID "ap-your-ssid"
#define STAPSK "ap-your-psk"

const char *ssid = STASSID;
const char *password = STAPSK;

// 天気予報 API（livedoor 天気互換）
// https://weather.tsukumijima.net/
// https://weather.tsukumijima.net/primary_area.xml
// 270000 is Osaka
const char *host = "http://weather.tsukumijima.net/api/forecast/city/270000";

ESP8266WiFiMulti WiFiMulti;

void setup(void) {
  Serial.begin(115200);

  // TFT
  Serial.println("Hello! ST77xx TFT Test");

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ST77XX_WHITE);

  Serial.println("Initialized");

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(8, 16);
  tft.print("WiFi connecting...");

  // File system
  LittleFS.begin();

  // WiFi
  for (uint8_t t = 3; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println("done");
}

void loop() {
  if ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.println("WiFi connecting...");
    return;
  }

  tft.fillScreen(ST77XX_BLACK);
  dispWeather();

  delay(10000UL);
}

void dispWeather(void) {
  WiFiClient client;
  HTTPClient http;
  String payload = "";

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, host)) {  // HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        payload = http.getString();
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }

  if (payload.length()) {
    //    Serial.println(payload);
    DynamicJsonDocument doc(10240);
    deserializeJson(doc, payload);

    // Today
    int i = 0, x = 0;
    tft.setCursor(x + 16, 16);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("Today");
    tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(x + 20, 38);
    String date = doc["forecasts"][i]["date"];
    String day = date.substring(5);
    day.setCharAt(2, '/');
    tft.print(day);

    tft.setCursor(x + 6, 60);
    String temp = doc["forecasts"][i]["temperature"]["max"]["celsius"];
    temp = "max " + temp + " c";
    tft.print(temp);

    String url = doc["forecasts"][i]["image"]["url"];
    String img = url.substring(41, 44);
    img = "/" + img + ".bmp";
    File file = LittleFS.open(img, "r");
    if (file) {
      bmpDraw(file, x + 10, 64 + 10);
      file.close();
    }

    // Tomorrow
    i++;
    x += 80;
    tft.setCursor(x, 16);
    tft.print("Tomorrow");

    tft.setCursor(x + 20, 38);
    String t_date = doc["forecasts"][i]["date"];
    String t_day = t_date.substring(5);
    t_day.setCharAt(2, '/');
    tft.print(t_day);

    tft.setCursor(x + 6, 60);
    String t_temp = doc["forecasts"][i]["temperature"]["max"]["celsius"];
    t_temp = "max " + t_temp + " c";
    tft.print(t_temp);

    String t_url = doc["forecasts"][i]["image"]["url"];
    String t_img = t_url.substring(41, 44);
    t_img = "/" + t_img + ".bmp";
    file = LittleFS.open(t_img, "r");
    if (file) {
      bmpDraw(file, x + 10, 64 + 10);
      file.close();
    }
  }
}

#define BUFFPIXEL 20

void bmpDraw(File bmpFile, uint8_t x, uint8_t y) {
  int bmpWidth, bmpHeight;          // W+H in pixels
  uint8_t bmpDepth;                 // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;          // Start of image data in file
  uint32_t rowSize;                 // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];  // pixel buffer (R+G+B per pixel)
  uint8_t buffidx = sizeof(sdbuffer);  // Current position in sdbuffer
  boolean goodBmp = false;             // Set to true on valid header parse
  boolean flip = true;                 // BMP is stored bottom-to-top
  int w, h, row, col;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) {  // BMP signature
    Serial.print("File size: ");
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);             // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);  // Start of image data
    Serial.print("Image Offset: ");
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: ");
    Serial.println(read32(bmpFile));
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) {    // # planes -- must be '1'
      bmpDepth = read16(bmpFile);  // bits per pixel
      Serial.print("Bit Depth: ");
      Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {  // 0 = uncompressed

        goodBmp = true;  // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.startWrite();
        tft.setAddrWindow(x, y, w, h);

        for (row = 0; row < h; row++) {  // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip)  // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else  // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) {  // Need seek?
            tft.endWrite();
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);  // Force buffer reload
          }

          for (col = 0; col < w; col++) {  // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) {  // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;  // Set index to beginning
              tft.startWrite();
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          }  // end pixel
        }    // end scanline
        tft.endWrite();
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      }  // end goodBmp
    }
  }

  if (!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();  // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();  // MSB
  return result;
}
