/*
 * lfs_port.h
 *
 *  Created on: Sep 9, 2020
 *      Author: 17616
 */

#ifndef LITTLEFS_LFS_PORT_H_
#define LITTLEFS_LFS_PORT_H_
#include"stm32f4xx_hal.h"
#include"lfs.h"
int FLASH_LfsGetDefaultConfig(struct lfs_config* lfsc);
int FLASH_LfsRead(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
int FLASH_LfsProg(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
int FLASH_LfsErase(const struct lfs_config* c, lfs_block_t block);
int FLASH_LfsSync2(const struct lfs_config* c);

int FLASH_TEST(void);

extern lfs_t *hlfs;
int FLASH_Init(void);



#endif /* LITTLEFS_LFS_PORT_H_ */
