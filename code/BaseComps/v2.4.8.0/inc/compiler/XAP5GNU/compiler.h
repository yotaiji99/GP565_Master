#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <string.h>

#define INLINE inline
#define ALWAYS_INLINE static inline __attribute__((always_inline))
#define PACKED_PRE
#define PACKED_POST             __attribute__((packed))
#define LINKER_SECTION(name)    __attribute__((section((name))))

//please note: this hinders debugging!!
#define NORETURN __attribute__((noreturn))

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned long       uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed long         int32_t;
typedef unsigned long long  int64_t;

typedef uint16_t            uint_fast8_t;
typedef uint16_t            uint_fast16_t;
typedef uint32_t            uint_fast32_t;

typedef int16_t             int_fast8_t;
typedef int16_t             int_fast16_t;
typedef int32_t             int_fast32_t;

#ifdef __XAP5_NEAR__
//Pointer type, use this type to perform pointer arithmetic
typedef unsigned short UIntPtr;
#else
typedef unsigned long  UIntPtr;
#endif
typedef UIntPtr        UIntPtr_P;

/* MEMCPY from far region - used for FLASH and other accesses */
#ifdef __XAP5_NEAR__
extern void* memcpy_far(void* dst, uint32_t src, size_t len);

#define MEMCPY_PF(dst, src, len)    memcpy_far(dst, src, len)
#else
#define MEMCPY_PF(dst, src, len)    memcpy(dst, src, len)
#endif

//Check for bug in MEMSET - Can't set 0xFF - use -1 instead
#define MEMSET(dst, value, len) { COMPILE_TIME_ASSERT(value != 0xFF); memset(dst, value, len); }

/* Miscellaneous useful access macros, really needed only for NEAR mode, but should be harmless in FAR mode */
#define xap_read_far8(address)({    \
    uint32_t _addr = (address);       \
    uint16_t _data;                   \
    __asm__ volatile ("ld.8z.fi    %0, @(0, %1)" : "=r" (_data) : "r" (_addr));   \
    _data;                          \
    })
#define xap_read_far16(address)({    \
    uint32_t _addr = (address);       \
    uint16_t _data;                   \
    __asm__ volatile ("ld.fi       %0, @(0, %1)" : "=r" (_data) : "r" (_addr));   \
    _data;                          \
    })
#define xap_read_far32(address)({   \
    uint32_t _addr = (address);       \
    uint32_t _data;                   \
    __asm__ volatile ("ld.32.fi    %0, @(0, %1)" : "=r" (_data) : "r" (_addr));   \
    _data;                          \
    })


#endif //_COMPILER_H_
