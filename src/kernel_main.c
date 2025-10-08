#include "rprintf.h"
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6
const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port);
    


//Already two bytes long
struct termbuf {
      char ascii;
     char color;
};

int x = 0;
int y = 0;
void putc(int data){
	struct termbuf *vram = (struct termbuf*)0xB8000;
    //This helps us check if it is a new line. 
    //Once it checks if it is a newline.
    //Then we make x start back at the begging and
    //We bring y down by one.
  if (data == '\n') {
        x = 0;
        y++;
       //But it checks that if  y gets past the screen
        if (y >= 25){
	//Then in calls the scroll to move the conent
            scroll();
            y = 24;
	//And make y = 24 so it can print at the bottom space.
        }
        return;
    }
	//We want to check if our x is greater then 80
	 if (x >= 80){
	//Then we restart x
	     x = 0;
	//And incramnet y to go to the next line 
             y++;
    	}
	//We check if y is greater then y
	if(y>=25){
	//call the scroll function
	scroll();
	//Then we stop becuase its full
         y = 24;	   
	}
	//Helps us track where we can the charachter and we dont
	//have to multiply by to because termbuf already helps us
	//with the 2 byts 
        int offset = y * 80 + x;
	vram[offset].ascii = data;
	vram[offset].color = 7;
	x++;

}

void scroll(){
    struct termbuf* vram = (struct termbuf*)0xB8000;

	//This for loops job is to make sure that when there 
	//is no space it will shift the content.
    for (int y = 1; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            vram[(y - 1)  * 80 + x] = vram[y * 80 + x];
        }
    }

	//What this will do will that it will clear 
	//the very last row to make space
    for (int x = 0; x < 80; x++) {
        vram[(25 - 1) * 80 + x].ascii = ' ';
        vram[(25 - 1) * 80 + x].color = 7;
    }

	
}



void main(){ 

     // These are helper that helps us enable the interrupts for the platoform
    remap_pic();  //This help set the interrupt controller
    load_gdt();   //This load thr global descripter table
    init_idt();   // Initialize the interrupt descriptor table
    asm("sti");   // Enable interrupts
   
    
   
 
    while(1) {
        asm("hlt");  // Halt until next interrupt
    }
}


