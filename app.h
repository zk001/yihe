#ifndef __APP_H__
#define __APP_H__

typedef struct {
  u8  rf_len1;
  u16 vid;
  u32 pid;
  u8  control_key;
  u8  rf_seq_no;
  u8  start_code;// a1;

  u8 d1;
  u8 d2;
  u8 j1;
  u8 ad5;
  u8 ad6;
  u8 ad7;
  u8 j2;
  u8 unused;
}__attribute__((packed))rf_package_t;

typedef enum{
  NOW_SHUIWEN = 1,
  NOW_FENGWEN,
  NOW_ZUOWEN
}now_level_t;

extern _attribute_data_retention_  now_level_t now_level;

#define M_KEY_CHONGSHUI          KEY8
#define M_KEY_TUNBUQINGXI        KEY4 //WAKE UP
#define M_KEY_NUANFENGHONGGAN    KEY0 //WAKE UP
#define M_KEY_YEDENG             KEY2
#define M_KEY_FANGGAI            KEY6
#define M_KEY_TINGZHI            KEY9 //WAKE UP
#define M_KEY_NVXINGQINGXI       KEY5 //WAKE UP
#define M_KEY_LENGREANMO         KEY1
#define M_KEY_PENZUIQINGJIE      KEY3
#define M_KEY_FANQUANG           KEY7

#define T_KEY_PENZUI_UP           KEY11
#define T_KEY_PENZUI_DOWN         KEY10
#define T_KEY_ERTONGQINGXI        KEY12
#define T_KEY_QINGXIQIANGDU_UP    KEY13
#define T_KEY_QINGXIQIANGDU_DOWN  KEY14
#define T_KEY_SHUIWENTIAOJIE      KEY17
#define T_KEY_ZUOWENTIAOJIE       KEY16
#define T_KEY_FENWENTIAOJIE       KEY15

#define T_KEY_PENZUI_UP_LED           HAL_LED_1
#define T_KEY_ERTONGQINGXI_LED        HAL_LED_2
#define T_KEY_QINGXIQIANGDU_UP_LED    HAL_LED_3
#define PEIZUIWEIZHI_TUBIAO_LED1      HAL_LED_4
#define PEIZUIWEIZHI_TUBIAO_LED2      HAL_LED_5
#define QINGXIQIANGDU_TUBIAO_LED1     HAL_LED_6
#define QINGXIQIANGDU_TUBIAO_LED2     HAL_LED_7
#define T_KEY_PENZUI_DOWN_LED         HAL_LED_8
#define T_KEY_QINGXIQIANGDU_DOWN_LED  HAL_LED_9
#define T_LED_SHUIWEN_INDICATE        HAL_LED_10
#define T_LED_ZUOWEN_INDICATE         HAL_LED_12
#define T_LED_FENGWEN_INDICATE        HAL_LED_11

#define T_KEY_SHUIWENTIAOJIE_LED      HAL_LED_13
#define T_KEY_ZUOWENTIAOJIE_LED       HAL_LED_14
#define T_KEY_FENWENTIAOJIE_LED       HAL_LED_15

#define COMBIN_KEY_SETUP_TIME  (1000*16*1000)
#define COMBIN_TIME            (3000*16*1000)//ms2tick(3000)
#define COMBIN_KEY_LAST_TIME   (3000*16*1000)

#define SHORT_TIME  (3000*16*1000)//ms2tick(3000)
#define LONG_TIME   (4000*16*1000)//ms2tick(3000)
#define STUCK_TIME  (50000*16*1000)//ms2tick(30000)

#define KEY_PROCESS_TIME        5000//US
#define LED_UPDATE_PROCESS_TIME 10000//US
#define LED_BREATH_PROCESS_TIME 50000//US

#define LED_ON_TIME     5000 //MS
#define SLEEP_WAIT_TIME 5000 //MS
#define LEVEL_INDI_LED_ON_TIME 5000 //MS

#define ID_Flash_Addr	0x020000 //address store id
#define VAL_ADDRESS     0x021000 //address store global var ,zuowen fenwen etc

#define THRESHOLD     2490
#define THRESHOLD_2_3 2270

#define PWM_PERIOD  20000  //US
#define PWM_ON_DUTY 60     //US

#define LED_BRIGHT_LEVEL 40

#define PAIR_KEY_VALUE	0x55
#define MAX_EVENT       55//the number is the register_key_event call times

extern bool peidui_ok_exit;
extern bool reload_led_off;
extern u32 peidui_ok_exit_time;

extern void app_init();
extern void power_on_led();
extern void reload_led(u32 now_off_led);
extern u32 chk_now_level_led(now_level_t level);

#endif
