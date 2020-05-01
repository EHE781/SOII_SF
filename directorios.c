#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    bool hayDir = false;
    if (camino[0] == '/')
    {
        for (int i = 1; i < strlen(camino); i++)
        {
            if (camino[i] == '/')
            { //caso en el que le pasemos //algo.txt no gestionado(doble barra)
                strncpy(inicial, camino, sizeof(char) * i - 1);
                hayDir = true;
                strcpy(final, camino + i);
                *tipo = 'd';
            }
        }
        if (!hayDir)
        {
            strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1); //+1 para evitar la primera '/'
            *tipo = 'f';
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
    bread(posSB, &SB);
    if (camino_parcial[0] == '/' && strlen(camino_parcial) == 1)
    {
        *p_inodo = SB.posInodoRaiz; //inodo raíz
        *p_entrada = 0;             //entrada 0 pertenece a inodo raíz
        return EXIT_SUCCESS;
    }
    struct entrada entrada;
    char inicial[sizeof(entrada.nombre)];
    char final[sizeof(strlen(camino_parcial))];
    char tipo;
    struct inodo inodo_dir;
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
    memset(buf_entradas, 0, BLOCKSIZE / sizeof(struct entrada));
    if (num_entrada_inodo > 0) //cant_entardas_inodo???
    {
        //leemos entradas
        offset += mi_read_f(*p_inodo_dir, buf_entradas, offset, BLOCKSIZE);
        //leemos primera entrada(0)
        while ((num_entrada_inodo < cant_entradas_inodo) && (inicial != buf_entradas[num_entrada_inodo].nombre))
        {
            num_entrada_inodo++;
            if ((num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))) == 0)
            { //hemos leido el buffer entero
                offset += mi_read_f(*p_inodo_dir, &buf_entradas, offset, BLOCKSIZE);
            }
        }
    }
    if ((num_entrada_inodo == cant_entradas_inodo) && (inicial != buf_entradas[num_entrada_inodo].nombre))
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
                strcpy(&entrada.nombre, &inicial);
                if (inodo_dir.tipo == 'd')
                {
                    if (strcmp(final, "/"))
                    {
                        entrada.ninodo = reservar_inodo('d', 2);
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    entrada.ninodo = reservar_inodo('f', 2);
                }
                error = mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog,
                                   sizeof(struct entrada));
                if (error == EXIT_FAILURE)
                {
                    if (entrada.ninodo != -1)
                    {
                        liberar_inodo(entrada.ninodo);
                    }
                    return EXIT_FAILURE;
                }
            }
        }
    }
    /*     
    si hemos llegado al final del camino  entonces 
    if (???)
    {
        si (num_entrada_inodo < cant_entradas_inodo) && (reservar=1) entonces 
        if (num_entrada_inodo < cant_entradas_inodo) && (reservar=1)
        {
        //modo escritura y la entrada ya existe
        return ERROR_ENTRADA_YA_EXISTENTE
        fsi 
        }
        // cortamos la recursividad

        asignar a *p_inodo el numero de inodo del directorio/fichero creado/leido
        asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene
        return EXIT_SUCCESS
    }else
    {
    si_no
        asignamos a *p_inodo_dir el puntero al inodo que se indica en la entrada;
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    fsi
    }
ffuncion
        */
}