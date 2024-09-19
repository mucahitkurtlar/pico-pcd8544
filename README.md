# pico-pcd8544

A simple library for the PCD8544 LCD controller (Nokia 5110) for the Raspberry Pi Pico.

This project heavily inspired by [Adafruit's PCD8544 library](https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library) for Arduino.

## Running Examples

### Wiring

| Pico Pin | PCD8544 Pin |
|----------|-------------|
| GP12     | RST         |
| GP13     | CE          |
| GP11     | D/C         |
| GP15     | DIN         |
| GP14     | CLK         |
| 3V3      | VCC         |
| 3v3      | BL          |
| GND      | GND         |

### Building and Flashing

```bash
git clone https://github.com/mucahitkurtlar/pico-pcd8544
cd pico-pcd8544/examples/circle
cmake -B build -S .
cmake --build build
cp build/circle.uf2 /path/to/pico
```

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
