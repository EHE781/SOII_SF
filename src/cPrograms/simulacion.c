#include "../headers/directorios.h"
#include "../headers/noInclude/simulacion.h"
#include <sys/wait.h>
#include <signal.h>
#define REGMAX 500000
#define NUMPROCESOS 100
#define NUMESCRITURAS 50

int acabados = 0;
struct REGISTRO registro;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended = waitpid(-1, NULL, WNOHANG))>0) {
    acabados++;
    //Podemos testear qué procesos van acabando:
    fprintf(stderr, "[simulación.c → Acabado proceso con PID %d, total acabados: %d\n", ended, acabados);
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
    char *camino_d = malloc(14);
    char *camino_f = malloc(25);
    char *temp = malloc(3);
    strcpy(camino, "/simul_");
    bmount(argv[1]);
    sprintf(tiempo, "%d%02d%02d%02d%02d%02d", tm.tm_year + 1900, 
    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    strcat(camino, tiempo);
    strcat(camino, "/");
    if((mi_creat(camino, 6)) == EXIT_FAILURE){
        bumount(argv[1]);
        return EXIT_FAILURE;
    }
    signal(SIGCHLD, reaper);
    for(int proceso = 1; proceso <= 100; proceso ++){
        strcpy(camino_d, "");
        strcpy(camino_f, "");
        pid_t pid = fork();
        if(pid == 0){
            bmount(argv[1]);
            sprintf(camino_d, "proceso_PID");
            snprintf( temp, 3, "%d", getpid() );
            strcat(camino_d, temp);
            strcat(camino_d, "/");
            if((mi_creat(camino_d, 6)) == EXIT_FAILURE){
                bumount(argv[1]);
            }
            strcat(camino_f, camino_d);
            strcat(camino_f, "/prueba.dat");
            if((mi_creat(camino_f, 6)) == EXIT_FAILURE){
                bumount(argv[1]);
            }
            srand(time(NULL)+getpid());
            for(int i = 0; i < NUMESCRITURAS; i++){
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = i+1;
                registro.nRegistro = rand() % REGMAX;
                mi_write(camino_f, &registro, registro.nRegistro * sizeof(struct REGISTRO), 
                sizeof(struct REGISTRO));
                fprintf(stderr, "[simulación.c → Escritura %d en %s\n", i, camino_f);
                usleep(50000);
            }
            bumount(argv[1]);
            exit(0);
            fprintf(stderr, "Proceso %d: Completadas %d escrituras en %s", proceso, 
            NUMESCRITURAS, camino_f);
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
    free(camino_d);
    free(camino_f);
    free(temp);
    return EXIT_SUCCESS;
}