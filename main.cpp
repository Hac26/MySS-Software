#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

typedef struct nodo{
    unsigned int id;
    unsigned int t_ingreso;
    unsigned int t_salidaCola;
    struct nodo* detras;
}nodo;

unsigned int idn=0;
//entrada por menu
int llegadaRandom=0;
int t_llegada=45; //si la llegada es constante
int llegadaMin=30;// si no es constante
int llegadaMax=50;

int servicioRandom=0;
int t_servicio=50;
int servicioMax=40;
int servicioMin=60;

int descanso=1; //activa descanso si es 1, desactiva si es 0
int descansoRandom=0;
int descansoMin = 50;
int descansoMax = 70;
int t_descanso=60;
int descansandoRandom=0;
int descansandoMin = 50;
int descansandoMax = 70;
int t_descansando=60;

int desercion=1 ; //punto 3
int desercionRandom=0;
int desercionMin;
int desercionMax;
int t_desercion=200;
int salidas[4]={0,0,0,0};

int prioridad=1; //punto 4
nodo *frenteA=NULL;
nodo *finA =NULL;
int probA = 50;

int servidorEstado=1;
int titulos;
int ocupado=0;
int habilitarClientesMax=0; //si es 1 la simulacion se limita al numero de clientes
int clientesMaximos=10; //nro de clientes maximos a atender
int cont=0; //contador de clientes atendidos
int habilitarHorasMax=0;   //si es 1 la simulacion se limita al numero de clientes, solo puede estar activo esta o habilitarClientesMax
int horaFinSimulacion;
int descansoAplicado=0;

//fin de entrada por menu

nodo *frente = NULL; //primero
nodo *fin = NULL;     //ultimo
//*fin->detras = NULL;

void enCola();
void desencolar();
unsigned int clientesCola();
void mostrarEvento();
unsigned int horaLlegada();
unsigned int horaServicio();
unsigned int horaDescanso();
void proximoEvento();
unsigned int horaDescansando();
unsigned int horaDesercion();
unsigned int proximaSalida();
void procesarAbandono();
void cuatroSalidasSiguientes();
void encolaPrioridad(unsigned int horas[]);
void desencolarPrioridad();
unsigned int clientesColaPrioridad();
void mostrarHora(unsigned int segundos);


void main(){
    srand(time(NULL));

    printf("-------MODELO Y SIMULACION DE SISTEMAS------\n");
/*
    printf("\nEl intervalo de tiempo en el que llega el cliente es aleatorio?\n0)NO\n1)SI\n");
    do{scanf("%d",&llegadaRandom);
    if(llegadaRandom > 1 || llegadaRandom < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(llegadaRandom > 1 || llegadaRandom < 0);
    if(llegadaRandom){
        printf("\nIngrese el tiempo minimo en el que puede llegar un cliente:  ");
        do{
            scanf("%d",&llegadaMin);
            if(llegadaMin<=0) printf("Ingrese un numero mayor a 0:  ");
        }while(llegadaMin<=0);
        printf("\nIngrese el tiempo maximo en el que puede llegar un cliente:  ");
        do{
            scanf("%d",&llegadaMax);
            if(llegadaMax<=0 || llegadaMax <= llegadaMin) printf("Ingrese un numero mayor a 0 y que sea mayor al tiempo minimo:  ");
        }while(llegadaMax<=0 || llegadaMax <= llegadaMin);
    }else{
    printf("\nIngrese el tiempo (constante) en el que puede llegar un cliente:  ");
        do{
            scanf("%d",&t_llegada);
            if(t_llegada<=0) printf("Ingrese un numero mayor a 0:  ");
        }while(t_llegada<=0);
    }
    printf("\nEl intervalo de tiempo en el que el cliente es atendido es aleatorio?\n0)NO\n1)SI\n");
    do{scanf("%d",&servicioRandom);
    if(servicioRandom > 1 || servicioRandom < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas\n");
    }while(servicioRandom > 1 || servicioRandom < 0);
    if(servicioRandom){
        printf("Ingrese el tiempo minimo en el que puede llegar un cliente\n");
        do{
            scanf("%d",&servicioMin);
            if(servicioMin<=0) printf("Ingrese un numero mayor a 0\n");
        }while(servicioMin<=0);
        printf("Ingrese el tiempo maximo en el que puede llegar un cliente\n");
        do{
            scanf("%d",&servicioMax);
            if(servicioMax<=0 || servicioMax <= servicioMin) printf("Ingrese un numero mayor a 0 y que sea mayor al tiempo minimo\n");
        }while(servicioMax<=0 || servicioMax <= servicioMin);
    }else{
    printf("Ingrese el tiempo (constante) en el que puede llegar un cliente\n");
        do{
            scanf("%d",&t_servicio);
            if(t_servicio<=0) printf("Ingrese un numero mayor a 0\n");
        }while(t_servicio<=0);
    }
    printf("\nEl servidor puede tomarse un descanso? \n0) NO.\n1) SI.\n");
do {scanf("%d", &descanso);
    if (descanso > 1 || descanso < 0)
        printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
}while(descanso > 1 || descanso < 0);

if(descanso){printf("\nEl tiempo de inicio de descanso es un numero aleatorio?\n0) NO.\n1) SI.\n");
    do {
        scanf("%d", &descansoRandom);
        if (descansoRandom > 1 || descansoRandom < 0)
            printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    } while (descansoRandom > 1 || descansoRandom < 0);

    if (descansoRandom) { // // inicio descanso aleat
        printf("\nIngrese el tiempo minimo que puede iniciar a descansar el servidor:  ");
        do {scanf("%d", &descansoMin);
            if (descansoMin <= 0)
                printf("Ingrese un numero mayor a 0:  ");
        } while (descansoMin <= 0);

        printf("\nIngrese el tiempo maximo que puede iniciar el descanso el servidor:  ");
        do {scanf("%d", &descansoMax);
            if (descansoMax <= 0 || descansoMax <= descansoMin)
                printf("\nIngrese un numero mayor a 0 y mayor al minimo:  ");
        } while (descansoMax <= 0 || descansoMax <= descansoMin);

    } else {//inicio descanso cte
        printf("\nIngrese el tiempo (constante) en el que puede iniciar descanso el servidor:  ");
        do {
            scanf("%d", &t_descanso);
            if (t_descanso <= 0)
                printf("Ingrese un numero mayor a 0:  ");
        } while (t_descanso <= 0);
    }

   printf("\nEl tiempo de descanso del servidor es aleatorio? \n0) NO.\n1) SI.\n");
    do {scanf("%d", &descansandoRandom);
        if (descansandoRandom > 1 || descansandoRandom < 0)
            printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    } while (descansandoRandom > 1 || descansandoRandom < 0);

    if (descansandoRandom) {printf("\nIngrese el tiempo minimo que puede descansar el servidor:  ");//descanso aleat
        do {scanf("%d", &descansandoMin);
            if (descansandoMin <= 0)
                printf("Ingrese un numero mayor a 0:  ");
        } while (descansandoMin <= 0);

        printf("\nIngrese el tiempo maximo que puede descansar el servidor:  ");
        do {
            scanf("%d", &descansandoMax);
            if (descansandoMax <= 0 || descansandoMax <= descansandoMin)
                printf("\nIngrese un numero mayor a 0 y mayor al minimo:  ");
        } while (descansandoMax <= 0 || descansandoMax <= descansandoMin);

    } else {printf("\nIngrese el tiempo (constante) en el que puede descansar el servidor:  ");//descanso cte
        do {
            scanf("%d", &t_descansando);
            if (t_descansando <= 0)
                printf("Ingrese un numero mayor a 0:  ");
        } while (t_descansando <= 0);
    }

}//---
    printf("\nEl servidor tiene abandono de cola? \n0) NO.\n1) SI.\n"); //punto 3
    do{scanf("%d",&desercion);
    if(desercion> 1 || desercion < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(desercion> 1 ||desercion< 0);
    if(desercion){printf("\nEl tiempo de abandono de cola es un numero aleatorio?\n0) NO.\n1) SI.\n");
    do{scanf("%d",&desercionRandom);
    if(desercionRandom > 1 || desercionRandom < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(desercionRandom > 1 || desercionRandom < 0);
    if(desercionRandom){
        printf("\nIngrese el tiempo minimo en el que puede abandonar la cola un cliente:  ");
        do{
            scanf("%d",&desercionMin);
            if(desercionMin<=0) printf("Ingrese un numero mayor a 0:  ");
        }while(desercionMin<=0);
        printf("\nIngrese el tiempo maximo en el que puede abandonar la cola un cliente:  ");
        do{
            scanf("%d",&desercionMax);
            if(desercionMax<=0 || desercionMax <= desercionMin) printf("\nIngrese un numero mayor a 0 y que sea mayor al tiempo minimo:  ");
        }while(desercionMax<=0 || desercionMax <= desercionMin);
    }
    else{
    printf("\nIngrese el tiempo (constante) en el que puede abandonar la cola un cliente:  ");
        do{
            scanf("%d",&t_desercion);
            if(t_desercion<=0) printf("Ingrese un numero mayor a 0:  ");
        }while(t_desercion<=0);
    }}
    printf("\nEl servidor tiene prioridad de cliente?\n0) NO.\n1) SI.\n");
    do{scanf("%d",&prioridad);
    if(prioridad> 1 || prioridad < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(prioridad> 1 ||prioridad< 0);
    if(prioridad){printf("\nIndique la probabilidad de que un cliente sea prioritario (0-99): ");//qqqqqqqqqqqqqqqqqqqqqqqq
    do{scanf("%d",&probA);
    if(probA> 99 || probA < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(probA> 99 || probA < 0);
    }
    */
    titulos = descanso*3+desercion*4+prioridad;
    int n=3+descanso*2;
    unsigned int horas[n];
    unsigned int flags[n+desercion-1];
    for(int i=0;i<n;i++) horas[i]=0;
    for(int i=0;i<(n-1);i++) flags[i]=0;
    //inicio del menu

    int temp=0;

    printf("\nEl programa finalizara su ejecucion por \n1) Nro de clientes atendidos.\n2) Hora de finalizacion estipulada.\n");
    do{scanf("%d",&temp);
    if(temp > 2 || temp < 1) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(temp > 2 || temp < 1);
    if(temp==1){
        habilitarClientesMax=1;
        habilitarHorasMax=0;
        horaFinSimulacion=INT_MAX;
        printf("\nIngrese el numero de clientes atendidos para que finalice la simulacion:  ");
        do{
            scanf("%d",&temp);
            if(temp<=0) printf("Ingrese un numero mayor a 0:  ");
        }while(temp<=0);
        clientesMaximos=temp;
    }else{
    habilitarClientesMax=0;
    habilitarHorasMax=1;
    clientesMaximos=INT_MAX;
        printf("\nIngrese el la hora a la que quiere termine la simulacion\n");
         int h,m,s;
        do{
            printf("[horas]:[minutos]:[segundos]: ");
            scanf("%d %d %d",&h,&m,&s);
            horaFinSimulacion=h*3600+m*60+s;
            if(horaFinSimulacion<=0) printf("La hora de finalizacion debe ser mayor a 0:  ");
        }while(horaFinSimulacion<=0);
    }
//------

    //carga del vector inicial
    printf("\n");


    printf("Hora de inicio.\n");
    int h,m,s;
    do{
     printf("[horas]:[minutos]:[segundos]: ");
            scanf("%d %d %d",&h,&m,&s);
            horas[0]=h*3600+m*60+s;
    if(horas[0]<0) printf("Ingrese un valor mayor o igual a 0: ");
    }while(horas[0]<0);
    if(!prioridad){
        printf("\nCantidad de clientes en cola:  ");
    do{scanf("%d",&temp);
    if(temp<0) printf("Ingrese un valor mayor o igual a 0:  ");
    }while(temp<0);
    for(int i=0;i<temp;i++) enCola(horas);
    }else{
        printf("\nCantidad de clientes en cola prioritaria:  ");
    do{scanf("%d",&temp);
    if(temp<0) printf("Ingrese un valor mayor o igual a 0:  ");
    }while(temp<0);
    for(int i=0;i<temp;i++) encolaPrioridad(horas);
    printf("\nCantidad de clientes en cola NO prioritaria:  ");
    do{scanf("%d",&temp);
    if(temp<0) printf("Ingrese un valor mayor o igual a 0:  ");
    }while(temp<0);
    for(int i=0;i<temp;i++) enCola(horas);
    }

    printf("\nEl servidor esta ocupado? \n0) NO.\n1) SI.\n");
    do{scanf("%d",&ocupado);
    if(ocupado > 1 || ocupado < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(ocupado > 1 || ocupado < 0);

    if(descanso){
      printf("\nEl servidor esta activo? \n0) NO.\n1) SI.\n");
    do{scanf("%d",&servidorEstado);
    if(servidorEstado > 1 || servidorEstado < 0) printf("Opcion no reconocida. Por favor elija una de las opciones provistas:  ");
    }while(servidorEstado > 1 || servidorEstado < 0);

    }else servidorEstado=1;

    if(ocupado && !servidorEstado){
        printf("\nCuantos segundos le faltan al cliente en puesto de servicio para terminar de ser atendido?\n");
        do{
            scanf("%d",&temp);
            if(temp<0) printf("Ingrese un valor positivo.\n");
        }while(temp<0);
        printf("\n");
    }
    //fin del menu
    horas[1] = horas[0] + horaLlegada();
    if(ocupado)horas[2]= horas[0]+horaServicio();
    if(servidorEstado)horas[3] = horas[0] + horaDescanso();
    else horas[4]= horas[0] +horaDescansando();
    if(horas[2]<horas[4] && ocupado && !servidorEstado){
        descansoAplicado=1;
        horas[2] = horas[4]+temp;
        }
    printf("---------------------------------------------------");
    for(int i=0;i<titulos;i++)printf("----------");
    printf("\n");
    printf("|H. Actual");
    printf("|H Llegada");
    printf("|Fin de S.");
    if(descanso){
    printf("|Descanso ");
    printf("|V.Trabajo");
    printf("|Estado S.");
    }
    if(!prioridad){
            printf("|  Queue  ");
    }else{
        printf("| Cola A  ");
        printf("| Cola B  ");
        }
    printf("| Ocupado ");
    if(desercion){
    printf("|Salida N1");
    printf("|Salida N2");
    printf("|Salida N3");
    printf("|Salida N4");
    }
    printf("|\n");
    //mostrarEvento(n,horas);
   /* if(servidorEstado){
    if(!prioridad){
        if(clientesCola()>0 && !ocupado){
            desencolar();
            ocupado=1;
            horas[2]=horas[0]+horaServicio();
        }else if(clientesCola()>0 && ocupado)horas[2]=horas[0]+horaServicio();
    }else{
        if((clientesColaPrioridad()>0 || clientesCola() > 0) && !ocupado){
            desencolarPrioridad();
            ocupado=1;
            horas[2]=horas[0]+horaServicio();
        }else if((clientesColaPrioridad()>0 || clientesCola() > 0) && ocupado)horas[2]=horas[0]+horaServicio();
    }
    }
    else{
        horas[4] = horas[0]+horaDescansando();
        if(ocupado) horas[2] = horas[4]+horaServicio() ;
    }*/
    while(((habilitarClientesMax &&(cont <= clientesMaximos))||habilitarHorasMax&&(horas[0]<horaFinSimulacion))){
    mostrarEvento(n, horas);
    proximoEvento(n, horas, flags);
    }
    if(habilitarHorasMax){
    unsigned int siguiente=INT_MAX;
    int indice = -1;
    for(int i=0;i<n;i++){
        if(horas[i] > horas[0] && horas[i] < siguiente){
            siguiente=horas[i];
        }
    }
    if(siguiente==horaFinSimulacion)proximoEvento(n,horas, flags);
    else horas[0]=horaFinSimulacion;
        mostrarEvento(n,horas);
    }
printf("---------------------------------------------------");
    for(int i=0 ;i<titulos;i++)printf("----------");
    printf("\n");
}

void mostrarEvento(int n, int v[n]){
    printf("---------------------------------------------------");
    for(int i=0;i<titulos;i++)printf("----------");
    printf("\n|");
    mostrarHora(v[0]);
    mostrarHora(v[1]);
    if(ocupado)mostrarHora(v[2]);
        else printf("         |");
     if(descanso){
    if(servidorEstado)mostrarHora(v[3]);
    else printf("         |");
    if(!servidorEstado)mostrarHora(v[4]);
    else printf("         |");
    if(servidorEstado)printf("  Activo |"); else printf(" Inactivo|");
    }
    if(!prioridad){
            printf("%9d|", clientesCola());
    }else{
        printf("%9d|",clientesColaPrioridad());
        printf("%9d|",clientesCola());
    }
    if(ocupado)printf(" Ocupado |"); else printf("NoOcupado|");
    if(desercion){
    cuatroSalidasSiguientes();
    if(salidas[0])mostrarHora(salidas[0]); else printf("         |");
    if(salidas[1])mostrarHora(salidas[1]); else printf("         |");
    if(salidas[2])mostrarHora(salidas[2]); else printf("         |");
    if(salidas[3])mostrarHora(salidas[3]); else printf("         |");
    }
    printf("\n");
}

unsigned int horaLlegada(){
    if(llegadaRandom){
        int temp = llegadaMin + (rand()% (llegadaMax - llegadaMin + 1));
        return temp;
    }else{
        return t_llegada;
    }
}

unsigned int horaServicio(){
    if(servicioRandom){
        int temp = servicioMin + (rand() % (servicioMax - servicioMin + 1));
        return temp;
    }else{
        return t_servicio;
    }
}

void proximoEvento(int n, int v[n], int f[n+desercion-1]){    //v[0] es la hora actual, v1 es la prox llegada, v2 prox fin de servicio
    unsigned int siguiente=INT_MAX;              //v3 es la hora de descanso, v4 es la hora a la que termina el descanso
    int indice = -1;
    for(int i=0;i<n;i++){               //busca el tiempo mas corto
        if(v[i] > v[0] && v[i] < siguiente){
            siguiente=v[i];
            //indice = i;
        }
        if(i>0 && siguiente == v[i]) f[i-1]=1;
        else f[i-1]=0;
    }
    unsigned int salida = proximaSalida();
    if(desercion && salida == siguiente) f[4]=1; else f[4]=0;
    if(salida < siguiente) siguiente = salida;
    v[0] = siguiente;

    int probabilidad;
    if(prioridad) probabilidad = rand() % 101; else probabilidad = 101;

    if(f[0]){   //evento de llegada
        if(!ocupado && servidorEstado && (clientesCola()<=0 || clientesColaPrioridad() <= 0)){
        ocupado = 1;
        if(probabilidad >probA){ // si el valor obtenido es mayor a probA -> cliente tipo B
           enCola(v);    //encola al que llega
           }else{ //sino cliente tipo A
           encolaPrioridad(v);
           }
        desencolarPrioridad(v);
        v[2] = v[0] + horaServicio();
    }else{
        if(probabilidad >probA){ // si el valor obtenido es mayor a probA -> cliente tipo B
           enCola(v);    //encola al que llega
           }else{ //sino cliente tipo A
           encolaPrioridad(v);
           }
    }
    v[1] = v[0] + horaLlegada();
}
    if(f[1]){       //evento fin de servicio
        cont++;
        desencolarPrioridad();
    if(clientesColaPrioridad() > 0 || clientesCola() > 0){
        v[2] = siguiente + horaServicio();
        ocupado = 1;
    }else{
        ocupado = 0;
        //v[2] = 0;
    }
    }
    if(f[2] && descanso){ //inicio de descanso
        servidorEstado = 0;
        v[4] = v[0] + horaDescansando();

        if(ocupado && !descansoAplicado && v[2] > v[3]){
        v[2] += (v[4] - v[3]);
        descansoAplicado=1;
        }
    }
    if(f[3] && descanso){ //fin de descanso
        servidorEstado = 1;
        descansoAplicado = 0;
        v[3]= v[0] + horaDescanso();
    }
    if(f[4] && desercion){ //salida de cola
        procesarAbandono(v[0]);
    }
}



void enCola(int horas[]){
 nodo *nuevo = malloc(sizeof(nodo));
 nuevo->id = idn+1;
 nuevo->detras = NULL;
 nuevo->t_ingreso = horas[0];
 nuevo->t_salidaCola = horas[0] + horaDesercion();
 if(fin == NULL){
    fin = nuevo;
    frente = nuevo;
 }else{
    fin->detras = nuevo;
    fin = nuevo;
 }
}

void desencolar(){
    if (frente == NULL) return; // cola vac a

    nodo *temp = frente;
    frente = frente->detras;

    if (frente == NULL) {
        // si la cola qued  vac a, actualizar fin
        fin = NULL;
    }

    free(temp); // liberar el nodo que sali
}

unsigned int clientesCola(){
    nodo *conteo = frente;
    int contador = 0;
    while(conteo != NULL){
        contador++;
        conteo = conteo->detras;
    }
    return contador;
}


unsigned int horaDescanso(){
    if(descansoRandom){
        int temp = descansoMin + (rand() % (descansoMax - descansoMin + 1));
        return temp;
    }else{
        return t_descanso;
    }
}

unsigned int horaDescansando(){
    if(descansandoRandom){
        int temp = descansandoMin + (rand() % (descansandoMax - descansandoMin + 1));
        return temp;
    }else{
        return t_descansando;
    }
}

unsigned int horaDesercion(){
    if(desercionRandom){
        int temp = desercionMin + (rand() % (desercionMax - desercionMin + 1));
        return temp;
    }else{
        return t_desercion;
    }
}

void desencolarId(int id){
    nodo *actual = frenteA;
    nodo *anterior = NULL;

    // Buscar en A
    while(actual != NULL && actual->id != id){
        anterior = actual;
        actual = actual->detras;
    }

    if(actual != NULL){
        // Lo encontramos en A
        if(anterior == NULL){
            frenteA = actual->detras;
            if(frenteA == NULL) finA = NULL;
        }else{
            anterior->detras = actual->detras;
            if(actual == finA) finA = anterior;
        }
        free(actual);
        return;
    }

    // Buscar en B
    actual = frente;
    anterior = NULL;

    while(actual != NULL && actual->id != id){
        anterior = actual;
        actual = actual->detras;
    }

    if(actual != NULL){
        if(anterior == NULL){
            frente = actual->detras;
            if(frente== NULL) fin = NULL;
        }else{
            anterior->detras = actual->detras;
            if(actual == fin) fin = anterior;
        }
        free(actual);
    }
}

unsigned int proximaSalida(){
    if(!desercion) return INT_MAX;

    int salida = INT_MAX;

    nodo *temp = frenteA;
    while(temp != NULL){
        if(temp->t_salidaCola < salida)
            salida = temp->t_salidaCola;
        temp = temp->detras;
    }

    temp = frente;
    while(temp != NULL){
        if(temp->t_salidaCola < salida)
            salida = temp->t_salidaCola;
        temp = temp->detras;
    }

    return salida;
}

void procesarAbandono(unsigned int tiempoActual){
    nodo *temp = frenteA;

    while(temp != NULL){
        if(temp->t_salidaCola == tiempoActual){
            desencolarId(temp->id);
            return;
        }
        temp = temp->detras;
    }
    temp = frente;
    while(temp != NULL){
        if(temp->t_salidaCola == tiempoActual){
            desencolarId(temp->id);
            return;
        }
        temp = temp->detras;
    }
}

void cuatroSalidasSiguientes(){
    unsigned int n = clientesCola();
    if(n<=0){
        for(int i=0;i<4;i++)salidas[i]=0;
        return;
    }
    unsigned int auxiliar[n];
    nodo *temp = frente;
    for(int i=0;i<n;i++){
        auxiliar[i] = temp->t_salidaCola;
        temp = temp->detras;
}
for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
        if (auxiliar[j] > auxiliar[j+1]) {
            unsigned int tmp = auxiliar[j];
            auxiliar[j] = auxiliar[j+1];
            auxiliar[j+1] = tmp;
        }
    }
}
    if(n>=4){
    for(int i=0; i<4;i++)salidas[i]=auxiliar[i];
    }else{
    for(int i=0;i<n;i++)salidas[i]=auxiliar[i];
    for(int i=n;i<4;i++)salidas[i]=0;
    }
}

void encolaPrioridad(unsigned int horas[]){
    nodo *nuevo = malloc(sizeof(nodo));
 nuevo->id = idn+1;
 nuevo->detras = NULL;
 nuevo->t_ingreso = horas[0];
 nuevo->t_salidaCola = horas[0] + horaDesercion();
 if(finA == NULL){
    finA = nuevo;
    frenteA = nuevo;
 }else{
    finA->detras = nuevo;
    finA = nuevo;
 }
}

void desencolarPrioridad(){
    nodo *temp;

    if(frenteA != NULL){

        temp = frenteA;
        frenteA = frenteA->detras;
        if(frenteA == NULL) finA = NULL;

    }else if(frente != NULL){

        temp = frente;
        frente = frente->detras;
        if(frente == NULL) fin = NULL;

    }else{
        return;
    }

    free(temp);
}

unsigned int clientesColaPrioridad(){
    nodo *conteo = frenteA;
    int contador = 0;
    while(conteo != NULL){
        contador++;
        conteo = conteo->detras;
    }
    return contador;
}

void mostrarHora(unsigned int segundos)
{
     int h = segundos / 3600;
     int m = (segundos % 3600) / 60;
    int s = segundos % 60;

    printf("%03d:%02d:%02d|", h, m, s);}

