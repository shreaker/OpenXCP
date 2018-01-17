/**
*@file a2l_constants.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Constants for the data types used by A2L (ASAP) standard.
*/

#ifndef A2L_CONSTANTS_H
#define A2L_CONSTANTS_H

/**
Data type
 Explanation
 ASAP2 keyword
 Value range

Float
 32 bit floating point number
 FLOAT32_IEEE
 -3,4x1038 to 3,4x1038 (IEEE 32 Bit)

Double
 64 bit floating point number
 FLOAT64_IEEE
 -1,7x10308 to 1,7x10308 (IEEE 64 Bit)

SByte
 8 bit integer number with sign
 SBYTE
 -127 to 128

SWord
 16 bit integer number with sign
 SWORD
 -32767 to 32767

SLong
 32 bit integer number with sign
 SLONG
 -32768 to 32767

SInt64
 64 bit integer number with sign
 A_INT64
 -9223372036854775808 to 9223372036854775807

UByte
 8 bit integer number without sign
 UBYTE
 0 to 255

UWord
 16 bit integer number without sign
 UWORD
 0 to 65535

ULong
 32 bit integer number without sign
 ULONG
 0 to 4294967295

UInt64
 64 bit integer number without sign
 A_UINT64
 0 to 1,844674407370955x1016
*/

#define A2L_DATATYPE_UBYTE      "UBYTE"
#define A2L_DATATYPE_SBYTE      "SBYTE"
#define A2L_DATATYPE_UWORD      "UWORD"
#define A2L_DATATYPE_SWORD      "SWORD"
#define A2L_DATATYPE_ULONG      "ULONG"
#define A2L_DATATYPE_SLONG      "SLONG"
#define A2L_DATATYPE_A_UINT64   "A_UINT64"
#define A2L_DATATYPE_A_INT64    "A_INT64"
#define A2L_DATATYPE_FLOAT32_IEEE "FLOAT32_IEEE"
#define A2L_DATATYPE_FLOAT64_IEEE "FLOAT64_IEEE"

#define A2L_CHARACTERISTIC_PREFIX "__"

#endif // A2L_CONSTANTS_H
