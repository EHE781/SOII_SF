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
int initMB(){
    int setBloques = SB.cantBloquesLibres; //cuantos bloques de datos se ponen a 0
    int bloque = SB.posPrimerBloqueDatos; //primer bloque de datos en el cual escribir
    void *buf[BLOCKSIZE];
    memset(buf, 0, BLOCKSIZE);  //preparamos un buffer tamaño BLOCKSIZE para escribir 0s
    for(int i = 0; i < setBloques; i ++){
        bwrite(bloque, buf);
        bloque ++;
    }
}
int initAI(){
    int contInodos = SB.posPrimerInodoLibre + 1; //conector de inodos incremental
    for ( int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
        for (int j = 0; j < MAX_INODO; j++){
            inodos[j].tipo = 'l'; //tipo libre
            if (contInodos < SB.totInodos){
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;                
            }else {
                inodos[j].punterosDirectos[0] = UINT_MAX; //ultimo inodo apunta a UNIT_MAX
                break;
            }
        }
        //memcpy(buf, &inodos, sizeof(inodos));
        bwrite(i, &inodos); // en teoria bien si no hacer con buf
    }
}
