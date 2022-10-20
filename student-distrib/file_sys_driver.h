#include "lib.h"
#include "types.h"

extern int dir_open(const uint8_t* filename);
extern int dir_close(int32_t fd);
extern int dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int dir_write(int32_t fd, const void* buf, int32_t nbytes);

extern int file_open(const uint8_t* filename);
extern int file_close(int32_t fd);
extern int file_read(int32_t fd, void* buf, int32_t nbytes);
extern int file_write(int32_t fd, const void* buf, int32_t nbytes);
