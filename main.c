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

//Creación de los nombres de los semáforos usados
sem_t mutex_Buffer;
sem_t hayDato;
sem_t hayEspacio;
sem_t consumListo;
sem_t mutexLector;
//Creación de todas las variables globales usadas.
int *buffer1;
int numMaxPosible=3000000;
//indexB es nuestra memoria circular que usamos en Consumidor
int indexB=0;
int tamBuffer;
int numConsumidores;
//Variables para la lectura del nommbre del fichero que usaremos en main / productor
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

struct tipoNodoLista{
    //Estructura para el cuartil y la mediana
    int m_dato;
    struct tipoNodo* msiguiente;
};

struct valoresConsumidor *valores;
struct tipoNodoLista *lista;

void *productor(void *args){
    char *fichero;
    fichero=nombreFich;
    int j=0;
    int dato=0;
    char caracter;
    FILE *file;
    file=fopen(fichero,"r");
    if(file== NULL) {
        printf("No se ha podido encontrar el fichero");
        exit(-1);
    }
    while(feof(file)!=1){
        //cogemos caracter a caracter de la linea del fichero
        caracter=fgetc(file);
        if(((isdigit((int)caracter))==0)&&(caracter!='\n')){
            //quitamos los caracteres problematicos es decir las letras y cuando es un final de linea
        }else{

            int numero = caracter-'0';    //para transformar de char a int le restamos el caracter 0
            if(caracter=='\n'){
                //si es el final de linea dividimos entre 10 para tener el numero que hay en esa linea y lo guardamos en nuestro buffer
                dato=dato/10;
                //guardamos solo si el dato es diferente de 0.
                if(dato!=0){
                    sem_wait(&hayEspacio);
                    buffer1[j]=dato;
                    sem_post(&hayDato);
                    //Aumentamos la dirección de la memoria circular local
                    j=(j+1)%tamBuffer;
                    //reseteamos el valor a 0
                    dato=0;
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
    fclose(file);
    pthread_exit(0);
}
void* consumidor(void* arg){
    int *id = (int *)arg;
    //pasamos el valor del apuntador del numero de consumidor a ident
    int ident=*id;
    //creamos el rango segun el numero de consumidores
    int rango= numMaxPosible/(numConsumidores);
    //rango de abajo y rango de arriba se calculan segun el rango anterior
    int rango_down=*id*rango;
    int rango_up=((*id +1)*rango )-1;
    // creación de estructura con los valores de cada consumidor
    struct valoresConsumidor valCons;
    //variables locales que se usan para consumidor
    int suma=0;
    int  max=0;
    int  min=3000000;
    float media=0;
    int datob=0;
    int numeroDatosLeidos=0;
    //finalizador booleano para saber si ha terminado todos los datos de su rango para luego usarlo en el lector
    valCons.finish=false;
    while(true){
        sem_wait(&hayDato);
        sem_wait(&mutex_Buffer);
        datob=buffer1[indexB];


        if(datob== -1) {
            sem_post(&mutex_Buffer);
            sem_post(&hayDato);
            break;
        }

        if((rango_down<=datob)&&(datob<=rango_up)){
            //para saber si el valor tratado esta dentro de nuestro rango
            indexB=(indexB + 1)%tamBuffer;
            sem_post(&mutex_Buffer);
            sem_post(&hayEspacio);

            numeroDatosLeidos++;
            suma = suma + datob;
            if(datob>max){
                max=datob;
            }
            if(datob<min && (min!=0)){
                min=datob;
            }


        }else {
            //si no esta en nuestro rango, decimos que no hemos usado el dato.
            sem_post(&mutex_Buffer);
            sem_post(&hayDato);
        }

    }
    if(numeroDatosLeidos!=0) {
        //cuando el numero de datos es diferente de 0 los calculamos aqui, si no los ponemos a 0.
        media = (float) suma / (float) numeroDatosLeidos;

        valCons.sumatotal = suma;
        valCons.maximodato = max;
        valCons.minimodato = min;
        valCons.media = media;
        valCons.rango_down = rango_down;
        valCons.rango_up = rango_up;
        valCons.numerodatos = numeroDatosLeidos;
        valCons.finish = true;
        valores[ident] = valCons;
    }else{
        valCons.sumatotal=0;
        valCons.maximodato=0;
        valCons.minimodato=0;
        valCons.media=0;
        valCons.rango_down=rango_down;
        valCons.rango_up=rango_up;
        valCons.numerodatos=numeroDatosLeidos;
        valCons.finish=true;
        valores[ident]=valCons;
    }
    //semaforo P para advertir al lector que ya puede leer del consumidor.
    sem_post(&consumListo);
    pthread_exit(0);
}

void *lector(void* args){
    //variable local para el numero de lectores
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
                //guardamos en el fichero con la estructura mostrada en el pdf de la práctica.
                fprintf(fichSalida, "====== RANGO DEL HILO %d [%d]-[%d]\n", i, valores[i].rango_down,valores[i].rango_up);
                fprintf(fichSalida, "El numero de datos del hilo %d es: %d\n", i, valores[i].numerodatos);
                fprintf(fichSalida, "El maximo del hilo %d es:%d\n", i, valores[i].maximodato);
                fprintf(fichSalida, "El minimo del hilo %d es:%d\n", i, valores[i].minimodato);
                fprintf(fichSalida, "La suma total del hilo  %d es:%d\n", i, valores[i].sumatotal);
                fprintf(fichSalida, "La media del hilo %d es:%12.6f\n", i, valores[i].media);
                fprintf(fichSalida, "\n\n");
                //cambiamos la variable booleana a falso, ya que ha acabado de escribir.
                valores[i].finish = false;
                //limpiamos del buffer los datos sobrantes (si hay) para que la siguiente vez que se acceda no sea incorrecta
                fflush(fichSalida);
                numleidos++;
                break;
            }
        }
        sem_post(&mutexLector);
        if (numleidos == numConsumidores){
            // si el numero de lectores y consumidores coinciden entonces hemos acabado.
            break;
        }

    }
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    //leemos los argumentos metidos por teclado
    char *fichero= argv[1];
    nombreFich =fichero;
    char *ficheroSal= argv[2];
    char *ficherSalida= ficheroSal;

    fichSalida=fopen(ficherSalida,"w");


    sscanf(argv[3],"%d",&tamBuffer);
    sscanf(argv[4],"%d",&numConsumidores);
//comprobación de errores con los datos de entrada
    if(tamBuffer<1){
        printf("El tamaño del buffer introducido es incorrecto");
        return 0;
    }

    if(numConsumidores<1){
        printf("El numero de consumidores introducido es incorrecto");
        return 0;
    }

    //Memoria dinámica.
    buffer1=(int*)malloc(tamBuffer*sizeof(int));
    valores=(struct valoresConsumidor*)malloc(numConsumidores*sizeof(struct valoresConsumidor));
    //memoria dinamica para las listas ordenadas sin implementar aún.

    lista=(struct tipoNodoLista*)malloc(sizeof(struct tipoNodoLista));


    //iniciador hilo
    pthread_t productorhilo;
    pthread_t consumidorhilo[numConsumidores];
    pthread_t lectorhilo;
    //iniciadores de los diferentes semáforos.
    sem_init(&hayEspacio,0,tamBuffer);
    sem_init(&hayDato,0,0);
    sem_init(&mutex_Buffer,0,1);
    sem_init(&consumListo,0,0);
    sem_init(&mutexLector,0,1);
    //bucle for para almacenar en el vector de tamaño de los consumidores segun el numero de consumidores
    int id[numConsumidores];
    for(int i=0;i<numConsumidores;i++){
        id[i]=i;
    }
    //creación de hilos
    pthread_create(&productorhilo,NULL,productor,(void*)NULL);
    pthread_create(&lectorhilo,NULL,lector,(void*)NULL);

    //creación de los hilos consumidores segun el numeros de consumidores dados por teclado.
    for(int i=0;i<numConsumidores;i++){
        pthread_create(&consumidorhilo[i],NULL,consumidor,(void*)&id[i]);
    }
    //inicio de los hilos
    pthread_join(productorhilo,NULL);
    pthread_join(lectorhilo,NULL);
    //inicio de los hilos de consumidores segun el número de consumidores.
    for(int i=0;i<numConsumidores;i++){
        pthread_join(consumidorhilo[i],NULL);
    }

    sem_destroy(&hayEspacio);
    sem_destroy(&hayDato);
    sem_destroy(&mutex_Buffer);
    sem_destroy(&consumListo);
    sem_destroy(&mutexLector);

    return (0);
}

