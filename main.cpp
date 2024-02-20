#include <Arduino.h>
// #define LED_BUILT 5
// #include <FastLED.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "FS.h"
#include "SPIFFS.h"
// TFT_eSPI tft = TFT_eSPI();

#define FORMAT_SPIFFS_IF_FAILED true

WiFiMulti wifiMulti;
HTTPClient http;

/* this info will be read by the python script */
int currentVersion = 0; // increment currentVersion in each release

String baseUrljson = "https://github.com/runo4938/src1/blob/master/update.json";
String baseUrlfwName = "https://github.com/runo4938/src1/blob/master/firmware.bin";
String checkFile = "update.json";
/* end of script data */

int fwVersion = 2;
bool fwCheck = false;
String fwUrl = "", fwName = "";

/* your wifi credentials */
const char *WIFI_SSID = "RT-GPON-D5D9";
const char *WIFI_PASS = "tB5DVdR9";

static void rebootEspWithReason(String reason)
{
  Serial.println(reason);
  delay(1000);
  ESP.restart();
}

void performUpdate(Stream &updateSource, size_t updateSize)
{
  String result = "";
  if (Update.begin(updateSize))
  {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize)
    {
      Serial.println("Written : " + String(written) + " successfully");
    }
    else
    {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    result += "Written : " + String(written) + "/" + String(updateSize) + " [" + String((written / updateSize) * 100) + "%] \n";
    if (Update.end())
    {
      Serial.println("OTA done!");
      result += "OTA Done: ";
      if (Update.isFinished())
      {
        Serial.println("Update successfully completed. Rebooting...");
        result += "Success!\n";
      }
      else
      {
        Serial.println("Update not finished? Something went wrong!");
        result += "Failed!\n";
      }
    }
    else
    {
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      result += "Error #: " + String(Update.getError());
    }
  }
  else
  {
    Serial.println("Not enough space to begin OTA");
    result += "Not enough space for OTA";
  }
  // http send 'result'
}

void updateFromFS(fs::FS &fs)
{
  File updateBin = fs.open("/firmware.bin");
  if (updateBin)
  {
    if (updateBin.isDirectory())
    {
      Serial.println("Error, firmware.bin is not a file");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0)
    {
      Serial.println("Trying to start update");
      performUpdate(updateBin, updateSize);
    }
    else
    {
      Serial.println("Error, file is empty");
    }

    updateBin.close();

    // when finished remove the binary from spiffs to indicate end of the process
    Serial.println("Removing update file");
    fs.remove("/firmware.bin");

    rebootEspWithReason("Rebooting to complete OTA update");
  }
  else
  {
    Serial.println("Could not load update.bin from spiffs root");
  }
}

bool downloadFirmware()
{
  HTTPClient http;
  bool stat = false;
  Serial.println(fwUrl);
  File f = SPIFFS.open("/firmware.bin", "w");
  if (f)
  {
    http.begin(baseUrlfwName);
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        Serial.println("Downloading...");
        http.writeToStream(&f);
        stat = true;
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    f.close();
  }
  else
  {
    Serial.println("failed to open /firmware.bin");
  }
  http.end();

  return stat;
}

bool checkFirmware()
{
  HTTPClient http;
  http.begin(baseUrljson);
  int httpCode = http.GET();

  Serial.print("httpCode =");
  Serial.println(httpCode);

  bool stat = false;
  Stream &payload = http.getStream();
  // Serial.println(payload);
  DynamicJsonDocument json(2048);
  deserializeJson(json, payload);

  // Serial.print(String(json));

  if (httpCode == HTTP_CODE_OK)
  {
    fwVersion = 2;//json["versionCode"].as<int>();
    fwName = "firmware.bin";//baseUrlfwName;//json["fileName"].as<String>();
    fwUrl = baseUrlfwName;    // baseUrl + fwName;

    Serial.print("fwVersion =");
    Serial.println(fwVersion);
    Serial.print("currentVersion =");
    Serial.println(currentVersion);
    Serial.print("fieName = ");
    Serial.println(fwName);

    if (fwVersion > currentVersion)
    {
      Serial.println("Firmware update available");
      stat = true;
    }
    else
    {
      Serial.println("You have the latest version");
    }
    Serial.print("Version: ");
    Serial.print(fwName);
    Serial.print("\tCode: ");
    Serial.println(fwVersion);
  }
  http.end();

  return stat;
}

// second editting
//  #define NUM_LEDS 1
//  #define DATA_PIN 48
int i = 0;
int k = 9;
// CRGB leds[NUM_LEDS];

// for LED RGB indicator
void setup()
{
  // FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);

  // put your setup code here, to run once:
  Serial.begin(115200);
  // pinMode(LED_BUILT, OUTPUT);
  // analogWrite(LED_BUILT, 70);
  Serial.println();

  // tft.begin();
  // tft.setRotation(3);
  // tft.fillScreen(TFT_BLACK);

  // tft.initDMA();
  // // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.setTextSize(2);
  // tft.setCursor(40, 60);
  // tft.println("Starting Radio...");

  Serial.println("\n\n================================");
  Serial.println("Firmware Updates from Github");
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS Mount Failed");
    rebootEspWithReason("SPIFFS Mount Failed, rebooting...");
  }

  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

  /* No need to wait for wifi to connect */
  //  while ((wifiMulti.run() != WL_CONNECTED)) {
  //    Serial.print(".");
  //  }
}

void loop()
{

  /* check the firmware once connected */
  if ((wifiMulti.run() == WL_CONNECTED) && !fwCheck)
  {
    fwCheck = true;
    Serial.println("Wifi connected. Checking for updates");
    if (checkFirmware())
    {
      if (SPIFFS.exists("/firmware.bin"))
      {
        SPIFFS.remove("/firmware.bin");
        Serial.println("Removed existing update file");
      }
      if (downloadFirmware())
      {
        Serial.println("Download complete");
        updateFromFS(SPIFFS);
      }
      else
      {
        Serial.println("Download failed");
      }
    }
  }
}
