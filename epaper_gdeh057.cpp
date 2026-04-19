
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher

/*
 This is the driver for the GDEH0576T81 ePaper display.
 
 The display has a resolution of 920*680 pixels. It is controlled via SPI
 3 wire using a SSD2677 controller. Four levels of gray can be acheived.
 Partial display update is possible.
 */

#if 0


#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_


#define Source_BITS     920
#define Gate_BITS   680
#define ALLSCREEN_BYTES   Source_BITS*Gate_BITS/8


//EPD
void EPD_init(void);
void PIC_display (const unsigned char* picData);
void EPD_sleep(void);
void EPD_update(void);
void lcd_chkstatus(void);

void Display_All_Black(void);
void Display_All_White(void);

void EPD_init_Part(void);
void BLACK_TO_WHITE(void);
void WHITE_TO_BLACK(void);

void WRITE_A_TO_B1(void);
void WRITE_A_TO_B2(void);
void PIC_display_Part_ALL(const unsigned char *bytes1, const unsigned char *bytes2);
#endif
/***********************************************************
						end file
***********************************************************/











#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Arduino.h"

void delay_xms(unsigned int xms)
{
  delay(xms);
}
int Read_temp(void)
{

   unsigned char temp1;	
    EPD_W21_WriteCMD(0x40);
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
    temp1=EPD_W21_ReadDATA();  //temp1£ºD11 D10 D9 D8 D7 D6 D5 D4                         
	return temp1;
}
void Write_LUT_All(void)
{
	int tempvalue;
	int temp;
	temp = Read_temp();
  Serial.print(temp);
	if(temp <= 5)
	{
		tempvalue=232;  // -24
	}
	else if(temp <=10) 
	{
		tempvalue=235;   // -21
	}
	else if(temp <=20)
	{
		tempvalue=238;   // -18
	}
	else if(temp <=30)
	{
		 tempvalue=241;   // -15
	}
	else if(temp <=127)
	{
		tempvalue=244;    // -12
	}
	else
	{
		tempvalue=232;
	}
	
	EPD_W21_WriteCMD(0xE0); 
	EPD_W21_WriteDATA(0x02);
	EPD_W21_WriteCMD(0xE6);  
	EPD_W21_WriteDATA(tempvalue);
	
	EPD_W21_WriteCMD(0xA5);      
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
	delay_xms(10);//At least 10ms delay 	
	
}

void EPD_init(void)
{
	delay_xms(10);//At least 10ms delay 	
	EPD_W21_RST_0;		// Module reset
	delay_xms(10);//At least 10ms delay 
	EPD_W21_RST_1;
	delay_xms(10);//At least 10ms delay 
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

	EPD_W21_WriteCMD(0x00);  //PSR
	EPD_W21_WriteDATA(0x27);
	EPD_W21_WriteDATA(0x0E);
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

	EPD_W21_WriteCMD(0x06);//BTST
	EPD_W21_WriteDATA(0x0F);
	EPD_W21_WriteDATA(0x8B);
	EPD_W21_WriteDATA(0x9C);
	EPD_W21_WriteDATA(0xC1);

	EPD_W21_WriteCMD(0xE7);     //PST
	EPD_W21_WriteDATA(0xC1);

	EPD_W21_WriteCMD(0x30);// frame go with waveform
	EPD_W21_WriteDATA(0x08); 	

	EPD_W21_WriteCMD(0x50);//CDI
	EPD_W21_WriteDATA(0x77);  //border white

	EPD_W21_WriteCMD(0x61);//0x61	
	EPD_W21_WriteDATA(Source_BITS/256);	
	EPD_W21_WriteDATA(Source_BITS%256);	
	EPD_W21_WriteDATA(Gate_BITS/256);	
	EPD_W21_WriteDATA(Gate_BITS%256);	

	EPD_W21_WriteCMD(0x62);  // HTOTAL       
	EPD_W21_WriteDATA(0x98);                                                      	
	EPD_W21_WriteDATA(0x98);
	EPD_W21_WriteDATA(0x98);                                                      	
	EPD_W21_WriteDATA(0x75);
	EPD_W21_WriteDATA(0xCA);                                                      	
	EPD_W21_WriteDATA(0xB2); 
	EPD_W21_WriteDATA(0x98);                                                      	
	EPD_W21_WriteDATA(0x7E); 

	EPD_W21_WriteCMD(0x65);   //GSST
	EPD_W21_WriteDATA(0x00);  
	EPD_W21_WriteDATA(0x00);
	EPD_W21_WriteDATA(0x00);  
	EPD_W21_WriteDATA(0x00);

	EPD_W21_WriteCMD(0xE9);     //PST
	EPD_W21_WriteDATA(0x01);

	Write_LUT_All();
	
	EPD_W21_WriteCMD(0x04); //Power on
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
	

}
void EPD_sleep(void)
{  	
	EPD_W21_WriteCMD(0X02);  	//power off
	EPD_W21_WriteDATA(0x00);
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
   
	EPD_W21_WriteCMD(0X07);  	//deep sleep
	EPD_W21_WriteDATA(0xA5);
}
void EPD_update(void)
{   
  EPD_W21_WriteCMD(0x12); //Display Update Control
	EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();   
}

void lcd_chkstatus(void)
{ 
  while(1)
  {	 //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
  }  
}


void Display_All_Black(void)
{
  unsigned long i; 

  EPD_W21_WriteCMD(0x10);
	lcd_chkstatus();
	for(i=0;i<ALLSCREEN_BYTES;i++)
	{
		EPD_W21_WriteDATA(0x00);
	}   
  EPD_update();	
	
}

void Display_All_White(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
	lcd_chkstatus();
	for(i=0;i<ALLSCREEN_BYTES*2;i++)
	{
		EPD_W21_WriteDATA(0xFF);
	}
  
	 EPD_update();	
}

void EPD_W21_WriteDATA_1To2(const unsigned char* picData)
{
  int i,k;
	char temp1,temp2,temp3;
	for(i=0;i<ALLSCREEN_BYTES;i++)
	{ 
		temp3=0;
			temp1 = picData[i];  //1bit
			for(k=0;k<4;k++)
			{
			temp2=temp1&0x80;
			if(temp2==0x80)
				temp3 |= 0x03;//white
			else if(temp2==0x00)
				temp3 |= 0x00;  //black
			if(k<=2)
			{
			temp3 <<= 2;	
			}	
			temp1 <<= 1;			
		 }
		 EPD_W21_WriteDATA(temp3);	
		temp3=0;
			for(k=0;k<4;k++)
			{
			temp2 = temp1&0x80;
			if(temp2==0x80)
				temp3 |= 0x03;//white
			else if(temp2==0x00)
				temp3 |= 0x00;  //black
			if(k<=2)
			{
			temp3 <<= 2;	
			}	
			temp1 <<= 1;			
		 }
			EPD_W21_WriteDATA(temp3);				 
  }
}


void PIC_display(const unsigned char* picData)
{ 
  EPD_W21_WriteCMD(0x10);
	EPD_W21_WriteDATA_1To2(picData);
	//update
  EPD_update();	
}


void EPD_init_Part(void)
{
  delay_xms(10);//At least 10ms delay 	
	EPD_W21_RST_0;		// Module reset
	delay_xms(10);//At least 10ms delay 
	EPD_W21_RST_1;
	delay_xms(10);//At least 10ms delay 
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

	EPD_W21_WriteCMD(0x00);  //PSR
	EPD_W21_WriteDATA(0x27);
	EPD_W21_WriteDATA(0x0E);
	lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

	EPD_W21_WriteCMD(0x06);//BTST
	EPD_W21_WriteDATA(0x0F);
	EPD_W21_WriteDATA(0x8B);
	EPD_W21_WriteDATA(0x9C);
	EPD_W21_WriteDATA(0xC1);

	EPD_W21_WriteCMD(0xE7);     //PST
	EPD_W21_WriteDATA(0xC1);

	EPD_W21_WriteCMD(0x30);// frame go with waveform
	EPD_W21_WriteDATA(0x08); 	

	EPD_W21_WriteCMD(0x50);//CDI
	EPD_W21_WriteDATA(0x77);  //border white

	EPD_W21_WriteCMD(0x61);//0x61	
	EPD_W21_WriteDATA(Source_BITS/256);	
	EPD_W21_WriteDATA(Source_BITS%256);	
	EPD_W21_WriteDATA(Gate_BITS/256);	
	EPD_W21_WriteDATA(Gate_BITS%256);	

	EPD_W21_WriteCMD(0x62);  // HTOTAL       
	EPD_W21_WriteDATA(0x98);                                                      	
	EPD_W21_WriteDATA(0x98);
	EPD_W21_WriteDATA(0x98);                                                      	
	EPD_W21_WriteDATA(0x75);
	EPD_W21_WriteDATA(0xCA);                                                      	
	EPD_W21_WriteDATA(0xB2); 
	EPD_W21_WriteDATA(0x98);                                                      	
	EPD_W21_WriteDATA(0x7E); 

	EPD_W21_WriteCMD(0x65);   //GSST
	EPD_W21_WriteDATA(0x00);  
	EPD_W21_WriteDATA(0x00);
	EPD_W21_WriteDATA(0x00);  
	EPD_W21_WriteDATA(0x00);

	EPD_W21_WriteCMD(0xE9);     //PST
	EPD_W21_WriteDATA(0x01);
	
	//Partial OTP
	 EPD_W21_WriteCMD(0xE0); 
	 EPD_W21_WriteDATA(0x00);
	 EPD_W21_WriteCMD(0xA5);      
	 lcd_chkstatus();   
	 
	 EPD_W21_WriteCMD(0x04); //Power on
	 lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
				 
}
void BLACK_TO_WHITE(void)
{
  unsigned int i,j,pcnt,pcnt1;
  
 
 
   
	EPD_W21_WriteCMD(0x10);   // DTM1 Write
lcd_chkstatus();  
                
  pcnt = 0;
 
  for(i=0; i <680; i++)							
  {
    
          for(j=0; j<115; j++)						
          {
            
              EPD_W21_WriteDATA(0xFF);
              EPD_W21_WriteDATA(0x00);
							
            
            pcnt++;
           
           
          }     
  }
  EPD_update();
 

}

void WHITE_TO_BLACK(void)
{
  unsigned int i,j,pcnt,pcnt1;
  
 
 
   
	EPD_W21_WriteCMD(0x10);   // DTM1 Write
lcd_chkstatus();  
                
  pcnt = 0;
 
  for(i=0; i <680; i++)							
  {
    
          for(j=0; j<115; j++)						
          {
            
              EPD_W21_WriteDATA(0x00);
              EPD_W21_WriteDATA(0xFF);							
            
            pcnt++;
           
           
          }     
  }
  EPD_update();
 

}

void bitInterleave(const unsigned char bytes1, const unsigned char bytes2)
	{
   int i; 
    unsigned short result=0; 	
    for (i = 0; i < 8; i++) {
      
           
        result |= ((bytes1 >> (7 - i)) & 1) << (2 * (7-i)+1);
        result |= ((bytes2 >> (7 - i)) & 1) << (2 * (7-i));
              
        if(i == 3)
          EPD_W21_WriteDATA(result >> 8);
   
        
        if(i == 7)
          EPD_W21_WriteDATA(result);  
    }  
}
void PIC_display_Part_ALL(const unsigned char *bytes1, const unsigned char *bytes2)
{
	unsigned int i,j,pcnt,pcnt1;

	EPD_W21_WriteCMD(0x10);   // DTM1 Write
	lcd_chkstatus(); 
					
	pcnt = 0;

	for(i=0; i <680; i++)							
	{

		for(j=0; j<115; j++)						
		{
			bitInterleave(bytes1[pcnt],bytes2[pcnt]); 
			pcnt++;						
		}     
	}
	EPD_update();

}

/***********************************************************
						end file
***********************************************************/

#endif
