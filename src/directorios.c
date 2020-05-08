#include "directorios.h"
#define TAMFILA 100
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
struct superbloque SB;

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    bool hayDir = false;
    if (camino[0] == '/')
    {
        for (int i = 1; i < strlen(camino); i++)
        {
            if (camino[i] == '/')
            { //caso en el que le pasemos //algo.txt no gestionado(doble barra)
                strncpy(inicial, camino + 1, sizeof(char) * i - 1);
                hayDir = true;
                strcpy(final, camino + i);
                strcpy(tipo, "d");
                break;
            }
        }
        if (!hayDir)
        {
            strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1); //+1 para evitar la primera '/'
            strcpy(tipo, "f");
        }
        return 0;
    }
    else
    {
        fprintf(stderr, "No existe el camino o directorio\n");
        return EXIT_FAILURE;
    }
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    int error = 0;
    struct superbloque SB;
    struct entrada entrada;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    memset(&inicial, 0, sizeof(entrada.nombre));
    memset(&final, 0, strlen(camino_parcial));
    char tipo;
    struct inodo inodo_dir;
    bread(posSB, &SB);
    if (camino_parcial[0] == '/' && strlen(camino_parcial) == 1)
    {
        *p_inodo = SB.posInodoRaiz; //inodo raíz
        *p_entrada = 0;             //entrada 0 pertenece a inodo raíz
        return EXIT_SUCCESS;
    }
    printf("08[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n", inicial, final, reservar);
    error = extraer_camino(camino_parcial, inicial, final, &tipo);
    if (error == EXIT_FAILURE)
    {
        return ERROR_CAMINO_INCORRECTO;
    }
    error = leer_inodo(*p_inodo_dir, &inodo_dir);
    if (error == EXIT_FAILURE)
    {
        return ERROR_PERMISO_LECTURA;
    }
    struct entrada buf_entradas[BLOCKSIZE / sizeof(struct entrada)];
    int num_entrada_inodo = 0;
    int cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    int offset = 0;
    memset(buf_entradas, 0, (BLOCKSIZE / sizeof(struct entrada)) * sizeof(struct entrada));
    if (cant_entradas_inodo > 0)
    {
        //leemos entradas
        offset += mi_read_f(*p_inodo_dir, buf_entradas, offset, BLOCKSIZE);
        //leemos primera entrada(0)
        while ((num_entrada_inodo < cant_entradas_inodo) && (strcmp(inicial, buf_entradas[num_entrada_inodo].nombre) != 0))
        {
            num_entrada_inodo++;
            if ((num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))) == 0)
            { //hemos leido el buffer entero
                offset += mi_read_f(*p_inodo_dir, &buf_entradas, offset, BLOCKSIZE);
            }
        }
        //return EXIT_SUCCESS; ????????que hace esto aqui
    }
    if ((num_entrada_inodo == cant_entradas_inodo) && (inicial != buf_entradas[num_entrada_inodo].nombre))
    {
        //printf("00[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
        switch (reservar)
        {
        case 0: //modo consulta

            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
        case 1: //modo escritura
            if (inodo_dir.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            if ((inodo_dir.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    if (strcmp(final, "/") == 0)
                    {
                        entrada.ninodo = reservar_inodo('d', 6);
                        printf("04[buscar_entrada()->reservado inodo: %d tipo 'd' con permisos %c para: %s]\n", entrada.ninodo, permisos, entrada.nombre);
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {

                    entrada.ninodo = reservar_inodo('f', 6);
                    printf("04[buscar_entrada()->reservado inodo: %d tipo 'f' con permisos %c para: %s]\n", num_entrada_inodo, permisos, entrada.nombre);
                }
                error = mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog,
                                   sizeof(struct entrada));
                printf("09[buscar_entrada()-> creada entrada: %s inodo: %d] \n", inicial, num_entrada_inodo);
                if (error == EXIT_FAILURE)
                {
                    if (entrada.ninodo != -1)
                    {
                        liberar_inodo(entrada.ninodo);
                        printf("09[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
                    }
                    return EXIT_FAILURE;
                }
            }
        }
    }
    if ((strcmp(final, "/") == 0) || tipo == 'f')
    {
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            //modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        // cortamos la recursividad
        *p_inodo = buf_entradas[num_entrada_inodo].ninodo;
        *p_entrada = num_entrada_inodo;
        return EXIT_SUCCESS;
    }
    else
    {
        *p_inodo_dir = buf_entradas[num_entrada_inodo].ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
}

void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
    switch (error)
    {
    case -1:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -2:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -3:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break; //decir algo sobre el padre sin permisos????duda
    case -6:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -7:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    }
}

int mi_creat(const char *camino, unsigned char permisos)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        printf("***********************************************************************\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int mi_dir(const char *camino, char *buffer, char *tipo, bool ext)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        printf("***********************************************************************\n");
        return EXIT_FAILURE;
    }
    else
    {
        int offset = 0;
        char array[10]; //maximo valor de un "unsigned int"(10 cifras)
        struct entrada entrada;
        struct inodo inodo;
        struct tm *tm; //ver info: struct tm
        char tmp[100];
        if (camino[strlen(camino) - 1] == '/')
        { //mi_read_f(p_inodo, &entrada, offset, sizeof(struct entrada));
            leer_inodo(p_inodo, &inodo);
            *tipo = inodo.tipo;
            int cant_entradas_inodo = inodo.tamEnBytesLog / sizeof(struct entrada);
            fprintf(stderr, "Total: %i\n", cant_entradas_inodo);
            struct entrada buf_entradas[BLOCKSIZE / sizeof(struct entrada)];
            offset += mi_read_f(p_inodo, buf_entradas, offset, BLOCKSIZE);
            //p_inodo = entrada.ninodo;
            for (int i = 0; i < cant_entradas_inodo; i++)
            {
                if ((offset % (BLOCKSIZE / sizeof(struct entrada))) == 0)
                {
                    offset += mi_read_f(p_inodo, &entrada, offset, sizeof(struct entrada));
                }
                leer_inodo(buf_entradas[i].ninodo, &inodo);
                if (ext == false)
                {
                    strcat(buffer, GREEN);
                    strcat(buffer, buf_entradas[i].nombre); //ponemos el nombre en el buffer
                    strcat(buffer, RESET);
                }
                else
                {
                    if (inodo.tipo == 'd')
                    {
                        strcat(buffer, RED);
                    }
                    else
                    {
                        strcat(buffer, BLUE);
                    }
                    strncat(buffer, &inodo.tipo, sizeof(char));
                    strcat(buffer, "\t"); //y listamos sus permisos
                    strcat(buffer, YELLOW);
                    if ((inodo.permisos & 4) == 4)
                    {
                        strcat(buffer, "r");
                    }
                    else
                    {
                        strcat(buffer, "-");
                    }
                    if ((inodo.permisos & 2) == 2)
                    {
                        strcat(buffer, "w");
                    }
                    else
                    {
                        strcat(buffer, "-");
                    }
                    if ((inodo.permisos & 1) == 1)
                    {
                        strcat(buffer, "x");
                    }
                    else
                    {
                        strcat(buffer, "-");
                    }
                    strcat(buffer, MAGENTA);
                    strcat(buffer, "\t\t");
                    tm = localtime(&inodo.mtime);
                    sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900,
                            tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
                    strcat(buffer, tmp);
                    strcat(buffer, RESET);
                    sprintf(array, "%d", inodo.tamEnBytesLog); //convertimos el numero a chars
                    strcat(buffer, array);
                    strcat(buffer, "\t");
                    strcat(buffer, GREEN);
                    strcat(buffer, buf_entradas[i].nombre); //ponemos el nombre en el buffer
                    strcat(buffer, RESET);
                }
                if ((strlen(buffer) % TAMFILA) != 0)
                { //rellenamos hasta TAMFILA
                    while ((strlen(buffer) % TAMFILA) != 0)
                    {
                        strcat(buffer, " ");
                    }
                }
                strcat(buffer, "\n"); //separador para la próxima entrada
            }
        }
        else
        {
            mi_read_f(p_inodo_dir, &entrada, 0, sizeof(struct entrada));
            leer_inodo(entrada.ninodo, &inodo); //leemos el inodo asociado a ella
            *tipo = inodo.tipo;
            if (ext == false)
            {
                strcat(buffer, GREEN);
                strcat(buffer, entrada.nombre); //ponemos el nombre en el buffer
                strcat(buffer, RESET);
            }
            else
            {
                strcat(buffer, BLUE);
                strncat(buffer, &inodo.tipo, sizeof(char));
                strcat(buffer, "\t"); //y listamos sus permisos
                strcat(buffer, MAGENTA);
                if ((inodo.permisos & 4) == 4)
                {
                    strcat(buffer, "r");
                }
                else
                {
                    strcat(buffer, "-");
                }
                if ((inodo.permisos & 2) == 2)
                {
                    strcat(buffer, "w");
                }
                else
                {
                    strcat(buffer, "-");
                }
                if ((inodo.permisos & 1) == 1)
                {
                    strcat(buffer, "x");
                }
                else
                {
                    strcat(buffer, "-");
                }
                strcat(buffer, "\t\t");
                strcat(buffer, YELLOW);
                tm = localtime(&inodo.mtime);
                sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900,
                        tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
                strcat(buffer, tmp);
                sprintf(array, "%d", inodo.tamEnBytesLog); //convertimos el numero a chars
                strcat(buffer, array);
                strcat(buffer, "\t");
                strcat(buffer, GREEN);
                strcat(buffer, entrada.nombre); //ponemos el nombre en el buffer
                strcat(buffer, RESET);
            }
            if ((strlen(buffer) % TAMFILA) != 0)
            {
                while ((strlen(buffer) % TAMFILA) != 0)
                {
                    strcat(buffer, " ");
                }
            }
            strcat(buffer, "\n");
        }
    }
    return EXIT_SUCCESS;
}

int mi_chmod(const char *camino, unsigned char permisos){
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        printf("***********************************************************************\n");
        return EXIT_FAILURE;
    }
    mi_chmod_f(p_inodo, permisos);
    return EXIT_SUCCESS;
}
