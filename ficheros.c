#include "ficheros.h"
int mi_write_f(unsigned int ninodo, const void *buf_original,unsigned int offset ,unsigned int nbytes){
    struct inodo inodo;
    char buf_bloque[BLOCKSIZE];
    if(inodo.permisos & 2 == 2){
        int primerBLogico = offset/BLOCKSIZE;
        int ultimoBLogico = (offset + nbytes - 1) / BLOCKSIZE;
        int desp1 = offset % BLOCKSIZE;
        int desp2 = ultimoBLogico % BLOCKSIZE;
        int BFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        if(desp1 !=0){
            bread(BFisico, buf_bloque);
        }
        memcpy(buf_bloque + desp1,buf_original,BLOCKSIZE - desp1);
        int total=bwrite(BFisico,buf_bloque);
        for(int i = primerBLogico + 1;i != ultimoBLogico;i++){
            total = total + bwrite(traducir_bloque_inodo(ninodo,i,1),
            buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE);
        }
        int ultimoBFisico = traducir_bloque_inodo(ninodo,ultimoBLogico,1);
        if(desp2!=BLOCKSIZE){
            bread(ultimoBFisico,buf_bloque);
        }
        memcpy(buf_bloque,buf_original + (nbytes-desp2 - 1),desp2 + 1);
        total = total + bwrite(ultimoBFisico, buf_bloque);
        leer_inodo(ninodo,&inodo);
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
