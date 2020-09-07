#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

typedef struct wav
{
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];

    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    char subchunk2ID[4];
    uint32_t subchunk2Size;

    int32_t * data;

    uint8_t bytesperSample;
    uint32_t dataArraySize;
} WAV;

uint32_t buffer_to_uint ( uint8_t buffer[], uint8_t numBytes );
uint32_t buffer_to_int ( uint8_t buffer[], uint8_t numBytes);
uint32_t buffer_to_uint32 ( uint8_t buffer[]);
int32_t buffer_to_sint ( uint8_t buffer[], uint8_t numBytes);
void uint_to_buffer ( uint32_t integer, uint8_t buffer[], uint8_t numBytes);
int writeWAV ( WAV * datos ,char * filename);
WAV * abrirWAV ( char * filename );
void showWAVinfo(WAV * info);
WAV * volume (WAV * datos, float rate);
WAV * copyWAV ( WAV * datos );
void cerrarWAV ( WAV ** datos );
void printData ( WAV datos );