
MEMORY
{
    flash : ORIGIN = 0x8000, LENGTH = 120K
    ram :   ORIGIN = 0x4000, LENGTH = 8K
}

/* notation:
 * [ begin, end [
 * i.e. the begin is included in a range, but the end is not!
 */
SECTIONS
{
    /* Ram begin/end */
    ram_begin = 0x4000;
    ram_end   = 0x6000;

    /* Flash begin/end */
    flash_begin      = 0x8000;
    flash_usable_end = flash_begin + 0x1e000 - 128;

    .magic_word : {. += 0x04 ; } > ram
    .crc : { . += 0x04 ; } > ram
    .dummyalign_retention : { . = ALIGN(0x2); } > ram
    /* first retention part which overlaps with HW scratchpad */
    .retention1 : { . += 0x20 ; } > ram
    stack_begin = . ;
    /* second retention part which does not overlaps with HW scratchpad */
    .retention2 : { . += 0xE0 ; } > ram
    stack_nonoverlap = . ;
    .stack : { ; . += 0x400 ; } > ram
    pbm_options_begin = . ;
    .pbm_options : { . += 0x100 ; } > ram
    events_begin = . ;
    .events : { . += 0x20 ; } > ram
    events_end = . ;
    pbm_begin = . ;
    .pbm : { . += 0x280 ; } > ram
    pbm_ack_begin = . ;
    .pbmack : { . += 0x8 ; } > ram
    gp0_offset = . ;
    /* Only 0x37 bytes are in use in gp0 */
    .gp0 : { . += 0x40 ; } > ram
    gp1_offset = . ;
    gp2_offset = . ;
    gp3_offset = . ; /* Window overlayed with bss and data*/
    /* user license */
    .prgldmw            0x8030 : { LONG(0x693A5C81)       ;   } > flash
    .vector_ptr_ptr     0x8034 : { LONG(_vectors_ptr >> 2);   } > flash
    .version_sw         0x8050 : { KEEP(*(.version_sw))   ;   } > flash
    .version_app0       0x8060 :
    {
      KEEP(*(.version_app0)) ;
    } > flash
    .version_fgt        0x8064 : { KEEP(*(.version_fgt)) ;     } > flash
    .version_race       0x8068 : { KEEP(*(.version_race)) ;    } > flash
    .version_version    0x806C : { KEEP(*(.version_version)) ; } > flash

    /* vector table */
    .vectors            0x8100 :
    {
       _vectors_ptr = .;
       KEEP(*(.vectors))
    } > flash


    .dummyalign   : { . = ALIGN(0x100); } > flash
    .functable    : { *(.functable) } >flash
    .rodata       : { *(.rodata .rodata.*) } >flash
    .text         : { *(.text) *(.text.*) } >flash

    .data         : { *(.data) } >ram AT >flash
    .bss          : { *(.bss); } >ram
    .deepsleep    : { *(.deepsleep); } >ram

    /* that's all for RAM*/
    . = ALIGN(0x2);
    heap_begin = . ;

    /*find out the end of low/high flash*/
    .dummyhigh : { flash_end =  . ; } >flash

    .gpNvm flash_usable_end - 0xf80:
    {
        gpNvm_Start = . ;
        . = 0xf80 ;
        gpNvm_End = . ;
    } >flash

    bss_size = SIZEOF(.bss);
    bss_begin = ADDR(.bss);
    data_size = SIZEOF(.data);
    data_begin = ADDR(.data);
    data_src = LOADADDR(.data);

    /* make sure we do have a vector table*/
    ASSERT (SIZEOF(.vectors) > 8, "need a vector table" )
    /* make sure the alignment of vp is as required by xap*/
    ASSERT(ALIGN(ADDR(.vectors), 0x100) == ADDR(.vectors), "align")

    /* heap */
    heap_size   = 0x0;
    heap_end    = heap_begin + heap_size;
    stack_end   = pbm_options_begin;
    stack_size  = stack_end - stack_begin;
    ram_free    = ram_end - heap_end + stack_size;
    /* make sure stack boundaries are 2-byte aligned. */
    ASSERT (ALIGN("stack_begin", 2) == "stack_begin", "align stack begin")
    ASSERT (ALIGN("stack_end", 2) == "stack_end", "align stackend")
    ASSERT (ALIGN("ram_begin", 2) == "ram_begin", "align ram")
    /* make sure stack is not extremely small */
    ASSERT(stack_size >= 0x200, "tiny stack")

    elf_pbm_base = pbm_begin;
    elf_pbm_ack_begin = pbm_ack_begin;
    /* 2 bytes resolution */
    elf_wb_stack_begin = stack_begin / 2;
    elf_event_base = events_begin / 2;
    elf_pbm_options_base = pbm_options_begin / 2;
    elf_wb_stack_sz    = stack_size / 2;
    elf_virt_stack_top = 0x2000 + stack_size;
    elf_gp0_begin = gp0_offset / 2;
    elf_gp1_begin = gp1_offset / 2;
    elf_gp2_begin = gp2_offset / 2;
    elf_gp3_begin = gp3_offset / 2;
    elf_wb_stack_end = elf_wb_stack_begin + elf_wb_stack_sz;
    elf_stack_nonoverlap = stack_nonoverlap;
    total_nr_of_events_as_an_address = (events_end - events_begin) / 16;
}
