#include    "bloques.h"
#include    "ficheros_basico.h"
const char *dir;
void *buf[BLOCKSIZE];
int main(int argc, char **argv){
dir = argv[1];
bmount(dir);
bread(posSB, buf);
memcpy(&SB, buf, sizeof(struct superbloque)); //aprovechando la declaracion de SB, rellenarlo con lo que hemos leído
printf("DATOS DEL SUPERBLOQUE\n");
printf("posPrimerBloqueMB: %i\n",SB.posPrimerBloqueMB);
printf("posUltimoBloqueMB: %i\n", SB.posPrimerBloqueMB);
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
printf("sizeof struct superbloque: %li\n", sizeof(struct superbloque));
printf("sizeof struct inodo: %li\n\n", sizeof(struct inodo));
printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES:\n");
int bloqueAI = SB.posPrimerBloqueAI;
int changeBloque = MAX_INODO; // variable para contar cuando cambiar de bloque
int contInodo = 0; //variable para leer inodo a inodo en un bloque
for(int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
    bread(bloqueAI, &inodos); //si no en vez de &inodos meter buf
    //memcpy(&inodos, buf, sizeof(inodos));
    for (int j = 0; j < MAX_INODO; j++){
    if (contInodo >= SB.totInodos ){
        printf("%i \n", inodos[j].punterosDirectos[0]);
            break;
        }
    printf("%i ", inodos[j].punterosDirectos[0]); //imprimimos a donde apunta
    contInodo ++; //aumentamos en un inodo los leidos hasta ahora
    }
    bloqueAI++; //siguiente bloque a leer
}
    return 0;
}
