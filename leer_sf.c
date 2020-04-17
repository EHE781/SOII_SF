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
/* Comentado para ENTREGA 1
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
}--Al descomentar poner un end comment aqui
printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS:\n");
int reservado = reservar_bloque();
bwrite(posSB, &SB);
printf("Se ha reservado el bloque físico nº %i que era el 1º libre indicado por el MB.\n",reservado);
printf("SB.cantBloquesLibres: %i\n", SB.cantBloquesLibres);
liberar_bloque(reservado);
bwrite(posSB, &SB);
printf("Liberamos ese bloque, y después SB.cantBloquesLibres: %i\n\n",SB.cantBloquesLibres);
printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
int bit = leer_bit(posSB);
printf("valor del bit correspondiente a posSB (o sea al BF nº %i)  = %i\n",posSB,bit);
printf("[leer_bit()→ nbloque: %i]\n\n",posSB);
bit = leer_bit(SB.posPrimerBloqueMB);
printf("[leer_bit()→ nbloque: %i]\n",SB.posPrimerBloqueMB);
printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %i)  = %i\n\n",SB.posPrimerBloqueMB,bit);
bit = leer_bit(SB.posUltimoBloqueMB);
printf("[leer_bit()→ nbloque: %i]\n",SB.posUltimoBloqueMB);
printf("valor del bit correspondiente a posUltimoBloqueMB (o sea al BF nº %i)  = %i\n\n",SB.posUltimoBloqueMB,bit);
bit = leer_bit(SB.posPrimerBloqueAI);
printf("[leer_bit()→ nbloque: %i]\n",SB.posPrimerBloqueAI);
printf("valor del bit correspondiente a posPrimerBloqueAI (o sea al BF nº %i)  = %i\n\n",SB.posPrimerBloqueAI,bit);
bit = leer_bit(SB.posUltimoBloqueAI);
printf("[leer_bit()→ nbloque: %i]\n",SB.posUltimoBloqueAI);
printf("valor del bit correspondiente a posUltimoBloqueAI (o sea al BF nº %i)  = %i\n\n",SB.posUltimoBloqueAI,bit);
bit = leer_bit(SB.posPrimerBloqueDatos);
printf("[leer_bit()→ nbloque: %i]\n",SB.posPrimerBloqueDatos);
printf("valor del bit correspondiente a posPrimerBloqueDatos (o sea al BF nº %i)  = %i\n\n",SB.posPrimerBloqueDatos,bit);
bit = leer_bit(SB.posUltimoBloqueDatos);
printf("[leer_bit()→ nbloque: %i]\n",SB.posUltimoBloqueDatos);
printf("valor del bit correspondiente a posUltimoBloqueDatos (o sea al BF nº %i)  = %i\n\n",SB.posUltimoBloqueDatos,bit);
printf("\nDATOS DEL DIRECTORIO RAIZ\n\n");
/*DATOS INODO RAIZ, ETC  -- Descomentar solo esta línea para ello
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
printf("tipo: %c\n",inodo.tipo);
printf("permisos: %i\n",inodo.permisos);
printf("ID: %d \nATIME: %s \nMTIME: %s \nCTIME: %s\n",ninodo,atime,mtime,ctime);
printf("nlinks: %i\n",inodo.nlinks);
printf("tamaño en bytes lógicos: %i\n",inodo.tamEnBytesLog);
printf("Número de bloques ocupados: %i\n",inodo.numBloquesOcupados);
/*CHECK PARTE 4
printf("INODO 1. TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 16.843.019\n\n");
int inodoReservado = reservar_inodo('f',6);
traducir_bloque_inodo(inodoReservado,8,1);
traducir_bloque_inodo(inodoReservado,204,1);
traducir_bloque_inodo(inodoReservado,30004,1);
traducir_bloque_inodo(inodoReservado,400004,1);
traducir_bloque_inodo(inodoReservado,16843019,1);
Falta comprobacion de si el inodo ha puesto una f y tiene permisos 6, pero supondremos que si
*/
    return EXIT_SUCCESS;
}