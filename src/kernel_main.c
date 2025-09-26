
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}
//Already two bytes long
struct termbuf {
        char ascii;
        char color;
};

int x = 0;
int y = 0;
void putc(int data){
        struct termbuf *vram = (struct termbuf*)0xB8000;
        //We want to check if our x is greater then 80
         if (x >= 80){
        //Then we restart x
             x = 0;
        //And incramnet y to go to the next line 
             y++;
        }
        //We check if y is greater then y
        if(y >=25){
        //call the scroll function
        scroll();
        //Then we stop becuase its full
           return;
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


void main() {
   printf(putc,"Hello, World!\n");
   for(int i = 0; i < 30; i++){
        printf("Hello, World!\n");
    }
    while(1) {
        uint8_t status = inb(0x64);

        if(status & 1) {
            uint8_t scancode = inb(0x60);
        }
    }
}


