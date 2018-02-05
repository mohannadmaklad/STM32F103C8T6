
#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"
#include <stdint.h>


#define WIDTH   84
#define HEIGHT  48
#define LCD_SIZE 504 //(84*48)/8

#define func_set              (0x20)
#define vertical_addr         (0x02)



#define SPACE             LCD_data(0x00)
#define NRES              GPIOA->ODR |=  ((((uint16_t)  0x1)  << 8))
#define RES               GPIOA->ODR &=  (~(((uint16_t) 0x1)  << 8))
#define SET_DC            GPIOA->ODR |=  ((((uint16_t)  0x1)  << 9))
#define RESET_DC          GPIOA->ODR &=  (~(((uint16_t) 0x1)  << 9))

//flags
#define done                                       0
#define out_of_boundary                            1

//actions to take if it "indx" out of boundary
#define nothing                                    0
#define clearScreen                                1
#define addAtBottom                                2

/*    LCD CONFIGURATION    */
#define whenOutOfBoundaryDo                        addAtBottom

extern void wait(void);
void LCD_init(void);
char setX(uint8_t x);
char setY(uint8_t y);
void LCD_cmd(uint8_t cmd);
void LCD_data(uint8_t data);
char pixel(uint8_t x , uint8_t y); //DRAWS PIXEL
void LCD_clear(void);
void filldown(uint8_t x1 , uint8_t x2 , uint8_t y1);
void fillup(uint8_t x1 , uint8_t x2 , uint8_t y1);
void fill(uint8_t x1 , uint8_t x2 , uint8_t y1 , uint8_t y2);
char fillsegment(uint8_t y , uint8_t x1 , uint8_t x2);
void line(uint8_t x1 , uint8_t y1 , uint8_t x2 , uint8_t y2);

void px_vline(uint8_t x , uint8_t y1 , uint8_t y2); 
void vline(uint8_t x1 , uint8_t x2 , uint8_t y1 , uint8_t y2);
char square(uint8_t x1 , uint8_t y1 ,uint8_t x2 , uint8_t y2 );



/*------------------------------- New Functions -------------------------------*/

void display(uint8_t lcdArr[]);        //uploads data to an LCD
void resetFrame(uint8_t lcdArr[]);
void fillLCD(uint8_t* lcdArr);
uint8_t character(uint8_t* lcdArr,uint16_t chr);
uint8_t space(uint8_t* lcdArr);
uint8_t displayCursor(void);
void write(uint8_t* lcdArr,char* chr);
void writeInt(uint8_t frame[], int n);
uint8_t newLine(void);
void setPixel(uint8_t* LCDarr, uint8_t x , uint8_t y);
void resetPixel(uint8_t* LCDarr, uint8_t x , uint8_t y);
uint8_t isPixelSet(uint8_t* LCDarr, uint8_t x , uint8_t y);
void LCDsquare(uint8_t* LCDarr, uint8_t x, uint8_t y , uint8_t w , uint8_t h);
void LCDerase(uint8_t* LCDarr, uint8_t x, uint8_t y , uint8_t w , uint8_t h);
void indexToXY(uint32_t index, uint8_t* x , uint8_t* y); //stores X,Y inot the corresponding ptr
uint32_t XYtoIndex(uint8_t x, uint8_t y); //returns the index
void invert(uint8_t LCDarr[], uint32_t startIndex,uint32_t endIndex);
void horizLine(uint8_t frame[],uint8_t x1,uint8_t x2,uint8_t y);
void removeHorizLine(uint8_t frame[],uint8_t x1,uint8_t x2,uint8_t y);
#endif
