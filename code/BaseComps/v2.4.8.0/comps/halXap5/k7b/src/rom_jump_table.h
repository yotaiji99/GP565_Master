#ifndef __ROM_JUMP_TABLE_H__
#define __ROM_JUMP_TABLE_H__

/* #define ROM_JUMPTABLE_IS_32BITS 1 */

#define GP_MM_ROM_JUMP_TABLE_START  (GP_MM_ROM_START + 8)

#define SPI_ACCESS_MODE     0
#define GO_TO_SLEEP         1
#define FLASH_ERASE         2
#define FLASH_WRITE_PAGE    3

#endif  /* __ROM_JUMP_TABLE_H__ */
