#include "bloques.h"
char cwd[PATH_MAX];
int nbloques = 0;
unsigned char *buf;

int main(int argc, char **argv){
    memset(buf, 0, BLOCKSIZE);
    bmount("/home/ehe781/SOII_SF/disco");
    nbloques = atoi(argv[2]);
    for(int i = 0 ; i < nbloques ; i++){
        bwrite(i, buf);
    }
    bumount();
}