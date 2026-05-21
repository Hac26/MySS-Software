#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <list>
#include <queue>
#include <string>
#include <vector>

using  namespace std;
class ZonaSeguridad;
class PuestoServicio;
class Cliente;
class Cola;

typedef struct {
    unsigned int var;
    unsigned int t;
    unsigned int tMin;
    unsigned int tMax;
}horario;

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

vector<Cola*> direccionDeColas;
vector<PuestoServicio*> direccionPS;
vector<ZonaSeguridad*> direccionZS;     //guardan las direcciones de las colas, PS, ZS para que la interfaz grafica pueda acceder

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

int tiempoOcupado = 0;
int cantidadDescansos = 0;
int inicioServicio = 0;
int probA=30;

int habilitarClientesMax=0; //si es 1 la simulacion se limita al numero de clientes
int clientesMaximos=10; //nro de clientes maximos a atender
int contGlobal=0; //contador de clientes atendidos
int contDesertoresGlobal=0;
int contPS = 0;
int contZS = 0;
int contColas = 0;
int habilitarHorasMax=0;   //si es 1 la simulacion se limita al numero de clientes, solo puede estar activo esta o habilitarClientesMax
int horaFinSimulacion;

//fin de entrada por menu



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
        list<Cliente*> clientes;
        int clientesDesertados;
        bool prioridad;
        bool ZS;    //true si el destino es un ZS, false para un PS
        vector<PuestoServicio*> destinosPS;
        vector<ZonaSeguridad*> destinosZS;

        Cola(string n, bool p, bool zs){
            nombre = n;
            prioridad = p;
            ZS = zs;
            clientesDesertados = 0;
        }

        void TransferirCliente();

    void AnadirDestinoPS(PuestoServicio* direccion){
        destinosPS.push_back(direccion);// destinos no exite----> colaDestino
    }

    void AnadirDestinoZS(ZonaSeguridad* direccion){
        destinosZS.push_back(direccion);// destinos no exite----> colaDestino
    }


    Cola* inicializarCola(string nombre, bool p, bool zs){
        Cola* nueva = new Cola(nombre, p, zs);
        direccionDeColas.push_back(nueva);
        contColas++;
        return nueva;
    }       //se inicializa una cola -> Cola* nombre = inicializarCola("nombreDeCola", prioridad); y guarda la direccion del puntero en la variable nombre
};          //se agrega un cliente con cola->clientes.push(cliente)

class ZonaSeguridad{
public:

    bool ocupado;
    Cliente* clienteActual;
    int t_ingreso;
    int t_salida;
    int contClientes;
    vector<PuestoServicio*> PSDestino;
    int lastIndex=0;

    int tiempoOcupada=0; //a�adido para estadistica

    ZonaSeguridad(bool o){
        ocupado = o;
        clienteActual = nullptr;
        t_ingreso = 0;
        t_salida = 0;

        contClientes = 0;
    }

    ZonaSeguridad* InicializarZS(bool o, int horaEntrada){
    }


    void TransferirCliente();

    void AñadirDestino(PuestoServicio* direccion){
     PSDestino.push_back(direccion);
    }
};

class PuestoServicio{
public:
    string nombre;
    bool ocupado;
    bool Activo;
    bool descanso;
    bool esFin; // si no le sigue nada
    int contClientes = 0;
    int tiempoInicioDescanso=0;
    int tiempoFinDescanso=0;
    int cantidadDescansos=0;
    Cliente* clienteActual;
    vector<Cola*> colaDestino;
    vector<Cola*> colaDestinoPrioridad;
    int lastIndex=0;
    int lastIndexPrioridad=0;

    PuestoServicio(string n, bool o, bool fin){
        ocupado = o;
        esFin = fin;
        contPS++;
    }
    PuestoServicio* inicializarPS(string nombre, bool o, bool fin){
        PuestoServicio* nueva = new PuestoServicio(nombre, o, fin);
        direccionPS.push_back(nueva);
        contPS++;
        return nueva;
    }
    void TransferirCliente(){

        if(clienteActual != nullptr){ //si hay cliente
            ocupado = false;
            contClientes++;
            if(!esFin){
                    if(clienteActual->p){
                colaDestinoPrioridad[lastIndexPrioridad]->clientes.push_back(clienteActual);
                lastIndexPrioridad++;
                if(lastIndexPrioridad == colaDestinoPrioridad.size()) lastIndexPrioridad=0;
                    }else{
                colaDestino[lastIndex]->clientes.push_back(clienteActual);
                lastIndex++;
                if(lastIndex == colaDestino.size()) lastIndex=0;
                }
                }
            }else{
                delete clienteActual;
            }
            clienteActual = nullptr;
        }


    void AnadirDestinoCliente(Cola* direccion){
      if(direccion->prioridad){
        colaDestinoPrioridad.push_back(direccion);
      }else{
      colaDestino.push_back(direccion);
      }
    }


};




void Cola::TransferirCliente(){
            if(!clientes.empty()){
                if(ZS){
                    destinosZS[0]->clienteActual = clientes.front(); //hay que aplicar round robin a estos dos
                }else{
                    destinosPS[0]->clienteActual = clientes.front();
                }
                clientes.pop_front();
            }
        }
void ZonaSeguridad::TransferirCliente(){
        if(clienteActual != nullptr ){
            PSDestino[lastIndex]->clienteActual = this->clienteActual;
            lastIndex++;
            if(lastIndex == PSDestino.size()) lastIndex = 0;
        }
            ocupado = false;
            clienteActual = nullptr;
        tiempoOcupada += (t_salida - t_ingreso);
        contClientes++;
    }


int main(){
    srand(time(NULL));

    printf("-------MODELO Y SIMULACION DE SISTEMAS------\n");
    int prioridad=1; //en desuso, solo esta para que compile el programa
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
    //mostrarEvento(n, horas);
    //proximoEvento(n, horas, flags);
    }
    if(habilitarHorasMax){
    unsigned int siguiente=INT_MAX;
    int indice = -1;
    for(int i=0;i<n;i++){
        if(horas[i] > horas[0] && horas[i] < siguiente){
            siguiente=horas[i];
        }
    }
        //mostrarEvento(n,horas,flags);
    }
    if(habilitarClientesMax){
        //mostrarEvento(n,horas,flags);
    }
printf("-------------------------------------------------------");
    for(int i=0 ;i<titulos;i++)printf("----------");
    for(int i=0;i<n+desercion-1;i++)printf("---");
    if(desercion)printf("----");
    printf("\n");

    return 0;
}

unsigned int hora(horario h){
    if(h.var){
        unsigned int temp = h.tMin + (rand()% (h.tMax - h.tMin + 1));
        return temp;
    }else{
        return h.t;
    }
}

/*void proximoEvento(int n, int v[n], int f[5]){

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
*/

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
