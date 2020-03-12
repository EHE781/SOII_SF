#include "ficheros_basico.h"
struct superbloque SB;
/*DEVUELVE EL TAMAÑO DEL MAPA DE BITS*/
int tamMB(unsigned int nbloques){
    int size = ((nbloques/8)/BLOCKSIZE);
    double byteSize = nbloques/8;
    double blockSize = byteSize/BLOCKSIZE;
    double check = blockSize - (unsigned) blockSize;
    if(check != 0){ //si el tamaño de asignacion es decimal (12,3), añadir un bloque (13)
        size ++;
    }
    return size;
}
/*DEVUELVE EL TAMAÑO DEL ARRAY DE INODOS*/
int tamAI(unsigned int ninodos){
    int size = 0;
    if((size = ((ninodos * INODOSIZE) / BLOCKSIZE)) % 1 != 0){  //si se necesita mas espacio para los inodos
        size ++;
    }
    return size;
}
/*INICIALIZA EL SUPERBLOQUE*/
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
    return bwrite(posSB, buf);
}
/*INICIALIZA EL MAPA DE BITS*/
int initMB(){
    int setBloques = SB.cantBloquesLibres; //cuantos bloques de datos se ponen a 0
    int bloque = SB.posPrimerBloqueDatos; //primer bloque de datos en el cual escribir
    void *buf[BLOCKSIZE];
    memset(buf, 0, BLOCKSIZE);  //preparamos un buffer tamaño BLOCKSIZE
    int result;
    for(int i = 0; i < setBloques; i ++){
        result = bwrite(bloque, buf);
        bloque ++;
    }
    for(int i = 0; i < SB.posPrimerBloqueDatos; i++){ //reservamos los bloques de los META-datos
    reservar_bloque();
    }
    return result;
}
/*INICIALIZA EL ARRAY DE INODOS*/
int initAI(){
    int contInodos = SB.posPrimerInodoLibre + 1; //conector de inodos incremental
    int result;
    for ( int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
        for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++){
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
        result = bwrite(i, &inodos); // en teoria bien si no hacer con buf
    }
    return result;
}
/*ESCRIBE UN BIT PASADO POR PARÁMETRO EN LA POSICIÓN DEL BLOQUE
PASADO POR PARÁMETRO EN EL MAPA DE BITS*/
    int escribir_bit(unsigned int nbloque, unsigned int bit) {
        unsigned char mascara = 128; // 10000000
        unsigned char *bufferMB = malloc(BLOCKSIZE);
        int posbyte = nbloque / 8;
        int posbit = nbloque % 8;
        int nbloqueabs = posbyte / BLOCKSIZE + SB.posPrimerBloqueMB;
        bread(nbloqueabs, bufferMB);
        posbyte = posbyte % BLOCKSIZE;
        mascara >>= posbit; // desplazamiento de bits a la derecha
        if(bit == 0){
            bufferMB[posbyte] &= ~mascara; 
        }
        else{
            bufferMB[posbyte] |= mascara; 
        }
        bwrite(nbloqueabs,bufferMB);
        free(bufferMB);
        return EXIT_SUCCESS;
    }

unsigned char leer_bit(unsigned int nbloque){
        unsigned char mascara = 128; // 10000000
        unsigned char *bufferMB = malloc(BLOCKSIZE);
        int posbyte = nbloque / 8;
        int posbit = nbloque % 8;
        int nbloqueabs = posbyte / BLOCKSIZE + SB.posPrimerBloqueMB;
        bread(nbloqueabs, bufferMB);
        posbyte = posbyte % BLOCKSIZE;
        mascara >>= posbit;
        mascara &= bufferMB[posbyte]; // p.e->posbit=3 --> resultado-> 000x0000
        mascara >>= (7-posbit);         // desplazamiento de bits a la derecha
        free(bufferMB);
        if(mascara == 0){ //si x=0
            return EXIT_SUCCESS;
        }
        else{ // si x=1
            return 1;
        }

}
int reservar_bloque(){
    if(SB.cantBloquesLibres == 0){
        return EXIT_FAILURE;
    }
    int cmp=0;
    unsigned int nbloque;
    bool equal = true;
    int posbyte = 0;
    int posBloqueMB = SB.posPrimerBloqueMB;
    unsigned char *bufferMB = malloc(BLOCKSIZE);
    const unsigned char *bufferAux = malloc(BLOCKSIZE);
    memset(bufferAux,255,BLOCKSIZE); //problem here
        while(equal == true){
        bread(posBloqueMB,bufferMB);
        cmp=memcmp(bufferMB,bufferAux,BLOCKSIZE);
        if(cmp != 0){
            equal=false;
            break;
        }
        posBloqueMB++;
        }
        for (int i = 0; i < BLOCKSIZE; i++)
        if(bufferMB[i] != 255){
            posbyte = i;
            break;
        }
        unsigned char mascara = 128; // 10000000
        int posbit = 0;
        while (bufferMB[posbyte] & mascara) {  
            posbit++;
            bufferMB[posbyte] <<= 1; // desplaz. de bits a la izqda
        }
        nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
        escribir_bit(nbloque, 1);
        SB.cantBloquesLibres--;
        bwrite(posSB, &SB);
        free(bufferMB);
        free(bufferAux);
        return nbloque;
}
int liberar_bloque(unsigned int nbloque){
    unsigned char mascara = 128; // 10000000
    unsigned char *bufferMB = malloc(BLOCKSIZE);
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueabs = posbyte / BLOCKSIZE + SB.posPrimerBloqueMB;
    bread(nbloqueabs,bufferMB);
    posbyte = posbyte % BLOCKSIZE;
    mascara>>=posbit;
    bufferMB[posbyte] &= ~mascara;
    bwrite(nbloqueabs,bufferMB);
    SB.cantBloquesLibres++;
    bwrite(posSB, &SB); //actualizamos el SB
    free(bufferMB);
    return nbloque;
}
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
    int result; //variable de gestión de errores
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; //buffer para los inodos
    int posBloque = ninodo/INODOSIZE + SB.posPrimerBloqueAI; //posicion real del bloque
    result = bread(posBloque, inodos);   //leemos el bloque que contiene el inodo
    if (result == EXIT_FAILURE){ //es necesario???
        return EXIT_FAILURE;
    }
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)]=inodo; //escribimos el inodo en la posicion que le toca
    bwrite(posBloque, inodos); //volvemos a escribir el bloque en memoria
    return EXIT_SUCCESS;
}
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    int result; //variable de gestión de errores
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; //buffer para los inodos
    struct inodo inodoLeido;
    int posBloque = ninodo/INODOSIZE + SB.posPrimerBloqueAI; //posicion real del bloque
    result = bread(posBloque, inodos);   //leemos el bloque que contiene el inodo
    if (result == EXIT_FAILURE){ //es necesario???
        return EXIT_FAILURE;
    }
    *inodo = inodos[ninodo%BLOCKSIZE/INODOSIZE]; //escribimos el inodo en la posicion que le toca
    return EXIT_SUCCESS; //volvemos a escribir el bloque en memoria
}
int reservar_inodo(unsigned char tipo, unsigned char permisos){
    if(SB.cantInodosLibres == 0){
        return EXIT_FAILURE;
    }
    struct inodo inodo;
    int posInodoReservado = SB.posPrimerInodoLibre; //SB.posPrimerBloqueAI quitado, posicion del inodo a reservar
    SB.posPrimerInodoLibre++;
    SB.cantInodosLibres--;
    bwrite(posSB, &SB); //actualizamos SB
    inodo.tipo = tipo;
    inodo.permisos = permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = inodo.numBloquesOcupados = 0;
    inodo.atime = inodo.ctime = inodo.mtime = time(NULL);
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 3; j++){
            inodo.punterosIndirectos[j] = 0;
        }
        inodo.punterosDirectos[i] = 0;
    }
    escribir_inodo(posInodoReservado, inodo);
    return posInodoReservado;
}

