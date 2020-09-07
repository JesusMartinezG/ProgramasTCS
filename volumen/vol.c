#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

typedef struct wavHeader
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

    int16_t * data16;
    int32_t * data32;

    uint8_t bytesperSample;
    uint32_t dataArraySize;
} wavHeader;

uint32_t buffer_to_uint ( uint8_t buffer[], uint8_t numBytes );
uint32_t buffer_to_uint32 ( uint8_t buffer[]);
int32_t buffer_to_sint ( uint8_t buffer[], uint8_t numBytes);
void uint_to_buffer ( uint32_t integer, uint8_t buffer[], uint8_t numBytes);
int writeWAV ( wavHeader * datos ,char * filename);
wavHeader * readWAV ( char * filename );
wavHeader * volume (wavHeader * datos, float rate);


uint32_t buffer_to_uint ( uint8_t buffer[], uint8_t numBytes) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
{
    uint32_t integer = 0;
    uint32_t shift = 0;
    for(int i=0; i<numBytes; i++)
    {
        integer = integer | (buffer[i]<<shift);
        shift += 8;
    }

    return integer;
}

void uint_to_buffer ( uint32_t integer, uint8_t buffer[], uint8_t numBytes) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
{
    uint8_t shift = 0;
    for(int i=0; i<numBytes; i++)
    {
        buffer[i] = (uint8_t)(integer>>shift);
        shift += 8;
    }
}

int32_t buffer_to_sint ( uint8_t buffer[], uint8_t numBytes) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
{
    int32_t integer = 0;
    int32_t shift = 0;
    for(uint8_t i=0; i<numBytes; i++)
    {
        integer = integer | (buffer[i]<<shift);
        shift += 8;
    }

    return integer;
}

uint32_t buffer_to_uint32 ( uint8_t buffer[]) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
{
    uint32_t integer = buffer[0];
    integer += buffer[1]<<8;
    integer += buffer[2]<<16;
    integer += buffer[3]<<24;

    return integer;
}

int writeWAV ( wavHeader * datos ,char * filename)
{
    if (datos == NULL)
    {
        return -1;
    }

    FILE * archivo = fopen( filename, "wb" );

    if (archivo == NULL)
    {
        return -2;
    }
    uint8_t buffer[4];

    //4 bytes chunkID Big endian
    for ( int i=0; i<4; i++ ) putc( datos->chunkID[i], archivo ); //RIFF

    //4 bytes chunkSize Little endian
    uint_to_buffer ( datos->chunkSize, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    //4 bytes format Big endian
    for ( int i=0; i<4; i++ ) putc(datos->format[i], archivo );//WAVE

    //4 bytes subchunk1ID Big endian
    for ( int i=0; i<4; i++ ) putc( datos->subchunk1ID[i], archivo ); //fmt

    //4 bytes subchunk1Size Little endian
    uint_to_buffer ( datos->subchunk1Size, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    //2 bytes audioFormat Little endian
    uint_to_buffer ( datos->audioFormat, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    //2 bytes numChannels Little endian
    uint_to_buffer ( datos->numChannels, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    //4 bytes sampleRate Little endian
    uint_to_buffer ( datos->sampleRate, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    //4 bytes byteRate Little endian
    uint_to_buffer ( datos->byteRate, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    //2 bytes BlockAlign Little endian
    uint_to_buffer ( datos->blockAlign, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    //2 bytes bitsPerSample Little endian
    uint_to_buffer ( datos->bitsPerSample, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    //4 bytes subchunk2ID Big endian
    for ( int i=0; i<4; i++ ) putc( datos->subchunk2ID[i], archivo ); //data

    //4 bytes subchunk2Size Little endian
    uint_to_buffer ( datos->subchunk2Size, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    //datos
    if (datos->data32 == NULL)
    {
        for( int i=0; i<datos->dataArraySize; i++ )
        {
            uint_to_buffer ( datos->data16[i], buffer, datos->bytesperSample );
            
            for( int j=0; j<datos->bytesperSample; j++) putc(buffer[j], archivo);
        }
    }
    else
    {
        for( int i=0; i<datos->dataArraySize; i++ )
        {
            uint_to_buffer ( datos->data32[i], buffer, datos->bytesperSample );
            
            for( int j=0; j<datos->bytesperSample; j++) putc(buffer[j], archivo);
        }
    }

    fclose(archivo);
    return 0;
}

wavHeader * readWAV ( char * filename )
{
    FILE * archivo = fopen( filename, "rb" );

    if (archivo == NULL)
    {
        printf("ERROR: No se pudo abrir el archivo de entrada %s\n", filename);
        return NULL;
    }

    wavHeader * datos = (wavHeader*) malloc(sizeof(wavHeader));
    uint8_t buffer[4];

    if ( archivo ) 
    {
        //4 bytes chunkID Big endian
        for ( int i=0; i<4; i++ ) datos->chunkID[i] = getc( archivo ); //RIFF

        //4 bytes chunkSize Little endian
        for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
        datos->chunkSize = buffer_to_uint32( buffer );

        //4 bytes format Big endian
        for ( int i=0; i<4; i++ ) datos->format[i] = getc( archivo );  //WAVE

        //Verifica si el archivo es de formato wav
        if( !(datos->format[0] == 'W' && datos->format[1] == 'A' && datos->format[2] == 'V' && datos->format[3] == 'E') )
        {
            free( datos );
            fclose(archivo);
            return NULL;
        }

        //4 bytes subchunk1ID Big endian
        for ( int i=0; i<4; i++ ) datos->subchunk1ID[i] = getc( archivo ); //fmt

        //4 bytes subchunk1Size Little endian
        for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
        datos->subchunk1Size = buffer_to_uint( buffer, 4 );

        //2 bytes audioFormat Little endian
        for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
        datos->audioFormat = buffer_to_uint( buffer, 2 );

        //2 bytes numChannels Little endian
        for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
        datos->numChannels = buffer_to_uint( buffer, 2 );

        //4 bytes sampleRate Little endian
        for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
        datos->sampleRate = buffer_to_uint( buffer, 4 );

        //4 bytes byteRate Little endian
        for ( int i=0; i<4; i++ ) buffer[i] = getc(archivo);
        datos->byteRate = buffer_to_uint( buffer, 4 );

        //2 bytes BlockAlign Little endian
        for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
        datos->blockAlign = buffer_to_uint( buffer, 2 );

        //2 bytes bitsPerSample Little endian
        for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
        datos->bitsPerSample = buffer_to_uint( buffer, 2 );

        //4 bytes subchunk2ID Big endian
        for ( int i=0; i<4; i++ ) datos->subchunk2ID[i] = getc( archivo ); //data

        //4 bytes subchunk2Size Little endian
        for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
        datos->subchunk2Size = buffer_to_uint( buffer, 4 );
    
        //datos
        datos->bytesperSample = datos->bitsPerSample / 8;
        //datos->dataArraySize = datos->subchunk2Size / datos->bytesperSample;
        datos->dataArraySize = datos->subchunk2Size / (datos->numChannels * datos->bytesperSample);

        if ( datos->bytesperSample < 3 ) // Si las muestras caben en 2 bytes
        {
            datos->data16 = (int16_t*)malloc( sizeof(int16_t)*datos->dataArraySize );
            datos->data32 = NULL;
            uint32_t j=0, k=0;

            for( int i=1; i<=datos->subchunk2Size; i++ )
            {
                buffer[j++] = getc( archivo );
                if( j == datos->bytesperSample )
                {
                    datos->data16[k] = buffer_to_sint( buffer, datos->bytesperSample); //Convierte las muestras en el buffer
                    k++;
                    j=0;
                }
            }
        }
        else // Lo mismo pero en un arreglo de datos de 32 bits
        {
            datos->data32 = (int32_t*)malloc( sizeof(int32_t)*datos->dataArraySize );
            datos->data16 = NULL;
            uint32_t j=0, k=0;

            for( int i=1; i<=datos->subchunk2Size; i++ )
            {
                buffer[j++] = getc( archivo );
                if( j == datos->bytesperSample ) // Si el byte actual es multiplo del numero de bytes por muestra
                {
                    datos->data32[k++] = buffer_to_uint( buffer, datos->bytesperSample); //Convierte las muestras en el buffer
                    j=0; // Vuelve a escribir desde el inicio del buffer
                }
            }
        }
    }
    fclose(archivo);
    return datos;
}


wavHeader * volume (wavHeader * datos, float rate)
{
    wavHeader * copy = datos; //copyWAV(datos);

    if( copy->data16 )
    {
        for (int i=0; i<copy->dataArraySize; i++)
        {
            copy->data16[i] = copy->data16[i]*rate;
        }
    }
    else
    {
        for (int i=0; i<copy->dataArraySize; i++)
        {
            copy->data32[i] = copy->data32[i]*rate;
        }
    }
    return copy;
}

int main( int argc, char *argv[] )
{
    if( argc != 3 )
    {
        exit(0);
    }

    char * arg1 = argv[1];
    char * arg2 = argv[2];

    wavHeader * entrada = readWAV(arg1);

    if (entrada == NULL)
    {
        exit(0);
    }

    wavHeader * vol = volume( entrada, 0.5 );

    writeWAV( vol, arg2 );

    return 0;
}