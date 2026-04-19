
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher

/*
 This is the driver for the GDEY058T81 ePaper display.
 
 The display has a resolution of 648*480 pixels. It is controlled via SPI
 3 wire using a SSD1677 controller. Four levels of gray can be acheived.
 Partial display update is possible.
 */

#if 0


 #ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_


#define EPD_WIDTH   648 
#define EPD_HEIGHT  480
#define EPD_ARRAY  EPD_WIDTH*EPD_HEIGHT/8  


//Full screen update display
void EPD_Init(void); 
void EPD_Init_180(void);	
void EPD_WhiteScreen_ALL(const unsigned char *datas);
void EPD_WhiteScreen_White(void);
void EPD_WhiteScreen_Black(void);
void EPD_DeepSleep(void);
//Partial update display 
void EPD_Init_Part(void);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_PartAll(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_Time(unsigned int x_start,unsigned int y_start,
	                      const unsigned char * datas_A,const unsigned char * datas_B,
												const unsigned char * datas_C,const unsigned char * datas_D,const unsigned char * datas_E,
                        unsigned char num,unsigned int PART_COLUMN,unsigned int PART_LINE);										 
//Fast update display
void EPD_Init_Fast(void);
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas);   
//4 Gray
void EPD_Init_4G(void);
void EPD_WhiteScreen_ALL_4G(const unsigned char *datas);		
void Display_4Level_Gray(void);
//GUI display					 
void EPD_Display(unsigned char *Image);	
												
#endif





#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

unsigned char PartImage[1000];//Define Partial canvas space  
//Delay Functions
void delay_xms(unsigned int xms)
{
  delay(xms);
}

////////////////////////////////////E-paper demo//////////////////////////////////////////////////////////
//Busy function
#ifdef ESP8266
void lcd_chkstatus(void)
{ 
  while(1)
  {  //=0 BUSY
     if(isEPD_W21_BUSY==1) break;  
     ESP.wdtFeed(); //Feed dog to prevent system reset  
  }  
}  
#endif
#ifdef Arduino_UNO
void lcd_chkstatus(void)
{ 
  while(1)
  {  //=0 BUSY
     if(isEPD_W21_BUSY==1) break;    
  }  
}  
#endif
//Full screen refresh initialization
void EPD_Init(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  EPD_W21_WriteCMD(0x00);
  EPD_W21_WriteDATA(0x1F);
  
  EPD_W21_WriteCMD(0x04); //POWER ON
  delay(300);  
  lcd_chkstatus();
  EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
  EPD_W21_WriteDATA(0x21);
  EPD_W21_WriteDATA(0x07);
  
}
//Fast refresh 1 initialization
void EPD_Init_Fast(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  EPD_W21_WriteCMD(0x00);
  EPD_W21_WriteDATA(0x1F); 

  EPD_W21_WriteCMD(0x04); //POWER ON
  delay(300);  
  lcd_chkstatus();
  EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
  EPD_W21_WriteDATA(0x29);
  EPD_W21_WriteDATA(0x07);
  
  EPD_W21_WriteCMD(0xE0);
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteCMD(0xE5);
  EPD_W21_WriteDATA(0x5A); 
}
//Partial refresh  initialization
void EPD_Init_Part(void)
{ 
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 

  EPD_W21_WriteCMD(0x00);
  EPD_W21_WriteDATA(0x1F);

  EPD_W21_WriteCMD(0x04); //POWER ON
  delay(300);  
  lcd_chkstatus();

  EPD_W21_WriteCMD(0xE0);
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteCMD(0xE5);
  EPD_W21_WriteDATA(0x6E);

}

//////////////////////////////Display Update Function///////////////////////////////////////////////////////
//Full screen refresh update function
void EPD_Update(void)
{   
  //Refresh
  EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
  delay(1);              //!!!The delay here is necessary, 200uS at least!!!     
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
}

//////////////////////////////Display Data Transfer Function////////////////////////////////////////////
//Full screen refresh display function
void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
   unsigned int i;  
  EPD_W21_WriteCMD(0x10);  //write old data 
  for(i=0;i<EPD_ARRAY;i++)
   {               
     EPD_W21_WriteDATA(0xff);
   }
  EPD_W21_WriteCMD(0x13);  //write new data 
  for(i=0;i<EPD_ARRAY;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
   }   
   EPD_Update();   
}
//Fast refresh display function
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas)
{
  unsigned int i; 
  EPD_W21_WriteCMD(0x10);  //write old data 
  for(i=0;i<EPD_ARRAY;i++)
   {               
     EPD_W21_WriteDATA(0xff);
   }
  EPD_W21_WriteCMD(0x13);  //write new data 
  for(i=0;i<EPD_ARRAY;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
   }   
   EPD_Update();     
}

//Clear screen display
void EPD_WhiteScreen_White(void)
{
  unsigned int i;
  //Write Data
  EPD_W21_WriteCMD(0x10);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    EPD_W21_WriteDATA(0xff);  
  }
  EPD_W21_WriteCMD(0x13);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    EPD_W21_WriteDATA(0xff);  
  }
   EPD_Update();   
}
//Display all black
void EPD_WhiteScreen_Black(void)
{
  unsigned int i;
  //Write Data
  EPD_W21_WriteCMD(0x10);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    EPD_W21_WriteDATA(0xff);  
  }
  EPD_W21_WriteCMD(0x13);      
  for(i=0;i<EPD_ARRAY;i++)       
  {
    EPD_W21_WriteDATA(0x00);  
  }
   EPD_Update();  
}
//Partial refresh of background display, this function is necessary, please do not delete it!!!
void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i; 
  EPD_W21_WriteCMD(0x10);  //write old data 
  for(i=0;i<EPD_ARRAY;i++)
   {               
     EPD_W21_WriteDATA(0xff);
   }
  EPD_W21_WriteCMD(0x13);  //write new data 
  for(i=0;i<EPD_ARRAY;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
   }   
   EPD_Update();     
   
}
//Partial refresh display
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i;  
  unsigned int x_end,y_end;

  x_end=x_start+PART_LINE-1; 
  y_end=y_start+PART_COLUMN-1;

  EPD_W21_WriteCMD(0x50);
  EPD_W21_WriteDATA(0xA9);
  EPD_W21_WriteDATA(0x07);

  EPD_W21_WriteCMD(0x91);   //This command makes the display enter partial mode
  EPD_W21_WriteCMD(0x90);   //resolution setting
  EPD_W21_WriteDATA (x_start/256);
  EPD_W21_WriteDATA (x_start%256);   //x-start    

  EPD_W21_WriteDATA (x_end/256);    
  EPD_W21_WriteDATA (x_end%256-1);  //x-end 

  EPD_W21_WriteDATA (y_start/256);  //
  EPD_W21_WriteDATA (y_start%256);   //y-start    

  EPD_W21_WriteDATA (y_end/256);    
  EPD_W21_WriteDATA (y_end%256-1);  //y-end
  EPD_W21_WriteDATA (0x01); 

  EPD_W21_WriteCMD(0x13);        //writes New data to SRAM.
  for(i=0;i<PART_COLUMN*PART_LINE/8;i++)       
   {
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));  
    
   }  
    EPD_Update(); 

}
//Full screen partial refresh display
void EPD_Dis_PartAll(const unsigned char * datas)
{
  unsigned int i;  
  unsigned int x_start=0,y_start=0,x_end,y_end;
  unsigned int PART_COLUMN=EPD_HEIGHT,PART_LINE=EPD_WIDTH;

  x_end=x_start+PART_LINE-1; 
  y_end=y_start+PART_COLUMN-1;

  EPD_W21_WriteCMD(0x50);
  EPD_W21_WriteDATA(0xA9);
  EPD_W21_WriteDATA(0x07);

  EPD_W21_WriteCMD(0x91);   //This command makes the display enter partial mode
  EPD_W21_WriteCMD(0x90);   //resolution setting
  EPD_W21_WriteDATA (x_start/256);
  EPD_W21_WriteDATA (x_start%256);   //x-start    

  EPD_W21_WriteDATA (x_end/256);    
  EPD_W21_WriteDATA (x_end%256-1);  //x-end 

  EPD_W21_WriteDATA (y_start/256);  //
  EPD_W21_WriteDATA (y_start%256);   //y-start    

  EPD_W21_WriteDATA (y_end/256);    
  EPD_W21_WriteDATA (y_end%256-1);  //y-end
  EPD_W21_WriteDATA (0x01); 

  EPD_W21_WriteCMD(0x13);        //writes New data to SRAM.
  for(i=0;i<PART_COLUMN*PART_LINE/8;i++)       
   {
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));   
   }  
    EPD_Update(); 

}
//Deep sleep function
void EPD_DeepSleep(void)
{   
    EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING     
    EPD_W21_WriteDATA(0xf7); //WBmode:VBDF 17|D7 VBDW 97 VBDB 57    WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7  

    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
    delay(100);   //!!!The delay here is necessary, 200uS at least!!!       
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}

//Partial refresh write address and data
void EPD_Dis_Part_RAM(unsigned int x_start,unsigned int y_start,
                        const unsigned char * datas_A,const unsigned char * datas_B,
                        const unsigned char * datas_C,const unsigned char * datas_D,const unsigned char * datas_E,
                        unsigned char num,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i,j;  
  unsigned int x_end,y_end;

  x_end=x_start+PART_LINE*num-1; 
  y_end=y_start+PART_COLUMN-1;

  EPD_W21_WriteCMD(0x50);
  EPD_W21_WriteDATA(0xA9);
  EPD_W21_WriteDATA(0x07);

  EPD_W21_WriteCMD(0x91);   //This command makes the display enter partial mode
  EPD_W21_WriteCMD(0x90);   //resolution setting
  EPD_W21_WriteDATA (x_start/256);
  EPD_W21_WriteDATA (x_start%256);   //x-start    

  EPD_W21_WriteDATA (x_end/256);    
  EPD_W21_WriteDATA (x_end%256-1);  //x-end 

  EPD_W21_WriteDATA (y_start/256);  //
  EPD_W21_WriteDATA (y_start%256);   //y-start    

  EPD_W21_WriteDATA (y_end/256);    
  EPD_W21_WriteDATA (y_end%256-1);  //y-end
  EPD_W21_WriteDATA (0x01); 

  EPD_W21_WriteCMD(0x13);        //writes New data to SRAM.
  for(i=0;i<PART_COLUMN;i++)       
   {
      for(j=0;j<PART_LINE/8;j++)       
        EPD_W21_WriteDATA(pgm_read_byte(&datas_A[i*PART_LINE/8+j]));    
      for(j=0;j<PART_LINE/8;j++)       
        EPD_W21_WriteDATA(pgm_read_byte(&datas_B[i*PART_LINE/8+j]));  
      for(j=0;j<PART_LINE/8;j++)       
        EPD_W21_WriteDATA(pgm_read_byte(&datas_C[i*PART_LINE/8+j]));    
      for(j=0;j<PART_LINE/8;j++)       
        EPD_W21_WriteDATA(pgm_read_byte(&datas_D[i*PART_LINE/8+j]));  
      for(j=0;j<PART_LINE/8;j++)       
        EPD_W21_WriteDATA(pgm_read_byte(&datas_E[i*PART_LINE/8+j]));       
   }  
    
}
//Clock display
void EPD_Dis_Part_Time(unsigned int x_start,unsigned int y_start,
                        const unsigned char * datas_A,const unsigned char * datas_B,
                        const unsigned char * datas_C,const unsigned char * datas_D,const unsigned char * datas_E,
                        unsigned char num,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  EPD_Dis_Part_RAM(x_start,y_start,datas_A,datas_B,datas_C,datas_D,datas_E,num,PART_COLUMN,PART_LINE);
  EPD_Update();
}                        


////////////////////////////////Other newly added functions////////////////////////////////////////////
//Display rotation 180 degrees initialization
void EPD_Init_180(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  EPD_W21_WriteCMD(0x00);
  EPD_W21_WriteDATA(0x13);
  
  EPD_W21_WriteCMD(0x04); //POWER ON
  delay(300);  
  lcd_chkstatus();
  EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
  EPD_W21_WriteDATA(0x21);
  EPD_W21_WriteDATA(0x07);
}


// 4 Gray display
void EPD_Init_4G(void) 
{	
	EPD_W21_RST_0;  // Module reset   
	delay_xms(10);//At least 10ms delay 
	EPD_W21_RST_1;
	delay_xms(10); //At least 10ms delay 
  
	EPD_W21_WriteCMD(0x00);
	EPD_W21_WriteDATA(0x1F); 

	EPD_W21_WriteCMD(0x04); //POWER ON
	delay_xms(300);  
	lcd_chkstatus();
	EPD_W21_WriteCMD(0X50);			//VCOM AND DATA INTERVAL SETTING
	EPD_W21_WriteDATA(0x21);
	EPD_W21_WriteDATA(0x07);
	
	EPD_W21_WriteCMD(0xE0);
	EPD_W21_WriteDATA(0x02);
	EPD_W21_WriteCMD(0xE5);
	EPD_W21_WriteDATA(0x5F); //0x5A--1.5s, 0x5F--4 Gray
	
}	

void EPD_WhiteScreen_ALL_4G(const unsigned char *datas)
{
	unsigned int i,j,k;
	unsigned char temp1,temp2,temp3;

	  //old  data
		EPD_W21_WriteCMD(0x10);	       

		for(i=0;i<38880;i++)	               //38880*2  648*480
		{ 
			temp3=0;
      for(j=0;j<2;j++)	
			{
				temp1 = pgm_read_byte(&datas[i*2+j]);
				for(k=0;k<4;k++)
				{
					temp2 = temp1&0xC0 ;
					if(temp2 == 0xC0)
						temp3 |= 0x01;//white
					else if(temp2 == 0x00)
						temp3 |= 0x00;  //black
					else if((temp2>=0x80)&&(temp2<0xC0)) 
						temp3 |= 0x00;  //gray1
					else if(temp2 == 0x40)
						temp3 |= 0x01; //gray2
        if((j==0&&k<=3)||(j==1&&k<=2))
				{
				temp3 <<= 1;	
				temp1 <<= 2;
				}
			 }
		 }	
     EPD_W21_WriteDATA(temp3);			
		}

    //new  data
		EPD_W21_WriteCMD(0x13);	       
		for(i=0;i<38880*2;i++)	               //38880*2   648*480
		{ 
			temp3=0;
      for(j=0;j<2;j++)	
			{
				temp1 = pgm_read_byte(&datas[i*2+j]);
				for(k=0;k<4;k++)
				{
					temp2 = temp1&0xC0 ;
					if(temp2 == 0xC0)
						temp3 |= 0x01;//white
					else if(temp2 == 0x00)
						temp3 |= 0x00;  //black
					else if((temp2>=0x80)&&(temp2<0xC0)) 
						temp3 |= 0x01;  //gray1
					else if(temp2 == 0x40)
						temp3 |= 0x00; //gray2
       
        if((j==0&&k<=3)||(j==1&&k<=2))
				{
				temp3 <<= 1;	
				temp1 <<= 2;
				}
			 }	
		 }
			EPD_W21_WriteDATA(temp3);			
	 }

   EPD_Update();
   
}
/*
Black 00  00
Gray1 00  01
Gray2 01  00
White 01  01 
*/
void Display_4Level_Gray(void)
{
	unsigned int i,j; 

	lcd_chkstatus();
	EPD_W21_WriteCMD(0x10);   
  for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0x00); //Black
		}
  	
 for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0xFF); //Gray 1
		}
		
		for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0x00); //Gray 2
		}
		
		for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0xFF);//White
		}
			
	lcd_chkstatus();
	EPD_W21_WriteCMD(0x13);   
	for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0x00);
		}
  	
 for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0x00);
		}
		
		for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0xFF);
		}
		
		for(i=0;i<648*480/16/2;i++)
		{
				EPD_W21_WriteDATA(0xFF);
		}

		EPD_Update();
	}
/***********************************************************
            end file
***********************************************************/

#endif
