#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include<string.h>
#include "wav.h"
#define PI 3.14159265358979323846

double * respuesta_pasabajas ( int f_corte, int f_muestreo, int num_muestras )
{
    /* Retorna un apuntador a un arreglo de doubles
    que contiene muestras de la respuesta al impulso
    de un circuito pasa bajas con las caracteristicas indicadas*/
    double RC = 1/(2*PI*f_corte);
    double * a = (double*)malloc(sizeof(double)*num_muestras); // Reserva memoria para las num_muestras muestras
    double f2c = 2*f_corte;
    double pif = f2c*PI;
    
    // Muestrea la función
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

    double * respuesta = respuesta_pasabajas(f_corte, f_muestreo, muestras_respuesta); // Obten 100 muestras de la respuesta al impulso del circuito pasa bajas con fc=2000Hz
    int32_t * salidai = (int32_t*)malloc(sizeof(int32_t)*tam_salida); // Reserva memoria para el archivo de salida
    double * salidad = (double*)malloc(sizeof(double)*tam_salida); // Reserva memoria para el arreglo de salida auxiliar
    

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

    // Guardar en un archivo WAV
    WAV * out = (WAV*)malloc(sizeof(WAV));
    out->numChannels = 1; // Mono
    out->bitsPerSample = senial.bitsPerSample; // Misma presicion que la entrada
    out->bytesperSample = out->bitsPerSample / 8;
    out->audioFormat = senial.audioFormat; // PCM
    out->sampleRate = senial.sampleRate; // 44100
    out->subchunk1Size = senial.subchunk1Size; // igual porque sigue siendo pcm
    memcpy(out->chunkID,senial.chunkID, sizeof(senial.chunkID[0])*4); // Copia los valores del arreglo original
    memcpy(out->format,senial.format, sizeof(senial.format[0])*4);
    memcpy(out->subchunk1ID,senial.subchunk1ID, sizeof(senial.subchunk1ID[0])*4);
    memcpy(out->subchunk2ID,senial.subchunk2ID, sizeof(senial.subchunk2ID[0])*4);

    //Calcula el tamaño de los chunks y otros valores
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
        printf("Error en los argumento de ejecucion\n%s <archivo de entrada> <archivo de salida>\n",argv[0]);
        exit(0);
    }

    char * arg1 = argv[1];
    char * arg2 = argv[2];

    WAV * entrada = abrirWAV(arg1);

    if(entrada == NULL) exit(0);

    printf("Archivo de entrada:\n");
    showWAVinfo(entrada);
    WAV * salida = convolucion(*entrada, 2000, 44100, 100);

    printf("\nArchivo de salida:\n");
    showWAVinfo(salida);
    writeWAV(salida, arg2);
    cerrarWAV(&entrada);
    cerrarWAV(&salida);
    return 0;
}