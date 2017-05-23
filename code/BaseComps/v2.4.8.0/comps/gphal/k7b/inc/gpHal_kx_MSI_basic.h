
#ifndef _GPHAL_KX_MSI_BASIC_H_
#define _GPHAL_KX_MSI_BASIC_H_

#include "gpHal_HW_MM.h"

#ifdef __GNUC__
#define ALWAYS_INLINE static inline __attribute__((always_inline))
#elif __18CXX
#define ALWAYS_INLINE
#else
#define ALWAYS_INLINE static inline
#endif

#ifdef __XAP5_NEAR__
#define address_t UInt16
#else
#define address_t UInt32
#endif

/*copied from kiwi/hal.h*/
#define GP_WB_CHECK_OFFSET_PBM(offset) do {                                                    \
    COMPILE_TIME_ASSERT((((offset) - GP_MM_RAM_PBM_OPTS_START)                                 \
                         % GP_MM_RAM_PBM_OPTS_OFFSET) == 0);                                   \
    COMPILE_TIME_ASSERT((offset) >= GP_MM_RAM_PBM_OPTS_START);                                 \
    COMPILE_TIME_ASSERT((offset) <  GP_MM_RAM_PBM_OPTS_START + 8UL * GP_MM_RAM_PBM_OPTS_OFFSET); \
} while (false)

#define GP_WB_CHECK_OFFSET_PBM_FORMAT_T GP_WB_CHECK_OFFSET_PBM
#define GP_WB_CHECK_OFFSET_PBM_FORMAT_R GP_WB_CHECK_OFFSET_PBM

#ifndef __18CXX
ALWAYS_INLINE void _GP_WB_WRITE_U8(address_t address, UInt8 value,
                                   const char * file, UInt16 line)
{
    GP_HAL_WRITE_REG(address, value);
    //msi_safe_write(file, line, address, value);
}
ALWAYS_INLINE void _GP_WB_WRITE_U16(address_t address, UInt16 value,
                                    const char * file, UInt16 line)
{
    GP_HAL_WRITE_BYTE_STREAM(address, (void*)&value, 2);
    //msi_safe_blockwrite(file, line, address, &value, 2);
}
ALWAYS_INLINE void _GP_WB_WRITE_U24(address_t address, UInt32 value,
                                    const char * file, UInt16 line)
{
    GP_HAL_WRITE_BYTE_STREAM(address, (void*)&value, 3);
    //msi_safe_blockwrite(file, line, address, &value, 3);
}
ALWAYS_INLINE void _GP_WB_WRITE_U32(address_t address, UInt32 value,
                                    const char * file, UInt16 line)
{
    GP_HAL_WRITE_BYTE_STREAM(address, (void*)&value, 4);
    //msi_safe_blockwrite(file, line, address, &value, 4);
}
ALWAYS_INLINE void _GP_WB_WRITE_U48(address_t address, UInt64Struct_t value,
                                    const char * file, UInt16 line)
{
    GP_HAL_WRITE_BYTE_STREAM(address,   (void*)&value.LSB, 4);
    GP_HAL_WRITE_BYTE_STREAM(address+4, (void*)&value.MSB, 2);
    //msi_safe_blockwrite(file, line, address, &value, 6);
}
ALWAYS_INLINE void _GP_WB_WRITE_U64(address_t address, UInt64Struct_t value,
                                    const char * file, UInt16 line)
{
    GP_HAL_WRITE_BYTE_STREAM(address,   (void*)&value.LSB, 4);
    GP_HAL_WRITE_BYTE_STREAM(address+4, (void*)&value.MSB, 4);
    //msi_safe_blockwrite(file, line, address, &value, 8);
}
ALWAYS_INLINE UInt16 _GP_WB_READ_U16(address_t address,
                                     const char * file, UInt16 line)
{
    UInt16 n;
    GP_HAL_READ_BYTE_STREAM(address, &n, 2);
    //msi_safe_blockread(file, line, address, &n, 2);
    return n;
}
ALWAYS_INLINE UInt32 _GP_WB_READ_U24(address_t address,
                                     const char * file, UInt16 line)
{
    UInt32 n=0;
    GP_HAL_READ_BYTE_STREAM(address, (void*)&n, 3);
    //msi_safe_blockread(file, line, address, &n, 3);
    return n;
}
ALWAYS_INLINE UInt32 _GP_WB_READ_U32(address_t address,
                                     const char * file, UInt16 line)
{
    UInt32 n;
    GP_HAL_READ_BYTE_STREAM(address, (void*)&n, 4);
    //msi_safe_blockread(file, line, address, &n, 4);
    return n;
}
ALWAYS_INLINE UInt64Struct_t _GP_WB_READ_U48(address_t address,
                                     const char * file, UInt16 line)
{
    UInt64Struct_t n;
    GP_HAL_READ_BYTE_STREAM(address,   (void*)&n.LSB, 4);
    GP_HAL_READ_BYTE_STREAM(address+4, (void*)&n.MSB, 2);
    //msi_safe_blockread(file, line, address, &n, 6);
    return n;
}

ALWAYS_INLINE UInt64Struct_t _GP_WB_READ_U64(address_t address,
                                     const char * file, UInt16 line)
{
    //UInt64 n;
    UInt64Struct_t n;
    GP_HAL_READ_BYTE_STREAM(address,   (void*)&n.LSB, 4);
    GP_HAL_READ_BYTE_STREAM(address+4, (void*)&n.MSB, 4);
    //msi_safe_blockread(file, line, address, &n, 8);
    return n;
}

ALWAYS_INLINE UInt8 _GP_WB_READ_U8(address_t address,
                                   const char * file, UInt16 line)
{
    //return msi_safe_read(file, line, address);
    return GP_HAL_READ_REG(address);
}
ALWAYS_INLINE void _GP_WB_MWRITE_U8(address_t address, UInt8 mask, UInt8 value,
                                    const char * file, UInt16 line)
{
    GP_HAL_READMODIFYWRITE_REG(address, mask, value);
    //return msi_safe_maskedwrite(file, line, address, mask, value);
}

#define _GP_WB_READ_U1(address, bit) \
    UInt16 __result = 0; \
    if (__builtin_constant_p(bit) && __builtin_constant_p(address)) \
    { \
        asm volatile ("ld.1.i  %%flags[c], @(" #address "["#bit "], 0)\n\t" \
                      "mov.1.r %0, %%flags[c]\n\t" : "=r" (__result) ); \
    } \
    else \
    { \
        __result = (GP_WB_READ_U8(address) >> bit) & 0x1; \
    }
#define GP_WB_READ_U1(address, bit) ({ _GP_WB_READ_U1(address, bit); __result; })

#define GP_WB_MWRITE_U1(address, bit, val) do \
{ \
    if (__builtin_constant_p(val) && __builtin_constant_p(address)) \
    { \
        if ((val) == 0) asm volatile ("st.1.i #0, @(" #address "[" #bit "], 0)"); \
        if ((val) == 1) asm volatile ("st.1.i #1, @(" #address "[" #bit "], 0)"); \
    } \
    else \
    { \
        GP_WB_MWRITE_U8(address, (1 << (bit)), (val) << (bit)); \
    } \
} while (0)
#define GP_WB_WRITE_U8(address, value)          \
    _GP_WB_WRITE_U8(address, value, __FILE__, __LINE__)
#define GP_WB_WRITE_U16(address, value)             \
    _GP_WB_WRITE_U16(address, value, __FILE__, __LINE__)
#define GP_WB_WRITE_U24(address, value)             \
    _GP_WB_WRITE_U24(address, value, __FILE__, __LINE__)
#define GP_WB_WRITE_U32(address, value)             \
    _GP_WB_WRITE_U32(address, value, __FILE__, __LINE__)
#define GP_WB_WRITE_U48(address, value)             \
    _GP_WB_WRITE_U48(address, value, __FILE__, __LINE__)
#define GP_WB_WRITE_U64(address, value)             \
    _GP_WB_WRITE_U64(address, value, __FILE__, __LINE__)

#define GP_WB_READ_U8(address)                  \
    _GP_WB_READ_U8(address, __FILE__, __LINE__)
#define GP_WB_READ_U16(address)                 \
    _GP_WB_READ_U16(address, __FILE__, __LINE__)
#define GP_WB_READ_U24(address)                 \
    _GP_WB_READ_U24(address, __FILE__, __LINE__)
#define GP_WB_READ_U32(address)                 \
    _GP_WB_READ_U32(address, __FILE__, __LINE__)
#define GP_WB_READ_U48(address)                 \
    _GP_WB_READ_U48(address, __FILE__, __LINE__)
#define GP_WB_READ_U64(address)                 \
    _GP_WB_READ_U64(address, __FILE__, __LINE__)

#define GP_WB_MWRITE_U8(address, mask, value)           \
    _GP_WB_MWRITE_U8(address, mask, value, __FILE__, __LINE__)

#define GP_WB_RANGE_CHECK(val, max) do              \
    {                                               \
        COMPILE_TIME_ASSERT((val) <= (max));        \
        GP_ASSERT_DEV_INT((val) <= (max));          \
    } while (false)

#define GP_WB_RANGE_CHECK_SIGNED(val, min, max) do  \
    {                                               \
        COMPILE_TIME_ASSERT((val) >= (min));        \
        COMPILE_TIME_ASSERT((val) <= (max));        \
        GP_ASSERT_DEV_INT((val) >= (min));                  \
        GP_ASSERT_DEV_INT((val) <= (max));                  \
    } while (false)

/* sign extension/casting macros: extends the sign bit for len-bit 
   numbers and singned casting */
#define GP_WB_S8(val, len)  (((Int8 )(val<<( 8-len)))>>( 8-len))
#define GP_WB_S16(val, len) (((Int16)(val<<(16-len)))>>(16-len))
#define GP_WB_S24(val, len) (((Int32)(val<<(32-len)))>>(32-len))
#define GP_WB_S32(val, len) (((Int32)(val<<(32-len)))>>(32-len))

#else   /* __18CXX */

#define GP_WB_WRITE_U8(address, value)          \
    GP_HAL_WRITE_REG(address, value)
#define GP_WB_WRITE_U16(address, value)             \
    __tmp16 = value; \
    GP_HAL_WRITE_BYTE_STREAM(address, (void*)&__tmp16, 2)
#define GP_WB_WRITE_U24(address, value)             \
    __tmp24 = value; \
    GP_HAL_WRITE_BYTE_STREAM(address, (void*)&__tmp24, 3)
#define GP_WB_WRITE_U32(address, value)             \
    __tmp32 = value; \
    GP_HAL_WRITE_BYTE_STREAM(address, (void*)&__tmp32, 4)
#define GP_WB_WRITE_U48(address, value)             \
    error "no 48-bit support"
#define GP_WB_WRITE_U64(address, value)             \
    error "no 64-bit support"

extern UInt16 __tmp16;
extern UInt32 __tmp24;
extern UInt32 __tmp32;

#define GP_WB_READ_U8(address)                  \
    GP_HAL_READ_REG(address)
#define GP_WB_READ_U16(address)                 \
    (GP_HAL_READ_BYTE_STREAM(address, (void*)&__tmp16, 2), __tmp16)
#define GP_WB_READ_U24(address)                 \
    (GP_HAL_READ_BYTE_STREAM(address, (void*)&__tmp24, 3), __tmp24)
#define GP_WB_READ_U32(address)                 \
    (GP_HAL_READ_BYTE_STREAM(address, (void*)&__tmp32, 4), __tmp32)
#define GP_WB_READ_U48(address)                 \
    error "no 48-bit support"
#define GP_WB_READ_U64(address)                 \
    error "no 64-bit support"

#define GP_WB_MWRITE_U1(address, _bit, _val) GP_WB_MWRITE_U8((address), (1 << (_bit)), (_val) << (_bit))

#define GP_WB_MWRITE_U8(address, mask, value)           \
    GP_HAL_READMODIFYWRITE_REG(address, mask, value)

#define GP_WB_RANGE_CHECK(val, max) do              \
    {                                               \
        COMPILE_TIME_ASSERT((val) <= (max));        \
        GP_ASSERT_DEV_INT((val) <= (max));          \
    } while (false)

#define GP_WB_RANGE_CHECK_SIGNED(val, min, max) do  \
    {                                               \
        COMPILE_TIME_ASSERT((val) >= (min));        \
        COMPILE_TIME_ASSERT((val) <= (max));        \
        GP_ASSERT_DEV_INT((val) >= (min));                  \
        GP_ASSERT_DEV_INT((val) <= (max));                  \
    } while (false)

/* sign extension/casting macros: extends the sign bit for len-bit 
   numbers and singned casting */
#define GP_WB_S8(val, len)  (((Int8 )(val<<( 8-len)))>>( 8-len))
#define GP_WB_S16(val, len) (((Int16)(val<<(16-len)))>>(16-len))
#define GP_WB_S24(val, len) (((Int32)(val<<(32-len)))>>(32-len))
#define GP_WB_S32(val, len) (((Int32)(val<<(32-len)))>>(32-len))

#endif  /* __18CXX */
#endif //_GPHAL_KX_MSI_BASIC_H_
