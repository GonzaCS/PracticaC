//Trabajo realizado por:
//Diego Fraile Villa - 12421290W
//Gonzalo Calvo Sanz - 71186449T
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int B[101];

void* productor(void *args){
    int i=0;
    int j=0;
    int dato;
    char palabra[10];
    char nombre[]= "numeros.dat";
    FILE *file;
    file=fopen(nombre,"r");
    if(file== NULL){
      printf("No se ha podido encontrar el");
      exit(1);
    }

    while(feof(file)!=1){
        fscanf(file,"%c",palabra);
        if((strcmp(palabra,"k")==0) || (strcmp(palabra,"d")==0) ){ //quitamos los caracteres problematicos
        i=i+1;
        }else{
        int numero =atoi(palabra);    //para transformar de char a int
        dato = dato + numero;
        if((strcmp(palabra,"\n")==0)){
          //si es el final de linea dividimos entre 10 para tener el numero que hay en esa linea y lo guardamos en nuestro buffer 
        dato=dato/10;
        if(dato!=0){
        B[j]=dato;
        j=j+1;
        dato=0;
        }
     }
        else{
         dato=dato*10; //para escribir bien el numero multiplicamos por 10 para sumarle el siguiente luego

        //printf("%c",*palabra); 
          i=i+1;
         }
        }

    }
    for (j=0;j<101;j++){
     printf("%d\n",B[j]);
     
    }  
    fclose(file);

    pthread_exit(0);
}
void* consumidor(void *args){
  pthread_exit(0);
  //Aún no hemos implementado el código del primer consumidor
}


struct valoresConsumidor{
  //Creación de estructura con la que trabajaremos en cada Hilo consumidor
int numerodatos;
int maximodato;
int minimodato;
int sumatotal;
int media;
int mediana;
int cuartil;
};
int main(int argc, char* argv[]) {
    //iniciador hilo
    pthread_t productorhilo;
    pthread_t consumidorhilo;

    //creador hilo
    pthread_create(&productorhilo,NULL,productor,(void*)NULL);
    pthread_create(&consumidorhilo,NULL,consumidor,(void*)NULL);


    pthread_join(productorhilo,NULL);
    pthread_join(consumidorhilo,NULL);

    return 0;
}