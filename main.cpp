
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Matthias Melcher


#if 0

//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  

void setup() {
   pinMode(A14, INPUT);  //BUSY
   pinMode(A15, OUTPUT); //RES 
   pinMode(A16, OUTPUT); //DC   
   pinMode(A17, OUTPUT); //CS   
   pinMode(18, OUTPUT); //SCL   
   pinMode(23, OUTPUT); //SDA   

   Serial.begin(115200);
}

//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial update.
3.Please make sue that EPD enters sleep mode when update is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/
void loop() {
   unsigned char i;

#if 1 //Full screen update, partial update demostration.
		/************Full display*******************/
		EPD_init(); //Full screen update initialization.
		PIC_display(gImage_1);//To Display one image using full screen update.
		EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
		delay(1000); //Delay for 1s.
    EPD_init(); //Full screen update initialization.
		PIC_display(gImage_2);//To Display one image using full screen update.
		EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
		delay(1000); //Delay for 1s.
    EPD_init(); //Full screen update initialization.
		PIC_display(gImage_3);//To Display one image using full screen update.
		EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(1000); //Delay for 1s.       
  #if 1 //Partial update demostration.
  //Partial update demo support displaying a clock at 5 locations with 00:00.  If you need to perform partial update more than 5 locations, please use the feature of using partial update at the full screen demo.
  //After 5 partial updatees, implement a full screen update to clear the ghosting caused by partial updatees.

		/************Partial display*******************/
		EPD_init(); //Full screen update initialization.
		PIC_display(gImage_p1);//To Display one image using full screen update.
		EPD_init_Part();
		PIC_display_Part_ALL(gImage_p1, gImage_p2);
		PIC_display_Part_ALL(gImage_p2, gImage_p3);
		PIC_display_Part_ALL(gImage_p3, gImage_3);
		EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.	
  #endif

    delay(3000);//Delay for 3s.
		EPD_init(); //Full screen update initialization.
		Display_All_White();
		EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
#endif
    while(1); // The program stops here
}




//////////////////////////////////END//////////////////////////////////////////////////


#endif


