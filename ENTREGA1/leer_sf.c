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
printf("DATOS DEL SUPERBLOQUE\n");
printf("posPrimerBloqueMB: %i\n",SB.posPrimerBloqueMB);
printf("posUltimoBloqueMB: %i\n", SB.posUltimoBloqueMB);
printf("posPrimerBloqueAI: %i\n",SB.posPrimerBloqueAI);
printf("posUltimoBloqueAI: %i\n", SB.posUltimoBloqueAI);
printf("posPrimerBloqueDatos: %i\n", SB.posPrimerBloqueDatos);
printf("posUltimoBloqueDatos: %i\n", SB.posUltimoBloqueDatos);
printf("posInodoRaíz: %i\n", SB.posInodoRaiz);
printf("posPrimerInodoLibre: %i\n", SB.posPrimerInodoLibre);
printf("cantBloquesLibres: %i\n", SB.cantBloquesLibres);
printf("cantInodosLibres: %i\n", SB.cantInodosLibres);
printf("totBloques: %i\n", SB.totBloques);
printf("totInodos: %i\n\n", SB.totInodos);
return EXIT_SUCCESS;
}