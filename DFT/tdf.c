#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include "wav.h"

// Guarda la fft dentro de la misma estructura
// Solo considera archivos WAV monoaurales
// Guarda los resultados en un arreglo de 32 bits

void DFT( wavHeader wav )
{
    if (wav.numChannels != 1) // Si el archivo no es monoaural no hagas nada
        return;
    
    double pi2 = 2.0 * M_PI;
    double ang, cosA, sinA;
    double invs = 1.0 / wav.dataArraySize;

    uint32_t * out = (uint16_t*)malloc(sizeof(uint16_t));

    for(unsigned int y = 0;y < wav.dataArraySize; y++) //Para cada muestra en la estructura
    {
        output[y] = 0;
        for(unsigned int x = 0;x < size;x++) {
            angleTerm = pi2 * y * x * invs;
            cosineA = cos(angleTerm);
            sineA = sin(angleTerm);
            output_seq[y].real += input_seq[x].real * cosineA - input_seq[x].imag * sineA;
            output_seq[y].imag += input_seq[x].real * sineA + input_seq[x].imag * cosineA;
        }
        output_seq[y] *= invs;
    }
}