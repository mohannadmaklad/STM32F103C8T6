#include "stm32f10x.h"                  // Device header
#include "HWInit.h"
#include "general.h"
#include "lcd.h"
#include "interrupt.h"
#include "GPIO.h"

/*------------------------Menu------------------------*/
uint8_t currentOption = 1 , numberOfOptions = 3;
uint32_t tmpIndex1,tmpIndex11, tmpIndex2,tmpIndex22,tmpIndex3, tmpIndex33, tmpIndex4;
/*----------------------Animation---------------------*/
void frameTransition();
uint8_t tmpFrame[LCD_SIZE];

/*-----------------------Frames-----------------------*/

uint8_t configFrame[LCD_SIZE];
uint8_t drawFrame[LCD_SIZE];

uint32_t writingSpeed = 40000; //decrease this value to Increase the speed
void initConfigFrame(uint8_t frame[]);
void initDrawFrame(uint8_t frame[]);
void updateDrawFrame(uint8_t frame[],uint8_t x , uint8_t y);
void animate(uint8_t* frames[],uint8_t nuOfFrames,uint32_t delay);

/*---------------------State Machine---------------------*/
enum states{drawState,configState};
uint8_t CurrentState = drawState;
/*------------------------Square-------------------------*/
//The initial square on the screen (the brush of the painter :D)
uint8_t sqWidth  = 2;  // 2 Pixels wide
uint8_t sqHeight = 2;  // 2 Pixels height
uint8_t sqX = 0 , sqY = 0 ;
uint8_t eraseFlag = 0 ; //This flag is set when the erase mode is on
/*-------------------------------------------------------*/

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

/*-------------- LCD FUNCTIONS  --------------*/
uint16_t volatile *sr_ptr = &(SPI2->SR) ; 

int main(void)
 { 
  CLK_init ();
	RCC_init ();
	GPIO_init();
	SPI_init ();	
	EXTI_init();
	LCD_init ();
	 
	// Initial commands to init the LCD
	LCD_cmd(0x21);  // Function set : H = 1
	LCD_cmd(0xb1);  // 1011 0001 - Vop register 
	LCD_cmd(0x05);  //
	LCD_cmd(0x14);  //14
	LCD_cmd(0x20);
	LCD_cmd(0x0c); // Normal mode , 0x9 turns all segments on
	LCD_cmd(0xB0);
	 
	//resetFrame makes the frame blank (all zeros)
	resetFrame(drawFrame);
	resetFrame(configFrame);
	resetFrame(tmpFrame);
	 
	initConfigFrame(configFrame);
	initDrawFrame(tmpFrame);
	
	display(tmpFrame);
	
	//EXTI0 ----> 6 .. EXTI4 ----> 10
	//EXTI5 ----> 23
	enableInterrupt(6);    //LEFT
	enableInterrupt(7);    //RIGHT
	enableInterrupt(8);    //UP
	enableInterrupt(9);    //DOWN
	enableInterrupt(10);   //OK
	enableInterrupt(23);   //CONFIG
	
	while(1)
	{

	}
}

/****************************/
void wait(void)
{
	while ((*sr_ptr & ((uint16_t)0x1 << 1 ) ) != 0x2 );
	
}


void EXTI0_IRQHandler(void) ////////////LEFT BUTTON
{
	uint32_t volatile counter = 0;
	while(1)
	{
		if(getInput(GPIOA,0))
		{
			counter = 0;
			break;
		} else counter++ ;
		
		if(counter >= 5000)
		{
			/*-------  Your Code Goes Here  -------*/
			switch(CurrentState)
			{
				case drawState :
				{
					while(!getInput(GPIOA,0))
					{
						if(eraseFlag == 1)
						{
							copyArray(drawFrame,tmpFrame,LCD_SIZE);
							if(sqX > 0) --sqX; else sqX = WIDTH-1 ;
							if(isPixelSet(tmpFrame,sqX,sqY) || isPixelSet(tmpFrame,sqX+sqWidth,sqY+sqHeight))
								LCDerase(tmpFrame, sqX, sqY, sqWidth, sqHeight);
							else{LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);}
							display(tmpFrame);
							delay(writingSpeed/2);
						}
						else
						{
							copyArray(drawFrame,tmpFrame,LCD_SIZE);
							if(sqX > 0) --sqX; else sqX = WIDTH-1 ;
							LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);
							display(tmpFrame);
							if(!getInput(GPIOA,4))
							{
								copyArray(tmpFrame,drawFrame,LCD_SIZE);
								display(drawFrame);
							}
							delay(writingSpeed);
						}
					}
					break;
				}
				case configState : 
				{
					extern uint16_t indx;
					uint16_t tmpIndx = indx;
					
					if((currentOption == 1) && sqWidth > 2)
					{
						--sqWidth ;
						indx = tmpIndex11;
						invert(configFrame,tmpIndex1,tmpIndex2);
						write(configFrame,"  ");
						indx = tmpIndex11;
						writeInt(configFrame,sqWidth);
						invert(configFrame,tmpIndex1,tmpIndex2);
						display(configFrame);
						updateDrawFrame(tmpFrame,sqX,sqY);
					}
					else if((currentOption == 2) && sqHeight > 2)
					{
						--sqHeight;
						indx = tmpIndex22;
						invert(configFrame,tmpIndex2,tmpIndex3);
						write(configFrame,"  ");
						indx = tmpIndex22;
						writeInt(configFrame,sqHeight);
						invert(configFrame,tmpIndex2,tmpIndex3);
						display(configFrame);
						updateDrawFrame(tmpFrame,sqX,sqY);
					}
					else if(currentOption == 3)
					{
						extern uint16_t indx;
						uint8_t tmp_indx22 = indx;
						indx = tmpIndex33;
						eraseFlag ^= 1;
						invert(configFrame,tmpIndex3,tmpIndex4-1);
						if(eraseFlag) write(configFrame,"  (ON)"); else write(configFrame, " (OFF)");
						invert(configFrame,tmpIndex3,tmpIndex4-1);
						display(configFrame);
					}
					
					break;
				}
		}
		break;
		}
	}
	
	resetPendingBit(0);
}

void EXTI1_IRQHandler(void) //Right
{
	
		uint32_t volatile counter = 0;
	while(1)
	{
		if(getInput(GPIOA,1))
		{
			counter = 0;
			break;
		} else counter++ ;
		
		if(counter >= 5000)
		{
			/*-------  Your Code Goes Here  -------*/

			switch(CurrentState)
			{
				case drawState :
				{
					while(!getInput(GPIOA,1))
					{
						if(eraseFlag)
						{
							copyArray(drawFrame,tmpFrame,LCD_SIZE);
							if(sqX < WIDTH-1) ++sqX; else sqX = 0 ;
							if(isPixelSet(tmpFrame,sqX,sqY) || isPixelSet(tmpFrame,sqX+sqWidth,sqY+sqHeight)) LCDerase(tmpFrame, sqX, sqY, sqWidth, sqHeight);
							else{LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);}
							display(tmpFrame);
							delay(writingSpeed/2);
						}
						else
						{
							copyArray(drawFrame,tmpFrame,LCD_SIZE);
							if(sqX < WIDTH-1) ++sqX; else sqX = 0 ;
							LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);
							display(tmpFrame);
							delay(writingSpeed);
							if(!getInput(GPIOA,4))
							{
								copyArray(tmpFrame,drawFrame,LCD_SIZE);
								display(drawFrame);
							}
						}
					}
					break;
				}
				case configState : 
				{
					extern uint16_t indx;
					uint16_t volatile tmpIndx = indx;
					
					if((currentOption == 1) && sqWidth < WIDTH-1)
					{
						++sqWidth ;
						indx = tmpIndex11;
						invert(configFrame,tmpIndex1,tmpIndex2);
						write(configFrame,"  ");
						indx = tmpIndex11;
						writeInt(configFrame,sqWidth);
						invert(configFrame,tmpIndex1,tmpIndex2);
						display(configFrame);
						updateDrawFrame(tmpFrame,sqX,sqY);
					}
					else if((currentOption == 2) && sqHeight < HEIGHT-1)
					{
						++sqHeight;
						indx = tmpIndex22;
						invert(configFrame,tmpIndex2,tmpIndex3);
						write(configFrame,"  ");
						indx = tmpIndex22;
						writeInt(configFrame,sqHeight);
						invert(configFrame,tmpIndex2,tmpIndex3);
						display(configFrame);
						updateDrawFrame(tmpFrame,sqX,sqY);
					}
					else if(currentOption == 3)
					{
						extern uint16_t indx;
						uint8_t tmp_indx22 = indx;
						indx = tmpIndex33;
						eraseFlag ^= 1;
						invert(configFrame,tmpIndex3,tmpIndex4-1);
						if(eraseFlag) write(configFrame,"  (ON)"); else write(configFrame, " (OFF)");
						invert(configFrame,tmpIndex3,tmpIndex4-1);
						display(configFrame);
					}
					
					break;
				}
		}
		break ;
	}
}
	
	resetPendingBit(1);
}

void EXTI2_IRQHandler(void) //UP
{
	uint32_t volatile counter=0;
	while(1)
	{
		if(getInput(GPIOA,2))
		{
			counter = 0;
			break;
		} else counter++ ;
		if(counter >= 500)
		{
				switch(CurrentState)
				{
					case drawState :
					{
						while(!getInput(GPIOA,2))
						{
							if(eraseFlag)
							{
								copyArray(drawFrame,tmpFrame,LCD_SIZE);
								if(sqY > 0) --sqY; else sqY = HEIGHT-1 ;
								if(isPixelSet(tmpFrame,sqX,sqY) || isPixelSet(tmpFrame,sqX+sqWidth,sqY+sqHeight)) LCDerase(tmpFrame, sqX, sqY, sqWidth, sqHeight);
								else{LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);}
								display(tmpFrame);
								delay(writingSpeed/2);
							}
							else
							{
								copyArray(drawFrame,tmpFrame,LCD_SIZE);
								if(sqY > 0) --sqY; else sqY = HEIGHT-1 ;
								LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);
								display(tmpFrame);
								if(!getInput(GPIOA,4))
								{
								copyArray(tmpFrame,drawFrame,LCD_SIZE);
								display(drawFrame);
								}
								delay(writingSpeed);
							}
						}
						break;
					}
					case configState : 
					{
						switch(currentOption)
						{
							case 1 :
							{
								invert(configFrame, tmpIndex1, tmpIndex2-1);
								invert(configFrame, tmpIndex3, tmpIndex4-1);
								break;
							}
							case 2 :
							{
								invert(configFrame, tmpIndex1, tmpIndex2-1);
								invert(configFrame, tmpIndex2, tmpIndex3-1);
								break;
							}
							case 3 :
							{
								invert(configFrame, tmpIndex2, tmpIndex3-1);
								invert(configFrame, tmpIndex3, tmpIndex4-1);
								break;
							}
						}
						if(currentOption > 1 ) --currentOption; else currentOption= 3;
						display(configFrame);
						break;
					}
				}
			break ;
		}
	}
	
	resetPendingBit(2);
}

void EXTI3_IRQHandler(void) //DOWN
{
	
	uint32_t volatile counter=0;
	while(1)
	{
		if(getInput(GPIOA,3))
		{
			counter = 0;
			break;
		} else counter++ ;
		if(counter >= 5000)
		{
			switch(CurrentState)
			{
				case drawState :
				{
					while(!getInput(GPIOA,3))
					{
						if(eraseFlag)
						{
							copyArray(drawFrame,tmpFrame,LCD_SIZE);
							if(sqY < HEIGHT-1) ++sqY; else sqY = 0 ;
							if(isPixelSet(tmpFrame,sqX,sqY) || isPixelSet(tmpFrame,sqX+sqWidth,sqY+sqHeight)) LCDerase(tmpFrame, sqX, sqY, sqWidth, sqHeight);
							else{LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);}
							display(tmpFrame);
							delay(writingSpeed/2);
						}
						else
						{
							copyArray(drawFrame,tmpFrame,LCD_SIZE);
							if(sqY < HEIGHT-1) ++sqY; else sqY = 0 ;
							LCDsquare(tmpFrame, sqX, sqY, sqWidth, sqHeight);
							display(tmpFrame);
							if(!getInput(GPIOA,4))
							{
								copyArray(tmpFrame,drawFrame,LCD_SIZE);
								display(drawFrame);
							}
							delay(writingSpeed);
						}
					}	
					break;
				}
				case configState : 
				{
					switch(currentOption)
						{
							case 1 :
							{
								invert(configFrame, tmpIndex1, tmpIndex2-1);
								invert(configFrame, tmpIndex2, tmpIndex3-1);
								break;
							}
							case 2 :
							{
								invert(configFrame, tmpIndex3, tmpIndex4-1);
								invert(configFrame, tmpIndex2, tmpIndex3-1);
								break;
							}
							case 3 :
							{
								invert(configFrame, tmpIndex1, tmpIndex2-1);
								invert(configFrame, tmpIndex3, tmpIndex4-1);
								break;
							}
						}
						if(currentOption < numberOfOptions ) ++currentOption; else currentOption= 1;
						display(configFrame);
						break;
				}
			}
			break;
		}
	}
	
	resetPendingBit(3);
}

void EXTI4_IRQHandler(void) //OK
{
	uint32_t volatile counter=0;
	while(1)
	{
		if(getInput(GPIOA,4))
		{
			counter = 0;
			break;
		} else counter++ ;
		if(counter >= 4000)
		{
			switch(CurrentState)
			{
				case drawState :
				{
					if(eraseFlag)
					{
						copyArray(tmpFrame,drawFrame,LCD_SIZE);
						LCDerase(drawFrame, sqX, sqY, sqWidth, sqHeight);
						display(drawFrame);
					}
					else
					{
						copyArray(tmpFrame,drawFrame,LCD_SIZE);
						display(drawFrame);
					}
					break;
				}
				case configState :
				{
					CurrentState = drawState ;
					frameTransition();
					copyArray(drawFrame,tmpFrame,LCD_SIZE);
					updateDrawFrame(tmpFrame,sqX,sqY);
					display(tmpFrame);
					break;
				}
			}
			break;
		}
	}
	resetPendingBit(3);
}

void EXTI9_5_IRQHandler(void) //config
{
	uint32_t volatile counter=0;
	while(1)
	{
		if(getInput(GPIOA,5))
		{
			counter = 0;
			break;
		} else counter++ ;
		
		if(counter >=5000)
		{
			switch(CurrentState)
			{
				case drawState :
				{
					CurrentState = configState;
					frameTransition();
					display(configFrame);
					break;
				}
			}
			break;
		}
	}
	resetPendingBit(6);
	resetPendingBit(7);
}


void animate(uint8_t* frames[],uint8_t nuOfFrames,uint32_t delayTime)
{
	for(uint8_t i = 0 ; i < nuOfFrames;i++)
	{
		display(frames[i]);
		delay(delayTime);
	}
}

void initConfigFrame(uint8_t frame[])
{
	extern uint16_t indx ;
	uint16_t temp_indx = indx;
	indx = 0;
	newLine();
	char string[1];
	sprintf(string,"%d",sqWidth);
	
	
	tmpIndex1 = indx ;
	space(frame); write(frame,"Width = "); tmpIndex11 = indx ;
	write(frame,string) ; newLine();
	
	tmpIndex2 = indx;	
	space(frame); write(frame,"Height = "); tmpIndex22 = indx;
	writeInt(configFrame,sqWidth); newLine();
	tmpIndex3 = indx;
	
	space(frame); write(frame,"Erase"); tmpIndex33 = indx;
	write(frame,"( OFF)");newLine();
	tmpIndex4 = indx;
	
	invert(frame,tmpIndex1,tmpIndex2-1);

	
	indx = temp_indx;
}

void updateDrawFrame(uint8_t frame[],uint8_t xxx , uint8_t yyy)
{
	LCDsquare(frame,xxx,yyy,sqWidth,sqHeight);
}

void frameTransition()
{
	resetFrame(tmpFrame);
	for(uint8_t i =0 ; i < 80 ;i+=5 )
	{
		LCDsquare(tmpFrame,0,0,i,47);
		delay(2000);
		display(tmpFrame);
	}
	resetFrame(tmpFrame);
	for(uint8_t i =80 ; i > 0 ;i-=5 )
	{
		LCDsquare(tmpFrame,i,0,80,47);
		delay(2000);
		display(tmpFrame);
	}
	resetFrame(tmpFrame);
	display(tmpFrame);
}

void initDrawFrame(uint8_t frame[])
{
	LCDsquare(frame,0,0,sqWidth,sqHeight);
}


