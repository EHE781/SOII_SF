#include "../headers/directorios.h"
 
void mostrar_buscar_entrada(char *camino, char reservar){
 unsigned int p_inodo_dir = 0;
 unsigned int p_inodo = 0;
 unsigned int p_entrada = 0;
 int error;
 printf("\ncamino: %s, reservar: %d\n", camino, reservar);
 if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
   mostrar_error_buscar_entrada(error);
 }
 printf("**********************************************************************\n");
 return;
}
 
int main(int argc, char **argv){
 if (argc!=2) {
   fprintf(stderr, "Sintaxis: pruebas_buscar_entrada <nombre_dispositivo>\n");
   exit(-1);
  }
  //montamos el dispositivo
  if(bmount(argv[1])<0) return -1;
 
//Mostrar creación directorios y errores
 mostrar_buscar_entrada("pruebas/", 1); //ERROR_CAMINO_INCORRECTO
 mostrar_buscar_entrada("/pruebas/", 0); //ERROR_NO_EXISTE_ENTRADA_CONSULTA
 mostrar_buscar_entrada("/pruebas/docs/", 1); //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
 mostrar_buscar_entrada("/pruebas/", 1); // creamos /pruebas/
 mostrar_buscar_entrada("/pruebas/docs/", 1); //creamos /pruebas/docs/
 mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
 mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1); //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
 mostrar_buscar_entrada("/pruebas/", 1); //ERROR_ENTRADA_YA_EXISTENTE
 mostrar_buscar_entrada("/pruebas/docs/doc1", 0); //consultamos /pruebas/docs/doc1
 mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
 mostrar_buscar_entrada("/pruebas/casos/", 1); //creamos /pruebas/casos/
 mostrar_buscar_entrada("/pruebas/docs/doc2", 1); //creamos /pruebas/docs/doc2
 
 bumount(argv[1]);
}
