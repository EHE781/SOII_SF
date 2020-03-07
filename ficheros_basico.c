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
    int escribir_bit(unsigned int nbloque, unsigned int bit) {
        unsigned char mascara = 128; // 10000000
        unsigned char *bufferMB;
        int posbyte = nbloque/8;
        int posbit = nbloque % 8;
        int nbloqueabs = posbyte / BLOCKSIZE + SB.posPrimerBloqueMB;
        bread(nbloqueabs,bufferMB);
        posbyte = posbyte % BLOCKSIZE;
        mascara >>= posbit; // desplazamiento de bits a la derecha
        if(bit == 0){
            bufferMB[posbyte] &= ~mascara; 
        }
        else{
            bufferMB[posbyte] |= mascara; 
        }
        bwrite(nbloqueabs,bufferMB);
    }
unsigned char leer_bit(unsigned int nbloque){
        unsigned char mascara = 128; // 10000000
        unsigned char *bufferMB;
        int posbyte = nbloque / 8;
        int posbit = nbloque % 8;
        int nbloqueabs = posbyte / BLOCKSIZE + SB.posPrimerBloqueMB;
        bread(nbloqueabs,bufferMB);
        posbyte = posbyte % BLOCKSIZE;
        mascara >>= posbit;
        bufferMB[posbyte] &= mascara; // p.e->posbit=3 --> resultado-> 000x0000
        if(bufferMB[posbyte] == 0){ //si x=0
            return 0;
        }
        else{ // si x=1
            return 1;
        }

}
int reservar_bloque(){
    int cmp=0;
    unsigned int nbloque;
    bool equal=true;
    bool equalv2=true;
    int posbyte=0;
    int posBloqueMB=SB.posPrimerBloqueMB;
    const void *bufferMB;
    const void *bufferAux;
    unsigned char *byteBuff;
    memset(bufferAux,255,BLOCKSIZE);
    if(SB.cantBloquesLibres>0){
        while(equal == true){
        bread(posBloqueMB,bufferMB);
        cmp=memcmp(bufferMB,bufferAux,BLOCKSIZE);
        if(cmp!=0){
            equal=false;
        }
        posBloqueMB++;
        }
        posBloqueMB=posBloqueMB-SB.posPrimerBloqueMB;
        memcpy(byteBuff, bufferMB, BLOCKSIZE);//ella lo hace con bufferMB pero no me va, ask coti
        for (int i = 0; i < BLOCKSIZE; i++)
        if(byteBuff[i] != 255){
            posbyte = i;
            break;
        }
        unsigned char mascara = 128; // 10000000
        int posbit = 0;
        while (byteBuff[posbyte] & mascara) {  
            posbit++;
            byteBuff[posbyte] <<= 1; // desplaz. de bits a la izqda
        }
        nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE+ posbyte) * 8 + posbit;
        escribir_bit(nbloque, 1);
        return nbloque;
        
    }
    else{
        //no hay bloques disponibles
        return EXIT_FAILURE;
    }
}
int liberar_bloque(unsigned int nbloque){
    unsigned char mascara = 128; // 10000000
    unsigned char *bufferMB;
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueabs = posbyte / BLOCKSIZE + SB.posPrimerBloqueMB;
    bread(nbloqueabs,bufferMB);
    mascara>>=posbit;
    bufferMB[posbyte]&=~mascara;
    bwrite(nbloqueabs,bufferMB);
    SB.cantBloquesLibres++;
    return nbloque;
}


