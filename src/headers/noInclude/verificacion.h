//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
//verificacion.h
#include "../headers/noInclude/simulacion.h"

struct INFORMACION {
    int pid; 
    unsigned int nEscrituras; // validadas 
    struct REGISTRO PrimeraEscritura; 
    struct REGISTRO UltimaEscritura;
    struct REGISTRO MenorPosicion;
    struct REGISTRO MayorPosicion;
};
