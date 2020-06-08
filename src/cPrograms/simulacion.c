//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "../headers/noInclude/simulacion.h"

int acabados = 0;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended = waitpid(-1, NULL, WNOHANG))>0) {
    acabados++;
    //Podemos testear qué procesos van acabando:
    //fprintf(stderr, "[simulación.c → Acabado proceso con PID %d, total acabados: %d\n", ended, acabados);
  }
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "La sintaxis correcta es: ./simulacion <disco>\n");
        return EXIT_FAILURE;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *tiempo = malloc(14);
    char *camino = malloc(21);
    strcpy(camino, "/simul_"); //parte fija del directorio de simulacion
    bmount(argv[1]); // montamos el disco
    sprintf(tiempo, "%d%02d%02d%02d%02d%02d", tm.tm_year + 1900, 
    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec); //segunda parte, variable segun tiempo, del directorio de simulacion
    strcat(camino, tiempo); //la adjuntamos al path
    strcat(camino, "/");//y le añadimos la barrita de directorio
    if((mi_creat(camino, 6)) == EXIT_FAILURE){ //creamos dicho directorio
        bumount(argv[1]); //si hay un error, desmontamos el disco
        return EXIT_FAILURE; //y salimos
    }
    fprintf(stderr, "Directorio simulación: %s\n", camino);
    signal(SIGCHLD, reaper); //asignamos "reaper" a la señal SIGCHILD

    pid_t pid;
    for(int proceso = 1; proceso <= NUMPROCESOS; proceso ++){ //iterador de procesos hijos

        pid = fork(); //creamos un nuevo hijo
        if(pid == 0){ // si se ha creado correctamente
            bmount(argv[1]); //montamos el disco para EL PROCESO HIJO (a partir de ahora todo se hará de forma concurrente)
            char *camino_d = malloc(38); 
            char *camino_f = malloc(48);
            char *temp1 = malloc(12);
            char *temp = malloc(sizeof(int));
            strcpy(camino_d, camino); //juntamos el nombre del directorio de simulacion
            sprintf(temp1, "proceso_PID"); //le añadimos la parte fija del nombre a una temporal
            sprintf(temp, "%d", getpid() ); //imprimimos el PID en una variable temporal
            strcat(temp1, temp); //y la concatenamos a la otra temporal
            strcat(temp1, "/"); //asegurandonos de hacerlo directorio
            strcat(camino_d, temp1); //y lo concatenamos ahora si al path definitivo
            if((mi_creat(camino_d, 6)) == EXIT_FAILURE){ //creamos el directorio
                bumount(argv[1]); // y si hay algun problema desmontamos
                exit(0);
            }
            strcpy(camino_f, camino_d); //copiamos el camino al directorio a camino_f
            strcat(camino_f, "prueba.dat"); // y le añadimos el nombre fijo de prueba.dat
            if((mi_creat(camino_f, 6)) == EXIT_FAILURE){ // y creamos el fichero donde escribiremos los registros
                bumount(argv[1]);//desmontamos si hay algun problema
                exit(0);
            }
            srand(time(NULL)+getpid()); //aleatorizamos el PID registral
            for(int i = 0; i < NUMESCRITURAS; i++){
                struct REGISTRO registro;
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = i+1;
                registro.nRegistro = rand() % REGMAX;
                mi_write(camino_f, &registro, (registro.nRegistro * sizeof(struct REGISTRO)), sizeof(struct REGISTRO));
                //fprintf(stderr, "[simulación.c → Escritura %d en %s\n", i, camino_f);
                usleep(50000);
                if(i == NUMESCRITURAS-1){
                    fprintf(stderr, "Proceso %d: Completadas %d escrituras en %s\n", proceso, 
                    NUMESCRITURAS, camino_f);
                }
            }
            bumount(argv[1]);
            exit(0);
        }
        usleep(200000);
    }
    //Permitir que el padre espere por todos los hijos: 
    while (acabados < NUMPROCESOS){
    pause();
    }
    bumount(argv[1]);
    fprintf(stderr, "Total de procesos terminados: %d\n", acabados);
    free(tiempo);
    free(camino);
    return EXIT_SUCCESS;
}