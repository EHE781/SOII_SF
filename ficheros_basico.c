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
    bread(posSB, &SB);
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
        bread(posSB, &SB);
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
        bread(posSB, &SB);
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
    bread(posSB, &SB);
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
    bread(posSB, &SB);
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
    bread(posSB, &SB);
    int result; //variable de gestión de errores
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; //buffer para los inodos
    int posBloque = ninodo/(BLOCKSIZE/INODOSIZE) + SB.posPrimerBloqueAI; //posicion real del bloque
    result = bread(posBloque, inodos);   //leemos el bloque que contiene el inodo
    if (result == EXIT_FAILURE){ 
        return EXIT_FAILURE;
    }
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)]=inodo; //escribimos el inodo en la posicion que le toca
    bwrite(posBloque, inodos); //volvemos a escribir el bloque en memoria
    return EXIT_SUCCESS;
}
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    bread(posSB, &SB);
    int result; //variable de gestión de errores
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; //buffer para los inodos
    struct inodo inodoLeido;
    int posBloque = ninodo/(BLOCKSIZE/INODOSIZE) + SB.posPrimerBloqueAI; //posicion real del bloque
    result = bread(posBloque, inodos);   //leemos el bloque que contiene el inodo
    if (result == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    inodoLeido = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
    memcpy(inodo, &inodoLeido, INODOSIZE);
    return EXIT_SUCCESS; //volvemos a escribir el bloque en memoria
}
int reservar_inodo(unsigned char tipo, unsigned char permisos){
    bread(posSB, &SB);
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
int obtener_nrangoBL(struct inodo inodo,unsigned int nblogico, unsigned int *ptr){
    bread(posSB, &SB);
    if(nblogico < DIRECTOS){
        *ptr = inodo.punterosDirectos[nblogico];
        return 0;
    }
    else if(nblogico < INDIRECTOS0){
        *ptr = inodo.punterosIndirectos[0];
        return 1;
    }
    else if(nblogico < INDIRECTOS1){
        *ptr = inodo.punterosIndirectos[1];
        return 2;
    }
    else if(nblogico < INDIRECTOS2){
        *ptr = inodo.punterosIndirectos[2];
        return 3;
    }
    else{
        *ptr = 0;    
        return EXIT_FAILURE;
    }
}
int obtener_indice(int nblogico,int nivel_punteros){
    if(nblogico < DIRECTOS){
        return nblogico;
    }
    else if (nblogico < INDIRECTOS0){
        return nblogico - DIRECTOS;
    }
    else if (nblogico < INDIRECTOS1){
        if(nivel_punteros == 2){
            return(nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        else if(nivel_punteros == 1){
            return(nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }
    else if (nblogico < INDIRECTOS2){
        if (nivel_punteros == 3){
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        else if(nivel_punteros == 2){
            return((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }
        else if(nivel_punteros == 1){
            return((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;
        }
    }
}
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar){
    bread(posSB, &SB);
    struct inodo inodo;
    int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice;
    int buffer[NPUNTEROS];  
    leer_inodo (ninodo, &inodo);
    ptr = ptr_ant = salvar_inodo = 0;
    nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
    nivel_punteros = nRangoBL; //el nivel_punteros +alto es el que cuelga del inodo
    while (nivel_punteros > 0){ //iterar para cada nivel de indirectos
        if(ptr == 0){ //no cuelgan bloques de punteros
            if(reservar == 0 ){
                return EXIT_FAILURE;//error lectura bloque inexistente
            }   
            else{//reservar bloques punteros y crear enlaces desde inodo hasta datos
                salvar_inodo = 1;
                ptr = reservar_bloque(); //de punteros          
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL); //fecha actual
                if(nivel_punteros == nRangoBL){
                //el bloque cuelga directamente del inodo
                inodo.punterosIndirectos[nRangoBL-1] = ptr; // (imprimirlo para test)
                }   
                else {   //el bloque cuelga de otro bloque de punteros
                buffer[indice] = ptr;// (imprimirlo para test)
                bwrite(ptr_ant, buffer);
                }
            }
        }
        bread(ptr, buffer); 
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr; //guardamos el puntero
        ptr = buffer[indice]; // y lo desplazamos al siguiente nivel
        nivel_punteros--;    
    }  //al salir de este bucle ya estamos al nivel de datos
   
   if (ptr == 0){ //no existe bloque de datos
      if (reservar == 0){  //error lectura ∄ bloque  
        return EXIT_FAILURE;
      }  
      else{
         salvar_inodo = 1;
         ptr = reservar_bloque(); //de datos
         inodo.numBloquesOcupados++;
         inodo.ctime = time(NULL);
         if(nRangoBL == 0){
            inodo.punterosDirectos[nblogico] = ptr; // (imprimirlo para test)
         }
         else{
            buffer[indice] = ptr; // (imprimirlo para test)
            bwrite(ptr_ant, buffer);
        }
      }
    }
    if (salvar_inodo == 1){
        escribir_inodo(ninodo, inodo);  //sólo si lo hemos actualizado
    }
    /*
    //CHECKEADOR START (BORRABLE)
    if(nRangoBL == 0){
    printf("[traducir_bloque_inodo()→ inodo.punterosDirectos[%i] = %i (reservado BF %i para BL %i)]\n",
    nblogico,nblogico+SB.posPrimerBloqueDatos,nblogico+SB.posPrimerBloqueMB,nblogico);
    }else if(nRangoBL < 2 ){
        printf("[traducir_bloque_inodo()→ inodo.punteros_nivel1[%i] = %i (reservado BF %i para BL %i)]\n",
    indice,nblogico+SB.posPrimerBloqueDatos,nblogico+SB.posPrimerBloqueMB,nblogico);
    }else if(nRangoBL < 3){
        printf("[traducir_bloque_inodo()→ inodo.punteros_nivel2[%i] = %i (reservado BF %i para BL %i)]\n",
    indice,nblogico+SB.posPrimerBloqueDatos,nblogico+SB.posPrimerBloqueMB,nblogico);
    }else {
        printf("[traducir_bloque_inodo()→ inodo.punteros_nivel3[%i] = %i (reservado BF %i para BL %i)]\n",
    indice,nblogico+SB.posPrimerBloqueDatos,nblogico+SB.posPrimerBloqueMB,nblogico);
    }
    //CHECKEADOR END (BORRABLE)
    */
   return ptr; //nbfisico
}
int liberar_bloques_inodo(unsigned int primerBL,struct inodo *inodo) {
    unsigned int nRangoBL,nivel_punteros,indice,ptr,nBL,ultimoBL;
    int bloques_punteros[3][NPUNTEROS];
    int ptr_nivel[3];
    int indices[3];
    int liberados;

    liberados = 0;
    if (inodo -> tamEnBytesLog == 0){
        return 0;
    }
    if (inodo -> tamEnBytesLog % BLOCKSIZE == 0) {
        ultimoBL = inodo -> tamEnBytesLog/BLOCKSIZE - 1;
    }else{
        ultimoBL = inodo -> tamEnBytesLog / BLOCKSIZE;
    }
    ptr = 0;
    for (nBL = primerBL ; nBL == ultimoBL ; nBL++){ // !!!!!!!!!
        nRangoBL = obtener_nrangoBL(*inodo,nBL,&ptr);
        if (nRangoBL < 0){
            return EXIT_FAILURE;
        }
        nivel_punteros = nRangoBL;
        while (ptr > 0 && nivel_punteros > 0){
            indice = obtener_indice(nBL,nivel_punteros);
            if( indice == 0 || nBL == primerBL){
                bread(ptr,bloques_punteros[nivel_punteros - 1]);
            }
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros -- ;
        }
        if (ptr > 0){
            liberar_bloque(ptr);
            liberados++;
            if (nRangoBL == 0){
                inodo -> punterosDirectos[nBL] = 0;
            }else{
                while (nivel_punteros < nRangoBL){
                    indice = indices[nivel_punteros];
                    bloques_punteros[nivel_punteros][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros];
                    if (bloques_punteros[nivel_punteros] == 0){
                        liberar_bloque(ptr);
                        liberados ++;
                        nivel_punteros ++;
                        if (nivel_punteros = nRangoBL){
                            inodo -> punterosIndirectos[nRangoBL - 1] = 0;
                        }
                    }else{
                        bwrite(ptr,bloques_punteros[nivel_punteros]);
                        nivel_punteros = nRangoBL;
                    }
                }
            }
        }
    }
    return liberados;
}
int liberar_inodo(unsigned int ninodo){
    struct superbloque SB;
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    inodo.numBloquesOcupados -= liberar_bloques_inodo(0, &inodo);
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;
    bread(posSB, &SB);
    SB.cantInodosLibres++;
    inodo.punterosDirectos[0] = SB.posPrimerInodoLibre; //hacemos que apunte al primer inodo libre
    SB.posPrimerInodoLibre = ninodo; //y el primer libre ahora es el inodo del parámetro
    escribir_inodo(ninodo,inodo); //escribir el inodo
    bwrite(posSB,&SB); //actualizamos superbloque
    return ninodo; //devolver el número del inodo liberado
}



