// #include <Arduino.h>

// #include <Pins_Arduino.h>
#include <TFT_eSPI.h> 
#define LED_BUILT 5
//edit in github
TFT_eSPI tft = TFT_eSPI();
// #include <SPI.h>
//   #include <FS.h>          //this needs to be first, or it all crashes and burns...
//   #include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

// #include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 48
int i = 0;

CRGB leds[NUM_LEDS];

// for LED RGB indicator
void setup()
{
  FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);

  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILT, OUTPUT);
  analogWrite(LED_BUILT, 70);
  Serial.println();

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  tft.initDMA();
  // tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 60);
  tft.println("Starting Radio...");

  Serial.println("\n\n================================");

  String bootlog;
  bootlog += F("\n\t");
  bootlog += F("CHIPmodel  = ");
  bootlog += ESP.getChipModel();
  bootlog += F("\n\t");
  bootlog += F("Revision ");
  bootlog += ESP.getChipRevision();
  bootlog += F("\n\t");
  bootlog += F("Cores = ");
  bootlog += ESP.getChipCores();
  bootlog += F("\n\t");
  bootlog += F("FlashChipSize = ");
  bootlog += ESP.getFlashChipSize();
  bootlog += F("\n\t");
  bootlog += F("HeapSize = ");
  bootlog += ESP.getHeapSize();
  bootlog += F("\n\t");
  bootlog += F("FreeHeap = ");
  bootlog += ESP.getFreeHeap();
  bootlog += F("\n\t");
  bootlog += F("SkethSize = ");
  bootlog += ESP.getSketchSize();
  bootlog += F("\n\t");
  bootlog += F("FreeSkethSpace = ");
  bootlog += ESP.getFreeSketchSpace();
  bootlog += F("\n\t");
  bootlog += F("PSRAM = ");
  bootlog += ESP.getPsramSize();
  bootlog += F("\n\t");
  bootlog += F("FreePsram = ");
  bootlog += ESP.getFreePsram();
  Serial.print('\t');
  Serial.println(bootlog);
  Serial.println("================================");
}

void loop()
{
  if (i != 4)
  {
    for (i = 0; i < 4; i++)
    {
      leds[0] = CRGB::Red;
      FastLED.show();
      Serial.println("RED LED is ON");
      delay(500);
      leds[0] = CRGB::Green;
      FastLED.show();
      Serial.println("GREEN LED is ON");
      delay(500);
      leds[0] = CRGB::Blue;
      FastLED.show();
      Serial.println("BLUE LED is ON");
      delay(500);
      leds[0] = CRGB::Black;
      FastLED.show();
      Serial.println("LED's are OFF");
      delay(1500);
    }
  }
}
