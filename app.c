#include "../../drivers.h"
#include "../../genfsk_ll/genfsk_ll.h"
#include "key.h"
#include "app.h"
#include "led.h"
#include "key.h"
#include "board.h"
#include "mac_id.h"
#include "rf.h"
#include "n_timer.h"
#include "power_saving.h"
#include "wakeup.h"
#include "main.h"
#include "low_power.h"

typedef enum {
  CHONGSHUI = 0,
  TINGZHI,
  TUNXI,
  FUXI,
  NUANFENGHONGGAN,
  LENREANMO,
  YEDENG,
  PENZUIQINGXI,
  FANGAI,
  FANQUANG,
  PENZUIWEIZHI_UP,
  PENZUIWEIZHI_DOWN,
  ERTONGQINGXI,
  QINGXIQIANGDU_PLUS,
  QINGXIQIANGDU_DEC,
  SHUIWENTIAOJIE,
  ZUOWENTIAOJIE,
  FENGWENTIAOJIE,
  WEIBO_KAIGUAN,
  WIFI,
  SHEZHI,
  DUIMA//21
}function_t;

static const u8 key_d[] = {
  0x15,
  0x04,
  0x0c,
  0x0d,
  0x0a,
  0x11,
  0x16,
  0x05,
  0x17,
  0x1a,
  0x0e,
  0x0f,
  0x0b,//ertongqingxi
  0x08,
  0x09,
  0x06,
  0x07,
  0x1c,
  0x1b,
  0x03,
  0x1f//20
};

static const u8 key_ad[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0x11,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,//20
};

static const u8 shuiwen_d[11] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02
};
static const u8 zuowen_d[11] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02
};
static const u8 fenwen_d[11] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02
};
static const u8 penzui_d[12] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
};
static const u8 shuiya_d[12] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
};

static const u8 shuiwen_ad[11] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
};
static const u8 zuowen_ad[11] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
};
static const u8 fenwen_ad[11] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
};
static const u8 penzui_ad[12] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
  0x02,
};
static const u8 shuiya_ad[12] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
  0x02,
};
//not use
static const u8 taocichongshui_ad[4] = {
  0x00,
  0x01,
  0x02,
  0x03,
};
//not use
static const u8 weibo_ad[4] = {
  0x00,
  0x01,
  0x02,
  0x03,
};

_attribute_data_retention_ static struct {
  u8 shuiya;
  u8 penzui;
  u8 fenwen;
  u8 shuiwen;
  u8 zuowen;
  u8 unused;
  u8 taoci;
  u8 weibo;
}user_value __attribute__ ((aligned (4)));

typedef enum{
  SHUIYA_INDEX = 1,
  DAOGANG_INDEX,
  FENGWEN_INDEX,
  SHUIWEN_INDEX,
  ZUOWEN_INDEX
}user_en_t;

bool peidui_ok_exit;
u32  peidui_ok_exit_time;

_attribute_data_retention_ static u32 uid;
_attribute_data_retention_ static u32 new_id;
now_level_t now_level = NOW_NULL;

void touch_key_wakeup_power_on_led(u32 leds_off)
{
  u32 ledon;
  u32 ledoff;
  
  if(low_power_threshold() == THRESHOLD_2_3){
    ledoff = HAL_LED_ALL ^ (T_LED_FENGWEN_INDICATE | T_LED_ZUOWEN_INDICATE | T_LED_SHUIWEN_INDICATE);
    HalLedSet(ledoff, HAL_LED_MODE_OFF);
  }else{
    ledon = HAL_LED_ALL ^ (T_LED_FENGWEN_INDICATE | T_LED_ZUOWEN_INDICATE | T_LED_SHUIWEN_INDICATE | leds_off);
    HalLedSet(ledon, HAL_LED_MODE_ON);
  }
}

void power_on_led()
{
  u32 ledon;

  ledon = HAL_LED_ALL ^ (T_LED_FENGWEN_INDICATE | T_LED_ZUOWEN_INDICATE | T_LED_SHUIWEN_INDICATE);
  HalLedSet(ledon, HAL_LED_MODE_ON);
}

void set_user_value(user_en_t u_val, u8 val)
{
  switch(u_val){
    case SHUIYA_INDEX:  user_value.shuiya  = val;break;
    case DAOGANG_INDEX: user_value.penzui  = val;break;
    case FENGWEN_INDEX: user_value.fenwen  = val;break;
    case SHUIWEN_INDEX: user_value.shuiwen = val;break;
    case ZUOWEN_INDEX:  user_value.zuowen  = val;break;
    default:break;
  }

  flash_erase_sector(VAL_ADDRESS);

  flash_write_page(VAL_ADDRESS, sizeof(user_value), (unsigned char *)&user_value);
}

u8 get_user_value(user_en_t u_val)
{
  u8 r_val;

  flash_read_page(VAL_ADDRESS, sizeof(user_value), (unsigned char *)&user_value);

  switch(u_val){
    case SHUIYA_INDEX:  r_val = user_value.shuiya;break;
    case DAOGANG_INDEX: r_val = user_value.penzui;break;
    case FENGWEN_INDEX: r_val = user_value.fenwen;break;
    case SHUIWEN_INDEX: r_val = user_value.shuiwen;break;
    case ZUOWEN_INDEX:  r_val = user_value.zuowen;break;
    default:r_val = 0;break;
  }

  return r_val;
}

void set_default_user_value()
{
  flash_read_page(VAL_ADDRESS, sizeof(user_value), (unsigned char *)&user_value);

  if(user_value.shuiya == 0xff &&\
      user_value.penzui == 0xff &&\
      user_value.fenwen == 0xff &&\
      user_value.shuiwen == 0xff &&\
      user_value.zuowen == 0xff){

    user_value.shuiya    = 2;
    user_value.penzui    = 2;
    user_value.fenwen    = 2;
    user_value.shuiwen   = 2;
    user_value.zuowen    = 2;
    user_value.weibo     = 0;
    user_value.taoci     = 0;

    flash_erase_sector(VAL_ADDRESS);

    flash_write_page(VAL_ADDRESS, sizeof(user_value), (unsigned char *)&user_value);
  }
}

void fix_pack_with_user_value(rf_package_t *rf_pack, function_t fn)
{
  u8 fun;
  fun = fn;

  rf_pack->rf_len1     = 0;
  rf_pack->vid         = 0x5453;

  if(fn == DUIMA){
    gen_random_id(&new_id);

    rf_pack->pid           = new_id;
    rf_pack->control_key   = PAIR_KEY_VALUE;

    rf_pack->rf_seq_no     = 1;

    rf_pack->start_code    = 1;
    rf_pack->d1            = 1;
    rf_pack->d2            = 1;
    rf_pack->j1            = 1;
    rf_pack->ad5           = 1;
    rf_pack->ad6           = 1;
    rf_pack->ad7           = 1;
    rf_pack->j2            = 1;
  }else{
    rf_pack->pid         = uid;
    rf_pack->control_key = 0;

    rf_pack->rf_seq_no   = 1;

    rf_pack->start_code  = 0x3a;
    if(fn == ERTONGQINGXI)
      rf_pack->d1     = 0x80 | (user_value.shuiya << 5) | key_d[fn];
    else
      rf_pack->d1     = 0x00 | (user_value.shuiya << 5) | key_d[fn];
    rf_pack->d2     = (fenwen_d[user_value.fenwen] << 6) | (penzui_d[user_value.penzui] << 4) | (zuowen_d[user_value.zuowen]  << 2) | shuiwen_d[user_value.shuiwen];
    rf_pack->j1     = (rf_pack->start_code + rf_pack->d1 + rf_pack->d2) & 0xff;
    rf_pack->ad5    = (shuiya_ad[user_value.shuiya] << 4) | (taocichongshui_ad[user_value.taoci] << 2) | (weibo_ad[user_value.weibo]);

    rf_pack->ad6    = (fenwen_ad[user_value.fenwen] << 6 ) | (penzui_ad[user_value.penzui] << 4) | (zuowen_ad[user_value.zuowen] << 2) | shuiwen_ad[user_value.shuiwen];
    rf_pack->ad7    = key_ad[fn];
    rf_pack->j2     = (rf_pack->ad5 ^ rf_pack->ad6 ^ rf_pack->ad7) & 0xff;
  }
}

void set_led_level_on(u32 led)
{
  u32 ledoff;

  if(is_bat_warn()){
    led &= ~T_LED_ZUOWEN_INDICATE;
    ledoff = led ^ (T_LED_FENGWEN_INDICATE | T_LED_SHUIWEN_INDICATE);
  }else
    ledoff = led ^ (T_LED_FENGWEN_INDICATE | T_LED_ZUOWEN_INDICATE| T_LED_SHUIWEN_INDICATE);

  HalLedSet (ledoff, HAL_LED_MODE_OFF);
  HalLedSet (led, HAL_LED_MODE_ON);
}

u32 set_led_level(u8 user_value)
{
  u32 leds = 0;

  switch(user_value){
    case 0: leds = 0;  break;
    case 1: leds = T_LED_SHUIWEN_INDICATE ;break;
    case 2: leds = T_LED_SHUIWEN_INDICATE | T_LED_ZUOWEN_INDICATE;break;
    case 3: leds = T_LED_FENGWEN_INDICATE | T_LED_SHUIWEN_INDICATE | T_LED_ZUOWEN_INDICATE;break;
    default:break;
  }

  return leds;
}

void chk_now_level_led(now_level_t level)
{
  u8 temp;
  u32 leds;

  switch(level){
    case NOW_FENGWEN:
      temp = get_user_value(FENGWEN_INDEX);
      leds = set_led_level(temp);
      set_led_level_on(leds);break;
    case NOW_SHUIWEN:
      temp = get_user_value(SHUIWEN_INDEX);
      leds = set_led_level(temp);
      set_led_level_on(leds);break;
    case NOW_ZUOWEN:
      temp = get_user_value(ZUOWEN_INDEX);
      leds = set_led_level(temp);
      set_led_level_on(leds);break;
    case NOW_NULL:break;
    default:break;
  }
}

void touch_key_led_breath()
{
  if(low_power_threshold() != THRESHOLD_2_3){
    switch(cur_key){
      case T_KEY_PENZUI_UP:
        HalLedSet (T_KEY_PENZUI_UP_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_PENZUI_DOWN:
        HalLedSet (T_KEY_PENZUI_DOWN_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_ERTONGQINGXI:
        HalLedSet (T_KEY_ERTONGQINGXI_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_QINGXIQIANGDU_UP:
        HalLedSet (T_KEY_QINGXIQIANGDU_UP_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_QINGXIQIANGDU_DOWN:
        HalLedSet (T_KEY_QINGXIQIANGDU_DOWN_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_SHUIWENTIAOJIE:
        HalLedSet (T_KEY_SHUIWENTIAOJIE_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_ZUOWENTIAOJIE:
        HalLedSet (T_KEY_ZUOWENTIAOJIE_LED, HAL_LED_MODE_BREATHE);break;
      case T_KEY_FENWENTIAOJIE:
        HalLedSet (T_KEY_FENWENTIAOJIE_LED, HAL_LED_MODE_BREATHE);break;
      default:break;
    }
  }
}

bool is_enter_set_status()
{
  static bool tiaojie_status = 0;

  if(pre_key == M_KEY_TUNBUQINGXI || pre_key == M_KEY_NVXINGQINGXI){
    tiaojie_status = 1;
  }else if(pre_key != M_KEY_TUNBUQINGXI && pre_key != M_KEY_NVXINGQINGXI &&\
      pre_key != T_KEY_QINGXIQIANGDU_UP && pre_key != T_KEY_QINGXIQIANGDU_DOWN &&\
      pre_key != T_KEY_PENZUI_UP && pre_key != T_KEY_PENZUI_DOWN){
    tiaojie_status = 0;
  }

  return tiaojie_status;
}

bool is_touch_key_wakeup()
{
  if(wakeup_key == T_KEY_PENZUI_UP)
    return 1;
  else if(wakeup_key == T_KEY_PENZUI_DOWN)
	return 1;
  else if(wakeup_key == T_KEY_ERTONGQINGXI)
    return 1;
  else if(wakeup_key == T_KEY_QINGXIQIANGDU_UP)
    return 1;
  else if(wakeup_key == T_KEY_QINGXIQIANGDU_DOWN)
    return 1;
  else if(wakeup_key == T_KEY_SHUIWENTIAOJIE)
    return 1;
  else if(wakeup_key == T_KEY_ZUOWENTIAOJIE)
    return 1;
  else if(wakeup_key == T_KEY_FENWENTIAOJIE)
    return 1;
  return 0;
}

bool is_mechanial_key_wakeup()
{
  if(wakeup_key == M_KEY_TUNBUQINGXI)
	return 1;
  else if(wakeup_key == M_KEY_NUANFENGHONGGAN)
	return 1;
  else if(wakeup_key == M_KEY_NVXINGQINGXI)
	return 1;
  return 0;	
}

void clr_wakeup_key()
{
  wakeup_key = 255;
}

//"key action" "which key" "cmd-code"
void short_m_key_chongshui_chongshui()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, CHONGSHUI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_tingzhi_tingzhi()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, TINGZHI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_tunbuqingxi_tunxi()
{
  rf_package_t rf_pack;
  u8 shuiwen;
  u32 leds;
  
  now_level = NOW_SHUIWEN;
  
  shuiwen = get_user_value(SHUIWEN_INDEX);

  leds = set_led_level(shuiwen);
  
  if(low_power_threshold() != THRESHOLD_2_3)
    power_on_led();

  set_led_level_on(leds);

  fix_pack_with_user_value(&rf_pack, TUNXI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_nvxingqingxi_fuxi()
{
  rf_package_t rf_pack;
  u8 shuiwen;
  u32 leds;
  
  now_level = NOW_SHUIWEN;
  
  shuiwen = get_user_value(SHUIWEN_INDEX);

  leds = set_led_level(shuiwen);

  if(low_power_threshold() != THRESHOLD_2_3)
    power_on_led();

  set_led_level_on(leds);

  fix_pack_with_user_value(&rf_pack, FUXI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_nuanfenghonggan_nuanfenghonggan()
{
  rf_package_t rf_pack;
  u32 leds;
  u8 fenwen;

  now_level = NOW_FENGWEN;
  
  fenwen = get_user_value(FENGWEN_INDEX);

  set_user_value(FENGWEN_INDEX, fenwen);

  leds = set_led_level(fenwen);

  if(low_power_threshold() != THRESHOLD_2_3)
    power_on_led();

  set_led_level_on(leds);

  fix_pack_with_user_value(&rf_pack, NUANFENGHONGGAN);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_lengreanmo_lengreanmo()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, LENREANMO);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_yedeng_yedeng()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, YEDENG);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_penzuiqingjie_penzuiqingjie()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, PENZUIQINGXI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_fanggai_fanggai()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, FANGAI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_fanquang_fanquang()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, FANQUANG);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_t_key_penzui_up_penzui_up()
{
  rf_package_t rf_pack;
  u8 daogan;
  u8 shuiwen;
  u32 leds;

  HalLedSet (T_KEY_PENZUI_UP_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    if(is_enter_set_status()){
      daogan = get_user_value(DAOGANG_INDEX);
      daogan++;

      if(daogan == 4)
        daogan = 3;

      set_user_value(DAOGANG_INDEX, daogan);
    }

    fix_pack_with_user_value(&rf_pack, PENZUIWEIZHI_UP);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_PENZUI_UP_LED);
    set_led_level_on(leds);
  }
}

void short_t_key_penzui_down_penzui_down()
{
  rf_package_t rf_pack;
  u8 daogan;
  u8 shuiwen;
  u32 leds;

  HalLedSet (T_KEY_PENZUI_DOWN_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    if(is_enter_set_status()){
      daogan = get_user_value(DAOGANG_INDEX);
      daogan--;

      if(daogan == 0)
        daogan = 1;

      set_user_value(DAOGANG_INDEX, daogan);
    }

    fix_pack_with_user_value(&rf_pack, PENZUIWEIZHI_DOWN);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_PENZUI_DOWN_LED);
    set_led_level_on(leds);
  }
}

void short_t_key_ertongqingxi_led_off()
{
  u8 shuiwen;
  u32 leds;

  HalLedSet (T_KEY_ERTONGQINGXI_LED, HAL_LED_MODE_OFF);

  if(is_touch_key_wakeup()){
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
	leds = set_led_level(shuiwen);
	touch_key_wakeup_power_on_led(T_KEY_ERTONGQINGXI_LED);
	set_led_level_on(leds);
  }
}

void short_t_key_ertongqingxi_ertongqingxi()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, ERTONGQINGXI);

  if(!is_touch_key_wakeup())
    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  else
	clr_wakeup_key();
}

void short_t_key_qingxiqiangdu_up_qingxiqiangdu_up()
{
  rf_package_t rf_pack;
  u8 shuiya;
  u8 shuiwen;
  u32 leds;

  HalLedSet (T_KEY_QINGXIQIANGDU_UP_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    if(is_enter_set_status()){
      shuiya = get_user_value(SHUIYA_INDEX);
      shuiya++;

      if(shuiya == 4)
        shuiya = 3;

      set_user_value(SHUIYA_INDEX, shuiya);
    }

    fix_pack_with_user_value(&rf_pack, QINGXIQIANGDU_PLUS);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_QINGXIQIANGDU_UP_LED);
  }
}

void short_t_key_qingxiqiangdu_down_qingxiqiangdu_down()
{
  rf_package_t rf_pack;
  u8 shuiya;
  u8 shuiwen;
  u32 leds;

  HalLedSet (T_KEY_QINGXIQIANGDU_DOWN_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    if(is_enter_set_status()){
      shuiya = get_user_value(SHUIYA_INDEX);
      shuiya--;

      if(shuiya == 0)
        shuiya = 1;

      set_user_value(SHUIYA_INDEX, shuiya);
    }

    fix_pack_with_user_value(&rf_pack, QINGXIQIANGDU_DEC);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_QINGXIQIANGDU_DOWN_LED);
    set_led_level_on(leds);
  }
}

void short_t_key_shuiwentiaojie_shuiwentioajie()
{
  rf_package_t rf_pack;
  u32 leds;
  u8 shuiwen;

  HalLedSet (T_KEY_SHUIWENTIAOJIE_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    now_level = NOW_SHUIWEN;

    shuiwen = get_user_value(SHUIWEN_INDEX);

    shuiwen++;

    if(shuiwen == 4)
      shuiwen = 0;

    set_user_value(SHUIWEN_INDEX, shuiwen);

    leds = set_led_level(shuiwen);

    set_led_level_on(leds);

    fix_pack_with_user_value(&rf_pack, SHUIWENTIAOJIE);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_SHUIWENTIAOJIE_LED);
    set_led_level_on(leds);
  }
}

void short_t_key_zuowentiaojie_zuowentiaojie()
{
  rf_package_t rf_pack;
  u32 leds;
  u8 zuowen;
  u8 shuiwen;

  HalLedSet (T_KEY_ZUOWENTIAOJIE_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    now_level = NOW_ZUOWEN;

    zuowen = get_user_value(ZUOWEN_INDEX);

    zuowen++;

    if(zuowen == 4)
      zuowen = 0;

    set_user_value(ZUOWEN_INDEX, zuowen);

    leds = set_led_level(zuowen);

    set_led_level_on(leds);

    fix_pack_with_user_value(&rf_pack, ZUOWENTIAOJIE);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
	now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_ZUOWENTIAOJIE_LED);   
    set_led_level_on(leds);
  }
}

void short_t_key_fenwentiaojie_fenwentiaojie()
{
  rf_package_t rf_pack;
  u32 leds;
  u8 fenwen;
  u8 shuiwen;

  HalLedSet (T_KEY_FENWENTIAOJIE_LED, HAL_LED_MODE_OFF);

  if(!is_touch_key_wakeup()){
    now_level = NOW_FENGWEN;

    fenwen = get_user_value(FENGWEN_INDEX);

    fenwen++;

    if(fenwen == 4)
      fenwen = 0;

    set_user_value(FENGWEN_INDEX, fenwen);

    leds = set_led_level(fenwen);

    set_led_level_on(leds);

    fix_pack_with_user_value(&rf_pack, FENGWENTIAOJIE);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else{
	clr_wakeup_key();
    now_level = NOW_SHUIWEN;
    shuiwen = get_user_value(SHUIWEN_INDEX);
    leds = set_led_level(shuiwen);
    touch_key_wakeup_power_on_led(T_KEY_FENWENTIAOJIE_LED);
    set_led_level_on(leds);
  }
}

void combin_m_key_lengreanmo_m_key_penzuiqingjie_weibo()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, WEIBO_KAIGUAN);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void long_m_key_yedeng_wifi()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, WIFI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void long_t_key_ertongqingxi_shezhi()
{
  rf_package_t rf_pack;

  if(!is_touch_key_wakeup()){
    fix_pack_with_user_value(&rf_pack, SHEZHI);

    send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
  }else
	clr_wakeup_key();
}

void long_m_key_tingzhi_yonghuduima()
{
  rf_package_t rf_pack;
  u8 rx_buf[32] = {0};
  u32 send_period;

  HalLedSet(HAL_LED_ALL ^ (T_LED_SHUIWEN_INDICATE |\
        T_LED_ZUOWEN_INDICATE |\
        T_LED_FENGWEN_INDICATE), HAL_LED_MODE_ON);

  HalLedBlink (T_LED_FENGWEN_INDICATE | T_LED_SHUIWEN_INDICATE | T_LED_ZUOWEN_INDICATE, 30, 50, MS2TICK(920));

  fix_pack_with_user_value(&rf_pack, DUIMA);

  send_period = clock_time();

  while(1){
    if(!HalLedUpdate(NULL)){
      rf_8359_set_tx();
      reload_sys_time();
      return;
    }

    if(n_clock_time_exceed(send_period, 1000000)){//1S
      rf_8359_set_tx();
      send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
      rf_8359_set_rx();
      send_period = clock_time();
    }

    if(receive_rf_data(rx_buf))
    {
      if(rx_buf[7] == 0x88){//received peer code
        rf_8359_set_tx();
        write_id(&new_id, 4);
        uid = new_id;
        HalLedBlink (T_LED_FENGWEN_INDICATE | T_LED_SHUIWEN_INDICATE | T_LED_ZUOWEN_INDICATE, 1, 100, MS2TICK(5000));
        peidui_ok_exit = 1;
        peidui_ok_exit_time = clock_time();
        reload_sys_time();
        return;
      }
    }

    if(app_read_single_key(M_KEY_TINGZHI)){
      rf_8359_set_tx();
      HalLedSet (T_LED_FENGWEN_INDICATE | T_LED_SHUIWEN_INDICATE | T_LED_ZUOWEN_INDICATE, HAL_LED_MODE_OFF);
      reload_sys_time();
      return;
    }
  }
}




#define SHORT_KEY_TIME 2000 //MS
#define LONG_KEY_TIME  3000 //MS
#define LONG_KEY_PEIDUI_TIME 5000

void app_init()
{
  if(!is_wakeup_from_sleep()){
    set_default_user_value();
    read_id(&uid, 4);
  }

  register_key_event(M_KEY_CHONGSHUI,          0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_chongshui_chongshui);

  register_key_event(M_KEY_TINGZHI,            0, MS2TICK(SHORT_KEY_TIME), 0, SHORT_KEY, short_m_key_tingzhi_tingzhi);
  register_key_event(M_KEY_TINGZHI,            0, MS2TICK(LONG_KEY_PEIDUI_TIME), 0, LONG_KEY, long_m_key_tingzhi_yonghuduima);

  register_key_event(M_KEY_TUNBUQINGXI,        0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_tunbuqingxi_tunxi);
  register_key_event(M_KEY_NVXINGQINGXI,       0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_nvxingqingxi_fuxi);
  register_key_event(M_KEY_NUANFENGHONGGAN,    0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_nuanfenghonggan_nuanfenghonggan);

  register_key_event(M_KEY_LENGREANMO,         0, MS2TICK(SHORT_KEY_TIME), 0, SHORT_KEY, short_m_key_lengreanmo_lengreanmo);

  register_key_event(M_KEY_YEDENG,             0, MS2TICK(SHORT_KEY_TIME), 0, SHORT_KEY, short_m_key_yedeng_yedeng);
  register_key_event(M_KEY_YEDENG,             0, MS2TICK(LONG_KEY_TIME), 0, LONG_KEY, long_m_key_yedeng_wifi);

  register_key_event(M_KEY_PENZUIQINGJIE,      0, MS2TICK(SHORT_KEY_TIME), 0, SHORT_KEY, short_m_key_penzuiqingjie_penzuiqingjie);

  register_key_event(M_KEY_FANGGAI,            0,  0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_fanggai_fanggai);

  register_key_event(M_KEY_FANQUANG,           0,  0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_fanquang_fanquang);

  register_key_event(M_KEY_LENGREANMO,    M_KEY_PENZUIQINGJIE, MS2TICK(3000), 0, COMBIN_KEY_IN_TIME, combin_m_key_lengreanmo_m_key_penzuiqingjie_weibo);
  register_key_event(M_KEY_PENZUIQINGJIE, M_KEY_LENGREANMO,    MS2TICK(3000), 0, COMBIN_KEY_IN_TIME, combin_m_key_lengreanmo_m_key_penzuiqingjie_weibo);

  register_key_event(T_KEY_PENZUI_UP,          0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_penzui_up_penzui_up);
  register_key_event(T_KEY_PENZUI_UP,          0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);

  register_key_event(T_KEY_PENZUI_DOWN,        0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_penzui_down_penzui_down);
  register_key_event(T_KEY_PENZUI_DOWN,        0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);

  register_key_event(T_KEY_ERTONGQINGXI,       0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_ertongqingxi_led_off);
  register_key_event(T_KEY_ERTONGQINGXI,       0, MS2TICK(SHORT_KEY_TIME), 0, SHORT_KEY, short_t_key_ertongqingxi_ertongqingxi);
  register_key_event(T_KEY_ERTONGQINGXI,       0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);
  register_key_event(T_KEY_ERTONGQINGXI,       0, MS2TICK(LONG_KEY_TIME), 0, LONG_KEY, long_t_key_ertongqingxi_shezhi);

  register_key_event(T_KEY_QINGXIQIANGDU_UP,   0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_qingxiqiangdu_up_qingxiqiangdu_up);
  register_key_event(T_KEY_QINGXIQIANGDU_UP,   0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);

  register_key_event(T_KEY_QINGXIQIANGDU_DOWN, 0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_qingxiqiangdu_down_qingxiqiangdu_down);
  register_key_event(T_KEY_QINGXIQIANGDU_DOWN, 0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);

  register_key_event(T_KEY_SHUIWENTIAOJIE,     0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_shuiwentiaojie_shuiwentioajie);
  register_key_event(T_KEY_SHUIWENTIAOJIE,     0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);

  register_key_event(T_KEY_ZUOWENTIAOJIE,      0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_zuowentiaojie_zuowentiaojie);
  register_key_event(T_KEY_ZUOWENTIAOJIE,      0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);

  register_key_event(T_KEY_FENWENTIAOJIE,      0, 0, 0, SHORT_KEY_IMMEDIATELY, short_t_key_fenwentiaojie_fenwentiaojie);
  register_key_event(T_KEY_FENWENTIAOJIE,      0, 0, 0, NO_TIME_LIMIT_KEY_RELEASED, touch_key_led_breath);
}
