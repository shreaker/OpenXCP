/**
*@file xcpprotocoldefinitions.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Definition of PID, CTO payload and ERR according the XCP protocol layer standard.
*/

#ifndef XCPPROTOCOLDEFINITIONS_H
#define XCPPROTOCOLDEFINITIONS_H

#include <stdint.h>

#define XCP_PID                        0

namespace XCP_HELPER {
const uint8_t RESERVED =        0x00;
}
/*-------------------------------------------------------------------------*/
/* Packet Identifiers Slave -> Master */
namespace XCP_CMD_RESPONSE_PACKET {
const uint8_t RES_POSITIVE     = 0xFF;   /* response packet        */
const uint8_t ERROR            = 0xFE;   /* error packet           */
const uint8_t EVENT            = 0xFD;   /* event packet           */
const uint8_t SERVICE_REQUEST  = 0xFC;   /* service request packet */
}

/******************************************************
* Error codes
*******************************************************/
namespace XCP_ERROR {
const uint8_t ERR_CMD_SYNCH                         = 0x00;
const uint8_t ERR_CMD_BUSY                          = 0x10;
const uint8_t ERR_DAQ_ACTIVE                        = 0x11;
const uint8_t ERR_PGM_ACTIVE                        = 0x12;
const uint8_t ERR_CMD_UNKNOWN                       = 0x20;
const uint8_t ERR_CMD_SYNTAX                        = 0x21;
const uint8_t ERR_OUT_OF_RANGE                      = 0x22;
const uint8_t ERR_WRITE_PROTECTED                   = 0x23;
const uint8_t ERR_ACCESS_DENIED                     = 0x24;
const uint8_t ERR_ACCESS_LOCKED                     = 0x25;
const uint8_t ERR_PAGE_NOT_VALID                    = 0x26;
const uint8_t ERR_MODE_NOT_VALID                    = 0x27;
const uint8_t ERR_SEGMENT_NOT_VALID                 = 0x28;
const uint8_t ERR_SEQUENCE                          = 0x29;
const uint8_t ERR_DAQ_CONFIG                        = 0x2A;
const uint8_t ERR_MEMORY_OVERFLOW                   = 0x30;
const uint8_t ERR_GENERIC                           = 0x31;
const uint8_t ERR_VERIFY                            = 0x32;
const uint8_t ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE = 0x33;
const uint8_t ERR_SUBCMD_UNKNOWN                    = 0x34;
}
/******************************************************
* Commands CTO
*******************************************************/
namespace XCP_CMD {
const uint8_t CONNECT =                 0xFF;
const uint8_t DISCONNECT =              0xFE;
const uint8_t GET_STATUS =              0xFD;
const uint8_t GET_SYNC =                0xFC;
const uint8_t SHORT_UPLOAD =            0xF4;
const uint8_t SET_MTA =                 0xF6;
const uint8_t DOWNLOAD =                0xF0;
const uint8_t BUILD_CHECKSUM =          0xF3;
//DAQ
const uint8_t FREE_DAQ =                0xD6;
const uint8_t ALLOC_DAQ =               0xD5;
const uint8_t ALLOC_ODT =               0xD4;
const uint8_t ALLOC_ODT_ENTRY =         0xD3;
const uint8_t SET_DAQ_PTR =             0xE2;
const uint8_t WRITE_DAQ =               0xE1;
const uint8_t SET_DAQ_LIST_MODE =       0xE0;
const uint8_t START_STOP_DAQ_LIST =     0xDE;
const uint8_t START_STOP_SYNCH =        0xDD;
}
/******************************************************
* Connect
*******************************************************/
namespace XCP_CONNECT {
const uint8_t MASTER_LENGTH =            2;
const uint8_t MASTER_MODE =              1;

const uint8_t SLAVE_LENGTH =             8;
const uint8_t SLAVE_RESOURCE =           1;
const uint8_t SLAVE_COMM_BASIC =         2;
const uint8_t SLAVE_MAX_CTO_SIZE =       3;
const uint8_t SLAVE_MAX_DTO_SIZE_0 =     4;
const uint8_t SLAVE_MAX_DTO_SIZE_1 =     5;
const uint8_t SLAVE_PROTOCOL_VERSION =   6;
const uint8_t SLAVE_TRANSPORT_VERSION =  7;

const uint8_t SLAVE_RESOURCE_CAL_PAG = 0;
const uint8_t SLAVE_RESOURCE_DAQ =     2;
const uint8_t SLAVE_RESOURCE_STIM =    3;
const uint8_t SLAVE_RESOURCE_PGM =     4;

const uint8_t SLAVE_COMM_BASIC_BYTE_ORDER =          0;
const uint8_t SLAVE_COMM_BASIC_ADDR_GRANULARITY_0 =  1;
const uint8_t SLAVE_COMM_BASIC_ADDR_GRANULARITY_1 =  2;
const uint8_t SLAVE_COMM_BASIC_SLAVE_BLOCK_MODE =    6;
const uint8_t SLAVE_COMM_BASIC_OPTIONAL =            7;
}
/******************************************************
* Disconnect
*******************************************************/
namespace XCP_DISCONNECT {
const uint8_t MASTER_LENGTH =       1;
}
/******************************************************
* Get status
*******************************************************/
namespace XCP_GET_STATUS {
const uint8_t MASTER_LENGTH =                  1;

const uint8_t SLAVE_LENGTH =                   5;
const uint8_t SLAVE_SESSION_STATUS =           1;
const uint8_t SLAVE_RESOURCE_PROTECT_STATUS =  2;
const uint8_t SLAVE_STATE_NR =                 3;
const uint8_t SLAVE_SESSION_CONFIG_ID =        4;

const uint8_t SLAVE_SESSION_STATUS_STORE_CAL_REQ = 0;
const uint8_t SLAVE_SESSION_STATUS_STORE_DAQ_REQ = 2;
const uint8_t SLAVE_SESSION_STATUS_CLEAR_DAQ_REQ = 3;
const uint8_t SLAVE_SESSION_STATUS_DAQ_RUNNING   = 6;
const uint8_t SLAVE_SESSION_STATUS_RESUME        = 7;

const uint8_t SLAVE_RESOURCE_PROTECT_STATUS_CAL_PAG = 0;
const uint8_t SLAVE_RESOURCE_PROTECT_STATUS_DAQ     = 2;
const uint8_t SLAVE_RESOURCE_PROTECT_STATUS_STIM    = 3;
const uint8_t SLAVE_RESOURCE_PROTECT_STATUS_PGM     = 4;
}
/******************************************************
* SYNCHRONIZE COMMAND EXECUTION AFTER TIMEOUT
*******************************************************/
namespace XCP_SYNCH {
const uint8_t MASTER_LENGTH =                  1;

const uint8_t SLAVE_LENGTH =                   2;
const uint8_t SLAVE_NEG_RESPONSE_ERROR_CODE =  2;
}
/******************************************************
* UPLOAD FROM SLAVE TO MASTER (SHORT VERSION)
*******************************************************/
namespace XCP_SHORT_UPLOAD {
const uint8_t MASTER_LENGTH =             8;
const uint8_t MASTER_ELEMENTS =           1;
const uint8_t MASTER_RESERVED =           2;
const uint8_t MASTER_ADDR_EXTENSION =     3;
const uint8_t MASTER_ADDR_0 =             4;
const uint8_t MASTER_ADDR_1 =             5;
const uint8_t MASTER_ADDR_2 =             6;
const uint8_t MASTER_ADDR_3 =             7;

const uint8_t SLAVE_BYTE_ALIGNMENT_ELEMENT_1 = 1;
}
/******************************************************
* SET MEMORY TRANSFER ADDRESS IN SLAVE
*******************************************************/
namespace XCP_SET_MTA {
const uint8_t MASTER_LENGTH =             8;
const uint8_t MASTER_RESERVED_0 =         1;
const uint8_t MASTER_RESERVED_1 =         2;
const uint8_t MASTER_ADDR_EXTENSION =     3;
const uint8_t MASTER_ADDR_0 =             4;
const uint8_t MASTER_ADDR_1 =             5;
const uint8_t MASTER_ADDR_2 =             6;
const uint8_t MASTER_ADDR_3 =             7;
}
/******************************************************
* DOWNLOAD FROM MASTER TO SLAVE
*******************************************************/
namespace XCP_DOWNLOAD {
const uint8_t MASTER_ELEMENTS =                 1;
const uint8_t MASTER_BYTE_ALIGNMENT_ELEMENT_1 = 2;
}
/******************************************************
*  BUILD CHECKSUM OVER MEMORY RANGE
*******************************************************/
namespace XCP_BUILD_CHECKSUM {
const uint8_t MASTER_LENGTH =             8;
const uint8_t MASTER_RESERVED_0 =         1;
const uint8_t MASTER_RESERVED_1 =         2;
const uint8_t MASTER_RESERVED_2 =         3;
const uint8_t MASTER_BLOCK_SIZE_0 =       4;
const uint8_t MASTER_BLOCK_SIZE_1 =       5;
const uint8_t MASTER_BLOCK_SIZE_2 =       6;
const uint8_t MASTER_BLOCK_SIZE_3 =       7;

const uint8_t SLAVE_LENGTH =              8;
const uint8_t SLAVE_CHECKSUM_TYPE =       1;
const uint8_t SLAVE_RESERVED_0 =          2;
const uint8_t SLAVE_RESERVED_1 =          3;
const uint8_t SLAVE_CHECKSUM_0 =          4;
const uint8_t SLAVE_CHECKSUM_1 =          5;
const uint8_t SLAVE_CHECKSUM_2 =          6;
const uint8_t SLAVE_CHECKSUM_3 =          7;
}
/******************************************************
* CLEAR DYNAMIC DAQ CONFIGURATION
*******************************************************/
namespace XCP_FREE_DAQ {
const uint8_t MASTER_LENGTH =       1;
}
/******************************************************
* ALLOCATE DAQ LISTS
*******************************************************/
namespace XCP_ALLOC_DAQ {
const uint8_t MASTER_LENGTH =       4;
const uint8_t MASTER_RESERVED =     1;
const uint8_t MASTER_DAQ_COUNT_0 =  2;
const uint8_t MASTER_DAQ_COUNT_1 =  3;
}
/******************************************************
* ALLOCATE ODTS TO A DAQ LIST
*******************************************************/
namespace XCP_ALLOC_ODT {
const uint8_t MASTER_LENGTH =           5;
const uint8_t MASTER_RESERVED =         1;
const uint8_t MASTER_DAQ_LIST_NR_0 =    2;
const uint8_t MASTER_DAQ_LIST_NR_1 =    3;
const uint8_t MASTER_ODT_COUNT =        4;
}
/******************************************************
* ALLOCATE ODT ENTRIES TO AN ODT
*******************************************************/
namespace XCP_ALLOC_ODT_ENTRY {
const uint8_t MASTER_LENGTH =               6;
const uint8_t MASTER_RESERVED =             1;
const uint8_t MASTER_DAQ_LIST_NR_0 =        2;
const uint8_t MASTER_DAQ_LIST_NR_1 =        3;
const uint8_t MASTER_ODT_NR =               4;
const uint8_t MASTER_ODT_ENTRIES_COUNT =    5;
}
/******************************************************
* SET POINTER TO ODT ENTRY
*******************************************************/
namespace XCP_SET_DAQ_PTR {
const uint8_t MASTER_LENGTH =               6;
const uint8_t MASTER_RESERVED =             1;
const uint8_t MASTER_DAQ_LIST_NR_0 =        2;
const uint8_t MASTER_DAQ_LIST_NR_1 =        3;
const uint8_t MASTER_ODT_NR =               4;
const uint8_t MASTER_ODT_ENTRY_NR =         5;
}
/******************************************************
* WRITE ELEMENT IN ODT ENTRY
*******************************************************/
namespace XCP_WRITE_DAQ {
const uint8_t MASTER_LENGTH =               8;
const uint8_t MASTER_BIT_OFFSET =           1;
const uint8_t MASTER_SIZE_DAQ_ELEMENT =     2;
const uint8_t MASTER_ADDR_EXTENSION =       3;
const uint8_t MASTER_ADDR_0 =               4;
const uint8_t MASTER_ADDR_1 =               5;
const uint8_t MASTER_ADDR_2 =               6;
const uint8_t MASTER_ADDR_3 =               7;
}
/******************************************************
* SET MODE FOR DAQ LIST
*******************************************************/
namespace XCP_SET_DAQ_LIST_MODE {
const uint8_t MASTER_LENGTH =               8;
const uint8_t MASTER_MODE =                 1;
const uint8_t MASTER_DAQ_LIST_NR_0 =        2;
const uint8_t MASTER_DAQ_LIST_NR_1 =        3;
const uint8_t MASTER_EVENT_CHANNEL_NR_0 =   4;
const uint8_t MASTER_EVENT_CHANNEL_NR_1 =   5;
const uint8_t MASTER_TRANS_RATE_PRESC =     6;
const uint8_t MASTER_DAQ_LIST_PRIO =        7;

const uint8_t MASTER_MODE_LENGTH =          8;
const uint8_t MASTER_MODE_ALTERNATE =       0;
const uint8_t MASTER_MODE_DIRECTION =       1;
const uint8_t MASTER_MODE_RESERVED_0 =      2;
const uint8_t MASTER_MODE_DTO_CTR =         3;
const uint8_t MASTER_MODE_TIMESTAMP =       4;
const uint8_t MASTER_MODE_PID_OFF =         5;
const uint8_t MASTER_MODE_RESERVED_1 =      6;
const uint8_t MASTER_MODE_RESERVED_2 =      7;
}
/******************************************************
* START_STOP_DAQ_LIST
*******************************************************/
namespace XCP_START_STOP_DAQ_LIST {
const uint8_t MASTER_LENGTH =               4;
const uint8_t MASTER_MODE =                 1;
const uint8_t MASTER_DAQ_LIST_NR_0 =        2;
const uint8_t MASTER_DAQ_LIST_NR_1 =        3;

const uint8_t SLAVE_LENGTH =                2;
const uint8_t SLAVE_FIRST_PID =             1;
}
/******************************************************
* START/STOP DAQ LISTS (SYNCHRONOUSLY)
*******************************************************/
namespace XCP_START_STOP_SYNCH {
const uint8_t MASTER_LENGTH =               2;
const uint8_t MASTER_MODE =                 1;
}


#endif // XCPPROTOCOLDEFINITIONS_H
