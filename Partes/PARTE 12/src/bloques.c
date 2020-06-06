#include "../headers/bloques.h"
#include "../headers/semaforo_mutex_posix.h"

static sem_t *mutex;
static unsigned int inside_sc = 0;

static int fd = 0; //file fd
/*MONTA EL SISTEMA DE FICHEROS Y DEVUELVE EL 
DESCRIPTOR DE FICHERO DEL FICHERO DESDE EL QUE
SE HA MONTADO EL SISTEMA DE FICHEROS*/

int bmount(const char *camino) {
   if (fd > 0) {
       close(fd);
   }
   if ((fd = open(camino, O_RDWR | O_CREAT, 0666)) == -1) {
      fprintf(stderr, "Error: bloques.c → bmount() → open()\n");
   }
   if (!mutex) { //mutex == 0
   //el semáforo es único y sólo se ha de inicializar una vez en nuestro sistema (lo hace el padre)
       mutex = initSem(); //lo inicializa a 1
       if (mutex == SEM_FAILED) {
           return EXIT_FAILURE;
       }
   }
   return fd;
}

/*DESMONTA EL SISTEMA DE FICHEROS MONTADO ANTERIORMENTE
Y DEVUELVE EXIT_SUCCESS SI NO HAY PROBLEMAS*/
int bumount() {
   fd = close(fd); 
   // hay que asignar el resultado de la operación a la variable ya que bmount() la utiliza
   if (fd == -1) {
       fprintf(stderr, "Error: bloques.c → bumount() → close(): %d: %s\n", errno, strerror(errno));
       return EXIT_FAILURE;
   }
   deleteSem(); // borramos semaforo 
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
//CONTROL SEMÁFOROS
void mi_waitSem(){
if (!inside_sc) {
waitSem(mutex);
}
inside_sc++;
}
void mi_signalSem() {
inside_sc--;
if (!inside_sc) {
    signalSem(mutex);
}
}

