#ifndef PCD8544_H
#define PCD8544_H

#include <stdbool.h>
#include "hardware/spi.h"

#define PCD8544_SPI_BAUDRATE      4000000

#define PCD8544_FUNCTION_SET      0x20
#define PCD8544_INSTRUCTION_BASIC 0x00
#define PCD8544_INSTRUCTION_EXT   0x01
#define PCD8544_ENTRY_MODE        0x02
#define PCD8544_POWER_DOWN        0x04

// Set Y address of RAM (Basic instruction set)
#define PCD8544_SETYADDR          0x40
// Set X address of RAM (Basic instruction set)
#define PCD8544_SETXADDR          0x80

// Set display configuration. (Basic instruction set)
#define PCD8544_DISPLAY_CONTROL   0x08
#define PCD8544_DISPLAY_BLANK     0x00
#define PCD8544_DISPLAY_NORMAL    0x04
#define PCD8544_DISPLAY_ALL_ON    0x01
#define PCD8544_DISPLAY_INVERTED  0x05

// Set temperature control (Extended instruction set)
#define PCD8544_TEMP_CONTROL      0x04
// Set bias system (Extended instruction set)
#define PCD8544_BIAS_SYSTEM       0x10
// Set Vop (contrast) (Extended instruction set)
#define PCD8544_SET_VOP           0x80

#define PCD8544_WIDTH             84
#define PCD8544_HEIGHT            48

typedef enum { BLACK = 0, WHITE = 1 } pcd8544_color_t;

struct pcd8544 {
    uint8_t buffer[504];
    spi_inst_t* spi;
    uint16_t rst;
    uint16_t ce;
    uint16_t dc;
    uint16_t din;
    uint16_t clk;
};

/*!
* \brief Initialize the PCD8544 display
* 
* \param scr Pointer to the pcd8544 struct
*/
void pcd8544_init(struct pcd8544* scr);

/*!
* \brief Set the display mode
*
* \param scr Pointer to the pcd8544 struct
* \param mode Display mode (PCD8544_DISPLAY_BLANK, PCD8544_DISPLAY_NORMAL, PCD8544_DISPLAY_ALL_ON, PCD8544_DISPLAY_INVERTED)
*/
void pcd8544_set_display_mode(struct pcd8544* scr, uint8_t mode);

/*!
* \brief Se the contrast of the display
*
* \param scr Pointer to the pcd8544 struct
* \param contrast Contrast value (0-127)
*/
void pcd8544_set_contrast(struct pcd8544* scr, uint8_t contrast);

/*!
* \brief Set the cursor position
*
* \param scr Pointer to the pcd8544 struct
* \param x X position
* \param y Y position
*/
void pcd8544_set_cursor(struct pcd8544* scr, uint8_t x, uint8_t y);

/*!
* \brief Clear the buffer
*
* \param scr Pointer to the pcd8544 struct
*/
void pcd8544_clear_buffer(struct pcd8544* scr);

/*!
* \brief Clear the screen
*
* \param scr Pointer to the pcd8544 struct
*/
void pcd8544_clear_screen(struct pcd8544* scr);

/*!
* \brief Draw a pixel on the screen
*
* \param scr Pointer to the pcd8544 struct
* \param x X position
* \param y Y position
*/
void pcd8544_draw_pixel(struct pcd8544* scr, uint8_t x, uint8_t y);

/*!
* \brief Draw a line on the screen
*
* \param scr Pointer to the pcd8544 struct
* \param x0 X position of the first point
* \param y0 Y position of the first point
* \param x1 X position of the second point
* \param y1 Y position of the second point
*/
void pcd8544_draw_line(struct pcd8544* scr, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/*!
* \brief Draw a rectangle on the screen
*
* \param scr Pointer to the pcd8544 struct
* \param x X position of the top-left corner
* \param y Y position of the top-left corner
* \param width Width of the rectangle
* \param height Height of the rectangle
* \param fill Fill the rectangle
*/
void pcd8544_draw_rectangle(struct pcd8544* scr, uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool fill);

/*!
* \brief Draw a triangle on the screen
*
* \param scr Pointer to the pcd8544 struct
* \param x0 X position of the first point
* \param y0 Y position of the first point
* \param x1 X position of the second point
* \param y1 Y position of the second point
* \param x2 X position of the third point
* \param y2 Y position of the third point
* \param fill Fill the triangle
*/
void pcd8544_draw_triangle(struct pcd8544* scr, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                           bool fill);

/*!
* \brief Draw a circle on the screen
*
* \param scr Pointer to the pcd8544 struct
* \param x0 X position of the center
* \param y0 Y position of the center
* \param radius Radius of the circle
* \param fill Fill the circle
*/
void pcd8544_draw_circle(struct pcd8544* scr, uint8_t x0, uint8_t y0, uint8_t radius, bool fill);

/*!
* \brief Update the display. This function sends the buffer to the display.
*
* \param scr Pointer to the pcd8544 struct
*/
void pcd8544_update(struct pcd8544* scr);

#endif
