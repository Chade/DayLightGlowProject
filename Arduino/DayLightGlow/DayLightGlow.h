#ifndef _DAYLIGHTGLOW_H_
#define _DAYLIGHTGLOW_H_

// *****************************************************************************
// Communication
// *****************************************************************************

#define BAUD_RATE                    115200

// *****************************************************************************
// Pins
// *****************************************************************************

// SensorLamp
#define LED_PIN                      11
#define SENSOR_PIN                   A0

// Encoder
#define ENCODER_LEFT_A_PIN           2
#define ENCODER_LEFT_B_PIN           4
#define ENCODER_LEFT_BUTTON_PIN      7
#define ENCODER_RIGHT_A_PIN          3
#define ENCODER_RIGHT_B_PIN          8
#define ENCODER_RIGHT_BUTTON_PIN     9



// *****************************************************************************
// Delays
// *****************************************************************************

#define CYCLE_TIME                   10


// *****************************************************************************
// Display settings
// *****************************************************************************

#define LCDML_DISP_WIDTH             128            // LCD width
#define LCDML_DISP_HEIGHT            64             // LCD height
// Font settings
#define LCDML_DISP_FONT              u8g_font_6x12  // u8glib font (more fonts under u8g.h line 1520 ...)
#define LCDML_DISP_FONT_W            6              // font width
#define LCDML_DISP_FONT_H            12             // font height
// Cursor settings
#define LCDML_DISP_CURSOR_CHAR       ">"            // cursor char
#define LCDML_DISP_CUR_SPACE_BEFORE  2              // cursor space between
#define LCDML_DISP_CUR_SPACE_BEHIND  4              // cursor space between
// Menu position and size
#define LCDML_DISP_BOX_X0            0              // start point (x0, y0)
#define LCDML_DISP_BOX_Y0            0              // start point (x0, y0)
#define LCDML_DISP_BOX_X1            128            // width x  (x0 + width)
#define LCDML_DISP_BOX_Y1            64             // hight y  (y0 + height)
#define LCDML_DISP_FRAME             1              // draw a box around the menu
#define LCDML_DISP_FRAME_OFFSET      LCDML_DISP_BOX_X0 + LCDML_DISP_FONT_W + LCDML_DISP_CUR_SPACE_BEHIND
// Scrollbar settings
#define LCDML_DISP_SCROLL_WIDTH      6              // scrollbar width (if this value is < 3, the scrollbar is disabled)
// Rows and cols
#define LCDML_DISP_COLS_MAX          ((LCDML_DISP_BOX_X1-LCDML_DISP_BOX_X0)/LCDML_DISP_FONT_W)
#define LCDML_DISP_ROWS_MAX          ((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0-((LCDML_DISP_BOX_Y1-LCDML_DISP_BOX_Y0)/LCDML_DISP_FONT_H))/LCDML_DISP_FONT_H)
// When you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// The program needs more ram with this changes
#define LCDML_DISP_ROWS              LCDML_DISP_ROWS_MAX  // max rows
#define LCDML_DISP_COLS              20                   // max cols


// *****************************************************************************
// General
// *****************************************************************************

#define NUM_CHANNEL                  8
#define FILE_NAME                    "config.txt"
#define BACKUP_NAME                  "backup.txt"


// *****************************************************************************
// Classes
// *****************************************************************************


#endif // _DAYLIGHTGLOW_H_
