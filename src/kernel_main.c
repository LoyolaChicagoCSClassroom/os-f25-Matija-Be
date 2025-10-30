#include "rprintf.h"
#include <stdint.h>
#include "page.h"
#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6
const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port);

struct page_directory_entry
{
   uint32_t present       : 1;   // Page present in memory
   uint32_t rw            : 1;   // Read-only if clear, R/W if set
   uint32_t user          : 1;   // Supervisor only if clear
   uint32_t writethru     : 1;   // Cache this directory as write-thru only
   uint32_t cachedisabled : 1;   // Disable cache on this page table?
   uint32_t accessed      : 1;   // Supervisor level only if clear
   uint32_t pagesize      : 1;   // Has the page been accessed since last refresh?
   uint32_t ignored       : 2;   // Has the page been written to since last refresh?
   uint32_t os_specific   : 3;   // Amalgamation of unused and reserved bits
   uint32_t frame         : 20;  // Frame address (shifted right 12 bits)
};
struct page
{
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
};

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));


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
void *map_pages(void *vaddr, struct ppgae *pglist, struct page_directory_entry *pd){
	struct ppage *current = pglist;
	
	while(current != NULL){
	//Since it is a 32 bit we want to get the first 10 bits for the page_directory.
	uint32_t  pdindex = (((uint32_t) vaddr  >> 22) & 0x3FF);
	//Then we want to get the next 10 bits.
	uint32_t  ptindex = (((uint32_t) vaddr  >> 12) & 0x3FF);
	//Then we want to check if the pd entry is present 
	if(!pd[pdindex].present){
		//We want to create Page directory entry 
		pd[pdindex].present = 1;
		pd[pdindex].rw = 1;
		pd[pdindex].user = 0;
		pd[pdindex].writethru = 0;
		pd[pdindex].cachedisabled = 0;
		pd[pdindex].accessed = 0;
		pd[pdindex].pagesize = 0;
		pd[pdindex].ignored = 0;
		pd[pdindex].os_specific = 0;
		pd[pdindex].frame = (uint32_t)pt >> 12;
	}
	//This also create a new virtual page mapping
	pt[ptindex].present = 1;
	pt[ptindex].rw = 1;
	pt[ptindex].user = 0;
	pd[pdindex].writethru = 0;
        pd[pdindex].cachedisabled = 0;
        pd[pdindex].accessed = 0;
        pd[pdindex].pagesize = 0;
        pd[pdindex].ignored = 0;
        pd[pdindex].os_specific = 0;
	pt[ptindex].frame = ((uint32_t)vaddr >> 12);
	

	vaddr = ((uint32_t)vaddr + 4096);
	current = current -> next;
	}

}

void loadPageDirectory(struct page_directory_entry *pd){
	asm("mov %0, %%cr3" :: "r"(pd) : );

}
void main(){ 
	
	struct ppage tmp;
	tmp.next = NULL;
	tmp.physical_addr = 0x100000;
	map_pages(0x100000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x101000;
        map_pages(0x101000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x102000;
        map_pages(0x102000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x103000;
        map_pages(0x103000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x104000;
        map_pages(0x104000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x105000;
        map_pages(0x105000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x106000;
        map_pages(0x106000, &tmp, pd);

	 tmp.next = NULL;
        tmp.physical_addr = 0x107000;
        map_pages(0x107000, &tmp, pd);


        tmp.next = NULL;
        tmp.physical_addr = 0xB8000;
        map_pages(0xB8000, &tmp, pd);
	
	 tmp.next = NULL;
        tmp.physical_addr = 0x7f000;
        map_pages(0x7f000, &tmp, pd);


     // These are helper that helps us enable the interrupts for the platoform
    remap_pic();  //This help set the interrupt controller
    load_gdt();   //This load thr global descripter table
    init_idt();   // Initialize the interrupt descriptor table
    asm("sti");   // Enable interrupts
   

        
    loadPageDirectory(pd);

    asm("mov %cr0, %eax\n"
        "or $0x80000001,%eax\n"
        "mov %eax,%cr0");
	
   
    init_pfa_list();    
    while(1) {
         
     }
}


