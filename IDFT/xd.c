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
void cerrarWAV ( WAV ** datos );

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

void sint_to_buffer ( int32_t integer, uint8_t buffer[], uint8_t numBytes) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
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

uint32_t buffer_to_uint32 ( uint8_t buffer[] ) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
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
        printf("Entrada no valida\n");
        exit(0);
    }

    FILE * archivo = fopen( filename, "wb" );

    if (archivo == NULL)
    {
        printf("No se pudo crear\n");
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
    for( int i=0; i<datos->dataArraySize; i++ )
    {
        sint_to_buffer ( datos->data[i], buffer, datos->bytesperSample );
        for( int j=0; j<datos->bytesperSample; j++) putc(buffer[j], archivo);
    }

    fclose(archivo);
    return 0;
}

WAV * abrirWAV ( char * filename ) //
{
    FILE * archivo = fopen( filename, "rb" ); // Abrir en modo binario

    if (archivo == NULL) // No se pudo abrir el archivo
    {
        printf("ERROR: No se pudo abrir el archivo de entrada %s\n", filename);
        return NULL;
    }

    WAV * datos = (WAV*) malloc(sizeof(WAV)); // Reserva memoria para el contenendor WAV
    uint8_t buffer[4];

    
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
        printf("El archivo no es WAV");
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
    datos->dataArraySize = datos->subchunk2Size / datos->bytesperSample;

    datos->data = (int32_t*)malloc( sizeof(int32_t)*datos->dataArraySize );
    uint32_t j=0, k=0;

        for( int i=1; i<=datos->subchunk2Size; i++ )
        {
            buffer[j++] = getc( archivo );
            if( j == datos->bytesperSample ) // Si el byte actual es multiplo del numero de bytes por muestra
            {
                datos->data[k++] = buffer_to_sint( buffer, datos->bytesperSample); //Convierte las muestras en el buffer
                //printf("%i\n", (int16_t)datos->data[k-1]);
                j=0; // Vuelve a escribir desde el inicio del buffer
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

// Lee un archivo wav stereo
// El canal 1 representa la parte real y el canal 2 la parte imaginaria
// Guarda los resultados en un archivo mono

WAV * IDFT( WAV wav ) // wav solo para lectura
{
    if (wav.numChannels != 2) // Si el archivo no es stereo no hagas nada
    {
        printf("El archivo no es stereo\n");
        return NULL;
    }
    
    double pi2 = 2.0 * PI;
    double ang, cosA, senA;
    double invs = 1.0 / (wav.dataArraySize / 2); // 1/N
    double aux = 0.0;

    WAV * out = (WAV*)malloc(sizeof(WAV));
    out->numChannels = 1;// 1 canal pues la salida es un numero real
    out->bitsPerSample = wav.bitsPerSample; // Misma presicion que la entrada
    out->bytesperSample = out->bitsPerSample / 8;
    out->audioFormat = wav.audioFormat; // PCM
    out->sampleRate = wav.sampleRate; // 44100
    memcpy(out->chunkID,wav.chunkID, sizeof(wav.chunkID[0])*4); // Copia los valores del arreglo original
    memcpy(out->format,wav.format, sizeof(wav.format[0])*4);
    memcpy(out->subchunk1ID,wav.subchunk1ID, sizeof(wav.subchunk1ID[0])*4);
    memcpy(out->subchunk2ID,wav.subchunk2ID, sizeof(wav.subchunk2ID[0])*4);

    out->byteRate = out->sampleRate * out->numChannels * (out->bytesperSample);
    out->blockAlign = out->numChannels * out->bytesperSample;
    out->dataArraySize = wav.dataArraySize / 2; // La mitad de muestras en la salida por ser un archivo mono
    out->subchunk1Size = wav.subchunk1Size; // igual porque sigue siendo pcm
    out->subchunk2Size = out->dataArraySize * out->bytesperSample;
    out->chunkSize = 4 + (8 + out->subchunk1Size) + (8 + out->subchunk2Size);

    //Reserva memoria para el arreglo de datos
    out->data = (int32_t*)malloc(sizeof(int32_t)*out->dataArraySize);

    for(int y=0; y < out->dataArraySize; y++) // Para cada muestra de la salida
    {
        aux = 0.0;
        for(int x=0; x < out->dataArraySize; x++) // Usa cada muestra de la entrada
        {
            ang = pi2 * y * x * invs;
            cosA = cos(ang);
            senA = sin(ang);
            aux += ((int16_t)wav.data[(x*2)]) * cosA - ((int16_t)wav.data[(x*2)+1]) * senA;
        }
        if(aux < INT16_MIN)
        {
            out->data[out->dataArraySize-y] = INT16_MAX;
        }
        else if (aux > INT16_MAX)
        {
            out->data[out->dataArraySize-y] = INT16_MIN;
        }
        else
        {
            out->data[out->dataArraySize-y] = -aux;
        }
    }

    return out;
}

int main( int argc, char *argv[] )
{
    if( argc != 3 )
    {
        printf("Error en los argumentos de ejecucion\n%s <archivo de entrada> <archivo de salida>\n",argv[0]);
        exit(0);
    }

    char * arg1 = argv[1];
    char * arg2 = argv[2];

    WAV * entrada = abrirWAV(arg1);
    WAV * salida = IDFT(*entrada);
    writeWAV(salida,arg2);

    cerrarWAV(&entrada);
    cerrarWAV(&salida);
    return 0;
}