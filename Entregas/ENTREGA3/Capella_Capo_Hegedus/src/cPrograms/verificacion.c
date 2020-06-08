//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "../headers/noInclude/verificacion.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "La sintaxis correcta es ./verificacion <disco> <directorio_simulacion>\n");
    }
    struct STAT statp;
    struct INFORMACION info; //struct para la informacion
    struct entrada buf_entradas[NUMPROCESOS * sizeof(struct entrada)];
    int total_entradas = 0;
    memset(buf_entradas, 0, (NUMPROCESOS * sizeof(struct entrada)) * sizeof(struct entrada));
    bmount(argv[1]);
    int ninodo = mi_stat(argv[2], &statp);
    int n_entradas = statp.tamEnBytesLog / sizeof(struct entrada);
    if (n_entradas != NUMPROCESOS)
    {
        bumount(argv[1]);
        return EXIT_FAILURE;
    }
    char *dir_informe = malloc(strlen(argv[2]) + 11); // directorio de la simulación + "informe.txt"
    strcpy(dir_informe, argv[2]);
    strcat(dir_informe, "informe.txt");
    if ((mi_creat(dir_informe, 6)) == EXIT_FAILURE)
    {
        bumount(argv[1]);
        return EXIT_FAILURE;
    }
    int offsetInforme = 0;
    int entrada = 0;
    mi_read_f(ninodo, &buf_entradas, 0, NUMPROCESOS * sizeof(struct entrada));
    while(entrada < n_entradas)
    { //para cada entrada de la simulación
        total_entradas++; //contador numero de entradas
        char *s_pid = malloc(5);
        int n_pid;
        int offset = 0;
        strcpy(s_pid, strchr(buf_entradas[entrada].nombre, 'D') + 1);
        char *const_path = malloc(strlen(argv[2]) + 26); //dir + prueba.dat
        strcpy(const_path, argv[2]);
        strcat(const_path, "proceso_PID");
        strcat(const_path, s_pid);
        strcat(const_path, "/prueba.dat");
        int ninode = mi_stat(const_path, &statp);
        n_pid = atoi(s_pid);
        info.pid = n_pid;                              //sabemos que la ultima letra es D, +1 dará el PID
        int cant_r_buf = 256;  //multiplo de BLOCKSIZE (256 para equilibrar diferentes maquinas)
        struct REGISTRO buffer_escrituras[cant_r_buf]; // espacio_ocupado = 256 * 24
        info.nEscrituras = 0;
        int temp = 0;
        while ((temp = mi_read_f(ninode, &buffer_escrituras, offset, cant_r_buf * sizeof(struct REGISTRO))) > 0)
        {
            offset += temp;
            int i = 0;
            while (i < cant_r_buf)
            { //iterador escrituras buffer
            //fprintf(stderr, "PID:%d\n", buffer_escrituras[i].pid);
            //fprintf(stderr, "FECHA:%d\n", buffer_escrituras[i].fecha);
            //fprintf(stderr, "nEscrituras:%d\n", buffer_escrituras[i].nEscritura);
            //fprintf(stderr, "nRegistro:%d\n", buffer_escrituras[i].nRegistro);
                if (buffer_escrituras[i].pid == info.pid)
                {
                    if (!info.nEscrituras)
                    {
                        info.MenorPosicion = buffer_escrituras[i];
                        info.MayorPosicion = buffer_escrituras[i];
                        info.PrimeraEscritura = buffer_escrituras[i];
                        info.UltimaEscritura = buffer_escrituras[i];
                        info.nEscrituras++;
                    }
                    else
                    {
                        if ((difftime(buffer_escrituras[i].fecha, info.PrimeraEscritura.fecha)) <= 0 &&
                            buffer_escrituras[i].nEscritura < info.PrimeraEscritura.nEscritura)
                        {
                            info.PrimeraEscritura = buffer_escrituras[i];
                        }
                        if ((difftime(buffer_escrituras[i].fecha, info.UltimaEscritura.fecha)) >= 0 &&
                            buffer_escrituras[i].nEscritura > info.UltimaEscritura.nEscritura)
                        {
                            info.UltimaEscritura = buffer_escrituras[i];
                        }
                        if (buffer_escrituras[i].nRegistro < info.MenorPosicion.nRegistro)
                        {
                            info.MenorPosicion = buffer_escrituras[i];
                        }
                        if (buffer_escrituras[i].nRegistro > info.MayorPosicion.nRegistro)
                        {
                            info.MayorPosicion = buffer_escrituras[i];
                        }
                        info.nEscrituras++;
                    }
                }
                i++;
            }
            memset(&buffer_escrituras, 0, cant_r_buf * sizeof(struct REGISTRO));
        }
        //fin for registros
        char pfecha[24], ufecha[24], maxfecha[24], minfecha[24];
        struct tm *tp;
        tp = localtime(&info.PrimeraEscritura.fecha);
        strftime(pfecha, sizeof(pfecha), "%a %Y-%m-%d %H:%M:%S", tp);
        tp = localtime(&info.UltimaEscritura.fecha);
        strftime(ufecha, sizeof(ufecha), "%a %Y-%m-%d %H:%M:%S", tp);
        tp = localtime(&info.MenorPosicion.fecha);
        strftime(minfecha, sizeof(minfecha), "%a %Y-%m-%d %H:%M:%S", tp);
        tp = localtime(&info.MayorPosicion.fecha);
        strftime(maxfecha, sizeof(maxfecha), "%a %Y-%m-%d %H:%M:%S", tp);
        unsigned int tam = BLOCKSIZE;
        char *buf[tam];
        memset(buf, 0, (tam) * sizeof(char));
        sprintf(buf, "PID: %d\nNumero de escrituras:\t%d\nPrimera escritura: "
                     "\t%d\t%d\t%s\nUltima escritura:\t%d\t%d\t%s\nMenor po"
                     "sición:\t\t%d\t%d\t%s\nMayor posición:\t\t%d\t%d\t%s\n\n",
                info.pid, info.nEscrituras, info.PrimeraEscritura.nEscritura,
                info.PrimeraEscritura.nRegistro, minfecha, info.UltimaEscritura.nEscritura,
                info.UltimaEscritura.nRegistro, ufecha, info.MenorPosicion.nEscritura,
                info.MenorPosicion.nRegistro, minfecha, info.MayorPosicion.nEscritura,
                info.MayorPosicion.nRegistro, maxfecha);
        if ((offsetInforme += mi_write(dir_informe, &buf, offsetInforme, BLOCKSIZE)) == EXIT_FAILURE)
        {
            bumount(argv[1]);
            return EXIT_FAILURE;
        }
        fprintf(stderr, "%d escrituras validadas en %s\n", info.nEscrituras,
                const_path);
                entrada++;
    } //fin for entradas
    fprintf(stderr, "Procesos validados: %d\n", total_entradas);
    return EXIT_SUCCESS;
}