#### Basic zephyr project

Device tree used to define LED, button, display and BME280 devices.

Controller: ESP32
Display: I2c SSD1306 - OLED - Single colour - 128 * 64

Code runs OK, am now able to add lvgl objects to screen and update.
However, a memory related issue causes and error, after updating the count label.  Sometimes the error occurs within a few iterations.  Other times, it can loop several hundred times without issue. Assuming I've done something silly with declarations / memory allocation