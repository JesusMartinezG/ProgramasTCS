#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"wav.h"

int main( int argc, char *argv[] )
{
    if( argc != 3 )
    {
        printf("Error en los argumento de ejecucion\n%s <archivo de entrada> <archivo de salida>\n",argv[0]);
        exit(0);
    }

    char * arg1 = argv[1];
    char * arg2 = argv[2];

    wavHeader * entrada = readWAV(arg1);

    if (entrada == NULL)
    {
        printf("ERROR: No se reconocio el archivo de entrada como un archivo WAV\n");
    }

    showWAVinfo(entrada);

    wavHeader * vol = volume( entrada, 0.5 );

    writeWAV( vol, arg2 );

    return 0;
}