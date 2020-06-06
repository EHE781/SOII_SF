//simulación.h
#include <time.h>
#include <sys/types.h>

struct REGISTRO {
    time_t fecha; //fecha de la escritura en formato epoch 
    pid_t pid; //PID del proceso que lo ha creado
    int nEscritura; //Entero con el número de escritura (de 1 a 50)
    int nRegistro; //Entero con el número del registro dentro del fichero (de 0 a REGMAX-1) 
};
