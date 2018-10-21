#include "GxGDEW0xxZ.h"

GxGDEW0xxZ::GxGDEW0xxZ(GxIO &io, uint8_t rst, uint8_t busy) : GxEPD(GxGDEW0xxZ_WIDTH, GxGDEW0xxZ_HEIGHT), IO(io), _rst(rst), _busy(busy)
{
}

template <typename T>
static inline void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}
static struct
{
    uint16_t xMin = 0;
    uint16_t xMax = GxGDEW0xxZ_WIDTH;
    uint16_t yMin = 0;
    uint16_t yMax = GxGDEW0xxZ_HEIGHT;
} ActiveArea;

void GxGDEW0xxZ::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    switch (getRotation())
    {
    case 1:
        swap(x, y);
        x = GxGDEW0xxZ_WIDTH - x - 1;
        break;
    case 2:
        x = GxGDEW0xxZ_WIDTH - x - 1;
        y = GxGDEW0xxZ_HEIGHT - y - 1;
        break;
    case 3:
        swap(x, y);
        y = GxGDEW0xxZ_HEIGHT - y - 1;
        break;
    }
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
        return;
    ActiveArea.xMin = min(ActiveArea.xMin, x);
    ActiveArea.xMax = max(ActiveArea.xMax, x);
    ActiveArea.yMin = min(ActiveArea.yMin, y);
    ActiveArea.yMax = max(ActiveArea.yMax, y);
    uint16_t i = x / 8 + y * GxGDEW0xxZ_WIDTH / 8;
    if (i >= GxGDEW0xxZ_BUFFER_SIZE)
        return;                                                          // for reduced buffer size
    _black_buffer[i] = (_black_buffer[i] & (0xFF ^ (1 << (7 - x % 8)))); // white
    _red_buffer[i] = (_red_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));     // white
    if (color == GxEPD_WHITE)
        return;
    else if (color == GxEPD_BLACK)
        _black_buffer[i] = (_black_buffer[i] | (1 << (7 - x % 8)));
    else if (color == GxEPD_RED)
        _red_buffer[i] = (_red_buffer[i] | (1 << (7 - x % 8)));
    else
    {
        if ((color & 0xF100) > (0xF100 / 2))
            _red_buffer[i] = (_red_buffer[i] | (1 << (7 - x % 8)));
        else if ((((color & 0xF100) >> 11) + ((color & 0x07E0) >> 5) & (color & 0x001F)) < 3 * 255 / 2)
        {
            _black_buffer[i] = (_black_buffer[i] | (1 << (7 - x % 8)));
        }
    }
}

void GxGDEW0xxZ::init(void)
{
    IO.init();
    IO.setFrequency(20000000); // 4MHz : 250ns > 150ns min RD cycle
    digitalWrite(_rst, 1);
    pinMode(_rst, OUTPUT);
    pinMode(_busy, INPUT);
    update();
    // fillScreen(GxEPD_WHITE);
}

void GxGDEW0xxZ::fillScreen(uint16_t color)
{
    uint8_t black = 0x00;
    uint8_t red = 0x00;
    if (color == GxEPD_WHITE)
        ;
    else if (color == GxEPD_BLACK)
        black = 0xFF;
    else if (color == GxEPD_RED)
        red = 0xFF;
    else if ((color & 0xF100) > (0xF100 / 2))
        red = 0xFF;
    else if ((((color & 0xF100) >> 11) + ((color & 0x07E0) >> 5) & (color & 0x001F)) < 3 * 255 / 2)
        black = 0xFF;
    for (uint16_t x = 0; x < GxGDEW0xxZ_BUFFER_SIZE; x++)
    {
        _black_buffer[x] = black;
        _red_buffer[x] = red;
    }
}

void GxGDEW0xxZ::update(void)
{
    // drawPicture(_black_buffer, _red_buffer, GxGDEW0xxZ_BUFFER_SIZE);

    _wakeUp();
    _writeCommand(0x10);
    for (int y = ActiveArea.yMin; y <= ActiveArea.yMax; ++y)
    {
        int xStart = ActiveArea.xMin / 8 + y * GxGDEW0xxZ_WIDTH / 8;
        int xEnd = ActiveArea.xMax / 8 + y * GxGDEW0xxZ_WIDTH / 8;
        for (uint32_t i = xStart; i <= xEnd; i++)
        {
            _writeData(_black_buffer[i]);
        }
    }
    _writeCommand(0x13);
    for (int y = ActiveArea.yMin; y <= ActiveArea.yMax; ++y)
    {
        int xStart = ActiveArea.xMin / 8 + y * GxGDEW0xxZ_WIDTH / 8;
        int xEnd = ActiveArea.xMax / 8 + y * GxGDEW0xxZ_WIDTH / 8;
        for (uint32_t i = xStart; i <= xEnd; i++)
        {
            _writeData(_red_buffer[i]);
        }
    }
    _writeCommand(0x12); //display refresh
    _waitWhileBusy("update display refresh");

    Serial.print("Update: ");
    Serial.println(ActiveArea.xMin);
    Serial.println(ActiveArea.xMax);
    Serial.println(ActiveArea.yMin);
    Serial.println(ActiveArea.yMax);
    ActiveArea.xMin = UINT16_MAX;
    ActiveArea.xMax = 0;
    ActiveArea.yMin = UINT16_MAX;
    ActiveArea.yMax = 0;

    _sleep();
}

void GxGDEW0xxZ::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
    drawBitmap(bitmap, x, y, w, h, color);
}

void GxGDEW0xxZ::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, bool mirror)
{
    if (mirror)
    {
        for (uint16_t x1 = x; x1 < x + w; x1++)
        {
            for (uint16_t y1 = y; y1 < y + h; y1++)
            {
                uint32_t i = (w - (x1 - x) - 1) / 8 + uint32_t(y1 - y) * uint32_t(w) / 8;
                uint16_t pixelcolor = (bitmap[i] & (0x01 << (x1 - x) % 8)) ? color : GxEPD_WHITE;
                drawPixel(x1, y1, pixelcolor);
            }
        }
    }
    else
    {
        for (uint16_t x1 = x; x1 < x + w; x1++)
        {
            for (uint16_t y1 = y; y1 < y + h; y1++)
            {
                uint32_t i = (x1 - x) / 8 + uint32_t(y1 - y) * uint32_t(w) / 8;
                uint16_t pixelcolor = (bitmap[i] & (0x80 >> (x1 - x) % 8)) ? color : GxEPD_WHITE;
                drawPixel(x1, y1, pixelcolor);
            }
        }
    }
}

void GxGDEW0xxZ::drawPicture(const uint8_t *black_bitmap, const uint8_t *red_bitmap, uint32_t size)
{
    _wakeUp();
    _writeCommand(0x10);
    for (uint32_t i = 0; i < GxGDEW0xxZ_BUFFER_SIZE; i++)
    {
        _writeData((i < size) ? black_bitmap[i] : 0x00);
    }
    _writeCommand(0x13);
    for (uint32_t i = 0; i < GxGDEW0xxZ_BUFFER_SIZE; i++)
    {
        _writeData((i < size) ? red_bitmap[i] : 0x00);
    }
    _writeCommand(0x12); //display refresh
    _waitWhileBusy("update display refresh");
    _sleep();
}

void GxGDEW0xxZ::drawBitmap(const uint8_t *bitmap, uint32_t size)
{
    _wakeUp();
    _writeCommand(0x10);
    for (uint32_t i = 0; i < GxGDEW0xxZ_BUFFER_SIZE; i++)
    {
        _writeData((i < size) ? bitmap[i] : 0x00);
    }
    _writeCommand(0x13);
    for (uint32_t i = 0; i < GxGDEW0xxZ_BUFFER_SIZE; i++)
    {
        _writeData(0);
    }
    _writeCommand(0x12); //display refresh
    _waitWhileBusy("update display refresh");
    _sleep();
}

void GxGDEW0xxZ::_writeCommand(uint8_t command)
{
    //while (!digitalRead(_busy));
    if (!digitalRead(_busy))
    {
        String str = String("command 0x") + String(command, HEX);
        _waitWhileBusy(str.c_str());
    }
    IO.writeCommandTransaction(command);
}

void GxGDEW0xxZ::_writeData(uint8_t data)
{
    IO.writeDataTransaction(data);
}

void GxGDEW0xxZ::_waitWhileBusy(const char *comment)
{
    unsigned long start = micros();
    while (1)
    {
        if (digitalRead(_busy) == 1)
            break;
        delay(1);
    }
    if (comment)
    {
        unsigned long elapsed = micros() - start;
        Serial.print(comment);
        Serial.print(" : ");
        Serial.println(elapsed);
    }
}
static bool partial = false;
void GxGDEW0xxZ::_wakeUp()
{
    /* Reset required for wakeup */
    digitalWrite(_rst, 0);
    delay(10);
    digitalWrite(_rst, 1);
    delay(10);

    /* Booster soft start */
    // _writeCommand(0x06);
    // _writeData(0x17);
    // _writeData(0x17);
    // _writeData(0x17);

    /* Power on */
    _writeCommand(0x04);
    _waitWhileBusy("_wakeUp Power On");

    /* Resolution setting */
    _writeCommand(0x61);
    if (_width > 255)
        _writeData(_width >> 8);
    _writeData(_width & 0xff);
    if (_height > 255)
        _writeData(_height >> 8);
    _writeData(_height & 0xff);

    /* Vcom and data interval setting */
    _writeCommand(0x50);
    _writeData(0x87);

    /* Partial Window */
    _writeCommand(0x90);
    if (_width > 255)
        _writeData(ActiveArea.xMin >> 8);
    _writeData(ActiveArea.xMin & 0xff);
    if (_width > 255)
        _writeData(ActiveArea.xMax >> 8);
    _writeData(ActiveArea.xMax & 0xff);
    if (_height > 255)
        _writeData(ActiveArea.yMin >> 8);
    _writeData(ActiveArea.yMin & 0xff);
    if (_height > 255)
        _writeData(ActiveArea.yMax >> 8);
    _writeData(ActiveArea.yMax & 0xff);
    /* PT_SCAN: 0: Gates scan only inside of the partial window. 
       1: Gates scan both inside and outside of the partial window. (default) */
    _writeData(0x00    );

    /* Partial In */
    _writeCommand(0x91);
}

void GxGDEW0xxZ::_sleep(void)
{
    _writeCommand(0X07); //power off
    _writeData(0xa5);
}
