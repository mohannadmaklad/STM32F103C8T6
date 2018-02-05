#include "general.h"
#include "stm32f10x.h" 

void delay(uint32_t max)
{
	uint32_t volatile count = 0 ;
	while( count < max )
	{
		count++;
	}
}

void copyArray(uint8_t*src, uint8_t*dest,uint32_t size)
{
	uint32_t i = 0;
	while(i <= size)  {dest[i]=src[i]; i++;}
}

void InverseCopyArray(uint8_t frame1[],uint8_t kernelFrame[], uint32_t size)
{
	uint32_t i = 0;
	while(i <= size)  {frame1[i] ^= kernelFrame[i]; i++;}
}

void ANDCopyArray(uint8_t frame1[],uint8_t frame2[],uint8_t kernel[], uint32_t size)
{
	uint32_t i = 0;
	while(i <= size)  {kernel[i] = frame1[i]&frame2[i]; i++;}
}
