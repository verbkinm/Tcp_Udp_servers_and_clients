#include "hex_dump.h"

static FILE* stream;

void hex_Dump(byte* const packet_bytes, size_t* const size, FILE* output)
{
    stream = output;
    fprintf_hexDump(packet_bytes, size);
    rewind(stream);
}
void fprintf_hexDump(byte* const byteArray, size_t * const size)
{
    byte* ASCII_String = (byte*)calloc(*size, *size * sizeof(byte));

    size_t i, j, n;
    unsigned line = 0;

    for (i = 0; i < *size; ++i) {
        if(isgraph(byteArray[i]))
            ASCII_String[i] = byteArray[i];
        else
            ASCII_String[i] = '.';
    }

    fprintf_boarder_line();

    fprintf(stream, "| %04u  ", line);
    //print lines
    for (i = 0, j = 0, n = 0; i < *size; ++i, ++j, ++n) {
        if(j == 8){
            fprintf(stream, " ");
            j = 0;
        }
        //print ASCII symbols at right of line
        if( n == 16){
            fprintf_ASSCII_string(&i, ASCII_String, size);
            n = 0;
            fprintf(stream, "| %04u  ", ++line);
        }
        fprintf(stream, "%02X ", byteArray[i]);
    }

    //append spaces for short line
    if(n < 16){
        if(n < 8){fprintf(stream, " ");}
        while (n < 16) {
            fprintf(stream, "   ");
            ++n;
            ++i;
        }
    }

    fprintf(stream, " ");
    fprintf_ASSCII_string(&i, ASCII_String, size);
    fprintf_boarder_line();

    free((void*)ASCII_String);
}
void fprintf_ASSCII_string(size_t * const i, byte* const ASCII_String, size_t* const size)
{
    size_t ii, jj;
    for (ii = 0, jj = 0; ii < 16; ++ii, ++jj) {
        if( (ii + *i - 16) >= *size)
                fprintf(stream, " ");
        else
            fprintf(stream, "%c", ASCII_String[ii + *i - 16]);

        if(jj == 7){fprintf(stream, " ");}
    }
    fprintf(stream, " |\n");
}
void fprintf_boarder_line(void)
{
    size_t tmp = 0;
    fprintf(stream, "|");
    while (tmp < 75) {
        fprintf(stream, "=");
        ++tmp;
    }
    fprintf(stream, "|\n");
}
