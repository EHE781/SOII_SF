int mi_write_f(unsigned int ninodo, const void *buf_original,unsigned int offset ,unsigned int nbytes){
    void *buf_bloque;
    if(inodo.permisos & 2){
        int primerBloqueLog=offset/BLOCKSIZE;
        int ultimoBloqueLog=(offset+nbytes-1)/BLOCKSIZE;
        int desp1=offset%BLOCKSIZE;
        int bloqueFisico=traducir_bloque_inodo(ninodo,primerBloqueLog,1);
    }
    else{
        //el inodo no tiene permisos de escritura
        return EXIT_FAILURE;
    }
}