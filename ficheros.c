#include "ficheros.h"
int mi_write_f(unsigned int ninodo, const void *buf_original,unsigned int offset ,unsigned int nbytes){
    struct inodo inodo;
    unsigned char *buf_bloque[BLOCKSIZE];
    int total;
    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 2) == 2){
        int primerBLogico = offset/BLOCKSIZE;
        int ultimoBLogico = (offset + nbytes - 1) / BLOCKSIZE;
        int desp1 = offset % BLOCKSIZE;
        int desp2 = (offset + nbytes) % BLOCKSIZE;
        int BFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        if(desp1 != 0){
            bread(BFisico, buf_bloque);
        }
        if(primerBLogico == ultimoBLogico){
            memcpy(buf_bloque + desp1, buf_original, desp2 + 1 - desp1);//-desp1...
            total += bwrite(BFisico, buf_bloque);
        }
        else{
            desp2 = offset + nbytes - 1;
            memcpy (buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            total += bwrite(BFisico, buf_bloque);
            for(int i = primerBLogico + 1; i != ultimoBLogico; i ++){
                BFisico = traducir_bloque_inodo(ninodo, i, 1);
                total += bwrite(BFisico, (buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE));
            }
            desp2 = desp2 % BLOCKSIZE;
            BFisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
            bread(BFisico, buf_bloque);
            memcpy (buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            total += bwrite(BFisico, buf_bloque);
        }
        /*Antiguo codigo
        memcpy(buf_bloque + desp1,buf_original,BLOCKSIZE - desp1);
        memcpy(buf_bloque + desp1,buf_original,desp2 - desp1 + 1);
        int total = bwrite(BFisico, buf_bloque);
        for(int i = primerBLogico + 1;i != ultimoBLogico;i++){
            memcpy(aux, (buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE), BLOCKSIZE);
            total += bwrite(traducir_bloque_inodo(ninodo, i, 1), aux);
        }
        int ultimoBFisico = traducir_bloque_inodo(ninodo,ultimoBLogico,1);
        if(desp2!=BLOCKSIZE){
            bread(ultimoBFisico,buf_bloque);
        }
        memcpy(buf_bloque,buf_original + (nbytes-desp2 - 1),desp2 + 1);
        total = total + bwrite(ultimoBFisico, buf_bloque);*/
        leer_inodo(ninodo, &inodo);//volvemos a leer el inodo, se han reservado bloques
        if(inodo.tamEnBytesLog < total){//modifica EOF fichero
            inodo.tamEnBytesLog = total; //Si lo escrito ocupa más que lo que ya habia
            inodo.ctime = time(NULL);// Actualizamos ctime, hemos modificado el inodo
        }
        inodo.mtime = time (NULL);//modificamos mtime, hemos escrito datos
        escribir_inodo(ninodo, inodo);//escribimos el inodo
        return total; //nbytes escritos que si va bien deberia ser total=nbytes
    }
    else{
        //el inodo no tiene permisos de escritura
        return EXIT_FAILURE;
    }
}
int mi_read_f(unsigned int ninodo, void *buf_original,unsigned int offset,unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    unsigned char *buf_bloque[BLOCKSIZE];
    if ((inodo.permisos & 4) == 4){
        int bytesLeidos = 0;
        if(offset >= inodo.tamEnBytesLog){
            bytesLeidos = 0;
            return bytesLeidos;
        }
        if((offset + nbytes) >= inodo.tamEnBytesLog){
            nbytes = inodo.tamEnBytesLog - offset;
        }
        int primerBLogico = offset/BLOCKSIZE;
        int ultimoBLogico = (offset + nbytes - 1) / BLOCKSIZE;
        int desp1 = offset % BLOCKSIZE;
        int desp2 = (offset + nbytes) % BLOCKSIZE;
        int BFisico = traducir_bloque_inodo(ninodo, primerBLogico, 0);
        if(primerBLogico == ultimoBLogico){
            bytesLeidos += bread(BFisico, buf_bloque);
            if(desp2 == 0){//caso en que se tenga que leer el ultimo bloque de una secuencia
                memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
            }else{
                memcpy(buf_original, buf_bloque + desp1, desp2 + 1 - desp1);
            }
        }
        else{
            //posible falta de deteccion de bloques vacíos error
            desp2 = offset + nbytes - 1;
            bytesLeidos += bread(BFisico, buf_bloque);
            memcpy (buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
            for(int i = primerBLogico + 1; i != ultimoBLogico; i ++){
                BFisico = traducir_bloque_inodo(ninodo, i, 0);
                    bytesLeidos += bread(BFisico, buf_bloque);
                    memcpy(buf_original, buf_bloque, BLOCKSIZE);
            }
            desp2 = desp2 % BLOCKSIZE;
            BFisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 0);
            bytesLeidos += bread(BFisico, buf_bloque);
            memcpy (buf_original, buf_bloque, desp2 + 1);
        }
        inodo.atime = time(NULL);
        escribir_inodo(ninodo, inodo);
        return bytesLeidos;
       
    }else
    {   //no tienes permisos de lectura
        return EXIT_FAILURE;
    }
    
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

    struct inodo inodo;

    leer_inodo(ninodo,&inodo);

    p_stat->tipo = inodo.tipo; 
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks; 
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    
    return EXIT_SUCCESS;
}
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    escribir_inodo(ninodo, inodo);
    return EXIT_SUCCESS;
}