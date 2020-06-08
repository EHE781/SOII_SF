//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "../headers/directorios.h"

int main(int argc, char **argv)
{
    if (argc == 4)
    {
        if (argv[3][strlen(argv[3] - 1)] != '/')
        {
            fprintf(stderr, "No es un directorio váilido\n");
            return EXIT_FAILURE;
        }
        else
        {
            if (strlen(argv[1]) != 2 || argv[1][1] != 'r')
            {
                fprintf(stderr, "Parámetro inválido, prueba con \"-r\"\n");
                return EXIT_FAILURE;
            }
        }
        bmount(argv[2]);
        mi_unlink(argv[3], true); //preguntar si se puede booleana
        bumount(argv[2]);
        return EXIT_SUCCESS;
    }
    else
    {
        if (argc == 3)
        {
            if (argv[2][strlen(argv[2] - 1)] != '/')
            {
                fprintf(stderr, "No es un directorio váilido\n");
                return EXIT_FAILURE;
            }
            bmount(argv[1]);
            mi_unlink(argv[2], false); //preguntar si se puede booleana
            bumount(argv[1]);
            return EXIT_SUCCESS;
        }
    }
    fprintf(stderr, "La sintaxis correcta es ./mi_rmdir [opcional-> -r] disco /ruta");
    return EXIT_FAILURE;
}