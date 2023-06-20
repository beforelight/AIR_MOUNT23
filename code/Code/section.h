#ifndef AVIS_RT_SECTION_H
#define AVIS_RT_SECTION_H

/*
 * 分区表
 *  ITC 0x0  128k 指令存储
 *  DTC 0x20,000,000 128k 数据存储  全局变量、堆栈
 *  OC 0x20,200,200,000 256k 通用存储,dma buff必须放在这个区 nocache区
 *  SDRAM 0x80,000,000 32M 混合存储代码和数据，代码是在上电之后拷贝到SDRAM区的
 *  FLASH 0x60,000,000 16M 只读区
 */


#define ALIGN(n) __attribute__((aligned(n)))

#define RAM_FUNC   __attribute__((section(".RamFunc.text")))
#define BSS_CCMRAM __attribute__((section(".ccmram.bss")))
//#define BSS_CCMRAM

#endif //AVIS_RT_SECTION_H
