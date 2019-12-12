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
sem_t consumListo;
sem_t mutexLector;

int *buffer1;


int numMaxPosible=3000000;

int tamBuffer;
int numConsumidores;
char *nombreFich;
FILE *fichSalida;

struct valoresConsumidor{
    //Creación de estructura con la que trabajaremos en cada Hilo consumidor
    int numerodatos;
    int maximodato;
    int minimodato;
    int sumatotal;
    int rango_up;
    int rango_down;
    float media;
    bool finish;
};

struct valoresConsumidor *valores;

void *productor(void *args){
    char *fichero;
    fichero=nombreFich;
    int j=0;
    int dato=0;
    char caracter;
    FILE *file;
    //"C:\\Users\\CLION\\untitled\\numeros.dat"
    file=fopen(fichero,"r");
    if(file== NULL) {
        printf("No se ha podido encontrar el fichero");
        exit(-1);
    }
    while(feof(file)!=1){

        caracter=fgetc(file);

        //if((strcmp(palabra,"d")==0)||(strcmp(palabra,"d")==0))
        if(((isdigit((int)caracter))==0)&&(caracter!='\n')){

            //quitamos los caracteres problematicos
        }else{

            int numero = caracter-'0';    //para transformar de char a int
            if(caracter=='\n'){
                //si es el final de linea dividimos entre 10 para tener el numero que hay en esa linea y lo guardamos en nuestro buffer
                dato=dato/10;
                if(dato!=0){
                    sem_wait(&hayEspacio);
                    //sem_wait(&mutex_Buffer);
                    buffer1[j]=dato;
                    //sem_post(&mutex_Buffer);
                    sem_post(&hayDato);
                    j=(j+1)%tamBuffer;
                    dato=0;
                    //sem_wait(&hayEspacio);
                }
            }else{
                dato = dato + numero;
                dato=dato*10; //para escribir bien el numero multiplicamos por 10 para sumarle el siguiente luego

            }
        }

    }
    sem_wait(&hayEspacio);
    buffer1[j]=-1;
    sem_post(&hayDato);


    // for para ver el contenido del buffer
    /*  for (int j=0;j<tamBuffer;j++){
          printf("%d\n",buffer1[j]);
      }*/
    fclose(file);
    pthread_exit(0);
}
void* consumidor(void* arg){
    int *id = (int *)arg;
    int ident=*id;
    int rango= numMaxPosible/(numConsumidores);
    int rango_down=*id*rango;
    int rango_up=((*id +1)*rango )-1;
    struct valoresConsumidor valCons;
    int suma=0;
    int  max=0;
    int  min=3000000;
    float media=0;
    int i=0;
    int datob=0;
    int numeroDatosLeidos=0;
    valCons.finish=false;
    while(true){
        sem_wait(&hayDato);
        //para saber si el valor tratado esta dentro de nuestro rango
        sem_wait(&mutex_Buffer);
        datob=buffer1[i];
        sem_post(&mutex_Buffer);

        if(datob== -1) {
            sem_post(&hayDato);
            break;
        }

        if((rango_down<=datob)&&(datob<=rango_up)){
            numeroDatosLeidos++;
            suma = suma + datob;
            if(datob>max){
                max=datob;
            }
            if(datob<min && (min!=0)){
                min=datob;
            }

            sem_post(&hayEspacio);
        }else {
            sem_post(&hayDato);
        }
        i=(i+1)%tamBuffer;
    }
    media=suma/numeroDatosLeidos;

    valCons.sumatotal=suma;
    valCons.maximodato=max;
    valCons.minimodato=min;
    valCons.media=media;
    valCons.rango_down=rango_down;
    valCons.rango_up=rango_up;
    valCons.numerodatos=numeroDatosLeidos;
    valCons.finish=true;
    valores[ident]=valCons;
    printf("ha terminado el %d consumidor", ident);
    sem_post(&consumListo);
    pthread_exit(0);
}

void *lector(void* args){
    int numleidos=0;
    if(fichSalida== NULL) {
        printf("No se ha podido encontrar el fichero");
        exit(-1);
    }
    while(true) {

        sem_wait(&consumListo);
        sem_wait(&mutexLector);

        for (int i = 0; i < numConsumidores; i++) {



            if (valores[i].finish == true) {
                fprintf(fichSalida, "====== RANGO DEL HILO %d [%d]-[%d]\n", i, valores[i].rango_down,valores[i].rango_up);
                fprintf(fichSalida, "El numero de datos del hilo %d es: %d\n", i, valores[i].numerodatos);
                fprintf(fichSalida, "El maximo del hilo %d es:%d\n", i, valores[i].maximodato);
                fprintf(fichSalida, "El minimo del hilo %d es:%d\n", i, valores[i].minimodato);
                fprintf(fichSalida, "La suma total del hilo  %d es:%d\n", i, valores[i].sumatotal);
                fprintf(fichSalida, "La media del hilo %d es:%12.6f\n", i, valores[i].media);
                fprintf(fichSalida, "\n\n");
                valores[i].finish = false;
                fflush(fichSalida);

                numleidos++;
                break;
            }
        }
        sem_post(&mutexLector);
        if (numleidos == numConsumidores){
            break;
        }

    }
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    char *fichero= argv[1];
    nombreFich =fichero;
    char *ficheroSal= argv[2];
    char *ficherSalida= ficheroSal;

    fichSalida=fopen(ficherSalida,"w");


    sscanf(argv[3],"%d",&tamBuffer);
    sscanf(argv[4],"%d",&numConsumidores);

    if(tamBuffer<1){
        printf("El tamaño del buffer introducido es incorrecto");
        return 0;
    }

    if(numConsumidores<1){
        printf("El numero de consumidores introducido es incorrecto");
        return 0;
    }

    //Memoria dinámica,
    buffer1=(int*)malloc(tamBuffer*sizeof(int));
    valores=(struct valoresConsumidor*)malloc(numConsumidores*sizeof(struct valoresConsumidor));


    //iniciador hilo
    pthread_t productorhilo;
    pthread_t consumidorhilo[numConsumidores];
    pthread_t lectorhilo;
    //iniciador de semaforo, esto me lo dijo el profe así que será así.
    sem_init(&hayEspacio,0,tamBuffer);
    sem_init(&hayDato,0,0);
    sem_init(&mutex_Buffer,0,1);
    sem_init(&consumListo,0,0);
    sem_init(&mutexLector,0,1);
    //creador hilo

    int id[numConsumidores];
    for(int i=0;i<numConsumidores;i++){
        id[i]=i;
    }
    pthread_create(&productorhilo,NULL,productor,(void*)NULL);
    pthread_create(&lectorhilo,NULL,lector,(void*)NULL);


    for(int i=0;i<numConsumidores;i++){
        pthread_create(&consumidorhilo[i],NULL,consumidor,(void*)&id[i]);
    }

    pthread_join(productorhilo,NULL);
    pthread_join(lectorhilo,NULL);
    for(int i=0;i<numConsumidores;i++){
        pthread_join(consumidorhilo[i],NULL);
    }

    return (0);
}
