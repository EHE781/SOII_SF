#include    "bloques.h"
#include    "ficheros_basico.h"
const char *dir;
struct superbloque SB;
void *buf[BLOCKSIZE];
int main(int argc, char **argv){
dir = argv[1];
bmount(dir);
bread(posSB, buf);
memcpy(&SB, buf, sizeof(struct superbloque)); //aprovechando la declaracion de SB, rellenarlo con lo que hemos leído
fprintf(stderr, "DATOS DEL SUPERBLOQUE\n");
fprintf(stderr, "posPrimerBloqueMB: %i\n",SB.posPrimerBloqueMB);
fprintf(stderr, "posUltimoBloqueMB: %i\n", SB.posUltimoBloqueMB);
fprintf(stderr, "posPrimerBloqueAI: %i\n",SB.posPrimerBloqueAI);
fprintf(stderr, "posUltimoBloqueAI: %i\n", SB.posUltimoBloqueAI);
fprintf(stderr, "posPrimerBloqueDatos: %i\n", SB.posPrimerBloqueDatos);
fprintf(stderr, "posUltimoBloqueDatos: %i\n", SB.posUltimoBloqueDatos);
fprintf(stderr, "posInodoRaíz: %i\n", SB.posInodoRaiz);
fprintf(stderr, "posPrimerInodoLibre: %i\n", SB.posPrimerInodoLibre);
fprintf(stderr, "cantBloquesLibres: %i\n", SB.cantBloquesLibres);
fprintf(stderr, "cantInodosLibres: %i\n", SB.cantInodosLibres);
fprintf(stderr, "totBloques: %i\n", SB.totBloques);
fprintf(stderr, "totInodos: %i\n\n", SB.totInodos);
return EXIT_SUCCESS;
}