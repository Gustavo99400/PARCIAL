#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
//COLORES
#define RESET_COLOR    "\x1b[0m"
#define NEGRO_T        "\x1b[30m"
#define NEGRO_F        "\x1b[40m"
#define ROJO_T     "\x1b[31m"
#define ROJO_F     "\x1b[41m"
#define VERDE_T        "\x1b[32m"
#define VERDE_F        "\x1b[42m"
#define AMARILLO_T "\x1b[33m"
#define    AMARILLO_F  "\x1b[43m"
#define AZUL_T     "\x1b[34m"
#define    AZUL_F      "\x1b[44m"
#define MAGENTA_T  "\x1b[35m"
#define MAGENTA_F  "\x1b[45m"
#define CYAN_T     "\x1b[36m"
#define CYAN_F     "\x1b[46m"
#define BLANCO_T   "\x1b[37m"
#define BLANCO_F   "\x1b[47m"

//VARIABLES GLOBALES
// variable central
#define N 5
//variable que define si un filosofo esta pensando o no.
#define THINKING 2
//variable que define si un filosofo esta hambriento o no.  
#define HUNGRY 1
//variable que define si un filosofo esta comiendo o no.
#define EATING 0
//variable que define el tenedor de la izquierda.
#define LEFT (phnum + 4) % N
//variable que define el tenedor de la derecha.
#define RIGHT (phnum + 1) % N
//variable que define en que estado esta el filosofo, si esta hambriento o esta comiendo, se usara para almacenar la informacion correspondiente y comparar con las variables anteriores
int state[N];
//variable que define la cantidad de comida total
int comida = 300000;
//variable que alamacenara la cantidad de veces que fue restaurada la comida
int restaurar = 0;
//SEMAFORO 1
sem_t mutex;
//SEMAFORO 2
sem_t S[N];
//SIMULACION DE ESTOMAGOS DE LOS DISTINTOS FILOSOFOS
int estomago_vacio[N];
int estomago_lleno[N];

//---------------------------------------------------------------

void test(int phnum,int arr[], int arr_[])
{    
    //condicional que verifica que el filosofo esta hambriento y que el filosofo de la izquierda y derecha no esten comiendo.
	if (state[phnum] == HUNGRY && state[LEFT] != EATING	&& state[RIGHT] != EATING)
    
    {
		//Dando el estado de comiendo al filosofo respectivo
		state[phnum] = EATING;

		sleep(0);

        //Le damos sus respectivos tenedores.
		printf(VERDE_T "El filósofo %d toma el tenedor %d y %d\n",
					phnum + 1, LEFT + 1, phnum + 1);

        // BUCLE QUE HACE LLENAR A UN FILOSOFO
        while(arr[phnum]<arr_[phnum])
        {
		    
            printf(AMARILLO_T"El filósofo %d está comiendo\n", phnum + 1);
            arr[phnum]=arr[phnum]+1;
            
        }	
		//DESBLOQUEO DE RECURSOS
		sem_post(&S[phnum]);
	}
}

//---------------------------------------------------------------
// funcion tomar tenedor
void take_fork(int phnum, int arr[], int arr_[])
{
    //BLOQUEO DE RECURSOS
	sem_wait(&mutex);

    //DANDO EL ESTADO DE HAMBRIENTO AL FILOSOFO                 CORRESPONDIENTE
	state[phnum] = HUNGRY;
        
	printf(CYAN_T "El filósofo %d tiene hambre\n", phnum + 1);
    //funcion test 
	test(phnum,arr,arr_);

    //DESBLOQUEO DE RECURSOS
	sem_post(&mutex);

    //BLOQUEO DE RECURSOS
	sem_wait(&S[phnum]);

	sleep(1);
}

//---------------------------------------------------------------

// Funcion dejar tenedor
void put_fork(int phnum,int arr[],int arr_[])
{
    //BLOQUEO DE RECURSOS
	sem_wait(&mutex);

    //Le damos el estado de pensando al filosofo respectivo 
	state[phnum] = THINKING;

    //Condicional que hace que solo un filosofo lleno pueda dejar sus tenedores.
    if(arr[phnum]==arr_[phnum] ){
        // Dejando sus respectivos tenedores
	    printf(ROJO_T"Filósofo %d poniendo tenedor %d y %d abajo\n",
		phnum + 1, LEFT + 1, phnum + 1); 
        // Una vez que el filosofo deja los tenedores, garantizamos que comio todas las porciones necesarias para llenarse.
        comida = comida - arr[phnum];
        printf("*%d*\n",comida);

        // Bucle que hace gastar energia a un filosofo
        while(arr[phnum]!=0){
            arr[phnum]=arr[phnum]-1;
            printf(MAGENTA_T"El filósofo %d está pensando\n", phnum + 1);
        }
     }        
    //  condicional que restarurara la comida una vez que esta se acabe y tambien contara cuantas restauraciones de alimentos hubo en total.
    if(comida == 0){
        comida = 300000;
        restaurar=restaurar+1;
        printf(BLANCO_T "Numero de restauracion %d\n ", restaurar);
    }

    //Ggarantiza que los filosofos aledaños no esten comiendo. 
	test(LEFT,arr,arr_);
	test(RIGHT,arr,arr_);

    //DESBLOQUEO DE RECURSOS
	sem_post(&mutex);
}

//---------------------------------------------------------------

void* philosopher(int* num)
{    
    //creacion de los estomagos vacios y llenos.
    int j, cont=0;    

    if(estomago_lleno[2]==0){
        while (cont < N)
        {
         j= rand () % 10;
         if (j>0){
             estomago_lleno[cont]=j;
             cont=cont +1 ;
         }        
    }
        
    }    

    //bucle infinito para simular el acto de comer y pensar
	while (1) {
                        
		int* i = num;       
        
		sleep(1);        

        //condicon de solo agarrar un tenedor si el                 estomago del filosofo selecionado esta vacio.
        if (estomago_vacio[*i]<estomago_lleno[*i])
        {        
            //funcion tomar tenedor
            take_fork(*i,estomago_vacio,estomago_lleno);            
        } else {        
        sleep (0)  ;

        //funcion dejar tenedor
        put_fork(*i,estomago_vacio,estomago_lleno);  
            }  
}        
    }

//---------------------------------------------------------------

int main()
{        
    // variable con valor 0.
    int a = 0;    
    // array con cada uno de los filosofos.
    int phil[N];
    // bucle que le dara un numero a cada filosofo.
    while (a < N)
    {
     phil[a]=a;
     a=a+1;
    }
    //variable que se usara en los bucles siguientes.
	int i;

    // variable tipo thread.
	pthread_t thread_id[N];

    //inicializando semaforo.
	sem_init(&mutex, 0, 1);

    //bucle para inicializar el segundo semaforo.
	for (i = 0; i < N; i++)

		sem_init(&S[i], 0, 0);

    // bucle para crear los hilos, uno para cada filosofo.
	for (i = 0; i < N; i++) {

			pthread_create(&thread_id[i], NULL,
					philosopher, &phil[i]);

		printf(MAGENTA_T"El filósofo %d está pensando\n", i + 1);
	}

    //bucle para bloquear los subprocesos
	for (i = 0; i < N; i++)

		pthread_join(thread_id[i], NULL);
}  

