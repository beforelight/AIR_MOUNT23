/*
 * lfs_port.cpp
 *
 *  Created on: Sep 9, 2020
 *      Author: 17616
 */
#include "lfs_port.h"
#include "string.h"
#include "stdio.h"
#define FLASH_RWSIZE (512*1024)//512kb
#define FLASH_SIZE (1024*1024)//1024kb
#define FLASH_RWADDR_START (FLASH_BASE + FLASH_SIZE - FLASH_RWSIZE)
#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE (128*1024)
#endif // !FLASH_PAGE_SIZE



 /* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */


int FLASH_LfsGetDefaultConfig(lfs_config* lfsc)
{
	memset(lfsc, 0, sizeof(lfs_config));
	lfsc->context = NULL;
	lfsc->read = FLASH_LfsRead;
	lfsc->prog = FLASH_LfsProg;
	lfsc->erase = FLASH_LfsErase;
	lfsc->sync = FLASH_LfsSync2;
	lfsc->read_size = 4;
	lfsc->prog_size = 4;
	lfsc->block_size = FLASH_PAGE_SIZE;
	lfsc->block_count = FLASH_RWSIZE / FLASH_PAGE_SIZE;
	lfsc->cache_size = 256;
	lfsc->lookahead_size = 32;
	lfsc->block_cycles = 500;
	return 0;
}

int FLASH_LfsRead(const lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	//printf("read 0x%x,size%d\r\n", (uint32_t)(FLASH_RWADDR_START + c->block_size * block + off), size);
	memcpy(buffer, (void*)(FLASH_RWADDR_START + c->block_size * block + off), size);
	return 0;
}
#include"cmsis_gcc.h"
int FLASH_LfsProg(const lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	//printf("prog 0x%x,size%d\r\n", (uint32_t)(FLASH_RWADDR_START + c->block_size * block + off), size);
	int rtval;
	HAL_FLASH_Unlock();     //解锁
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	uint32_t priMask;
	__asm volatile ("MRS %0, primask" : "=r" (priMask) :: "memory");
	__asm volatile ("cpsid i" : : : "memory");
	for (uint32_t i = 0; i < size/4; i++)
	{
		uint32_t addr = FLASH_RWADDR_START + c->block_size * block + off + i*4;
		//rtval = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, ((uint8_t*)buffer)[i]);
		//rtval = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, ((uint8_t*)buffer)[i]);
		rtval = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, ((uint32_t*)buffer)[i]);
		if (rtval != 0) { HAL_FLASH_Lock(); return rtval; }
	}
	__asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");

	HAL_FLASH_Lock();
	//return 0;
	return rtval;
}

int FLASH_LfsErase(const lfs_config* c, lfs_block_t block)
{
	//printf("erse 0x%x\r\n", (uint32_t)(FLASH_RWADDR_START + c->block_size * block));
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_1;
	EraseInitStruct.Sector = FLASH_SECTOR_6 + block;
	EraseInitStruct.NbSectors = 1;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	HAL_FLASH_Unlock();     //解锁
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	uint32_t priMask;
	__asm volatile ("MRS %0, primask" : "=r" (priMask) :: "memory");
	__asm volatile ("cpsid i" : : : "memory");
	int rtval = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	__asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
	HAL_FLASH_Lock();
	//return 0;
	return rtval;
}

int FLASH_LfsSync2(const lfs_config* c)
{
	//HAL_FLASH_Unlock();     //解锁
	//HAL_FLASH_Lock();
	return 0;
}

int FLASH_TEST(void)
{
	uint8_t buf[2048];
	lfs_config lfsc;
	FLASH_LfsGetDefaultConfig(&lfsc);
	FLASH_LfsErase(&lfsc, 0);
	FLASH_LfsRead(&lfsc, 0, 0, buf, 2048);
    for (size_t i = 0; i < 2048; i++)
	{
		if (buf[i] != 0xff) {
			printf("erase error\r\n");
			return -1;
		}
	}
	for (size_t i = 0; i < 2048; i++)
	{
		buf[i] = i;
	}
	FLASH_LfsProg(&lfsc, 0, 0, buf, 2048);
	FLASH_LfsRead(&lfsc, 0, 0, buf, 2048);
	for (size_t i = 0; i < 2048; i++)
	{
		if (buf[i] != ((uint8_t)i)) {
			printf("prog error\r\n");
			return -1;
		}
	}
	return 0;
}

lfs_t *hlfs = nullptr;
lfs_t lfs0;
struct lfs_config cfg;
int FLASH_Init(void)
{
  FLASH_LfsGetDefaultConfig(&cfg);
  // mount the filesystem
  int err = lfs_mount(&lfs0, &cfg);
  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err) {
    printf("重新格式化文件系统\r\n");
    lfs_format(&lfs0, &cfg);
    lfs_mount(&lfs0, &cfg);
  }

  hlfs = &lfs0;
  printf("文件系统已初始化\r\n");
  return 0;
}
