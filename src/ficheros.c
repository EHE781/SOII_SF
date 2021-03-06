//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "../headers/ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original,unsigned int offset ,unsigned int nbytes){
    struct inodo inodo;
    void *buf_bloque =  malloc(BLOCKSIZE);
    int total = 0;
    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 2) == 2){
        int primerBLogico = offset/BLOCKSIZE;
        int ultimoBLogico = (offset + nbytes - 1) / BLOCKSIZE;
        int desp1 = offset % BLOCKSIZE;
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        mi_waitSem();
        int BFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        mi_signalSem();
        if(desp1 != 0){
            bread(BFisico, buf_bloque);
        }
        if(primerBLogico == ultimoBLogico){
            memcpy(buf_bloque + desp1, buf_original, desp2 + 1 - desp1);//-desp1...
            total += desp2 + 1 - desp1;
            bwrite(BFisico, buf_bloque);
        }
        else{
            desp2 = offset + nbytes - 1;
            memcpy (buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            total += BLOCKSIZE - desp1;
            bwrite(BFisico, buf_bloque);
            mi_waitSem();
            for(int i = primerBLogico + 1; i != ultimoBLogico; i ++){
                BFisico = traducir_bloque_inodo(ninodo, i, 1);
                total += bwrite(BFisico, (buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE));
            }
            desp2 = desp2 % BLOCKSIZE;
            BFisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
            mi_signalSem();
            bread(BFisico, buf_bloque);
            memcpy (buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            total += desp2 + 1;
            bwrite(BFisico, buf_bloque);
        }
        total += offset;
        mi_waitSem();
        leer_inodo(ninodo, &inodo);//volvemos a leer el inodo, se han reservado bloques
        if(inodo.tamEnBytesLog < total){//modifica EOF fichero
            inodo.tamEnBytesLog = total; //Si lo escrito ocupa más que lo que ya habia
            inodo.ctime = time(NULL);// Actualizamos ctime, hemos modificado el inodo
        }
        inodo.mtime = time (NULL);//modificamos mtime, hemos escrito datos
        escribir_inodo(ninodo, inodo);//escribimos el inodo
        mi_signalSem();
        return total; //nbytes escritos que si va bien deberia ser total=nbytes
    }
    else{
        fprintf(stderr, "\n¡No hay permisos de escritura!\n");
        //el inodo no tiene permisos de escritura
        return EXIT_FAILURE;
    }
}
int mi_read_f(unsigned int ninodo, void *buf_original,unsigned int offset,unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    void *buf_bloque = malloc(BLOCKSIZE);
    int offsetBuf = 0;
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
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        int BFisico = traducir_bloque_inodo(ninodo, primerBLogico, 0);
        if(primerBLogico == ultimoBLogico){
            if(BFisico != EXIT_FAILURE){
                bread(BFisico, buf_bloque);
                if(desp2 == 0){//caso en que se tenga que leer el ultimo bloque de una secuencia
                    memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
                    bytesLeidos += BLOCKSIZE - desp1;
                }else{
                    memcpy(buf_original, buf_bloque + desp1, desp2 + 1 - desp1);
                    bytesLeidos += desp2 + 1 - desp1;
                }
            }else{
                bytesLeidos += BLOCKSIZE;
            }
        }
        else{
            //posible falta de deteccion de bloques vacíos error
            desp2 = offset + nbytes - 1;
            if(BFisico != -1){
                //cosas con desp1!!!!
                bread(BFisico, buf_bloque);
                memcpy (buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
                bytesLeidos += BLOCKSIZE - desp1;
                offsetBuf += BLOCKSIZE - desp1;
            }else{
                bytesLeidos += BLOCKSIZE;
            }
            for(int i = primerBLogico + 1; i != ultimoBLogico; i ++){
                BFisico = traducir_bloque_inodo(ninodo, i, 0);
                if(BFisico != -1){
                    bytesLeidos += bread(BFisico, buf_bloque);
                    memcpy(buf_original + offsetBuf, buf_bloque, BLOCKSIZE);
                    offsetBuf += bread(BFisico, buf_bloque);
                }else{
                    bytesLeidos += BLOCKSIZE;
                }
            }
            desp2 = desp2 % BLOCKSIZE;
            BFisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 0);
            if(BFisico != -1){
                bread(BFisico, buf_bloque);
                memcpy (buf_original + offsetBuf, buf_bloque, desp2 + 1);
                bytesLeidos += desp2 + 1;
            }else{
                    bytesLeidos += BLOCKSIZE;
                }
        }
        mi_waitSem();
        leer_inodo(ninodo, &inodo);
        inodo.atime = time(NULL);
        escribir_inodo(ninodo, inodo);
        mi_signalSem();
        return bytesLeidos;
       
    }else
    {   //no tienes permisos de lectura
        fprintf(stderr, "\n No hay permisos de lectura\n");
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
    mi_waitSem();
    leer_inodo(ninodo, &inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    escribir_inodo(ninodo, inodo);
    mi_signalSem();
    return EXIT_SUCCESS;
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){

    int primerBL = 0;
    struct inodo inodo;
    //Hay que comprobar que el inodo tenga permisos de escritura. 
    //No se puede truncar más allá del tamaño en bytes lógicos del fichero/directorio.
    //Nos basaremos en la función liberar_bloques_inodo() . 

    //Para saber que nº de bloque lógico le hemos de pasar como primer bloque lógico a liberar: 
    /*si nbytes % BLOCKSIZE = 0  entonces primerBL := nbytes/BLOCKSIZE 
    si_no primerBL := nbytes/BLOCKSIZE + 1*/
    mi_waitSem();
    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 2) == 2){
        if (nbytes >= inodo.tamEnBytesLog){ //si es mayor o igual a los bytes del inodo
            return EXIT_FAILURE;
        }
        if (nbytes % BLOCKSIZE == 0){
            primerBL = nbytes/BLOCKSIZE;
        }
        else{
            primerBL = nbytes/BLOCKSIZE + 1;
        }
        int bloquesL = liberar_bloques_inodo(primerBL, &inodo);
        inodo.numBloquesOcupados -= bloquesL;
        inodo.tamEnBytesLog = nbytes;
        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        escribir_inodo(ninodo, inodo);
        mi_signalSem();
        return bloquesL;
    }
    else{
        mi_signalSem();
        fprintf(stderr, "\n¡No hay permisos de escritura!\n");
        return EXIT_FAILURE;
    }
}