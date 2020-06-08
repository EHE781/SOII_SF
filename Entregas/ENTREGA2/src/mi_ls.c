//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "headers/directorios.h"
#define TAMFILA 100
#define TAMDIR 10
#define TAMFILE 59

int main(int argc, char **argv)
{
    if (argv[1] == NULL || argv[2] == NULL)
    {
        fprintf(stderr, "La sintaxis correcta es ./mi_ls <disco> </ruta_directorio> [opcional]<-l>[opcional]\n");
        return EXIT_FAILURE;
    }
    else
    {
        char tipo = '\0';
        struct superbloque SB;
        bool ext = false;
        bmount(argv[1]);
        bread(posSB, &SB);
#define TAMBUFFER (TAMFILA * SB.totInodos)
        char *buffer = malloc(TAMBUFFER);
        memset(buffer, 0, sizeof(char) * TAMBUFFER);
        if (argv[3] != NULL)
        {
            if ((strcmp(argv[3], "-l") == 0))
            {
                ext = true;
            }
            else
            {
                fprintf(stderr, "No existe la opción \"%s\"", argv[3]);
                return EXIT_FAILURE;
            }
        }
        if ((mi_dir(argv[2], buffer, &tipo, ext)) == EXIT_FAILURE)
        {
            bumount(argv[1]);
            return EXIT_FAILURE;
        }
        else
        {
            if (argv[2][(strlen(argv[2]) - 1)] != '/' && tipo == 'd')
            {
                fprintf(stderr, "No se ha encontrado el fichero, quizás es un directorio?\n");
                bumount(argv[1]);
                return EXIT_FAILURE;
            }
            else
            {

                if (ext == true)
                {
                    fprintf(stderr, "Tipo\tPermisos        mTime\t\t\tTamaño\tNombre\n");
                    fprintf(stderr, "----------------------------------------------------------------------\n%s", buffer);
                }
                else
                {
                    fprintf(stderr, "Nombre\n");
                    fprintf(stderr, "----------------\n%s", buffer);
                }
            }
        }
    }
    bumount(argv[1]);
}