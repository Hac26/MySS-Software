#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <queue>
#include <vector>

class ZonaSeguridad;

class Cliente{
public:
    unsigned int id;
    unsigned int t_salidaCola;
    bool p;
    bool zonaSeguridad; //true si lo lleva a una zona de seguridad, false directo a PS

    Cliente(unsigned int t_llegada, int probVal, bool zs){
    id=++idn;
    t_salidaCola= t_llegada + hora(deser);
    p = establecerPrioridad(probVal);
    zonaSeguridad = zs;
    }

};

class PuestoServicio{
public:

    bool ocupado;
    bool esFin;
    int contClientes = 0;
    int tiempoOcupado=0;
    int cantidadDescansos=0;
    Cliente* clienteActual;
    <queue<Cliente*>* colaDestino;
    int nColaDestino=0;
    <queue<Cliente*>* destinoLlegada;
    int nDestinoLlegada=0;

    ZonaSeguridad* zsAsociada;//asociacion simple
    vector<ZonaSeguridad*> zonasAsociadas;//asociacion multiple

    PuestoServicio(bool o, bool fin){
        ocupado = o;
        esFin = fin;
        zsAsociada = nullptr; //puesto inicio no tiene zona asociada
        contPS++;
    }

    void transferirCliente(){

        if(clienteActual != nullptr){
            if(esFin){
                ocupado = false;
                contClientes++;
                return;
            }else{
                for(int i=0; i<nColaDestino;i++){   //aplicar RoundRobin
                colaDestino[i]->push(clienteActual);
                }
                clienteActual = nullptr;
                ocupado = false;
            }
        }
    }
    void AnadirDestinoCliente(queue<Cliente*>* direccion){
      colaDestino.push_back(direccion);
        nColaDestino++;
    }

    void AnadirLlegadaCliente(queue<Cliente*>* direccion){
       destinoLlegada.push_back(direccion);;
        nDestinoLlegada++;
    }

    void asociarZonaSimple(ZonaSeguridad* zs){ //funcion asociacion simple
        zsAsociada = zs;
    }
    void asociarZonaMultiple(ZonaSeguridad* zs){ //fucnion asociacion multiple
        zonasAsociadas.push_back(zs);
    }

    void recibirCliente();
};

class ZonaSeguridad{
public:

    int id; //identificar zona o cliente?
    bool ocupado;
    Cliente* clienteActual;

    int t_ingreso;
    int t_salida;

    int contClientes;
    int tiempoOcupada=0; //añadido para estadistica

    ZonaSeguridad(int identificador){
        id = identificador;
        ocupado = false;
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
        tiempoOcupada += (t_salida - t_ingreso);
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

queue<Cliente*>cola; //variables globales de funciones
queue<Cliente*> colaPrioridad;

Cliente* clienteActual = nullptr;

int maxCola = 0;
int tiempoOcupado = 0;
int cantidadDescansos = 0;
int inicioServicio = 0;

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

void enCola();
void desencolar();
unsigned int clientesCola();
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
        horas[2] = horas[4]+ hora(servicio);
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

void mostrarEvento(int n, int v[n],int f[n+desercion-1]){
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

void proximoEvento(int n, int v[n], int f[n+desercion-1]){

    unsigned int siguiente = INT_MAX;

    for(int i=1; i<n; i++){
        if(v[i] > v[0] && v[i] < siguiente){
            siguiente = v[i];
        }
    }

    unsigned int salida = proximaSalida();

    if(salida < siguiente)
        siguiente = salida;

    v[0] = siguiente;

    for(int i=1; i<5; i++){
        if(siguiente == v[i])
            f[i-1] = 1;
        else
            f[i-1] = 0;
    }
    if(salida == siguiente)
        f[4] = 1;
    else
        f[4] = 0;

    int probabilidad;

    if(prioridad)
        probabilidad = rand() % 101;
    else
        probabilidad = 101;

    // EVENTO DE LLEGADA
    if(f[0]){

        Cliente* nuevoCliente = new Cliente(v[0], probA, false);

        if(!ocupado && servidorEstado &&
           (cola.empty() && colaPrioridad.empty())){

            ocupado = 1;

            clienteActual = nuevoCliente;
            inicioServicio = v[0];

            v[2] = v[0] + hora(servicio);

        }else{
        if(nuevoCliente->p){
            colaPrioridad.push(nuevoCliente);

            if(colaPrioridad.size() > maxCola){
                maxCola = colaPrioridad.size();
            }
        }else{
            cola.push(nuevoCliente);

            if(cola.size() > maxCola){
                maxCola = cola.size();
            }
        }
    }
    v[1] = v[0] + hora(llegada);
}

    // FIN DE SERVICIO
    if(f[1]){

        cont++;

        if(clienteActual != nullptr){

            tiempoOcupado += (v[0] - inicioServicio);

            delete clienteActual;
            clienteActual = nullptr;
        }

        if(!colaPrioridad.empty()){

            clienteActual = colaPrioridad.front();
            inicioServicio = v[0];
            colaPrioridad.pop();
            v[2] = siguiente + hora(servicio);
            ocupado = 1;

        }else if(!cola.empty()){

            clienteActual = cola.front();
            inicioServicio = v[0];
            cola.pop();
            v[2] = siguiente + hora(servicio);
            ocupado = 1;

        }else{
            ocupado = 0;
            v[2] = INT_MAX;
        }
    }
    // INICIO DESCANSO
    if(f[2] && descanso){

        cantidadDescansos++;

        servidorEstado = 0;
        v[4] = v[0] + hora(descansan);

        if(ocupado && !descansoAplicado && v[2] > v[3]){
            v[2] += (v[4] - v[3]);
            descansoAplicado = 1;
        }
    }
    // FIN DESCANSO
    if(f[3] && descanso){

        servidorEstado = 1;
        descansoAplicado = 0;

        v[3] = v[0] + hora(descan);
    }
    // ABANDONO DE COLA
    if(f[4] && desercion){

        procesarAbandono(siguiente);
        }
}

void enCola(int horas[]){

    Cliente* nuevo = new Cliente(horas[0], probA, false);
    cola.push(nuevo);

    if((cola.size() + colaPrioridad.size()) > maxCola){
        maxCola = cola.size()+ colaPrioridad.size();
    }
}

void desencolar(){ //usa misma variable global

    if(cola.empty()) return;
    Cliente* temp = cola.front();
    cola.pop();

    delete temp;
}

unsigned int clientesCola(){

    return cola.size();
}

void desencolarId(int id){ //usa colaPrioridad

    queue<Cliente*> auxiliar;
    while(!colaPrioridad.empty()){

        Cliente* actual = colaPrioridad.front();
        colaPrioridad.pop();

        if(actual->id == id){
            contDesertores++;
            delete actual;
        }
            else{
            auxiliar.push(actual);
        }
    }
    colaPrioridad = auxiliar;
    while(!cola.empty()){

        Cliente* actual = cola.front();
        cola.pop();

        if(actual->id == id){
            contDesertores++;
            delete actual;
        }else{
            auxiliar.push(actual);
        }
    }
   cola = auxiliar;
}

unsigned int proximaSalida(){//mismas variables globales

    if(!desercion) return INT_MAX;
    unsigned int salida = INT_MAX;
    queue<Cliente*> auxiliar;

    while(!colaPrioridad.empty()){
        Cliente* temp = colaPrioridad.front();
        colaPrioridad.pop();
        if(temp->t_salidaCola < salida){
            salida = temp->t_salidaCola;
        }
        auxiliar.push(temp);
    }

    colaPrioridad = auxiliar;
    while(!cola.empty()){
        Cliente* temp = cola.front();
        cola.pop();
        if(temp->t_salidaCola < salida){
            salida = temp->t_salidaCola;
        }
        auxiliar.push(temp);
        }

    cola = auxiliar;

    return salida;
}

void procesarAbandono(unsigned int tiempoActual){//mismas variables globales

    queue<Cliente*> auxiliar;
      while(!colaPrioridad.empty()){

        Cliente* temp = colaPrioridad.front();
        colaPrioridad.pop();

        if(temp->t_salidaCola == tiempoActual){
            desencolarId(temp->id);
            return;
        }
        auxiliar.push(temp);
        }

    colaPrioridad = auxiliar;

    while(!cola.empty()){

        Cliente* temp = cola.front();
        cola.pop();

        if(temp->t_salidaCola == tiempoActual){
            desencolarId(temp->id);
            return;
            }

        auxiliar.push(temp);
    }
        cola = auxiliar;
}

void cuatroSalidasSiguientes(){

    unsigned int n = clientesCola();
    unsigned int nA = clientesColaPrioridad();

    if((n+nA) <= 0){
        for(int i=0; i<4; i++) salidas[i]=0;
        return;
    }

    unsigned int auxiliar[n+nA];
    int indice = 0;

    queue<Cliente*> auxCola;
    queue<Cliente*> auxPrioridad;

    while(!cola.empty()){

        Cliente* temp = cola.front();
        cola.pop();

        auxiliar[indice] = temp->t_salidaCola;
        indice++;

        auxCola.push(temp);
    }

    cola = auxCola;

    while(!colaPrioridad.empty()){

        Cliente* temp = colaPrioridad.front();
        colaPrioridad.pop();

        auxiliar[indice] = temp->t_salidaCola;
        indice++;

        auxPrioridad.push(temp);
    }

    colaPrioridad = auxPrioridad;

    for(int i=0; i<(n+nA)-1; i++){
        for(int j=0; j<(n+nA)-i-1; j++){

            if(auxiliar[j] > auxiliar[j+1]){

                unsigned int tmp = auxiliar[j];
                auxiliar[j] = auxiliar[j+1];
                auxiliar[j+1] = tmp;
            }
        }
    }

    if((n+nA) >= 4){

        for(int i=0; i<4; i++){
            salidas[i] = auxiliar[i];
        }

    }else{

        for(int i=0; i<(n+nA); i++){
            salidas[i] = auxiliar[i];
        }

        for(int i=(n+nA); i<4; i++){
            salidas[i] = 0;
        }
    }
}

void encolaPrioridad(unsigned int horas[]){

    Cliente* nuevo = new Cliente(horas[0], probA, false);
    colaPrioridad.push(nuevo);

    if((cola.size() + colaPrioridad.size()) > maxCola){
        maxCola = cola.size()+ colaPrioridad.size();
    }
}

void desencolarPrioridad(){//mismas variables globales

    Cliente* temp;

    if(!colaPrioridad.empty()){
        temp = colaPrioridad.front();
        colaPrioridad.pop();
    }
    else if(!cola.empty()){
        temp = cola.front();
        cola.pop();
    }
    else{
        return;
    }

    cont++;

    delete temp;
}

unsigned int clientesColaPrioridad(){ //10

    queue<Cliente*> auxiliar;
    unsigned int contador = 0;

    while(!colaPrioridad.empty()){

        Cliente* temp = colaPrioridad.front();
        colaPrioridad.pop();

        contador++;

        auxiliar.push(temp);
    }
    colaPrioridad = auxiliar;
    return contador;
}

void mostrarHora(unsigned int segundos)//11 no es necesario modificar
{
     int h = segundos / 3600;
     int m = (segundos % 3600) / 60;
    int s = segundos % 60;

    printf("%03d:%02d:%02d|", h, m, s);}


void establecerTiempoParados(unsigned int s){

    queue<Cliente*> auxiliar;

    while(!cola.empty()){

        Cliente* temp = cola.front();
        cola.pop();

        temp->t_salidaCola = hora(deser) - s;

        auxiliar.push(temp);
     }
    cola = auxiliar;
}
bool establecerPrioridad(int probVal){

    int probabilidad = rand() % 101;

    if(probabilidad > probVal){
        return false; // Cliente tipo B
    }else{
        return true; // Cliente tipo A
    }
}
