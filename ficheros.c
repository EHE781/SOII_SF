int mi_write_f(unsigned int ninodo, const void *buf_original,unsigned int offset ,unsigned int nbytes){
    struct inodo *inodo;
    void *buf_bloque[BLOCKSIZE];
    if(inodo.permisos & 2 == 2){
        int primerBloqueLog = offset/BLOCKSIZE;
        int ultimoBloqueLog = (offset + nbytes - 1) / BLOCKSIZE;
        int desp1 = offset % BLOCKSIZE;
        int desp2 = ultimoBloqueLog % BLOCKSIZE;
        int bloqueFisico = traducir_bloque_inodo(ninodo,primerBloqueLog,1);
        bread(bloqueFisico,buf_bloque);
        memcpy(buf_bloque + desp1,buf_original,BLOCKSIZE - desp1);
        int total=bwrite(bloqueFisico,buf_bloque);
        for(int i = primerBloqueLog + 1;i != ultimoBloqueLog;i++){
        total=total+bwrite(traducir_bloque_inodo(ninodo,i,1),buf_original + (BLOCKSIZE - desp1) + (i - primerBloqueLog - 1) * BLOCKSIZE);
        }
        int ultBloqueFisico = traducir_bloque_inodo(ninodo,ultimoBloqueLog,1);
        bread(ultBloqueFisico,buf_bloque);
        memcpy(buf_bloque,buf_original + (nbytes-desp2 - 1),desp2 + 1);
        total=total+bwrite(ultBloqueFisico,buf_bloque);
        leer_inodo(ninodo,inodo);
        //
        //
        //
        return total; //nbytes escritos que si va bien deberia ser total=nbytes
    }
    else{
        //el inodo no tiene permisos de escritura
        return EXIT_FAILURE;
    }
}