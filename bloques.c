#include "bloques.h"
static int fd = 0; //file descriptor
/*MONTA EL SISTEMA DE FICHEROS Y DEVUELVE EL 
DESCRIPTOR DE FICHERO DEL FICHERO DESDE EL QUE
SE HA MONTADO EL SISTEMA DE FICHEROS*/
int bmount(const char *camino){
    umask(000);//inicializar los permisos del fichero. 000 indica que se puede leer/escribir/ejecutar
    fd = open(camino, O_RDWR|O_CREAT, 0666);
if(fd == -1){
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    return EXIT_FAILURE;
}
return  fd;
}
/*DESMONTA EL SISTEMA DE FICHEROS MONTADO ANTERIORMENTE
Y DEVUELVE EXIT_SUCCESS SI NO HAY PROBLEMAS*/
int bumount(){
   int check = close(fd);
   if(check == -1){
       fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
       return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
/*ESCRIBE UN BUFFER PASADO POR PARÁMETRO EN EL BLOQUE
TAMBIÉN PASADO POR PARÁMETRO. DEVUELVE BYTES ESCRITOS*/
int bwrite(unsigned int nbloque, const void *buf){
    lseek(fd, nbloque * BLOCKSIZE, SEEK_SET);
    size_t wbytes = write(fd, buf, BLOCKSIZE);
    if(wbytes == -1){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    return wbytes;
}
/*LEE EL BLOQUE EN EL BUFFER PASADO POR PARÁMETRO
Y DEVUELVE CANTIDAD DE BYTES LEIDOS*/
int bread(unsigned int nbloque, void *buf){
    off_t pos = lseek(fd, nbloque * BLOCKSIZE, SEEK_SET);
    size_t rbytes = read(fd, buf, BLOCKSIZE);
    lseek(fd, pos + rbytes, SEEK_SET); 
    if(rbytes == -1){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    return rbytes;
}