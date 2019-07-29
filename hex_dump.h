#ifndef HEX_DUMP_H
#define HEX_DUMP_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

//    size_t size = sizeof(packet_bytes) / sizeof(packet_bytes[0]);
//    hex_Dump(packet_bytes, &size);

typedef unsigned char byte;

void fprintf_hexDump(byte* const byteArray, size_t* const size);
void fprintf_ASSCII_string(size_t* const i, byte* const ASCII_String, size_t* const size);
void fprintf_boarder_line(void);
void hex_Dump(byte* const packet_bytes, size_t* const size, FILE* output);

#endif // HEX_DUMP_H
