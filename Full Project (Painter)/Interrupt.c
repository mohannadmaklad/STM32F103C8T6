void EXTI0_IRQHandler(void)
{
	uint32_t volatile counter = 0;
	while(1)
	{
		if(getInput(GPIOB,0))
		{
			counter = 0;
			break;
		} else counter++ ;
		if(counter >= 5000)
		{
			copyArray(&LCD,&tmpLCD,LCD_SIZE); 
			LCDsquare(&tmpLCD,displayX,displayY,sqWidth,sqHeight);
			if(displayX<WIDTH && displayY<HEIGHT)
			{
				displayX++;displayY++;
			}
			else
			{
				if(displayX >= WIDTH)  displayX = 0;
				if(displayY >= HEIGHT) displayY = 0;
			}
			display(&tmpLCD);
			break;
		}
	}
	
	resetPendingBit(0);
}

void EXTI1_IRQHandler(void)
{
	
	uint32_t volatile counter=0;
	while(1)
	{
		if(getInput(GPIOB,1))
		{
			counter = 0;
			break;
		} else counter++ ;
	if(counter >= 5000)
		{
			copyArray(&tmpLCD,&LCD,LCD_SIZE);
			display(&LCD);
			break;
		}
	}
	
	resetPendingBit(1);
}


void EXTI9_5_IRQHandler(void)
{
	uint32_t volatile counter=0;
	while(1)
	{
		if(getInput(GPIOA,6)&&getInput(GPIOA,7))
		{
			counter = 0;
			break;
		} else counter++ ;
		
		if(counter >=4500)
		{
			copyArray(&LCD,&tmpLCD,LCD_SIZE); 
			if(sqWidth < 5 && sqHeight < 5){sqWidth++;sqHeight++;}
			else{sqWidth = 2; sqHeight =2;}
			LCDsquare(&tmpLCD,displayX,displayY,sqWidth,sqHeight);
			display(&tmpLCD);
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