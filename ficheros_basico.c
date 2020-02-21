#include "ficheros_basico.h"
struct superbloque SB;

int tamMB(unsigned int nbloques){
    int size = 0;
    if((size = ((nbloques/8)/BLOCKSIZE)) % 1 != 0){ //si el tamaño de asignacion es decimal (12,3), añadir un bloque (13)
        size ++;
    }
    return size;
}

int tamAI(unsigned int ninodos){
    int size = 0;
    if((size = ((ninodos * INODOSIZE) / BLOCKSIZE)) % 1 != 0){  //si se necesita mas espacio para los inodos
        size ++;
    }
    return size;
}

int initSB(unsigned int nbloques, unsigned int ninodos){
    void *buf = &SB; //asignamos el buffer a la estructura de SB
    SB.posPrimerBloqueMB = posSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0; //DESPUES DE P3, SE PONE A 1
    SB.cantBloquesLibres = nbloques;  //RECORDAR: al reservar un bloque, hacer SB.cantBloquesLibres --, y al liberar ++
    SB.cantInodosLibres = ninodos; //recordar lo mismo que con los bloques libres
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;
    bwrite(posSB, buf);
}
///continuara...