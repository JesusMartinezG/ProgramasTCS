#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include<string.h>
#define PI 3.14159265358979323846

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

uint32_t buffer_to_uint ( uint8_t buffer[], uint8_t numBytes)
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

void uint_to_buffer ( uint32_t integer, uint8_t buffer[], uint8_t numBytes)
{
    uint8_t shift = 0;
    for(int i=0; i<numBytes; i++)
    {
        buffer[i] = (uint8_t)(integer>>shift);
        shift += 8;
    }
}

void sint_to_buffer ( int32_t integer, uint8_t buffer[], uint8_t numBytes)
{
    uint8_t shift = 0;
    for(int i=0; i<numBytes; i++)
    {
        buffer[i] = (uint8_t)(integer>>shift);
        shift += 8;
    }
}

int32_t buffer_to_sint ( uint8_t buffer[], uint8_t numBytes)
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

uint32_t buffer_to_uint32 ( uint8_t buffer[])
{
    uint32_t integer = buffer[0];
    integer += buffer[1]<<8;
    integer += buffer[2]<<16;
    integer += buffer[3]<<24;

    return integer;
}

int writeWAV ( WAV * datos ,char * filename)
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

    for ( int i=0; i<4; i++ ) putc( datos->chunkID[i], archivo );

    uint_to_buffer ( datos->chunkSize, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    for ( int i=0; i<4; i++ ) putc(datos->format[i], archivo );
    for ( int i=0; i<4; i++ ) putc( datos->subchunk1ID[i], archivo );

    uint_to_buffer ( datos->subchunk1Size, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    uint_to_buffer ( datos->audioFormat, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    uint_to_buffer ( datos->numChannels, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    uint_to_buffer ( datos->sampleRate, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    uint_to_buffer ( datos->byteRate, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    uint_to_buffer ( datos->blockAlign, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    uint_to_buffer ( datos->bitsPerSample, buffer, 2 );
    for ( int i=0; i<2; i++ ) putc(buffer[i], archivo );

    for ( int i=0; i<4; i++ ) putc( datos->subchunk2ID[i], archivo );

    uint_to_buffer ( datos->subchunk2Size, buffer, 4 );
    for ( int i=0; i<4; i++ ) putc(buffer[i], archivo );

    for( int i=0; i<datos->dataArraySize; i++ )
    {
        sint_to_buffer ( datos->data[i], buffer, datos->bytesperSample );
        for( int j=0; j<datos->bytesperSample; j++) putc(buffer[j], archivo);
    }

    fclose(archivo);
    return 0;
}

WAV * abrirWAV ( char * filename )
{
    FILE * archivo = fopen( filename, "rb" );

    if (archivo == NULL)
    {
        return NULL;
    }

    WAV * datos = (WAV*) malloc(sizeof(WAV));
    uint8_t buffer[4];

    
    for ( int i=0; i<4; i++ ) datos->chunkID[i] = getc( archivo );

    for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
    datos->chunkSize = buffer_to_uint32( buffer );

    for ( int i=0; i<4; i++ ) datos->format[i] = getc( archivo );

    if( !(datos->format[0] == 'W' && datos->format[1] == 'A' && datos->format[2] == 'V' && datos->format[3] == 'E') )
    {
        free( datos );
        fclose(archivo);
        return NULL;
    }

    for ( int i=0; i<4; i++ ) datos->subchunk1ID[i] = getc( archivo );

    for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
    datos->subchunk1Size = buffer_to_uint( buffer, 4 );

    for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
    datos->audioFormat = buffer_to_uint( buffer, 2 );

    for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
    datos->numChannels = buffer_to_uint( buffer, 2 );

    for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
    datos->sampleRate = buffer_to_uint( buffer, 4 );

    for ( int i=0; i<4; i++ ) buffer[i] = getc(archivo);
    datos->byteRate = buffer_to_uint( buffer, 4 );

    for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
    datos->blockAlign = buffer_to_uint( buffer, 2 );

    for ( int i=0; i<2; i++ ) buffer[i] = getc( archivo );
    datos->bitsPerSample = buffer_to_uint( buffer, 2 );

    for ( int i=0; i<4; i++ ) datos->subchunk2ID[i] = getc( archivo );

    for ( int i=0; i<4; i++ ) buffer[i] = getc( archivo );
    datos->subchunk2Size = buffer_to_uint( buffer, 4 );

    datos->bytesperSample = datos->bitsPerSample / 8;
    datos->dataArraySize = datos->subchunk2Size / datos->bytesperSample;

    datos->data = (int32_t*)malloc( sizeof(int32_t)*datos->dataArraySize );
    uint32_t j=0, k=0;

        for( int i=1; i<=datos->subchunk2Size; i++ )
        {
            buffer[j++] = getc( archivo );
            if( j == datos->bytesperSample )
            {
                datos->data[k++] = buffer_to_sint( buffer, datos->bytesperSample);
                j=0;
            }
        }
    fclose(archivo);
    return datos;
}

void cerrarWAV ( WAV ** datos )
{
    free((*datos)->data);
    free(*datos);
}

void printData ( WAV datos )
{
    for (int i=0; i<datos.dataArraySize; i++)
        printf("%i\n",(int16_t)datos.data[i]);

}

double * respuesta_pasabajas ( int f_corte, int f_muestreo, int num_muestras )
{
    //double RC = 1/(2*PI*f_corte);
    double * a = (double*)malloc(sizeof(double)*num_muestras);
    double f2c = 2*f_corte;
    double pif = f2c*PI;
    
    a[0] = f2c;
    for(int i=1; i<num_muestras; i++)
    {
        a[i] = f2c * (sin(pif*i/f_muestreo)/(pif*i/f_muestreo));
    }

    return a;
}

WAV * convolucion ( WAV senial, int f_corte, int f_muestreo, int muestras_respuesta )
{
    int tam_salida = senial.dataArraySize + muestras_respuesta - 1;

    double * respuesta = respuesta_pasabajas(f_corte, f_muestreo, muestras_respuesta);
    int32_t * salidai = (int32_t*)malloc(sizeof(int32_t)*tam_salida);
    double * salidad = (double*)malloc(sizeof(double)*tam_salida);
    

    for (int n = 0; n < tam_salida; n++)
    {
        int kmin, kmax, k;

        salidad[n] = 0;

        kmin = (n >= muestras_respuesta - 1) ? n - (muestras_respuesta - 1) : 0;
        kmax = (n < senial.dataArraySize - 1) ? n : senial.dataArraySize - 1;

        for (k = kmin; k <= kmax; k++)
        {
            salidad[n] += senial.data[k] * respuesta[n - k];
        }
        salidad[n] /= (tam_salida);
        salidai[n] = (int16_t)salidad[n];
    }

    WAV * out = (WAV*)malloc(sizeof(WAV));
    out->numChannels = 1;
    out->bitsPerSample = senial.bitsPerSample;
    out->bytesperSample = out->bitsPerSample / 8;
    out->audioFormat = senial.audioFormat;
    out->sampleRate = senial.sampleRate;
    out->subchunk1Size = senial.subchunk1Size;
    memcpy(out->chunkID,senial.chunkID, sizeof(senial.chunkID[0])*4);
    memcpy(out->format,senial.format, sizeof(senial.format[0])*4);
    memcpy(out->subchunk1ID,senial.subchunk1ID, sizeof(senial.subchunk1ID[0])*4);
    memcpy(out->subchunk2ID,senial.subchunk2ID, sizeof(senial.subchunk2ID[0])*4);

    out->byteRate = out->sampleRate * out->numChannels * (out->bytesperSample);
    out->blockAlign = out->numChannels * out->bytesperSample;
    out->dataArraySize = tam_salida;
    out->subchunk2Size = out->dataArraySize * out->bytesperSample;
    out->chunkSize = 4 + (8 + out->subchunk1Size) + (8 + out->subchunk2Size);

    out->data = salidai;

    return out;
}

int main( int argc, char *argv[] )
{
    if( argc != 3 )
    {
        exit(0);
    }

    char * arg1 = argv[1];
    char * arg2 = argv[2];

    WAV * entrada = abrirWAV(arg1);

    if(entrada == NULL) exit(0);

    WAV * salida = convolucion(*entrada, 2000, 44100, 100);

    writeWAV(salida, arg2);
    cerrarWAV(&entrada);
    cerrarWAV(&salida);
    return 0;
}