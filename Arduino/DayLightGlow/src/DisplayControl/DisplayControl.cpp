#include "DisplayControl.h"

DisplayControl::DisplayControl() :
        display(U8G2_R0, U8X8_PIN_NONE)
{

}

DisplayControl::~DisplayControl()
{

}

void DisplayControl::init()
{
    display.begin();
}

void DisplayControl::forceUpdate()
{
    do_update = true;
    do
    {
        update();
    } while (do_update);
}

#ifdef FULL_BUFFER
void DisplayControl::update(const uint32_t &cycleTime)
{
    uint32_t current_cycle = millis();
    if ((cycleTime > 0) && (current_cycle - last_cycle) >= cycleTime)
        do_update = true;

    if (do_update)
    {
        display.clearBuffer();

        // Draw functions
        display.setDrawColor(1);
        drawVolume();
        drawTime();
        drawAlarms();
        drawFrequency();

        display.sendBuffer();

        do_update = false;
        last_cycle = current_cycle;
    }
}
#else
void DisplayControl::update(const uint32_t &cycleTime)
{
    uint32_t current_cycle = millis();
    if ((cycleTime > 0) && (current_cycle - last_cycle) >= cycleTime)
        do_update = true;

    if (do_update)
    {
        static uint8_t is_next_page = 0;


        // Call first page
        if (is_next_page == 0) {
            display.firstPage();
            is_next_page = 1;
        }

        // Draw functions
        display.setDrawColor(1);
        drawVolume();
        drawTime();
        drawAlarms();
        drawFrequency();

        // Call next page
        if (display.nextPage() == 0) {
            is_next_page = 0;     // ensure, that first page is called
            do_update = false;
            last_cycle = current_cycle;
        }
    }
}
#endif

void DisplayControl::drawVolume()
{
    uint8_t w = 5;
    uint8_t h = display.getHeight();
    uint8_t x = display.getWidth() - 7;
    uint8_t y = 0;

    display.drawFrame(x, y, w, h);
    display.drawVLine(x + w/2, h - current_volume, current_volume);

    if (current_mute)
    {
        display.setFont(u8g2_font_open_iconic_play_1x_t);
        display.drawGlyph(105, 10, 81);
        display.drawLine(105, 1, 114, 10);
    }
}

void DisplayControl::drawFrequency()
{

    uint8_t x = 5;
    uint8_t y = display.getHeight() - 15;
    uint8_t w = 105;
    uint8_t pos = (current_frequency - 8700) / 20;

    display.drawHLine(x, y, w);

    for (uint8_t i = 0; i <= w; i += 7)
    {
        if (i % 35)
            display.drawVLine(x + i, y, 2);
        else
            display.drawVLine(x + i, y, 5);
    }

    display.drawTriangle(x+pos, y+5, x+pos-5, y+15, x+pos+5, y+15);
}


void DisplayControl::drawTime()
{
    char buffer[6];
    sprintf (buffer, "%02d:%02d", hour(current_time), minute(current_time));
    display.setFont(u8g2_font_10x20_tn);
    display.drawStr((display.getWidth() - display.getStrWidth(buffer)) / 2, 15, buffer);
}

void DisplayControl::drawAlarms()
{
    char buffer[6];

    if (current_alarm0enabled)
    {
        sprintf(buffer, "%02d:%02d", hour(current_alarm0), minute(current_alarm0));
        display.setFont(u8g2_font_9x15B_tn);
        display.drawStr(8, display.getHeight() - 30, buffer);
    }

    if (current_alarm1enabled)
    {
        sprintf(buffer, "%02d:%02d", hour(current_alarm1), minute(current_alarm1));
        display.setFont(u8g2_font_9x15B_tn);
        display.drawStr(display.getWidth() - display.getStrWidth(buffer) - 20, display.getHeight() - 30, buffer);
    }
}

void DisplayControl::drawControls()
{
    display.setDrawColor(1);
    display.drawDisc(                     0, display.getHeight() - 1, 25, U8G2_DRAW_UPPER_RIGHT);
    display.drawDisc(display.getWidth() - 3, display.getHeight() - 1, 25, U8G2_DRAW_UPPER_LEFT);

    display.setDrawColor(0);
    display.setFont(u8g2_font_open_iconic_check_2x_t);
    display.drawGlyph(                      2, display.getHeight() - 1, 64);
    display.drawGlyph(display.getWidth() - 19, display.getHeight() - 1, 68);
}

void DisplayControl::drawTimeSetup(const time_t &time, const char *caption)
{
    char buffer[6];
    sprintf(buffer, "%02d:%02d", hour(time), minute(time));

    display.firstPage();
    do
    {
        display.setDrawColor(1);
        display.setFont(u8g2_font_6x12_tf);
        display.drawStr((display.getWidth() - display.getStrWidth(caption)) / 2, 10, caption);

        display.setFont(u8g2_font_10x20_tn);
        display.drawStr((display.getWidth() - display.getStrWidth(buffer)) / 2, 35, buffer);

        drawControls();
    } while (display.nextPage());
}

void DisplayControl::setVolume(const uint8_t &volume, const uint8_t &mute)
{
    current_volume = volume & 0x003F;
    current_mute = mute;
    do_update = true;
}

void  DisplayControl::setFrequency(const uint16_t &frequency)
{
    if (current_frequency != frequency)
    {
        current_frequency = frequency;
        do_update = true;
    }
}

void DisplayControl::setTime(const time_t &time)
{
    if (minute(current_time) != minute(time))
    {
        current_time = time;
        do_update = true;
    }
}

void DisplayControl::setAlarm(const time_t &time, const uint8_t &num, const uint8_t &enabled)
{
    if (num == 0)
    {
        current_alarm0 = time;
        current_alarm0enabled = enabled;
    }
    else if (num == 1)
    {
        current_alarm1 = time;
        current_alarm1enabled = enabled;
    }
    do_update = true;
}




