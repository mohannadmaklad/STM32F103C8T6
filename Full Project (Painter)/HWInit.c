/*-------------- Hardware Drivers--------------*/

#include "stm32f10x.h" 
#include "stdint.h"
#include "GPIO.h"
#include "interrupt.h"

void CLK_init(void)
{		
	RCC -> CFGR &= ~((uint32_t) 0xffff);      //RESET
	RCC -> CFGR |= (uint32_t) 0x1  << 16 ;    //PLLSRC ==1, HSE is selected = 8MHZ
	RCC -> CFGR |= (uint32_t) 0x7  << 18 ;    //PLLMUL ==7, 8*9 =72MHZ
	RCC -> CFGR |= (uint32_t) 0x5  <<  8 ;    //APB1 Clock divided/2 = 36MHZ
	RCC -> CFGR |= ((uint32_t) 0x2 <<  0);    //PLL is used	 
}
//////
void RCC_init(void)
{
	RCC->APB2ENR |= ((uint32_t)0x1)<<0;  //Enable AFIO pin
	RCC->APB2ENR |= ((uint32_t)0x3)<<2;  //Enable ports A,B
	RCC->APB1ENR |= ((uint32_t)0x1)<<14; //ENABLE SPI2
	RCC->CFGR    &= ~((uint16_t)0x3 << 14); // Reset ADC prescaler
	RCC->CFGR    |=  ((uint16_t)0x2 << 14); // Set ADC prescaler to 6 (72MHZ/6 = 12 MHZ)
	RCC->APB2ENR |=  ((uint16_t)0x1 << 2 ); // Enable GPIOA
	RCC->APB2ENR |=  ((uint16_t)0x1 << 9 ); // Enable ADC1 Clock
}

void GPIO_init(void)
{
	GPIOA->CRL &= ~((uint32_t)0xf << 4); //PIN A1 : MODE : INPIT - CNF ANALOG
	
	GPIOB->CRH &= ~(((uint32_t) 0xf)  << 16) ;   //PB12 OUTPUT - AFIO - PUSH PULL
	GPIOB->CRH |=  ((uint32_t)  0x9)  << 16  ;   //PB12 OUTPUT - AFIO - PUSH PULL      //////NSS//////
	
	GPIOB->CRH &= ~(((uint32_t) 0xf)  << 20) ;   //PB13 OUTPUT - AFIO - PUSH PULL
	GPIOB->CRH |=  ((uint32_t)  0x9)  << 20  ;   //PB13 OUTPUT - AFIO - PUSH PULL      //////SCK//////
  
	GPIOB->CRH &= ~(((uint32_t)  0xf) << 24) ;   //PB14 OUTPUT - AFIO - PUSH PULL
	GPIOB->CRH |=  ((uint32_t)  0x9)  << 24  ;   //PB15 OUTPUT - AFIO - PUSH PULL      //////MISO//////
	
	GPIOB->CRH &= ~(((uint32_t) 0xf)  << 28) ;   //PB15 OUTPUT - AFIO - PUSH PULL
	GPIOB->CRH |=  ((uint32_t)  0x9)  << 28  ;   //PB15 OUTPUT - AFIO - PUSH PULL      //////MOSI//////

	GPIOA->CRH &= ~(((uint32_t) 0xf)  << 0) ;    //PA8 OUTPUT - GENERAL PURPUSE - PUSH PULL
	GPIOA->CRH |=  (((uint32_t) 0x2)  << 0) ;    //PA8 OUTPUT - GENERAL PURPUSE - PUSH PULL
	
	GPIOA->CRH &= ~(((uint32_t) 0xf)  << 4) ;    //PA9 OUTPUT - GENERAL PURPUSE - PUSH PULL
	GPIOA->CRH |=  (((uint32_t) 0x2)  << 4) ;    //PA9 OUTPUT - GENERAL PURPUSE - PUSH PULL
	
	
	/*----------------- PC13 - LED is Active low----------------------*/
	RCC->APB2ENR |= ((uint32_t) 0x1)  << 4 ;
	resetGpioH(GPIOC,13);
	GPIOC->CRH |=  (((uint32_t) 0x2)  << ((13%8)*4)) ; //pin 13 - output/ push pull
	GPIOC->ODR &= ~((uint32_t)0x1 << 13) ; //low ouput
	
	/*--------------------------- PC13--------------------------------*/
	// pins A6, A7, A8, A9
	/*--------------------------Interrupt pins -----------------------*/
		resetGpioL(GPIOA,0); resetGpioL(GPIOA,1); resetGpioL(GPIOA,2);
		resetGpioL(GPIOA,3); resetGpioL(GPIOA,4); resetGpioL(GPIOA,5);
		
		confGpioL(GPIOA,0,pulledInput); confGpioL(GPIOA,1,pulledInput);
		confGpioL(GPIOA,2,pulledInput); confGpioL(GPIOA,3,pulledInput);
		confGpioL(GPIOA,4,pulledInput); confGpioL(GPIOA,5,pulledInput);
		
		setPin(GPIOA,0); setPin(GPIOA,1); setPin(GPIOA,2); //PULLED UP
		setPin(GPIOA,3); setPin(GPIOA,4); setPin(GPIOA,5); 

}

void SPI_init(void)
{ 
	SPI2->CR1  =  0x0000;
	SPI2->CR2 |= ((uint16_t)0x1)<<2;
	
	SPI2->CR1 |= ((uint16_t)0x01)<<4;    //divide by 16
	
	SPI2->CR1 |= ((uint16_t)0x1)<<2;    // Master mode
	SPI2->CR1 |= ((uint16_t)0x1)<<15;   // BIDIRECTION
 	SPI2->CR1 |= ((uint16_t)0x1)<<14;   // TRANSMIT ONLY
	SPI2->CR1 |= ((uint16_t)0x1)<<6;    // SPI1 enable
}

void EXTI_init(void)
{
	
	unmaskEXTI(0); unmaskEXTI(1); unmaskEXTI(2);
	unmaskEXTI(3); unmaskEXTI(4); unmaskEXTI(5); 
	
	fallingEdgeEXTI(0); fallingEdgeEXTI(1); fallingEdgeEXTI(2);
	fallingEdgeEXTI(3); fallingEdgeEXTI(4); fallingEdgeEXTI(5);
}

