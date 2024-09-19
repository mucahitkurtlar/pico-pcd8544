#include "../../include/pcd8544.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    struct pcd8544 screen = {.spi = spi1, .rst = 12, .ce = 13, .dc = 11, .din = 15, .clk = 14};
    pcd8544_init(&screen);

    while (true) {
        pcd8544_clear_buffer(&screen);
        pcd8544_draw_circle(&screen, PCD8544_WIDTH / 2, PCD8544_HEIGHT / 2, 20, true);
        pcd8544_update(&screen);
        sleep_ms(1000);

        pcd8544_clear_buffer(&screen);
        pcd8544_draw_circle(&screen, PCD8544_WIDTH / 2, PCD8544_HEIGHT / 2, 20, false);
        pcd8544_update(&screen);
        sleep_ms(1000);
    }

    return 0;
}
