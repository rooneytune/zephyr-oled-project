#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>

#include <zephyr/logging/log.h>
#include <zephyr/display/cfb.h>

#include "button.h"
#include <zephyr/drivers/sensor.h>

#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/zephyr.h>

// Init logging module and set name
LOG_MODULE_REGISTER(main);

#define LV_HOR_RES_MAX 128
#define LV_VER_RES_MAX 64

#define LV_COLOR_BLACK 0x000000
#define LV_COLOR_WHITE 0xFFFFFF

static lv_obj_t *temp_label, *temp_value_label;
static lv_obj_t *button_press_count_label, *button_press_count_value_label;


// Declaration of all Device tree struct
static const struct device *gpio_ct_dev =
    DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const int BUILTIN_LED_PIN = 2;

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(blinking_led), gpios);

static const struct device *display = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

static volatile int button_press_count=0;

/*
Methods / types used by button events
*/
static char *helper_button_evt_str(enum button_evt evt)
{
  switch (evt)
  {
  case BUTTON_EVT_PRESSED:
    return "Pressed";
  case BUTTON_EVT_RELEASED:
    return "Released";
    
  default:
    return "Unknown";
  }
}

static void button_event_handler(enum button_evt evt)
{
  button_press_count++;
  printk("Button event: %s - %d\n", helper_button_evt_str(evt),button_press_count);
  switch (evt)
  {
  case BUTTON_EVT_PRESSED:
    //gpio_pin_toggle_dt(&led);
    
    break;

  default:
    break;
  }
}

/*
Initialises a button press event handers.  Implementation in button.c
*/
bool setupButtonPressEventHandler()
{
  int err = -1;

  printk("Initialising button !\n");

  err = button_init(button_event_handler);
  if (err)
  {
    printk("Button Init failed: %d\n", err);
    return false;
  }

  printk("Init succeeded. Waiting for event...\n");
  LOG_INF("Button init succeeded. Waiting for event...\n");

  return true;
}

/*
 Setup for a single LED pin.  Works fine
*/
bool setupLeds()
{
  //  if (!device_is_ready(gpio_ct_dev))
  printk("Initialising LEDs !\n");
  if (!device_is_ready(led.port))
  {
    return;
  }
  int ret;
  /*ret = gpio_pin_configure(gpio_ct_dev, BUILTIN_LED_PIN, GPIO_OUTPUT_ACTIVE);
   */
  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret != 0)
  {
    return false;
  }
  LOG_INF("LED init complete...\n");
  return true;
}

// Basic steps, required for all display libraries
static bool setupDisplayDevice()
{
  printk("Initialising display !\n");
  if (display == NULL)
  {
    printk("Display device pointer is null !\n");
    LOG_ERR("Display device pointer is null");
    return false;
  }

  if (!device_is_ready(display))
  {
    printk("Display device not ready!\n");
    LOG_ERR("Display device not ready");
    return false;
  }

  /*
    LVGL INIT steps, as recommended by ChatGPT
    Both init and register methods not recognised at compile time
  */

  // lv_init();
  // lv_disp_drv_t disp_drv;
  // lv_disp_driv_init(&disp_drv);
  // // disp_drv.flush_cb = my_flush_cb;
  // lv_disp_driv_register(&disp_drv);
  // LOG_INF("LV display components initialised");
  return true;
};



//------------------------------------------------------------------------------------------
// Works, but maybe an error with memory allocation
//-------------------------------------------------------------------------------------------
 static void add_controls_to_display_LVGL(void)
{
  LOG_INF("add_controls_to_display_LVGL started\n");

  //  #define DRAW_BUF_SIZE  (128 * 64 / 10 * (LV_COLOR_DEPTH / 8))
  // uint32_t draw_buf[DRAW_BUF_SIZE / 4];
  lv_init();
  // size_t lvglMemory = 180000;
  // lv_mem_alloc(lvglMemory);

  LOG_INF("LVGL - init OK\n");

  static lv_style_t stSmall;
  lv_style_init(&stSmall);
  lv_style_set_text_font(&stSmall, &lv_font_unscii_8);

  // lv_style_set_text_font(&stRoberto, &rob);

  //Set counter controls
  temp_label = lv_label_create(lv_scr_act());
  lv_label_set_text(temp_label, "Counter:");
  lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_add_style(temp_label, &stSmall, 0);

  temp_value_label = lv_label_create(lv_scr_act());
  lv_label_set_text(temp_value_label, "*");
  lv_obj_align(temp_value_label, LV_ALIGN_TOP_LEFT, 65, 0);
  lv_obj_add_style(temp_value_label, &stSmall, 0);


  //NOT YET IMPLEMENTED
  button_press_count_label = lv_label_create(lv_scr_act());
  lv_label_set_text(button_press_count_label, "Btn Press Cnt:");
  lv_obj_align(button_press_count_label, LV_ALIGN_TOP_LEFT, 0, 12);
  lv_obj_add_style(button_press_count_label, &stSmall, 0);

  button_press_count_value_label = lv_label_create(lv_scr_act());
  lv_label_set_text(button_press_count_value_label, "*");
  lv_obj_align(button_press_count_value_label, LV_ALIGN_TOP_LEFT, 0, 20);
  lv_obj_add_style(button_press_count_value_label, &stSmall, 0);

  LOG_INF("LVGL - labels created\n");

  lv_task_handler();

}
static void updateDisplay(int loopIndex)
{
  char label_value[11] ;
  // Update Label with loop index
   snprintf(label_value, sizeof(label_value) - 1, "%d", loopIndex);
  lv_label_set_text(temp_value_label, label_value);


// Not implemented

   snprintf(label_value, sizeof(label_value) - 1, "%d", button_press_count);
  lv_label_set_text(button_press_count_value_label, label_value);


  // Update LVGL tasks
  lv_task_handler();
}
int main(void)
{

  /*-------------------
  -Setup
  ---------------------*/
  int ret;

  LOG_INF("Setup inititated. .  . .\n");
  setupLeds();
  setupButtonPressEventHandler();
  setupDisplayDevice();

  // k_msleep(2000);

  add_controls_to_display_LVGL();

  // Test the display - This method runs OK
  // displayHelloWorldMessage_CFB();

  // Main Loop that toggles led state (just to signify that everything has initialied)
  volatile int loopIndex = 0;
  gpio_pin_set_dt(&led, 0);
  while (true)
  {

    loopIndex++;
    //LOG_INF("Main loop - Iteration: %d...\n", loopIndex);
    updateDisplay(loopIndex);

    // Toggle led
    ret = gpio_pin_toggle_dt(&led);

    k_msleep(300);
  }
}
