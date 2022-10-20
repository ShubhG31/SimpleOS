#include "types.h"

extern void RTC_init();
int32_t RTC_open();
extern void RTC_read();
extern void RTC_write();
int32_t RTC_close();
extern void RTC_handle();
extern void RTC_handler();
int32_t Changing_RTC_rate(int8_t rate);
