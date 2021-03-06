/*
This is the layout for the Global Descriptor Table
 */

#include <libc/stdlib.h>
#include <libc/stdio.h>

#pragma pack(2)
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
};

#pragma pack(2)
struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
};

#pragma pack(2)
struct idt_entry
{
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0; //always needs to be 0
    unsigned char flags;
    unsigned short base_hi;
};

#pragma pack(2)
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
};

struct gdt_entry gdt[3];
struct gdt_ptr gdt_ptr;

struct idt_entry idt[256];
struct idt_ptr idt_ptr;

extern "C"
{
extern void gdt_flush();
extern void idt_load();
}

//sets up one segment of the GDT with the settings you give it
void gdt_add_entry(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    //descriptor table address
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0x0F;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}
void gdt_install()
{
    printf("Install GDT\n");
    //the two ends of memory for the gdt
    //the base is the starting location,
    //and the limiit is 3 entrys minus 1
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_ptr.base = (unsigned int)&gdt;

    gdt_add_entry(0,0,0,0,0);

    gdt_add_entry(1,0,0xFFFFFFFF, 0x9A, 0xCF);

    gdt_add_entry(2,0,0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush();
}
void idt_add_entry(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt[num].always0 = 0;
    idt[num].base_low = (unsigned short) (base & 0xFFFF);
    idt[num].base_hi = (unsigned short) (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].flags = flags;
}
void idt_install()
{
    printf("Install IDT\n");
    idt_ptr.limit = (sizeof(struct idt_entry) * 256)-1;
    memset((unsigned char*)&idt,0,sizeof(idt)); //0 out all that memory
    idt_ptr.base = (unsigned int)&idt;
    idt_load();
}