#include "../headers/noInclude/verificacion.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "La sintaxis correcta es ./verificacion <disco> <directorio_simulacion>\n");
    }
    struct STAT statp;
    struct INFORMACION info;
    struct entrada buf_entradas[NUMPROCESOS * sizeof(struct entrada)];
    int total_entradas = 0;
    memset(buf_entradas, 0, NUMPROCESOS * sizeof(struct entrada));
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
    mi_read_f(ninodo, &buf_entradas, 0, NUMPROCESOS * sizeof(struct entrada));
    for (int entrada = 0; entrada < n_entradas; entrada++)
    { //para cada entrada de la simulación
        total_entradas++;
        char *n_pid = malloc(5);
        int offset = 0;
        int offsetInforme = 0;
        strcpy(n_pid, strchr(buf_entradas[entrada].nombre, 'D') + 1);
        char *const_path = malloc(strlen(argv[2]) + 26); //dir + prueba.dat
        strcpy(const_path, argv[2]);
        strcat(const_path, "proceso_PID");
        strcat(const_path, n_pid);
        strcat(const_path, "/prueba.dat");
        info.pid = atoi(n_pid);                                              //sabemos que la ultima letra es D, +1 dará el PID
        int cant_registros_buffer_escrituras = 256;                          //multiplo de BLOCKSIZE (256 para equilibrar diferentes maquinas)
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras]; // espacio_ocupado = 256 * 24
        int temporal = 0;
        memset(buffer_escrituras, 0, sizeof(struct REGISTRO) * cant_registros_buffer_escrituras);
        while ((offset = mi_read(const_path, buffer_escrituras, offset, sizeof(buffer_escrituras), false)) > 0)
        {
            memset(buffer_escrituras, 0, sizeof(struct REGISTRO) * cant_registros_buffer_escrituras);
            for (int i = 0; i < cant_registros_buffer_escrituras; i++)
            { //iterador escrituras buffer
                if (buffer_escrituras[i].pid == n_pid)
                {
                    if (info.nEscrituras == 0)
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
                            buffer_escrituras[i].nEscritura < info.nEscrituras)
                        {
                            info.PrimeraEscritura = buffer_escrituras[i];
                        }
                        if ((difftime(buffer_escrituras[i].fecha, info.UltimaEscritura.fecha)) >= 0 &&
                            buffer_escrituras[i].nEscritura > info.nEscrituras)
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

            char *buf = malloc(38485738);
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "PID: %d\nNumero de escrituras:\t%d\nPrimera escritura:"
                         "\t%d\t%d\t%s\nUltima escritura:\t%d\t%d\t%s\nMenor po"
                         "sición:\t\t%d\t%d\t%s\nMayor posición:\t\t%d\t%d\t%s\n\n",
                    info.pid, info.nEscrituras, info.PrimeraEscritura.nEscritura,
                    info.PrimeraEscritura.nRegistro, minfecha, info.UltimaEscritura.nEscritura,
                    info.UltimaEscritura.nRegistro, ufecha, info.MenorPosicion.nEscritura,
                    info.MenorPosicion.nRegistro, minfecha, info.MayorPosicion.nEscritura,
                    info.MayorPosicion.nRegistro, maxfecha);
            if ((offsetInforme = mi_write(dir_informe, &buf, offsetInforme, sizeof(buf))) == EXIT_FAILURE)
            {
                bumount(argv[1]);
                return EXIT_FAILURE;
            }
            fprintf(stderr, "%d escrituras validadas en %s\n", info.nEscrituras,
                    const_path);
        }
    } //fin for entradas
    fprintf(stderr, "Procesos validados: %d\n", total_entradas);
    return EXIT_SUCCESS;
}