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
    /*
    int primero = SB.posPrimerBloqueAI + 1;     //primer bloque en el que se empieza a escribir
    void *buf; //buffer para almacenar datos y posteriormente escribirlas
    buf = (struct inodo*)inodos;
    int inodoCont = 0; //variable contador porque sabemos que caben BLOCKSIZE/INODOSIZE inodos en un bloque;
    for (int i = 0; i < SB.posUltimoBloqueAI; i++){ //un for para todos los inodos
        if (i == (SB.posUltimoBloqueAI - 1)){ //si es la ultima iteración
            conIn = UINT_MAX;   //apuntar al maximo entero sin signo
        }
        if (inodoCont == (MAX_INODO)){ // si se han escrito los inodos maximos en un bloque
            memcpy(buf, &inodos, sizeof(inodos)); //escribimos los inodos que tenemos con tamaño del array
            bwrite(primero, buf);   //escribe el bloque y lo incrementa
            primero ++;
            inodoCont = 0; //reset del contador de inodos
        }
        inodos[inodoCont].punterosDirectos[0] = conIn;
        conIn ++; //incrementamos el valor, apunta al inodo siguiente, luego al siguiente....
        inodoCont ++; //decrementamos en 1 la cantidad de inodos posibles
    }
       IMPORTANTE, HACE FALTA ESCRIBIR 0S DONDE NO HAY MAS DATOS EN EL ULTIMO BLOQUE?
    if (inodoCont != 0 && inodoCont != MAX_INODO) {//SI no se ha escrito todo el bloque y no es el bloque siguiente(escrito bloque completo)
        int noEscrito = MAX_INODO - inodoCont; //los que caben - los que ya ha escrito = espacio no escrito
        memset(buf, 0, noEscrito);
        bwrite(primero, buf);   //rellena de 0s el espacio restante
    }*/
}
