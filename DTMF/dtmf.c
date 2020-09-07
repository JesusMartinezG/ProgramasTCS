#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
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

uint32_t buffer_to_uint32 ( uint8_t buffer[]) //Recibe un arreglo de bytes little endian, toma numBytes elementos del arreglo y los convierte en un entero de 32 bits
{
    uint32_t integer = buffer[0];
    integer += buffer[1]<<8;
    integer += buffer[2]<<16;
    integer += buffer[3]<<24;

    return integer;
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

double tdf_i (WAV * wav, int n)
{
    double pi2 = 2.0 * PI;
    double ang, cosA, senA;
    double invs = 1.0 / wav->dataArraySize; // 1/N
    double auxreal=0.0;
    double auximag=0.0;
    double magnitud=0.0;

    for(unsigned int x = 0;x < wav->dataArraySize;x++)
    {
        ang = pi2 * n * x * invs;
        cosA = cos(ang);
        senA = sin(ang);
        auxreal += ((int16_t)wav->data[x]) * cosA; // No se calcula la parte imaginaria de la formula
        auximag += ((int16_t)wav->data[x]) * senA;
    }
    auxreal *= invs;
    auximag *= invs;
    magnitud = sqrt(auxreal*auxreal + auximag*auximag);
    return magnitud;
}

char dtmf (WAV * entrada)
{
    if(entrada->numChannels != 1) return 'X'; // Si no es monoaural no hagas nada

    //Calcula los indices de las componentes necesarias
    int componentef[4]; // componentes de las filas dtmf
    componentef[0] = (697  * entrada->dataArraySize) / entrada->sampleRate;
    componentef[1] = (770  * entrada->dataArraySize) / entrada->sampleRate;
    componentef[2] = (852  * entrada->dataArraySize) / entrada->sampleRate;
    componentef[3] = (941  * entrada->dataArraySize) / entrada->sampleRate;
    int componentec[4]; // Componentes de las columnas dtmf
    componentec[0] = (1209 * entrada->dataArraySize) / entrada->sampleRate;
    componentec[1] = (1336 * entrada->dataArraySize) / entrada->sampleRate;
    componentec[2] = (1477 * entrada->dataArraySize) / entrada->sampleRate;
    componentec[3] = (1633 * entrada->dataArraySize) / entrada->sampleRate;
    
    //Calcula la magnitud de las 8 componentes de la dft para las frecuencias necesarias
    double tdf=0.0;
    double aux;
    int indices[2];
    
    for(int i=0; i<4; i++) // obtene el indcie de la magnitud maxima de las frecuencias fila
    {
        aux = tdf_i(entrada, componentef[i]);
        if(tdf < aux)
        {
            tdf = aux;
            indices[0] = i;
        }
    }

    tdf=0.0;
    for(int i=0; i<4; i++) // obtene el indcie de la magnitud maxima de las frecuencias columna
    {
        aux = tdf_i(entrada, componentec[i]);
        if(tdf < aux)
        {
            tdf = aux;
            indices[1] = i;
        }
    }

    char matriz_dtmf[4][4] = {{'1','2','3','A'},
                              {'4','5','6','B'},
                              {'7','8','9','C'},
                              {'*','0','#','D'}}; 

    return matriz_dtmf[indices[0]][indices[1]];
}

void cerrarWAV ( WAV ** datos )
{
    free((*datos)->data);
    free(*datos);
}

int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        printf("Error en los argumento de ejecucion\n%s <archivo de entrada>\n",argv[0]);
        exit(0);
    }

    char * arg1 = argv[1];

    WAV * entrada = abrirWAV(arg1);

    printf("%c\n", dtmf(entrada));

    cerrarWAV(&entrada);
    return(0);
}