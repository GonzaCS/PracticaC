//Trabajo realizado por:
//Diego Fraile Villa - 12421290W
//Gonzalo Calvo Sanz - 71186449T
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>

sem_t mutex;

int *buffer1;
int tamBuffer=101;
int B[101];
bool finFichero=false;


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
        buffer1[j]=dato;
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
    for (int j=0;j<101;j++){
    printf("%d\n",buffer1[j]);
     
    } 
    
    fclose(file);
    finFichero=true;
    pthread_exit(0);
};
void* consumidor(){
  int suma=0;
  int  max=0;
  int min= 100000000;
  int media;
  while(finFichero==true){
  for(int i=0;i<tamBuffer;i++){
        if(i==0){
        max=buffer1[i];
        min=buffer1[i];
    }
      suma = suma + buffer1[i];
      if(buffer1[i]>max){
        max=buffer1[i];
      }
      else{
        min=buffer1[i];
      }
  }
  media=suma/tamBuffer;
  }
   //Esto hace lo que quiere hay que mirar porque
  printf("El max es:%d\n",max);
  printf("La media es:%d\n",media);
  printf("El min es:%d\n",min);
   
  pthread_exit(0);
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
  int tamBuffer=101;
   //Memoria dinámica, falta cambiarla para hacerla funcional en B
  //buffer1=(int*)argv[3];
  buffer1=(int*)malloc(tamBuffer*sizeof(int));
  
    //iniciador hilo

    pthread_t productorhilo;
    //pthread_t consumidorhilo;

  //iniciador de semaforo, esto me lo dijo le profe así que será así.
    //sem_init(&hay_espacio,0,1);
    //sem_init(&hay_dato,0,1);
    //creador hilo
    pthread_create(&productorhilo,NULL,productor,(void*)NULL);
    //pthread_create(&consumidorhilo,NULL,consumidor,(void*)NULL);


    pthread_join(productorhilo,NULL);
    //pthread_join(consumidorhilo,NULL);

    return 0;
};

