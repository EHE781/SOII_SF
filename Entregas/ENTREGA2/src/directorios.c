#include "headers/directorios.h"
#define TAMFILA 100
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define CACHE 5
#define DEBUG 0

struct superbloque SB;
struct UltimaEntrada UltimasEntradas[CACHE];

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
    bool creado = false;
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
    error = extraer_camino(camino_parcial, inicial, final, &tipo);
#if DEBUG
    fprintf(stderr, "[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n", inicial, final, reservar);
#endif
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
    }
    if ((num_entrada_inodo == cant_entradas_inodo) && (inicial != buf_entradas[num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))].nombre))
    {
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
#if DEBUG
                        fprintf(stderr, "[buscar_entrada()->reservado inodo: %d tipo 'd' con permisos %c para: %s]\n", entrada.ninodo, permisos, entrada.nombre);
#endif
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {

                    entrada.ninodo = reservar_inodo('f', 6);
#if DEBUG
                    fprintf(stderr, "[buscar_entrada()->reservado inodo: %d tipo 'f' con permisos %c para: %s]\n", entrada.ninodo, permisos, entrada.nombre);
#endif
                }
                error = mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog, sizeof(struct entrada));
#if DEBUG
                fprintf(stderr, "[buscar_entrada()-> creada entrada: %s inodo: %d] \n", inicial, num_entrada_inodo);
#endif
                if (error == EXIT_FAILURE)
                {
                    if (entrada.ninodo != -1)
                    {
                        liberar_inodo(entrada.ninodo);
#if DEBUG
                        fprintf(stderr, "[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
#endif
                    }
                    return EXIT_FAILURE;
                }
                creado = true;
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
        if (!creado)
        {
            *p_inodo = buf_entradas[num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))].ninodo;
        }
        else
        {
            *p_inodo = entrada.ninodo; //si hemo creado el fichero/directorio
        }
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
    if (camino[0] == '/' && strlen(camino) == 1)
    {
        fprintf(stderr, "No se puede crear la raíz\n");
        return EXIT_FAILURE;
    }
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        fprintf(stderr, "***********************************************************************\n");
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
#if DEBUG
        printf("***********************************************************************\n");
#endif
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
                leer_inodo(buf_entradas[i % (BLOCKSIZE / sizeof(struct entrada))].ninodo, &inodo);
                if (ext == false)
                {
                    strcat(buffer, GREEN);
                    strcat(buffer, buf_entradas[i % (BLOCKSIZE / sizeof(struct entrada))].nombre); //ponemos el nombre en el buffer
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
                    strcat(buffer, CYAN);
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
                    strcat(buffer, buf_entradas[i % (BLOCKSIZE / sizeof(struct entrada))].nombre); //ponemos el nombre en el buffer
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
                if ((offset % (BLOCKSIZE / sizeof(struct entrada))) == 0)
                {
                    offset += mi_read_f(p_inodo, buf_entradas, offset, sizeof(struct entrada));
                }
            }
        }
        else
        {
            mi_read_f(p_inodo_dir, &entrada, sizeof(struct entrada) * p_entrada, sizeof(struct entrada));
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
                strcat(buffer, CYAN);
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
                strcat(buffer, RESET);
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

int mi_chmod(const char *camino, unsigned char permisos)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    mi_chmod_f(p_inodo, permisos);
    return EXIT_SUCCESS;
}

int mi_stat(const char *camino, struct STAT *p_stat)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    //leemos el inodo, así que permisos bastan los de lectura
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}
//DUDA ADELAIDA, SE PUEDE LA VARIABLE BOOLEANA Y SE PUEDE LEER SB YA EN DIRECTORIOS.C
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    for (int i = 0; i < CACHE; i++)
    {
        if (strcmp(camino, UltimasEntradas[i].camino) == 0)
        {
            p_inodo = UltimasEntradas[i].p_inodo;
            return mi_write_f(p_inodo, buf, offset, nbytes) - offset;
        }
    }
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 2)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    for (int i = 0; i < CACHE; i++)
    {
        if (UltimasEntradas[i].camino[0] == '\000')
        { //si aun no se ha llenado la caché
            strcpy(UltimasEntradas[i].camino, camino);
            UltimasEntradas[i].p_inodo = p_inodo;
#if DEBUG
            fprintf(stderr, CYAN "\n[mi_write() → Actualizamos la caché de escritura]\n" RESET);
#endif
        }
    }
    if (UltimasEntradas[CACHE].camino[0] != '\000')
    { //si la caché está llena, reemplazamos con FIFO
        for (int i = 0; i < CACHE - 1; i++)
        {
            strcpy(UltimasEntradas[i].camino, UltimasEntradas[i + 1].camino);
            UltimasEntradas[i].p_inodo = UltimasEntradas[i + 1].p_inodo;
        }
        strcpy(UltimasEntradas[CACHE].camino, camino);
        UltimasEntradas[CACHE].p_inodo = p_inodo;
#if DEBUG
        fprintf(stderr, CYAN "\n[mi_write() → Actualizamos la caché de escritura]\n" RESET);
#endif
    }

    return mi_write_f(p_inodo, buf, offset, nbytes) - offset;
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{
    int total, check;
    total = check = 0;
    bool EndOfFile = false;
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    bool encontrado = false;
    while (!EndOfFile)
    {
        memset(buf, 0, nbytes);
        if (offset % nbytes == 0)
        {
            for (int i = 0; i < CACHE; i++)
            {
                if (strcmp(camino, UltimasEntradas[i].camino) == 0)
                {
                    p_inodo = UltimasEntradas[i].p_inodo;
                    encontrado = true;
#if DEBUG
                    fprintf(stderr, CYAN "\n[mi_read() → Utilizamos la caché de lectura en vez de llamar a buscar_entrada()]\n" RESET);
#endif
                    break;
                }
            }

            if (!encontrado && (error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 2)) < 0)
            {
                mostrar_error_buscar_entrada(error);
#if DEBUG
                printf("***********************************************************************\n");
#endif
                return EXIT_FAILURE;
            }
            for (int i = 0; i < CACHE; i++)
            {
                if (UltimasEntradas[i].camino[0] == '\000' && !encontrado)
                { //si aun no se ha llenado la caché
                    strcpy(UltimasEntradas[i].camino, camino);
                    UltimasEntradas[i].p_inodo = p_inodo;
                    break;
#if DEBUG
                    fprintf(stderr, CYAN "\n[mi_write() → Actualizamos la caché de lectura]\n" RESET);
#endif
                }
            }
            if (UltimasEntradas[CACHE].camino[0] != '\000' && !encontrado)
            { //si la caché está llena, reemplazamos con FIFO
                for (int i = 0; i < CACHE - 1; i++)
                {
                    strcpy(UltimasEntradas[i].camino, UltimasEntradas[i + 1].camino);
                    UltimasEntradas[i].p_inodo = UltimasEntradas[i + 1].p_inodo;
                }
                strcpy(UltimasEntradas[CACHE].camino, camino);
                UltimasEntradas[CACHE].p_inodo = p_inodo;
#if DEBUG
                fprintf(stderr, CYAN "\n[mi_write() → Actualizamos la caché de lectura]\n" RESET);
#endif
            }
        }
        total += mi_read_f(p_inodo, buf, offset, nbytes);
        check = mi_read_f(p_inodo, buf, offset, nbytes);
        if (check == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
        if (check < nbytes)
        {
            const char *buf_aux[check];
            memcpy(buf_aux, buf, check);
            write(1, buf_aux, check);
        }
        else
        {
            write(1, buf, check); //los que hemos leído esta vez
        }
        if (check == 0)
        {
            EndOfFile = true;
        }
        offset += check; //los que hemos leído esta vez
    }
    return total; //retornamos todos los bytes que hemos leído en total
}

int mi_link(const char *camino1, const char *camino2)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir1, p_inodo1, p_inodo_dir2, p_inodo2;
    p_inodo_dir1 = p_inodo1 = p_inodo_dir2 = p_inodo2 = SB.posInodoRaiz;
    unsigned int p_entrada1 = 0;
    unsigned int p_entrada2 = 0;
    int error;
    //leemos el inodo, así que permisos bastan los de lectura
    if ((error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    if ((error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    struct entrada entrada;
    struct inodo inodo;
    mi_read_f(p_inodo_dir2, &entrada, (sizeof(struct entrada) * p_entrada2), sizeof(struct entrada));
    entrada.ninodo = p_inodo1; //le asignamos el mismo inodo que la entrada1
    mi_write_f(p_inodo_dir2, &entrada, (sizeof(struct entrada) * p_entrada2), sizeof(struct entrada));
    liberar_inodo(p_inodo2); //liberamos el inodo creado para la entrada2
    leer_inodo(p_inodo1, &inodo);
    inodo.nlinks++;
    inodo.ctime = time(NULL);
    escribir_inodo(p_inodo1, inodo);
    return EXIT_SUCCESS;
}

int mi_unlink(const char *camino, bool rmdir_r)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    //leemos el inodo, así que permisos bastan los de lectura
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    struct inodo inodo;
    struct inodo inodo_dir;
    struct entrada entrada;
    leer_inodo(p_inodo, &inodo);
    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0 && !rmdir_r)
    {
        fprintf(stderr, "El directorio no está vacío\n");
        return EXIT_FAILURE;
    }
    else
    {
        if (rmdir_r)
        { //si es un directorio y da igual que esté o no vacío
            leer_inodo(p_inodo, &inodo_dir);
            int entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
            for (int i = 0; i < entradas; i++)
            {
                mi_read_f(p_inodo, &entrada, (sizeof(struct entrada) * i), (sizeof(struct entrada)));
                leer_inodo(entrada.ninodo, &inodo);
                if (inodo.tipo == 'd')
                {
                    char *aux = malloc(TAMFILA);
                    memset(aux, 0, sizeof(char) * TAMFILA);
                    strcpy(aux, camino);
                    strcat(aux, entrada.nombre);
                    strcat(aux, "/");
                    mi_unlink(aux, true);
                }
                else
                {
                    liberar_inodo(entrada.ninodo);
                }
            }
            liberar_inodo(p_inodo);
        }
        else
        {
            leer_inodo(p_inodo_dir, &inodo_dir);
            int entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
            if (p_entrada == (entradas - 1))
            {
                mi_truncar_f(p_inodo_dir, (inodo_dir.tamEnBytesLog - sizeof(struct entrada)));
            }
            else
            {
                struct entrada ultima_entrada;
                mi_read_f(p_inodo_dir, &entrada, (sizeof(struct entrada) * p_entrada), sizeof(struct entrada));
                mi_read_f(p_inodo_dir, &ultima_entrada, (sizeof(struct entrada) * (entradas - 1)), sizeof(struct entrada));
                strcpy(entrada.nombre, ultima_entrada.nombre);
                entrada.ninodo = ultima_entrada.ninodo; //le asignamos el mismo inodo que la ultima entrada
                mi_write_f(p_inodo_dir, &entrada, (sizeof(struct entrada) * p_entrada), sizeof(struct entrada));
                mi_truncar_f(p_inodo_dir, (inodo_dir.tamEnBytesLog - sizeof(struct entrada)));
            }
            inodo.nlinks--;
            if (inodo.nlinks == 0)
            {
                liberar_inodo(p_inodo);
            }
            else
            {
                inodo.ctime = time(NULL);
                escribir_inodo(p_inodo, inodo);
            }
        }
        return EXIT_SUCCESS;
    }
}

int mi_rn(const char *caminoViejo, const char *caminoNuevo)
{
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    unsigned int p_entrada = 0;
    int error;
    //leemos el inodo, así que permisos bastan los de lectura
    if ((error = buscar_entrada(caminoViejo, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    int p_entrada_Aux = p_entrada;
    if ((error = buscar_entrada(caminoNuevo, &p_inodo_dir, &p_inodo, &p_entrada, 1, 6)) < 0)
    {
        mostrar_error_buscar_entrada(error);
#if DEBUG
        printf("***********************************************************************\n");
#endif
        return EXIT_FAILURE;
    }
    struct entrada entradaVieja;
    struct entrada entradaNueva;
    struct inodo inodo_dir;
    mi_read_f(p_inodo_dir, &entradaVieja, sizeof(struct entrada) * p_entrada_Aux, sizeof(struct entrada));
    mi_read_f(p_inodo_dir, &entradaNueva, sizeof(struct entrada) * p_entrada, sizeof(struct entrada));
    liberar_inodo(entradaNueva.ninodo);
    strcpy(entradaVieja.nombre, entradaNueva.nombre);
    leer_inodo(p_inodo_dir, &inodo_dir);
    mi_truncar_f(p_inodo_dir, (inodo_dir.tamEnBytesLog - sizeof(struct entrada)));
    mi_write_f(p_inodo_dir, &entradaVieja, sizeof(struct entrada) * p_entrada_Aux, sizeof(struct entrada));
    escribir_inodo(p_inodo_dir, inodo_dir);
    return EXIT_SUCCESS;
}