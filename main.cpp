#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <queue>
#include <string>
#include <vector>

vector<Cola*> direccionDeColas;
vector<PuestoServicio*> direccionPS;
vector<ZonaSeguridad*> direccionZS;     //guardan las direcciones de las colas, PS, ZS para que la interfaz grafica pueda acceder

class Cliente{
public:
    unsigned int id;
    unsigned int t_salidaCola;
    bool p;
    int nColaDestino;

    Cliente(unsigned int t_llegada, int probVal){
    id=++idn;
    t_salidaCola= t_llegada + hora(deser);
    p = establecerPrioridad(probVal);
    }

};

class Cola{
public:

    string nombre;
    queue<Cliente*> clientes;
    int clientesDesertados;
    bool prioridad;
    bool ZS;    //true si el destino es un ZS, false para un PS
    vector<PuestoServicio*> destinosPS;
    int nDestinoPS;
    vector<ZonaSeguridad*> destinosZS;
    int nDestinoZS;

    Cola(string n, bool p, bool zs){
        nombre = n;
        bool prioridad = p;
        ZS = zs;
        clientesDesertados = 0;
        nDestinoPS = 0;
        nDestinoZS = 0;
    }

    void TransferirCliente(){
        if(!clientes.empty()){
            if(ZS){
                destinosZS[0]->ClienteActual = clientes.front(); //hay que aplicar round robin a estos dos
            }else{
                destinosPS[0]->ClienteActual = clientes.front();
            }
            clientes.pop();
        }
    }

    void AnadirDestinoPS(PuestoServicio* direccion){
        destinosPS.push_back(direccion);// destinos no exite----> colaDestino
        nDestinoPS++;
    }

    void AnadirDestinoZS(ZonaSeguridad* direccion){
        destinosZS.push_back(direccion);// destinos no exite----> colaDestino
        nDestinoZS++;
    }


    Cola* inicializarCola(string nombre, bool p, bool zs){
        Cola* nueva = new Cola(nombre, p, zs);
        direccionDeColas.push_back(nueva);
        contColas++;
        return nueva;
    }       //se inicializa una cola -> Cola* nombre = inicializarCola("nombreDeCola", prioridad); y guarda la direccion del puntero en la variable nombre
};          //se agrega un cliente con cola->clientes.push(cliente)



class PuestoServicio{
public:
    string nombre;
    bool ocupado;
    bool esFin; // si no le sigue nada
    int contClientes = 0;
    Cliente* clienteActual;
    queue<Cliente*>* colaDestino;
    int nColaDestino=0;

    ZonaSeguridad* zsAsociada;//asociacion simple
    vector<ZonaSeguridad*> zonasAsociadas;//asociacion multiple

    PuestoServicio(string n, bool o, bool fin){
        ocupado = o;
        esFin = fin;
        zsAsociada = nullptr; //puesto inicio no tiene zona asociada
        contPS++;
    }
    PuestoServicio* inicializarPS(string nombre, bool o, bool fin){
        Cola* nueva = new PuestoServicio(nombre, o, fin);
        direccionPS.push_back(nueva);
        contPS++;
        return nueva;
    }
    void TransferirCliente(){

        if(clienteActual != nullptr){ //si hay cliente
            ocupado = false;
            contClientes++;
            if(!esFin){  // lleva a otra instancia
                for(int i=0; i<nColaDestino;i++){   //aplicar RoundRobin
                colaDestino[i]->push(clienteActual);

                }
            }else{
                delete clienteActual;
            }           
            clienteActual = nullptr;
        }
    }
    
    void AnadirDestinoCliente(queue<Cliente*>* direccion){
        destinos.push_back(direccion);// destinos no exite----> colaDestino
        nColaDestino++;
    }

    void asociarZonaSimple(ZonaSeguridad* zs){ //funcion asociacion simple
        zsAsociada = zs;
    }
    void asociarZonaMultiple(ZonaSeguridad* zs){ //fucnion asociacion multiple
        zonasAsociadas.push_back(zs);
    }

    
};

class ZonaSeguridad{
public:

    bool ocupado;
    Cliente* clienteActual;
    int t_ingreso;
    int t_salida;

    ZonaSeguridad(bool o){
        id = identificador;
        ocupado = o;
        clienteActual = nullptr;

        t_ingreso = 0;
        t_salida = 0;

        contClientes = 0;
    }

    void recibirCliente(Cliente* c, int horaActual){
        clienteActual = c;
        ocupado = true;

        t_ingreso = horaActual;
    }

    void liberarCliente(int horaActual){
        clienteActual = nullptr;
        ocupado = false;

        t_salida = horaActual;

        contClientes++;
    }
};


typedef struct nodo{
    unsigned int id;
    unsigned int t_ingreso;
    unsigned int t_salidaCola;
    struct nodo* detras;
}nodo;

typedef struct {
    unsigned int var;
    unsigned int t;
    unsigned int tMin;
    unsigned int tMax;
}horario;

unsigned int idn=0;

//entrada por menu

horario llegada = {
    .var = 1,
    .t = 60,
    .tMin = 50,
    .tMax = 70,
};
horario servicio = {
    .var = 1,
    .t = 60,
    .tMin = 50,
    .tMax = 70,
};

int descanso=1; //activa descanso si es 1, desactiva si es 0
horario descan = {
    .var = 1,
    .t = 60,
    .tMin = 50,
    .tMax = 70,
};
horario descansan = {
    .var = 1,
    .t = 60,
    .tMin = 50,
    .tMax = 70,
};

int desercion=1 ; //punto 3
horario deser = {
    .var = 1,
    .t = 600,
    .tMin = 50,
    .tMax = 70,
};
int salidas[4]={0,0,0,0};

int prioridad=0; //punto 4
nodo *frenteA=NULL;
nodo *finA =NULL;
int probA = 50;

int servidorEstado=1;
int titulos;
int ocupado=0;
int habilitarClientesMax=0; //si es 1 la simulacion se limita al numero de clientes
int clientesMaximos=10; //nro de clientes maximos a atender
int cont=0; //contador de clientes atendidos
int contDesertores=0;
int contPS = 0;
int contZS = 0;
int contColas = 0;
int habilitarHorasMax=0;   //si es 1 la simulacion se limita al numero de clientes, solo puede estar activo esta o habilitarClientesMax
int horaFinSimulacion;
int descansoAplicado=0;

//fin de entrada por menu

nodo *frente = NULL; //primero
nodo *fin = NULL;     //ultimo


void mostrarEvento();
void proximoEvento();
unsigned int hora(horario h);
unsigned int proximaSalida();
void procesarAbandono();
void cuatroSalidasSiguientes();
void encolaPrioridad(unsigned int horas[]);
void desencolarPrioridad();
unsigned int clientesColaPrioridad();
void mostrarHora(unsigned int segundos);
void establecerTiempoParados(unsigned int s);
bool establecerPrioridad(int probVal);


int main(){
    srand(time(NULL));

    printf("-------MODELO Y SIMULACION DE SISTEMAS------\n");

    titulos = descanso*3+desercion*4+prioridad;
    int n=3+descanso*2;
    unsigned int horas[n];
    unsigned int flags[n+desercion-1];
    for(int i=0;i<n;i++) horas[i]=0;
    for(int i=0;i<(n+desercion-1);i++) flags[i]=0;
    //inicio del menu

    //fin del menu
    horas[1] = horas[0] + hora(llegada);
    if(ocupado)horas[2]= horas[0]+hora(servicio);
    if(servidorEstado)horas[3] = horas[0] + hora(descan);
    else horas[4]= horas[0] +hora(descansan);
    if(horas[2]<horas[4] && ocupado && !servidorEstado){
        descansoAplicado=1;
        horas[2] = horas[4]+temp;
        }
    printf("-------------------------------------------------------");
    for(int i=0;i<titulos;i++)printf("----------");
    for(int i=0;i<n+desercion-1;i++)printf("---");
    if(desercion)printf("----");
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
    printf("|Ll");
    printf("|FS");
    if(descanso){
    printf("|DS");
    printf("|VT");
    }
    if(desercion)printf("|DC");
    printf("|CLT");
    if(desercion)printf("|CDC");
    printf("|\n");

    while( habilitarClientesMax && (cont < clientesMaximos)  || habilitarHorasMax && ( horas[0]<horaFinSimulacion ) ){
    mostrarEvento(n, horas,flags);
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
        mostrarEvento(n,horas,flags);
    }
    if(habilitarClientesMax){
        mostrarEvento(n,horas,flags);
    }
printf("-------------------------------------------------------");
    for(int i=0 ;i<titulos;i++)printf("----------");
    for(int i=0;i<n+desercion-1;i++)printf("---");
    if(desercion)printf("----");
    printf("\n");

    return 0;
}

void mostrarEvento(int n, int v[n],int f[n-desercion-1]){
    printf("-------------------------------------------------------");
    for(int i=0;i<titulos;i++)printf("----------");
    for(int i=0;i<n+desercion-1;i++)printf("---");
    if(desercion)printf("----");
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
    for(int i=0;i<(n+desercion-1);i++){
        printf("%2d|",f[i]);
    }
    printf("%3d|", cont);
    if(desercion)printf("%3d|", contDesertores);
    printf("\n");
}

unsigned int hora(horario h){
    if(h.var){
        unsigned int temp = h.tMin + (rand()% (h.tMax - h.tMin + 1));
        return temp;
    }else{
        return h.t;
    }
}

void proximoEvento(int n, int v[n], int f[n+desercion-1]){    //v[0] es la hora actual, v1 es la prox llegada, v2 prox fin de servicio
    unsigned int siguiente=INT_MAX;              //v3 es la hora de descanso, v4 es la hora a la que termina el descanso
    //int indice = -1;
    for(int i=1;i<n;i++){               //busca el tiempo mas corto
        if(v[i] > v[0] && v[i] < siguiente){
            siguiente=v[i];
            //indice = i;
        }
    }
    unsigned int salida = proximaSalida();

    if(salida < siguiente) siguiente = salida;
     v[0] = siguiente;

    for (int i=1;i<5;i++){
        if(siguiente == v[i]) f[i-1]=1;
        else f[i-1]=0;
    }
    if(salida == siguiente)f[4]=1;
    else f[4]=0;


    int probabilidad;
    if(prioridad) probabilidad = rand() % 101; else probabilidad = 101;

    if(f[0]){   //evento de llegada
        if(!ocupado && servidorEstado && (clientesCola()<=0 && clientesColaPrioridad() <= 0)){
        ocupado = 1;
        v[2] = v[0] + hora(servicio);
    }else{
        if(probabilidad >probA){ // si el valor obtenido es mayor a probA -> cliente tipo B
           enCola(v);    //encola al que llega
           }else{ //sino cliente tipo A
           encolaPrioridad(v);
           }
    }
    v[1] = v[0] + hora(llegada);
}
    if(f[1]){       //evento fin de servicio
        cont++;

    if(clientesColaPrioridad() > 0 || clientesCola() > 0){
        desencolarPrioridad();
        v[2] = siguiente + hora(servicio); //posible error
        ocupado = 1;
    }else{
        ocupado = 0;
        v[2] = INT_MAX;
    }
    }
    if(f[2] && descanso){ //inicio de descanso
        servidorEstado = 0;
        v[4] = v[0] + hora(descansan);

        if(ocupado && !descansoAplicado && v[2] > v[3]){
        v[2] += (v[4] - v[3]);
        descansoAplicado=1;
        }
    }
    if(f[3] && descanso){ //fin de descanso
        servidorEstado = 1;
        descansoAplicado = 0;
        v[3]= v[0] + hora(descan);
    }
    if(f[4] && desercion){ //salida de cola
        procesarAbandono(siguiente);
        contDesertores++;
    }
}



void enCola(int horas[]){
 nodo *nuevo = malloc(sizeof(nodo));
 idn++;
 nuevo->id = idn;
 nuevo->detras = NULL;
 nuevo->t_ingreso = horas[0];
 nuevo->t_salidaCola = horas[0] + hora(deser);
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
    unsigned int nA = clientesColaPrioridad();
    if(n<=0){
        for(int i=0;i<4;i++)salidas[i]=0;
        return;
    }
    unsigned int auxiliar[n+nA];
    nodo *temp = frente;
    for(int i=0;i<n;i++){
        auxiliar[i] = temp->t_salidaCola;
        temp = temp->detras;
}
    temp = frenteA;
    for(int i = n; i<(n+nA);i++){
        auxiliar[i] = temp->t_salidaCola;
        temp = temp->detras;
    }
for (int i = 0; i < (n+nA) - 1; i++) {
    for (int j = 0; j < (n+nA) - i - 1; j++) {
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
idn++;
 nuevo->id = idn;
 nuevo->detras = NULL;
 nuevo->t_ingreso = horas[0];
 nuevo->t_salidaCola = horas[0] + hora(deser);
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


void establecerTiempoParados(unsigned int s){
    nodo *temp = frente;
    while(temp != NULL){
        temp->t_salidaCola = hora(deser) - s;
        temp=temp->detras;
    }
}

bool establecerPrioridad(int probVal){
     int probabilidad;
     bool prioridad;
    if(prioridad) probabilidad = rand() % 101; else probabilidad = 101;
        if(probabilidad >probVal){ // si el valor obtenido es mayor a probA -> cliente tipo B
           prioridad = false;    //encola al que llega
           }else{ //sino cliente tipo A
           prioridad = true;
        }
    return prioridad;
}
