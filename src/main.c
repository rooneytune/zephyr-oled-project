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
static lv_obj_t *hum_label, *hum_value_label;
static lv_obj_t *pm25_label, *pm25_value_label;
static lv_obj_t *voc_label, *voc_value_label;
static char temp_value[11] = {0};

// Declaration of all Device tree struct
static const struct device *gpio_ct_dev =
    DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const int BUILTIN_LED_PIN = 2;

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(blinking_led), gpios);

static const struct device *display = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

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
    displayHelloWorldMessage_CFB();
  default:
    return "Unknown";
  }
}

static void button_event_handler(enum button_evt evt)
{
  printk("Button event: %s\n", helper_button_evt_str(evt));
  switch (evt)
  {
  case BUTTON_EVT_PRESSED:
    gpio_pin_toggle_dt(&led);
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
bool setupDisplayDevice()
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
//-----------------------------------------------------------------------
// Uses CharacterFrameBuffer to configure display and display a message
// THIS WORKS !  So device tree setup must be OK
//-----------------------------------------------------------------------
// void displayHelloWorldMessage_CFB(void)
// {
//   int ret;
//   ret = cfb_framebuffer_init(display);
//   if (ret != 0)
//   {
//     printk("Could not initialise the display\n");
//     LOG_ERR("Could not initialise the display");
//     return false;
//   }

//   ret = cfb_framebuffer_invert(display);

//   ret = cfb_print(display, "Display OK 1", 0, 0);
//   if (ret != 0)
//   {
//     LOG_ERR("could not print to display");
//     return;
//   }

//   ret = cfb_framebuffer_finalize(display);

//   if (ret != 0)
//   {
//     printk("could not finalise!\n");
//     LOG_ERR("could not finalise");
//     return;
//   }
//   LOG_INF("Finalised display.\n");

// }

//------------------------------------------------------------------------------------------
// Sample code, not in use
//-------------------------------------------------------------------------------------------
void displayHelloWorldWithCounterLoop_LVGL(void)
{

  int ret;
  LOG_INF("Initialising LVLG code . . . \n");

  uint32_t count = 0U;
  char count_str[11] = {0};

  lv_obj_t *hello_world_label;
  lv_obj_t *count_label;

  printk("LVLG - labels defined\n");

  if (IS_ENABLED(CONFIG_LV_Z_POINTER_KSCAN))
  {
    lv_obj_t *hello_world_button;

    hello_world_button = lv_btn_create(lv_scr_act());
    lv_obj_align(hello_world_button, LV_ALIGN_CENTER, 0, 0);
    hello_world_label = lv_label_create(hello_world_button);
    printk("LVLG - button defined\n");
  }
  else
  {
    hello_world_label = lv_label_create(lv_scr_act());
  }

  lv_label_set_text(hello_world_label, "Hello world!");
  lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);
  printk("LVLG - set text\n");

  count_label = lv_label_create(lv_scr_act());
  lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);

  lv_task_handler();
  display_blanking_off(display);

  // printk("Entering display loop");
  // while (1)
  // { // Set the active screen pointer to screen1
  //   lv_scr_load(screen1);
  //   sprintf(count_str, "%d", count / 100U);
  //   lv_label_set_text(count_label, count_str);
  // }
  // lv_task_handler();
  // k_msleep(1000);
  // ++count;
};

//------------------------------------------------------------------------------------------
// Works, but maybe an error with memory allocation
//-------------------------------------------------------------------------------------------
void displayHelloWorldLabel_LVGL(void)
{
  LOG_INF("displayHelloWorldLabel_LVGL started\n");

  //********Processing stalls here !***********
  lv_init();
  // size_t lvglMemory;
  // lvglMemory = 180000;
  //  #define DRAW_BUF_SIZE  (128 * 64 / 10 * (LV_COLOR_DEPTH / 8))
  // uint32_t draw_buf[DRAW_BUF_SIZE / 4];
  
  // lv_mem_alloc(lvglMemory);
  

  LOG_INF("LVGL - init OK\n");

  static lv_style_t stSmall;
  lv_style_init(&stSmall);
  lv_style_set_text_font(&stSmall, &lv_font_unscii_8);

  // lv_style_set_text_font(&stRoberto, &rob);

  temp_label = lv_label_create(lv_scr_act());
  lv_label_set_text(temp_label, "Roon*Deluxe:");
  lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_add_style(temp_label, &stSmall, 0);

  temp_value_label = lv_label_create(lv_scr_act());
  lv_label_set_text(temp_value_label, "*");
  lv_obj_align(temp_value_label, LV_ALIGN_TOP_LEFT, 0, 14);
  lv_obj_add_style(temp_value_label, &stSmall, 0);

  LOG_INF("LVGL - labels created\n");

  lv_task_handler();

  //   // Create a screen
  //   lv_obj_t * screen = lv_scr_act();

  //   // Create a style for the background
  //   static lv_style_t style_bg;
  //   lv_style_init(&style_bg);
  //   lv_style_set_bg_color(&style_bg, lv_color_black()); // Set background color to black

  //   // Create a style for the text
  //   static lv_style_t style_text;
  //   lv_style_init(&style_text);
  //   lv_style_set_text_color(&style_text,  lv_color_white()); // Set text color to white

  //   // Create a label (text)
  //   lv_obj_t * label = lv_label_create(screen);
  //   lv_label_set_text(label, "Hello, World!");

  //   // Apply the style to the label
  //   lv_obj_add_style(label, LV_OBJ_DRAW_PART_RECTANGLE, &style_text);

  //   // Apply the style to the screen
  //   lv_obj_add_style(screen, LV_OBJ_DRAW_PART_RECTANGLE, &style_bg);

  // lv_disp_t * lv_disp = lv_disp_get_default();
  // lv_theme_default_init(lv_disp, lv_color_black(), lv_color_white(), 1, &lv_font_montserrat_14);
  // Initialize the LVGL display driver
  // lv_disp_drv_init(&display);

  // lv_disp_draw_buf_t disp_buf;
  // static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10]; // Declare a buffer for 1/10 screen size
  // lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10); // Adjust the size according to your display

  // lv_init();
  //  // Initialize the display
  // display_blanking_off(display);

  // Create your screens
  //  lv_obj_t *screen1 = lv_obj_create(NULL);

  // // Set the active screen pointer to screen1
  // lv_scr_load(screen1);

  // lv_obj_t *temp_label = lv_label_create(lv_scr_act());
  // lv_label_set_text(temp_label, "T: (C)");
  // lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 0);

  // lv_obj_t *temp_value_label = lv_label_create(lv_scr_act());
  // lv_label_set_text(temp_value_label, "*");
  // lv_obj_align(temp_value_label, LV_ALIGN_TOP_LEFT, 0, 14);

  // lv_task_handler();

  /*Change the active screen's background color*/
  // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

  // /*Create a white label, set its text and align it to the center*/
  // lv_obj_t *label = lv_label_create(lv_scr_act());
  // lv_label_set_text(label, "Hello world");
  // lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
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

  displayHelloWorldLabel_LVGL();

  // Test the display - This method runs OK
  // displayHelloWorldMessage_CFB();

  // Main Loop that toggles led state (just to signify that everything has initialied)
  volatile int loopIndex = 0;
  gpio_pin_set_dt(&led, 0);
  while (true)
  {
  
    loopIndex++;
    LOG_INF("Main loop - Iteration: %d...\n", loopIndex);

    //Update Label with loop index
    snprintf(temp_value, sizeof(temp_value) - 1, "%d", loopIndex);
    lv_label_set_text(temp_value_label, temp_value);

    // Update LVGL tasks
    lv_task_handler();
    

    //Toggle led
    ret = gpio_pin_toggle_dt(&led);
    
    k_msleep(1000);


  }
}
