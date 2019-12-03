#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int B[101];

void* productor(void *args){
    int i=0;
    int dato;
    char nombre[]= "numeros.dat";
    FILE *file;
    file=fopen(nombre,"r");
    while(file != EOF){
        fscanf(file,"%d",&dato);
        
        B[i]=dato;
        printf("%d",B[i]);
        printf(" ");
        i=i+1;



    }
    printf("%d",i);
    fclose(file);

    pthread_exit(0);
}


int main() {
    //iniciador hilo
    pthread_t productorhilo;
    //crador hilo
    pthread_create(&productorhilo,NULL,productor,(void*)NULL);

    pthread_join(productorhilo,NULL);


    return 0;
}

