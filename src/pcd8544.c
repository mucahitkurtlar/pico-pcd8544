#include "../include/pcd8544.h"
#include <stdlib.h>
#include <string.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

static void pcd8544_send_command(struct pcd8544* att, uint8_t cmd);
static void pcd8544_send_data(struct pcd8544* att, const uint8_t* data, size_t length);

void pcd8544_init(struct pcd8544* scr) {
    gpio_init(scr->dc);
    gpio_set_dir(scr->dc, GPIO_OUT);
    gpio_init(scr->ce);
    gpio_set_dir(scr->ce, GPIO_OUT);
    gpio_init(scr->rst);
    gpio_set_dir(scr->rst, GPIO_OUT);

    gpio_set_function(scr->din, GPIO_FUNC_SPI);
    gpio_set_function(scr->clk, GPIO_FUNC_SPI);

    gpio_put(scr->rst, false);
    sleep_us(1);
    gpio_put(scr->rst, true);

    spi_init(scr->spi, PCD8544_SPI_BAUDRATE);
    spi_set_format(scr->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    pcd8544_send_command(scr, PCD8544_FUNCTION_SET | PCD8544_INSTRUCTION_EXT);
    pcd8544_send_command(scr, PCD8544_SET_VOP | 0x3F);
    pcd8544_send_command(scr, PCD8544_TEMP_CONTROL | 0x00);
    pcd8544_send_command(scr, 0x14);
    pcd8544_send_command(scr, PCD8544_FUNCTION_SET);
    pcd8544_send_command(scr, PCD8544_DISPLAY_CONTROL | PCD8544_DISPLAY_NORMAL);
    pcd8544_clear_buffer(scr);
    pcd8544_clear_screen(scr);
}

void pcd8544_set_display_mode(struct pcd8544* scr, uint8_t mode) {
    if (mode == PCD8544_DISPLAY_BLANK || mode == PCD8544_DISPLAY_NORMAL || mode == PCD8544_DISPLAY_ALL_ON
        || mode == PCD8544_DISPLAY_INVERTED) {
        pcd8544_send_command(scr, PCD8544_DISPLAY_CONTROL | mode);
    }
}

void pcd8544_set_contrast(struct pcd8544* scr, uint8_t contrast) {
    if (contrast > 127) {
        contrast = 127;
    }
    pcd8544_send_command(scr, PCD8544_SET_VOP | contrast);
}

void pcd8544_set_cursor(struct pcd8544* scr, uint8_t x, uint8_t y) {
    if (x >= PCD8544_WIDTH || y >= PCD8544_HEIGHT) {
        return;
    }

    pcd8544_send_command(scr, PCD8544_SETXADDR | x);
    pcd8544_send_command(scr, PCD8544_SETYADDR | y);
}

void pcd8544_clear_buffer(struct pcd8544* scr) { memset(scr->buffer, 0, sizeof(scr->buffer)); }

void pcd8544_clear_screen(struct pcd8544* scr) {
    for (size_t i = 0; i < 504; i++) {
        pcd8544_send_data(scr, (uint8_t[]){0x00}, 1);
    }
}

void pcd8544_draw_pixel(struct pcd8544* scr, uint8_t x, uint8_t y) {
    if (x >= PCD8544_WIDTH || y >= PCD8544_HEIGHT) {
        return;
    }

    scr->buffer[x + (y / 8) * PCD8544_WIDTH] |= 1 << (y % 8);
}

void pcd8544_draw_line(struct pcd8544* scr, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        pcd8544_draw_pixel(scr, x0, y0);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void pcd8544_draw_rectangle(struct pcd8544* scr, uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool fill) {
    if (x >= PCD8544_WIDTH || y >= PCD8544_HEIGHT) {
        return;
    }

    if (fill) {
        for (uint8_t i = 0; i < height; i++) {
            pcd8544_draw_line(scr, x, y + i, x + width, y + i);
        }
    } else {
        pcd8544_draw_line(scr, x, y, x + width, y);
        pcd8544_draw_line(scr, x, y, x, y + height);
        pcd8544_draw_line(scr, x + width, y, x + width, y + height);
        pcd8544_draw_line(scr, x, y + height, x + width, y + height);
    }
}

void pcd8544_draw_triangle(struct pcd8544* scr, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
                           bool fill) {
    if (fill) {
        int16_t a, b, y, last;
        if (y0 > y1) {
            a = y1;
            b = y0;
        } else {
            a = y0;
            b = y1;
        }
        last = y1;

        if (y2 < a) {
            a = y2;
        } else if (y2 > b) {
            b = y2;
        }

        for (y = a; y <= b; y++) {
            if (y < y0) {
                pcd8544_draw_line(scr, x0 + (x1 - x0) * (y - y0) / (y1 - y0), y, x0 + (x2 - x0) * (y - y0) / (y2 - y0),
                                  y);
            } else if (y < y1) {
                pcd8544_draw_line(scr, x1 + (x0 - x1) * (y - y1) / (y0 - y1), y, x1 + (x2 - x1) * (y - y1) / (y2 - y1),
                                  y);
            } else {
                pcd8544_draw_line(scr, x2 + (x0 - x2) * (y - y2) / (y0 - y2), y, x2 + (x1 - x2) * (y - y2) / (y1 - y2),
                                  y);
            }
        }
    } else {
        pcd8544_draw_line(scr, x0, y0, x1, y1);
        pcd8544_draw_line(scr, x1, y1, x2, y2);
        pcd8544_draw_line(scr, x2, y2, x0, y0);
    }
}

void pcd8544_draw_circle(struct pcd8544* scr, uint8_t x0, uint8_t y0, uint8_t radius, bool fill) {
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    pcd8544_draw_pixel(scr, x0, y0 + radius);
    pcd8544_draw_pixel(scr, x0, y0 - radius);
    pcd8544_draw_pixel(scr, x0 + radius, y0);
    pcd8544_draw_pixel(scr, x0 - radius, y0);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (fill) {
            pcd8544_draw_line(scr, x0 - x, y0 + y, x0 + x, y0 + y);
            pcd8544_draw_line(scr, x0 - x, y0 - y, x0 + x, y0 - y);
            pcd8544_draw_line(scr, x0 - y, y0 + x, x0 + y, y0 + x);
            pcd8544_draw_line(scr, x0 - y, y0 - x, x0 + y, y0 - x);
        } else {
            pcd8544_draw_pixel(scr, x0 + x, y0 + y);
            pcd8544_draw_pixel(scr, x0 - x, y0 + y);
            pcd8544_draw_pixel(scr, x0 + x, y0 - y);
            pcd8544_draw_pixel(scr, x0 - x, y0 - y);
            pcd8544_draw_pixel(scr, x0 + y, y0 + x);
            pcd8544_draw_pixel(scr, x0 - y, y0 + x);
            pcd8544_draw_pixel(scr, x0 + y, y0 - x);
            pcd8544_draw_pixel(scr, x0 - y, y0 - x);
        }
    }

    if (fill) {
        pcd8544_draw_line(scr, x0 - radius, y0, x0 + radius, y0);
    }
}

void pcd8544_update(struct pcd8544* scr) {
    pcd8544_set_cursor(scr, PCD8544_SETXADDR, PCD8544_SETYADDR);
    pcd8544_send_data(scr, scr->buffer, sizeof(scr->buffer));
}

static void pcd8544_send_command(struct pcd8544* scr, uint8_t cmd) {
    gpio_put(scr->dc, false);
    gpio_put(scr->ce, false);
    spi_write_blocking(scr->spi, &cmd, 1);
    gpio_put(scr->ce, true);
}

static void pcd8544_send_data(struct pcd8544* scr, const uint8_t* data, size_t length) {
    gpio_put(scr->dc, true);
    gpio_put(scr->ce, false);
    spi_write_blocking(scr->spi, data, length);
    gpio_put(scr->ce, true);
}
