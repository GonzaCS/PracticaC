//Trabajo realizado por:
//Diego Fraile Villa - 12421290W
//Gonzalo Calvo Sanz - 71186449T
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>
#include <ctype.h>


sem_t mutex_Buffer;
sem_t hayDato;
sem_t hayEspacio;

int *buffer1;
int tamBuffer=100;
bool finFichero=false;
int rango=3000000;
int numConsumidores=0;
int* id;

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
      sem_wait(&hayEspacio);
        fscanf(file,"%c",palabra);
        //if((strcmp(palabra,"d")==0)||(strcmp(palabra,"d")==0))
        if((isdigit(palabra))==0){ //quitamos los caracteres problematicos
        }else{
        int numero =atoi(palabra);    //para transformar de char a int
        dato = dato + numero;
        if((strcmp(palabra,"\n")==0)){
          //si es el final de linea dividimos entre 10 para tener el numero que hay en esa linea y lo guardamos en nuestro buffer 
        dato=dato/10;
        if(dato!=0){
        buffer1[j]=dato;
        sem_post(&hayDato);
        j=(j+1)%tamBuffer;
        dato=0;
        //sem_wait(&hayEspacio);
        }
           }else{
         dato=dato*10; //para escribir bien el numero multiplicamos por 10 para sumarle el siguiente luego
          
         }
      }

    }
    
    // for para ver el contenido del buffer
    for (int j=0;j<tamBuffer;j++){
    printf("%d\n",buffer1[j]);
    } 
    fclose(file);
    finFichero=true;
    pthread_exit(0);
};
void* consumidor(void* arg){
  int *z = (int *)arg;
  rango=(int)z*rango;
  int suma=0;
  int  max=0;
  int  min=0;
  int media=0;;
  int i=0;
  while(true){
    sem_wait(&hayDato);
    //para saber si el valor tratado esta dentro de nuestro rango
    sem_wait(&mutex_Buffer);
    int datob=buffer1[i];
    sem_post(&mutex_Buffer);
    if (datob<(rango-1)){
    //por si solo se recorre una vez
        if(i==0){
        max=datob;
        min=datob;
        }
      suma = suma + datob;
      if(datob>max){
        max=datob;
      }
      if(datob<min && (min!=0)){
        min=datob;
      }
    }
media=suma/(tamBuffer/numConsumidores);
}
sem_post(&hayEspacio);
  printf("El max es:%d\n",max);
  printf("La suma es:%d\n",suma);
  printf("La media es:%d\n",media);
  printf("El min es:%d\n",min);
  pthread_exit(0);
}
int main(int argc, char* argv[]) {
  int tamBuffer= atoi(argv[3]);
  int numConsumidores=atoi(argv[4]);
  /*
  if(tamBuffer<1){
    printf("El tamaño del buffer introducido es incorrecto");
    return 0;
  }
 
  if(numConsumidores<1){
    printf("El numero de consumidores introducido es incorrecto");
    return 0;
  }
  */
  numConsumidores=1;
  rango = rango/numConsumidores;
   //Memoria dinámica,
  buffer1=(int*)malloc(tamBuffer*sizeof(int));
  
    //iniciador hilo
    pthread_t productorhilo;
    pthread_t consumidorhilo;
  //iniciador de semaforo, esto me lo dijo el profe así que será así.
    sem_init(&hayEspacio,0,tamBuffer);
    sem_init(&hayDato,0,0);
    sem_init(&mutex_Buffer,0,1);

    //creador hilo
  
  int id[numConsumidores];
  for(int i=0;i<numConsumidores;i++){
    id[i]=i;
  }
    pthread_create(&productorhilo,NULL,productor,(void*)NULL);
   

  for(int i=0;i<numConsumidores;i++){
    pthread_create(&consumidorhilo,NULL,consumidor,(void*)&id[i]);
  }

    pthread_join(productorhilo,NULL);
    //pthread_join(consumidorhilo,NULL);
    return (0);
};

