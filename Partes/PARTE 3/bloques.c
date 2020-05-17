#include "bloques.h"
static int fd = 0; //file descriptor

int bmount(const char *camino){
    umask(000);
    fd = open(camino, O_RDWR|O_CREAT, 0666);
if(fd == -1){
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    return EXIT_FAILURE;
}
return  fd;
}

int bumount(){
   int check = close(fd);
   if(check == -1){
       fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
       return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}

int bwrite(unsigned int nbloque, const void *buf){
    lseek(fd, nbloque * BLOCKSIZE, SEEK_SET);
    size_t wbytes = write(fd, buf, BLOCKSIZE);
    if(wbytes == -1){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    return wbytes;
}

int bread(unsigned int nbloque, void *buf){
    off_t pos = lseek(fd, nbloque * BLOCKSIZE, SEEK_SET);
    size_t rbytes = read(fd, buf, BLOCKSIZE);
    lseek(fd, pos + rbytes, SEEK_SET); //DUDA=SI ESCRIBES MENOS DE UN BLOQUE, APUNTA AL SIGUIENTE O AL VACIO
    if(rbytes == -1){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    return rbytes;
}