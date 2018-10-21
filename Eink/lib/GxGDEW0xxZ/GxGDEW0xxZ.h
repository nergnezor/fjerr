/************************************************************************************
   class GxGDEW0xxZ : Display class example for GDEW027C44 e-Paper from Dalian Good Display Co., Ltd.: www.good-display.com

   based on Demo Example from Good Display, now available on http://www.good-display.com/download_list/downloadcategoryid=34&isMode=false.html

   Author : J-M Zingg

   modified by :

   Version : 2.0

   Support: minimal, provided as example only, as is, no claim to be fit for serious use

   connection to the e-Paper display is through DESTM32-S2 connection board, available from GoodDisplay

   DESTM32-S2 pinout (top, component side view):
       |-------------------------------------------------
       |  VCC  |o o| VCC 5V
       |  GND  |o o| GND
       |  3.3  |o o| 3.3V
       |  nc   |o o| nc
       |  nc   |o o| nc
       |  nc   |o o| nc
       |  MOSI |o o| CLK
       |  DC   |o o| D/C
       |  RST  |o o| BUSY
       |  nc   |o o| BS
       |-------------------------------------------------

   note: for correct red color jumper J3 must be set on 0.47 side (towards FCP connector)
   
*/
#ifndef _GxGDEW0xxZ_H_
#define _GxGDEW0xxZ_H_

#include "../GxEPD.h"

#ifdef GDEW029Z10
#define GxGDEW0xxZ_WIDTH 128
#define GxGDEW0xxZ_HEIGHT 296
#elif defined GDEW042Z15
#define GxGDEW0xxZ_WIDTH 400
#define GxGDEW0xxZ_HEIGHT 300
#else
#error No device defined
#endif

#define GxGDEW0xxZ_BUFFER_SIZE GxGDEW0xxZ_WIDTH * GxGDEW0xxZ_HEIGHT / 8

// mapping from DESTM32-S1 evaluation board to Wemos D1 mini

// D10 : MOSI -> D7
// D8  : CS   -> D8
// E14 : RST  -> D4
// E12 : nc   -> nc

// D9  : CLK  -> D5 SCK
// E15 : DC   -> D3
// E13 : BUSY -> D2
// E11 : BS   -> GND

class GxGDEW0xxZ : public GxEPD
{
  public:
#if defined(ESP8266)
    GxGDEW0xxZ(GxIO& io, uint8_t rst = D4, uint8_t busy = D2);
#else
    GxGDEW0xxZ(GxIO& io, uint8_t rst = 9, uint8_t busy = 7);
#endif
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void init(void);
    void fillScreen(uint16_t color); // 0x0 black, >0x0 white, to buffer
    void update(void);
    // to buffer, may be cropped, drawPixel() used, update needed, old signature kept
    void  drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    // to buffer, may be cropped, drawPixel() used, update needed, new signature, mirror default set for example bitmaps
    void  drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, bool mirror = false);
    // to full screen, filled with white if size is less, no update needed
    void drawPicture(const uint8_t* black_bitmap, const uint8_t* red_bitmap, uint32_t size);
    // to full screen, filled with white if size is less, no update needed
    void drawBitmap(const uint8_t* bitmap, uint32_t size);
  private:
    void _writeData(uint8_t data);
    void _writeCommand(uint8_t command);
    void _writeLUT();
    void _wakeUp();
    void _sleep();
    void _waitWhileBusy(const char* comment=0);
  private:
    uint8_t _black_buffer[GxGDEW0xxZ_BUFFER_SIZE];
    uint8_t _red_buffer[GxGDEW0xxZ_BUFFER_SIZE];
    GxIO& IO;
    uint8_t _rst;
    uint8_t _busy;
};

#define GxEPD_Class GxGDEW0xxZ

#define GxEPD_WIDTH GxGDEW0xxZ_WIDTH
#define GxEPD_HEIGHT GxGDEW0xxZ_HEIGHT
#define GxEPD_BitmapExamples <GxGDEW0xxZ/BitmapExamples.h>
#define GxEPD_BitmapExamplesQ "GxGDEW0xxZ/BitmapExamples.h"

#endif

