#### Basic zephyr project

Device tree used to define LED, button, display and BME280 devices.

Controller: ESP32
Display: I2c SSD1306 - OLED - Single colour - 128 * 64

Code runs OK, until I attempt to reference LVGL methods.  Display is definately working, as I am able to display text using Copy Frame Buffer library.
As soon as code reaches a method containing a reference to LVGL, it appears to crash. Assuming this is caused by bad kconfig settings
