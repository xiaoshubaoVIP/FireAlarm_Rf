#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>


/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
#define INFINITE 0xFFFFFFFF

#ifndef NULL
#define NULL            (0)
#endif

#ifndef TRUE
  /** Value is true (boolean_t type) */
  #define TRUE        ((boolean_t) 1u)
#endif

#ifndef FALSE
  /** Value is false (boolean_t type) */
  #define FALSE       ((boolean_t) 0u)
#endif  


/** Returns the minimum value out of two values */
#define min( X, Y )  ((X) < (Y) ? (X) : (Y))

/** Returns the maximum value out of two values */
#define max( X, Y )  ((X) > (Y) ? (X) : (Y))

/** Returns the dimension of an array */
#define ARRAY_SZ( X )  (sizeof(X) / sizeof((X)[0]))

#ifdef __DEBUG_ASSERT
	#define ASSERT(x) do{ assert((x)> 0u) ; }while(0);
#else
	#define ASSERT(x) {}
#endif
/******************************************************************************
 * Global type definitions
 ******************************************************************************/
/** logical datatype (only values are TRUE and FALSE) */
//typedef uint8_t 	 bool;
typedef enum {false = 0, true = !false} bool;

/** logical datatype (only values are TRUE and FALSE) */
typedef uint8_t      boolean_t;
  
/** single precision floating point number (4 byte) */
typedef float        float32_t;

/** double precision floating point number (8 byte) */
typedef double       float64_t;

/** ASCCI character for string generation (8 bit) */
typedef char         char_t;

/** function pointer type to void/void function */
typedef void         (*func_ptr_t)(void);

/** function pointer type to void/uint8_t function */
typedef void         (*func_ptr_arg1_t)(uint8_t u8Param);

#endif
