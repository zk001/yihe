#include "../../drivers.h"
#include "../../genfsk_ll/genfsk_ll.h"
#include "key.h"
#include "rf.h"
#include "app.h"
#include "led.h"
#include "dc.h"
#include "n_timer.h"
#include "../../common/mmem.h"
#include "../../common/mempool.h"
#include "board.h"
#include "mac_id.h"
#include "gpio_key.h"
#include "gpio_led.h"
#include "power_saving.h"
#include "low_power.h"
#include "interrupt.h"
#include "timer_clock.h"
#include "wakeup.h"
#include "i2c_gpio_set.h"

bool led_low_power_indi;
bool start_low_power_led_indi;
bool apt_int_gpio;
bool low_power_led_indicate;
bool only_once;
bool low_low_power;
bool low_power;
bool low_bat_check;
u32 zuo_wen_led_time_ref;

MEMPOOL_DECLARE(KEY_EVENT_POOL, KEY_EVENT_POOL_MEM, sizeof(mem_block_t) + sizeof(event_handler_t), MAX_EVENT);
_attribute_data_retention_ key_map_t key_arry[MAX_GPIO_KEYS] = {
  {ROW0, COL0, IS_WAKE_UP},//M_KEY_NUANFENGHONGGAN
  {ROW0, COL1, IS_WAKE_UP},//M_KEY_LENGREANMO
  {ROW0, COL2, IS_WAKE_UP},//M_KEY_YEDENG
  {ROW0, COL3, IS_WAKE_UP},//M_KEY_PENZUIQINGJIE
  {ROW1, COL0, IS_WAKE_UP},//M_KEY_TUNBUQINGXI
  {ROW1, COL1, IS_WAKE_UP},//M_KEY_NVXINGQINGXI
  {ROW1, COL2, IS_WAKE_UP},//M_KEY_FANGGAI
  {ROW1, COL3, IS_WAKE_UP},//M_KEY_FANQUANG
  {ROW2, COL0, IS_WAKE_UP},//M_KEY_CHONGSHUI
  {ROW2, COL1, IS_WAKE_UP}//M_KEY_TINGZHI
};

const hal_led_t led_enum_arry[MAX_LEDS] = {
  {HAL_LED_1,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_2,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_3,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_4,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_5,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_6,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_7,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_8,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_9,  aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_10, aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_11, aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_12, aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_13, aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_14, aw9523_led_on_off, aw9523_led_breath},
  {HAL_LED_15, aw9523_led_on_off, aw9523_led_breath},
};

const key_type_t key_enum_arry[MAX_KEYS] = {
  {KEY0,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY1,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY2,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY3,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY4,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY5,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY6,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY7,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY8,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY9,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY10, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY11, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY12, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY13, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY14, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY15, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY16, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
  {KEY17, TOUCH_KEY, apt8_init, apt8_read, apt8_read},
};

bool is_mechinal_key()
{

	if(cur_key == M_KEY_CHONGSHUI ||\
	   cur_key == M_KEY_TUNBUQINGXI ||\
	   cur_key == M_KEY_NUANFENGHONGGAN ||\
	   cur_key == M_KEY_YEDENG ||\
	   cur_key == M_KEY_FANGGAI ||\
	   cur_key == M_KEY_TINGZHI ||\
	   cur_key == M_KEY_NVXINGQINGXI ||\
	   cur_key == M_KEY_LENGREANMO ||\
	   cur_key == M_KEY_PENZUIQINGJIE ||\
	   cur_key == M_KEY_FANQUANG)
		return 1;
	else
		return 0;
}

int main(void)
{
  blc_pm_select_internal_32k_crystal();

  cpu_wakeup_init();

  clock_init(SYS_CLK_24M_Crystal);

  gpio_init(1);

  dc_power_on();

  if(!is_wakeup_from_sleep()){
    gpio_key_alloc(key_arry, MAX_GPIO_KEYS);
    register_key(key_enum_arry, MAX_KEYS);

    register_led(led_enum_arry, MAX_LEDS);
  }

  //init m_key and t_key
  if(!is_wakeup_from_sleep())
    key_init();
  else
    key_wakeup_init();

  key_process(NULL);

  rf_8359_set_tx();

  aw9523_init();

  if(!is_wakeup_from_sleep())
    HalLedInit();

  ev_on_timer(key_process, NULL, KEY_PROCESS_TIME);

  ev_on_timer(HalLedUpdateBreath, NULL, LED_BREATH_PROCESS_TIME);

  ev_on_timer(HalLedUpdate, NULL, LED_UPDATE_PROCESS_TIME);

  if(!is_wakeup_from_sleep())
    id_init();

  //init mempool for key event, the mempool length is equal to (siezof(mem_block_t) + siezeof(event_handler_t))*MAX_EVENT
  mempool_init(&KEY_EVENT_POOL, &KEY_EVENT_POOL_MEM[0], sizeof(mem_block_t) + sizeof(event_handler_t), MAX_EVENT);

  //user should register key event in this function
  app_init();

  if(!is_wakeup_from_sleep())
	only_once = 1;

  if(!is_wakeup_from_sleep())
    idle_time_for_sleep(SLEEP_WAIT_TIME);

  reload_sys_time();

  low_bat_check = 1;
  start_low_power_led_indi = 1;

  while(1){
    ev_process_timer();

    if(low_bat_check){
      low_bat_check = 0;

      if(is_low_power(THRESHOLD_2_3)){
    	  apt_enter_sleep();
          low_low_power = 1;
          low_power_led_indicate = 1;
      }else{
    	  low_low_power = 0;
    	  apt_exit_sleep();
      }

      if(is_low_power(THRESHOLD)){
        low_power = 1;
      }
    }

    if(start_low_power_led_indi){
    	if(is_mechinal_key()  && (low_low_power || low_power)){
    	start_low_power_led_indi = 0;
    	if(low_low_power)
          HalLedSet(HAL_LED_ALL^(T_LED_ZUOWEN_INDICATE | T_LED_SHUIWEN_INDICATE | T_LED_FENGWEN_INDICATE) , HAL_LED_MODE_OFF);
    	HalLedBlink(T_LED_ZUOWEN_INDICATE, 5, 50, MS2TICK(1000));//5s
    	zuo_wen_led_time_ref = clock_time();
    	led_low_power_indi = 1;
    	}
    }

    if(led_low_power_indi){
      if(n_clock_time_exceed(zuo_wen_led_time_ref, 5000000)){//5S
    	  led_low_power_indi = 0;
        if(low_power)
        	low_power = 0;
        if(low_power_led_indicate)
        	low_power_led_indicate = 0;
  	  u32 level_ind;
  	  level_ind = chk_now_level_led(now_level);
  	  reload_led(level_ind);
      }
    }

    if(only_once && !low_low_power){
    	only_once = 0;
        power_on_led();
    }

	poll_key_event();

    if(poll_idle_time()){
      HalLedSet(HAL_LED_ALL, HAL_LED_MODE_OFF);
      gpio_key_sleep_setup();
      touch_key_sleep_setup();
      set_wakeup_flag();
      dc_shutdown();
      cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
    }
  }
}


