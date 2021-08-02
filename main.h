#ifndef _MAIN_H__
#define _MAIN_H__

#include "../../common/mempool.h"
#include "key.h"
#include "app.h"

#define CLOCK_SYS_CLOCK_HZ  	24000000

MEMPOOL_EXTERN(KEY_EVENT_POOL, KEY_EVENT_POOL_MEM, sizeof(mem_block_t) + sizeof(event_handler_t), MAX_EVENT);

extern bool low_power_led_indicate;
extern bool low_power;
extern bool low_low_power;
#endif
