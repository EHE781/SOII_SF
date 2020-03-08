#include "bloques.h"
#include <time.h>
#include <limits.h> 
#include <stdbool.h>
#define INODOSIZE 128
#define posSB 0
#define tamSB 1
struct superbloque {
   unsigned int posPrimerBloqueMB;             // Posición del primer bloque del mapa de bits en el SF
   unsigned int posUltimoBloqueMB;             // Posición del último bloque del mapa de bits en el SF
   unsigned int posPrimerBloqueAI;                // Posición del primer bloque del array de inodos en el SF
   unsigned int posUltimoBloqueAI;                // Posición del último bloque del array de inodos en el SF
   unsigned int posPrimerBloqueDatos;            // Posición del primer bloque de datos en el SF
   unsigned int posUltimoBloqueDatos;           // Posición del último bloque de datos en el SF
   unsigned int posInodoRaiz;                           // Posición del inodo del directorio raíz en el AI 
   unsigned int posPrimerInodoLibre;               // Posición del primer inodo libre en el AI
   unsigned int cantBloquesLibres;                   // Cantidad de bloques libres del SF
   unsigned int cantInodosLibres;                     // Cantidad de inodos libres del SF
   unsigned int totBloques;                                // Cantidad total de bloques del SF
   unsigned int totInodos;                                   // Cantidad total de inodos del SF
   char padding[BLOCKSIZE - 12 * sizeof(unsigned int)]; // Relleno para que ocupe 1 bloque
};

struct inodo {     // comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
   char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
   char permisos; // Permisos (lectura y/o escritura y/o ejecución)
   /* Por cuestiones internas de alineación de estructuras, si se está utilizando
    un tamaño de palabra de 4 bytes (microprocesadores de 32 bits):
   unsigned char reservado_alineacion1 [2];
   en caso de que la palabra utilizada sea del tamaño de 8 bytes
   (microprocesadores de 64 bits): unsigned char reservado_alineacion1 [6]; */
   char reservado_alineacion1[6];
   time_t atime; // Fecha y hora del último acceso a datos: atime
   time_t mtime; // Fecha y hora de la última modificación de datos: mtime
   time_t ctime; // Fecha y hora de la última modificación del inodo: ctime

   /* comprobar el tamaño del tipo time_t para vuestra plataforma/compilador:
   printf ("sizeof time_t is: %d\n", sizeof(time_t)); */

   unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
   unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos. Se actualizará al escribir si crece
   unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos

   unsigned int punterosDirectos[12];  // 12 punteros a bloques directos
   unsigned int punterosIndirectos[3]; /* 3 punteros a bloques indirectos:
   1 indirecto simple, 1 indirecto doble, 1 indirecto triple */

   /* Utilizar una variable de alineación si es necesario para vuestra plataforma/compilador */
   char
       padding[INODOSIZE - 2 * sizeof(unsigned char) - 3 * sizeof(time_t) - 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char)];
   // Hay que restar también lo que ocupen las variables de alineación utilizadas!!!
};
struct inodo inodos[BLOCKSIZE/INODOSIZE];
int tamMB(unsigned int nbloques);
int tamAI(unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI();
int escribir_bit(unsigned int nbloque, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inodo(unsigned int ninodo, struct inodo inodo);
