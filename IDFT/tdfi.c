#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include<string.h>
#include "wav.h"
#define PI 3.14159265358979323846

// Lee un archivo wav stereo
// El canal 1 representa la parte real y el canal 2 la parte imaginaria
// Guarda los resultados en un archivo mono

WAV * IDFT( WAV wav ) // wav solo para lectura
{
    if (wav.numChannels != 2) // Si el archivo no es stereo no hagas nada
        return NULL;
    
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

    //Calcula el tamaño de los chunks y otros valores
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
        printf("Error en los argumento de ejecucion\n%s <archivo de entrada> <archivo de salida>\n",argv[0]);
        exit(0);
    }

    char * arg1 = argv[1];
    char * arg2 = argv[2];

    WAV * entrada = abrirWAV(arg1);
    printf("Información del archivo de entrada\n");
    showWAVinfo(entrada);

    WAV * salida = IDFT(*entrada);
    printf("\n\nInformación del archivo de salida\n");
    showWAVinfo(salida);
    writeWAV(salida,arg2);

    cerrarWAV(&entrada);
    cerrarWAV(&salida);
    return 0;
}