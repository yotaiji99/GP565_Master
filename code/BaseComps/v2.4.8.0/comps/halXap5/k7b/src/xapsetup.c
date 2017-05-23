#define COMPLETE_MAP
#include "gpHal.h"
#include "hal.h"
#include "gpAssert.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#ifdef GP_DIVERSITY_LOG
#include "gpLog.h"
//To make sure this logging gets out, we flush the log buffer everytime
#define BLOCK_LOG(s, ...) do {              \
    GP_LOG_SYSTEM_PRINTF(s, 0, ##__VA_ARGS__);  /*http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html*/ \
    gpLog_Flush();                          \
} while(false)
#else
#define BLOCK_LOG(s, ...)
#endif //GP_DIVERSITY_LOG

//These are set by assembler.

//{r1,r0} holds address of faulty instruction
UInt32 pException;

// 0x4C InstructionError_R
// 0x50 NullPointer_R
// 0x54 DivideByZero_R
// 0x58 UnknownInstruction_R
// 0x5C AlignError_R
// 0x60 MMUDataError_R
// 0x64 MMUProgError_R
// 0xFF Anonymous exception
UInt8 ExceptionId;

#ifdef GP_DIVERSITY_LOG
static void Dump16(UIntPtr bottom, UIntPtr top)
{
    UIntPtr sp=top;
    while (sp>=bottom+0x08)
    {
        sp-=0x08;
        const UInt16 * p=(const UInt16 *) sp;
        NOT_USED(p);
        BLOCK_LOG("%x: %x %x %x %x",
                      sp,
                      p[3], p[2], p[1], p[0]
                      );
    }
    BLOCK_LOG("current sp: %lx [%x-%x]", xap_getAreg(XAP_SP0), bottom, top);
}
static void DumpStack(void)
{
    //Dump stack
    //we need to obtain the location of the stack.
    //normally, we would access it using the elf symbols.
    //however, this is difficult, this is shared code
    //wo we use the registers as an alternative and hope they are not corrupt
    //(should be obvious)
    
    UIntPtr bottom = GP_MM_RAM_GEN_WINDOW_VAR_0_START; // Address through window - real location: GP_MM_RAM_LINEAR_START + GP_WB_READ_MM_GEN_WINDOW_VAR_0_BASE_ADDRESS()*2;
    UIntPtr top = bottom + GP_WB_READ_MM_GEN_WINDOW_VAR_0_LEN()*2;

    Dump16(bottom, top);
}
#endif //GP_DIVERSITY_LOG

//flags     
//- XAP uc flags
//- |Priority0-3|State1-4|
//-|State0|Int enable|Mode1|Mode0||oVerflow|Carry|Negative|Zero|
//
//errcode   
//- |7-5|EventNo 0-4|
//- |Recovery|NMI-lock|Stack1En|Stack0En|SoftReset|EventType 8-10|
//- Event No:
//  0 - if Event Type == 2-7
//  2 - SoftReset
//  ... 31 - Other Exceptions
//- Event Type:
//  0 - Exception
//  2 - Memory error during Context Push  to Stack0 -> stackoverflow
//
//sp
//- stack pointer - pointing to RAM location of stack

void handle_soft_reset(UInt16 flags, UInt16 errcode, UInt16 sp)
{
#ifdef GP_DIVERSITY_LOG
    BLOCK_LOG("soft reset!");
    UInt16 ev_type = (errcode >>8) & 0x7;
    UInt16 ev_no = errcode & 0xF;

    BLOCK_LOG("ev type/no: %i/%i", ev_type, ev_no);
    if (ev_type == 2 && ev_no == 0)
    {
        BLOCK_LOG("-> stack ov, see pc");
    }
    else if (ev_type == 0 && ev_no == 2)
    {
        BLOCK_LOG("-> except, see pException");
    }
    BLOCK_LOG("ev flgs: %x", errcode & 0xF800);
    BLOCK_LOG("pc: %lx", xap_getBreg(XAP_BRK0)); //In far mode program counter will be un-even
    BLOCK_LOG("SP0 %x", sp);
    BLOCK_LOG("flags: %x", flags);
    BLOCK_LOG("pException: %lx", pException);
    BLOCK_LOG("ExceptionId: %x", ExceptionId);
    UInt16 badaddr = GP_WB_READ_XAP5_GP_LOGICAL_ADDRESS_ON_ERR(); 
    UInt8 status= GP_WB_READ_XAP5_GP_ERR_STATUS();
    BLOCK_LOG("MMU A: %x, S: %x", badaddr, status);

    DumpStack();
#endif //GP_DIVERSITY_LOG
    GP_ASSERT_SYSTEM(false);
}

INTERRUPT_H void unexpected_int(void)
{
#ifdef GP_DIVERSITY_LOG
    //Since we don't handle the interrupt it will probably keep occuring.
    UInt16 pi;
    GP_HAL_READ_REGS16(GP_WB_INT_CTRL_MASK_INT_ES_INTERRUPT_ADDRESS,&pi);
    BLOCK_LOG("Unexpected interrupt: masked: %x", (UInt16)pi);
#endif //GP_DIVERSITY_LOG

    xap_softreset();
}
