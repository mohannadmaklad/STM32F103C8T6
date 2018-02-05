#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <stdint.h>

void delay(uint32_t max);
void copyArray(uint8_t*sec, uint8_t*dest,uint32_t size);
void InverseCopyArray(uint8_t frame1[],uint8_t kernelFrame[], uint32_t size);
void ANDCopyArray(uint8_t frame1[],uint8_t frame2[],uint8_t kernel[], uint32_t size);



#endif
