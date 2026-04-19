


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


