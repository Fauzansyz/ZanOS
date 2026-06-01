#ifndef CC_H
#define CC_H

#include <stdint.h>
#include "uart.h"

/* Platform-specific types */
typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;
typedef uintptr_t mem_ptr_t;

/* Byte order */
#define BYTE_ORDER LITTLE_ENDIAN

/* Compiler hints for packing structures */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_FIELD(x) x

/* Debug output */
#define LWIP_PLATFORM_DIAG(x) do { uart_printf x; } while(0)
#define LWIP_PLATFORM_ASSERT(x) do { uart_print("ASSERT: "); uart_println(x); while(1); } while(0)

#define U8_F "d"
#define S8_F "d"
#define X8_F "x"
#define U16_F "d"
#define S16_F "d"
#define X16_F "x"
#define U32_F "d"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "d"

typedef int sys_prot_t;
extern uint32_t sys_now(void);
#define LWIP_RAND() (sys_now())

#endif
