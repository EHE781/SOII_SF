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
printf("sizeof struct superbloque: %lu\n", sizeof(struct superbloque));
printf("sizeof struct inodo: %lu\n\n", sizeof(struct inodo));
printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES(omitido por longevidad):\n");/*
int bloqueAI = SB.posPrimerBloqueAI;
int contInodo = 0; //variable para leer inodo a inodo en un bloque
for(int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
    bread(bloqueAI, &inodos); //si no en vez de &inodos meter buf
    //memcpy(&inodos, buf, sizeof(inodos));
    for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++){
    if (contInodo >= SB.totInodos ){
        printf("%i \n", inodos[j].punterosDirectos[0]);
            break;
        }
    printf("%i ", inodos[j].punterosDirectos[0]); //imprimimos a donde apunta
    contInodo ++; //aumentamos en un inodo los leidos hasta ahora
    }
    bloqueAI++; //siguiente bloque a leer
}*/
printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS:\n");
int reservado = reservar_bloque();
bwrite(posSB, &SB);
printf("Se ha reservado el bloque físico nº 3139 que era el 1º libre indicado por el MB.\n");
printf("SB.cantBloquesLibres: %i\n", SB.cantBloquesLibres);
liberar_bloque(reservado);
printf("Liberamos ese bloque, y después SB.cantBloquesLibres: %i\n\n",SB.cantBloquesLibres);
printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
int bit = leer_bit(posSB);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posSB (o sea al BF nº %i)  = %i",posSB,bit);
bit = leer_bit(SB.posPrimerBloqueMB);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %i)  = %i",SB.posPrimerBloqueMB,bit);
bit = leer_bit(SB.posUltimoBloqueMB);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posUltimoBloqueMB (o sea al BF nº %i)  = %i",SB.posUltimoBloqueMB,bit);
bit = leer_bit(SB.posPrimerBloqueAI);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posPrimerBloqueAI (o sea al BF nº %i)  = %i",SB.posPrimerBloqueAI,bit);
bit = leer_bit(SB.posUltimoBloqueAI);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posUltimoBloqueAI (o sea al BF nº %i)  = %i",SB.posUltimoBloqueAI,bit);
bit = leer_bit(SB.posPrimerBloqueDatos);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posPrimerBloqueDatos (o sea al BF nº %i)  = %i",SB.posPrimerBloqueDatos,bit);
bit = leer_bit(SB.posUltimoBloqueDatos);
printf("[leer_bit()→ nbloque: %i]\n");
printf("valor del bit correspondiente a posUltimoBloqueDatos (o sea al BF nº %i)  = %i",SB.posUltimoBloqueDatos,bit);
printf("\nDATOS DEL DIRECTORIO RAIZ\n");
struct tm *ts;
char atime[80];
char mtime[80];
char ctime[80];

struct inodo inodo;
int ninodo = 0; //el directorio raiz es el inodo 0
leer_inodo(ninodo, &inodo);
ts = localtime(&inodo.atime);
strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
ts = localtime(&inodo.mtime);
strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
ts = localtime(&inodo.ctime);
strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
printf("tipo: %c",&inodo.tipo);
printf("permisos: %i",&inodo.permisos);
printf("ID: %d \nATIME: %s \nMTIME: %s \nCTIME: %s\n",ninodo,atime,mtime,ctime);
printf("nlinks: %i",&inodo.nlinks);
printf("tamaño en bytes lógicos: %i",&inodo.tamEnBytesLog);
printf("Número de bloques ocupados: %i",&inodo.numBloquesOcupados);
    return 0;
}