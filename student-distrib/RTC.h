#include "types.h"
#include "lib.h"

extern void RTC_init();
int32_t RTC_open(const uint8_t* filename);
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes);
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t RTC_close(int32_t fd);
extern void RTC_handle();
extern void RTC_handler();
int32_t Changing_RTC_rate(int8_t rate);
