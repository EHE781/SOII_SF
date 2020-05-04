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
    memset (&inicial, 0, sizeof(entrada.nombre));
    memset (&final, 0, strlen(camino_parcial));
    char tipo;
    struct inodo inodo_dir;
    bread(posSB, &SB);
    if (camino_parcial[0] == '/' && strlen(camino_parcial) == 1)
    {
        *p_inodo = SB.posInodoRaiz; //inodo raíz
        *p_entrada = 0;             //entrada 0 pertenece a inodo raíz
        return EXIT_SUCCESS;
    }
    error = extraer_camino(camino_parcial, &inicial, &final, &tipo);
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
    if ((num_entrada_inodo == cant_entradas_inodo) && (inicial != buf_entradas[num_entrada_inodo].nombre))
    {
        //printf("[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
        switch (reservar)
        {
        case 0: //modo consulta
           // printf("[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
        case 1: //modo escritura
            if (inodo_dir.tipo == 'f')
            {
                 printf("[diasbuscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            if ((inodo_dir.permisos & 2) != 2)
            {
                 printf("[buenasbuscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                strcpy(&entrada.nombre, &inicial);
                if (tipo == 'd')
                {
                    if (strcmp(final, "/") == 0)
                    {
                        printf("ktal[buscar_entrada()->reservado inodo %d tipo d con permisos 6 para %s]\n",entrada.ninodo,entrada.nombre);
                        entrada.ninodo = reservar_inodo('d', 6);
                        
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    printf("adios[buscar_entrada()->reservado inodo %d tipo f con permisos 6 para %s]\n",entrada.ninodo,entrada.nombre);
                    entrada.ninodo = reservar_inodo('f', 6);
                    
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
       // printf("[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
    } 
    if ((strcmp(final, "/") == 0) || tipo == 'f')
    {
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
        //modo escritura y la entrada ya existe
        printf("[penebuscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
        return ERROR_ENTRADA_YA_EXISTENTE;
        }
        // cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return EXIT_SUCCESS;
    }else
    {
        printf("hola[buscar_entrada()-> inicial: %s, final: %s, reserva: %d] \n",inicial,final,reservar);
        *p_inodo_dir = buf_entradas[num_entrada_inodo].ninodo;
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
}

void mostrar_error_buscar_entrada(int error){
     // fprintf(stderr, "Error: %d\n", error);
   switch (error) {
   case -1: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
   case -2: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
   case -3: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
   case -4: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
   case -5: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
   case -6: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
   case -7: fprintf(stderr, "Error: No es un directorio.\n"); break;
   }

}