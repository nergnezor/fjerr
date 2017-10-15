#define xGDEW029Z10
#define GDEW042Z15

#include <GxEPD.h>
#include <GxGDEW0xxZ/GxGDEW0xxZ.cpp>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

GxIO_Class io(SPI, SS, D3, D4); // arbitrary selection of D3, D4 selected for default of GxEPD_Class
GxEPD_Class display(io);        // default selection of D4, D2

#include "omar_light.xbm"
#include "omar_dark.xbm"
#include "erik.xbm"
#include "erik2.xbm"

void setup()
{
  Serial.begin(115200);
  display.init();
  Serial.println("Setup done");
}

void loop()
{
  // int interval = 2;
  // int radius = 20;
  // for (int x = 0; x <= GxEPD_WIDTH; x += GxEPD_WIDTH / interval)
  // {
  //   for (int y = 0; y <= GxEPD_HEIGHT; y += GxEPD_HEIGHT / interval)
  //   {
  //     display.fillCircle(x, y, radius, GxEPD_BLACK);
  //     display.update();
  //   }
  // }
  int d = 2000;
  display.drawXBitmap(0, 0, erik_bits, erik_width, erik_height, GxEPD_RED);
  display.drawXBitmap(0, 0, erik2_bits, erik_width, erik_height, GxEPD_BLACK);
  display.update();
  delay(d);

  display.setRotation(2);
  display.drawXBitmap(0, 0, omar_dark_bits, omar_dark_width, omar_dark_height, GxEPD_RED);
  display.drawXBitmap(0, 0, omar_light_bits, omar_light_width, omar_light_height, GxEPD_BLACK);
  display.update();
  delay(d);

  display.setRotation(3);
  display.setTextColor(GxEPD_RED);
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 50);
  display.println("yo!");
  display.update();
  delay(d);

  display.setRotation(0);
  display.fillRoundRect(GxEPD_WIDTH / 3, GxEPD_WIDTH / 3, GxEPD_WIDTH / 3, GxEPD_WIDTH / 3, 10, GxEPD_BLACK);
  display.fillRoundRect(GxEPD_WIDTH / 3, GxEPD_WIDTH / 3, GxEPD_WIDTH / 3, GxEPD_WIDTH / 3, GxEPD_WIDTH / 6, GxEPD_RED);
  display.update();
  delay(d);
}
