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
class FuenteClientes;

typedef struct {
    unsigned int var;
    unsigned int t;
    unsigned int tMin;
    unsigned int tMax;
}horario;

unsigned int proximoEvento(unsigned int tiempo);
void procesarEvento(unsigned int tiempo);
void calcularEstadisticasGlobales();
unsigned int hora(horario h);
unsigned int proximaSalida();
void cuatroSalidasSiguientes();
void mostrarHora(unsigned int segundos);
void establecerTiempoParados(unsigned int s);
bool establecerPrioridad(int probVal);

vector<FuenteClientes*> direccionFC;
vector<Cola*> direccionDeColas;
vector<PuestoServicio*> direccionPS;
vector<ZonaSeguridad*> direccionZS;     //guardan las direcciones de las colas, PS, ZS para que la interfaz grafica pueda acceder

unsigned int idn=0;

void AnadirDestinoZS(ZonaSeguridad* direccion);
void AnadirDestinoPS(ZonaSeguridad* direccion);

void AnadirDestino(Cola* destino);
void TransferirCliente(unsigned int tiempo);
FuenteClientes* inicializarFC(unsigned int tiempoLlegada, int probVal);

PuestoServicio* obtenerPSDisponible();
void TransferirCliente();
void AnadirDestino(PuestoServicio* direccion);


void TransferirCliente();
void AnadirDestinoCliente(Cola* direccion);
void TransferirCliente();
 PuestoServicio* obtenerPSDisponible();


//entrada por menu

horario llegada = {
    .var = 0,
    .t = 45,
    .tMin = 50,
    .tMax = 70,
};
horario servicio = {
    .var = 0,
    .t = 50,
    .tMin = 50,
    .tMax = 70,
};

horario descan = {
    .var = 0,
    .t = 60,
    .tMin = 50,
    .tMax = 70,
};
horario descansan = {
    .var = 0,
    .t = 60,
    .tMin = 50,
    .tMax = 70,
};

horario deser = {
    .var = 0,
    .t = 600,
    .tMin = 50,
    .tMax = 70,
};

horario seguridad = {
    .var = 0,
    .t = 30,
    .tMin = 50,
    .tMax = 70,
};


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
int contFC = 0;
int contColas = 0;


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
        bool desercion;
        bool ZS;    //true si el destino es un ZS, false para un PS
        vector<PuestoServicio*> destinosPS;
        int lastIndexPS = 0;
        vector<ZonaSeguridad*> destinosZS;
        int lastIndexZS = 0;


        Cola(string n, bool p, bool zs, bool deser){
            desercion = deser;
            nombre = n;
            prioridad = p;
            ZS = zs;
            clientesDesertados = 0;
        }

        void TransferirCliente(unsigned int tiempo);

    void AnadirDestinoPS(PuestoServicio* direccion){
        destinosPS.push_back(direccion);// destinos no exite----> colaDestino
    }

    void AnadirDestinoZS(ZonaSeguridad* direccion){
        destinosZS.push_back(direccion);// destinos no exite----> colaDestino
    }


           //se inicializa una cola -> Cola* nombre = inicializarCola("nombreDeCola", prioridad); y guarda la direccion del puntero en la variable nombre
};          //se agrega un cliente con cola->clientes.push(cliente)

class FuenteClientes{
    public:
        unsigned int t_ProximaLlegada;
        int probabilidad;
        vector<Cola*> Destinos;
        vector<Cola*> DestinosPrioritarios;
        int lastIndex = 0;
        int lastIndexPrioridad = 0;

    FuenteClientes(unsigned int tiempoLlegada, int probVal){
        t_ProximaLlegada = tiempoLlegada + hora(llegada);
        probabilidad = probVal;
    }
    void AnadirDestino(Cola* destino){
        if(destino->prioridad) DestinosPrioritarios.push_back(destino);
        else Destinos.push_back(destino);
    }
    void TransferirCliente(unsigned int tiempo){
        Cliente* clienteGenerado = new Cliente(tiempo, probabilidad);
        if(clienteGenerado->p){
            for(int i = 0;i < DestinosPrioritarios.size();i++){
                int idx =(lastIndexPrioridad + i)% DestinosPrioritarios.size();
                DestinosPrioritarios[idx]->clientes.push_back(clienteGenerado);
                lastIndexPrioridad =(idx + 1)% DestinosPrioritarios.size();
                break;
            }
        }else{
            for(int i = 0;i < Destinos.size();i++){
                int idx =(lastIndex + i)% Destinos.size();
                Destinos[idx]->clientes.push_back(clienteGenerado);
                lastIndex =(idx + 1)% Destinos.size();
                break;
            }
        }
        t_ProximaLlegada = tiempo + hora(llegada);
    }

};
class ZonaSeguridad{
public:
    string nombre;
    bool ocupado;
    Cliente* clienteActual;
    int t_ingreso;
    int t_salida;
    int contClientes;
    vector<PuestoServicio*> PSDestino;
    int lastIndex=0;
    PuestoServicio* PSAsignado;

    int tiempoOcupada=0; //a�adido para estadistica

    ZonaSeguridad(string n, bool o, Cliente* c){
        nombre = n;
        ocupado = o;
        clienteActual = c;
        t_ingreso = 0;
        t_salida = 0;

        contClientes = 0;
    }


    PuestoServicio* obtenerPSDisponible();


    void TransferirCliente(unsigned int tiempo);

    void AnadirDestino(PuestoServicio* direccion){
     PSDestino.push_back(direccion);
    }
};

class PuestoServicio{
public:
    string nombre;
    bool ocupado;
    bool Activo;
    bool descanso;
    bool descansoAplicado;
    bool esFin; // si no le sigue nada
    unsigned int t_FinDeServicio;
    int contClientes = 0;
    int t_InicioDescanso=0;
    int t_FinDescanso=0;
    int cantidadDescansos=0;
    Cliente* clienteActual;
    vector<Cola*> colaDestino;
    vector<Cola*> colaDestinoPrioridad;
    int lastIndex=0;
    int lastIndexPrioridad=0;

    PuestoServicio(string n,bool o, bool act,bool des,bool fin, Cliente* actual, unsigned int tiempo){
        nombre = n;
        ocupado = o;
        esFin = fin;
        clienteActual = actual;
        Activo = act;
        descanso = des;
        descansoAplicado = false;
        if(descanso) {
                if(Activo){
                descansoAplicado = false;
                t_InicioDescanso = tiempo+hora(descan);
                }else{
                    unsigned int tDescanso = hora(descansan);
                    if(ocupado){
                        t_FinDeServicio += tDescanso;
                        descansoAplicado = true;
                    }
                    t_FinDescanso = tiempo + tDescanso;
                }
        }
        contPS++;
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




void Cola::TransferirCliente(unsigned int tiempo){
            bool transferido = false;
            if(clientes.empty()){
                return;
            }else {
                Cliente* cliente = clientes.front();
                if(ZS){
                for(int i = 0; i < destinosZS.size(); i++){
                    int idx = (lastIndexZS + i) % destinosZS.size();
                    ZonaSeguridad* zs = destinosZS[idx];
                    PuestoServicio* ps = zs->obtenerPSDisponible();
                    if(!zs->ocupado &&ps != nullptr){
                        zs->clienteActual = cliente;
                        zs->ocupado = true;
                        zs->t_ingreso = tiempo;
                        zs->t_salida = tiempo + hora(seguridad);
                        zs->PSAsignado = ps;
                        lastIndexZS = (idx + 1) % destinosZS.size();
                        transferido = true;
                        break;
                    }
                }
                }else{
                    for(int i = 0; i < destinosPS.size(); i++){
                        int idx = (lastIndexPS + i) % destinosPS.size();
                        PuestoServicio* ps = destinosPS[idx];
                        if(!ps->ocupado && ps->Activo){
                            ps->clienteActual = cliente;
                            ps->ocupado = true;
                            ps->t_FinDeServicio = tiempo + hora(servicio);
                            lastIndexPS = (idx + 1) % destinosPS.size();
                            transferido = true;
                            break;
                        }

                    }
                }
            }
         if(transferido) clientes.pop_front();

        }
void ZonaSeguridad::TransferirCliente(unsigned int tiempo){
        if(clienteActual == nullptr) return;
        if(PSAsignado == nullptr)return;
        if(PSAsignado->ocupado || !PSAsignado->Activo) return;
            PSAsignado->clienteActual = clienteActual;
            PSAsignado->ocupado = true;
            PSAsignado->t_FinDeServicio = tiempo + hora(servicio);
            tiempoOcupada += (t_salida - t_ingreso);
            contClientes++;
            clienteActual = nullptr;
            ocupado = false;
            PSAsignado = nullptr;
    }

    PuestoServicio* ZonaSeguridad::obtenerPSDisponible(){

    if(PSDestino.empty())
        return nullptr;

    for(int i = 0; i < PSDestino.size(); i++){
        int idx = (lastIndex + i) % PSDestino.size();
        PuestoServicio* ps =
            PSDestino[idx];

        if(ps->Activo && !ps->ocupado){
            lastIndex = (idx + 1) % PSDestino.size();
            return ps;
        }
    }
    return nullptr;
    }
    Cola* inicializarCola(string nombre, bool p, bool zs, bool desercion);
    ZonaSeguridad* InicializarZS(string n, bool o, Cliente* c);
    PuestoServicio* inicializarPS(string n,bool o, bool act,bool des,bool fin, Cliente* actual,unsigned int tiempo);
    FuenteClientes* inicializarFC(unsigned int tiempoLlegada, int probVal);

#include "simqueue_csv.h"

int main(){
    SimCSV::init();
    srand(time(NULL));
    int horaInicio=0;
    int tiempo=horaInicio;
    bool habilitarHorasMax=true;   //si es 1 la simulacion se limita al numero de clientes, solo puede estar activo esta o habilitarClientesMax
    int horaFinSimulacion = 20*60;
    bool habilitarClientesMax=false; //si es 1 la simulacion se limita al numero de clientes
    int clientesMaximos=10;

    printf("-------MODELO Y SIMULACION DE SISTEMAS------\n");
    /*      Machete
    Cola* inicializarCola(string nombre, bool prioridad, bool zonaseguridad, bool desercion);
    ZonaSeguridad* InicializarZS(string n, bool o, Cliente* c);
    PuestoServicio* inicializarPS(string n,bool ocupado, bool activo,bool descanso,bool fin, Cliente* actual, unsigned int tiempo);
    FuenteClientes* inicializarFC(unsigned int tiempoLlegada, int probabilidadClientePrioritario); */

    Cola* ColaB = inicializarCola("Cola", false, true,true);
    Cola* ColaP = inicializarCola("Prioridad",true,false,false);
    ZonaSeguridad* ZS = InicializarZS("ZS",false,nullptr);
    PuestoServicio* PS = inicializarPS("PS",false, true,true,true, nullptr, tiempo);
    PuestoServicio* PS2 = inicializarPS("PS2",false, true,true,true, nullptr, tiempo);
    FuenteClientes* Gen1 = inicializarFC(tiempo, 30);
    Gen1->AnadirDestino(ColaB);
    Gen1->AnadirDestino(ColaP);
    ColaB->AnadirDestinoZS(ZS);
    ZS->AnadirDestino(PS);
    ColaP->AnadirDestinoPS(PS2);



    unsigned int proximo = proximoEvento(tiempo);
    mostrarEvento(tiempo);
    for(tiempo = horaInicio ; habilitarClientesMax && (contGlobal < clientesMaximos)  || habilitarHorasMax && ( tiempo<=horaFinSimulacion ); tiempo ++){
    //Ejecucion en bucle
        if(tiempo == proximo){
            procesarEvento(tiempo);
            proximo = proximoEvento(tiempo);
            calcularEstadisticasGlobales();
            mostrarEvento(tiempo);
        }
        if(tiempo % 60 == 0){

        }
    }

        //mostrarEvento();
}

unsigned int hora(horario h){
    if(h.var){
        unsigned int temp = h.tMin + (rand()% (h.tMax - h.tMin + 1));
        return temp;
    }else{
        return h.t;
    }
}

unsigned int proximoEvento(unsigned int tiempo){
    unsigned int siguiente = INT_MAX;

    for(auto FC : direccionFC){ //LLegadas
        if(FC->t_ProximaLlegada > tiempo &&FC->t_ProximaLlegada < siguiente){
        siguiente = FC->t_ProximaLlegada;
        }
    }
    for (auto cola : direccionDeColas){
        for(auto cliente : cola->clientes){//deserciones
            if(cliente->t_salidaCola > tiempo && cliente->t_salidaCola < siguiente){
            siguiente =cliente->t_salidaCola;
            }
        }
    }

    for(auto ps : direccionPS){
    if(ps->t_FinDeServicio > tiempo && ps->t_FinDeServicio < siguiente){
        siguiente = ps->t_FinDeServicio; //Fin de servicio
    }
    if(ps->t_InicioDescanso > tiempo && ps->t_InicioDescanso < siguiente){
        siguiente = ps->t_InicioDescanso; //InicioDescanso
    }
    if(ps->t_FinDescanso > tiempo && ps->t_FinDescanso < siguiente){
        siguiente = ps->t_FinDescanso; //Fin del descanso
    }
    }

    for(auto zs : direccionZS){
        if(zs->t_salida > tiempo && zs->t_salida < siguiente){
            siguiente = zs->t_salida;
        }
    }
    return siguiente;
}
void procesarEvento(unsigned int tiempo){
    //LLEGADAS
    for (auto fuente: direccionFC){
        if(fuente->t_ProximaLlegada == tiempo){
            fuente->TransferirCliente(tiempo);
        }
    }
    //DESERCIONES
    for(auto cola : direccionDeColas){
        if(cola->desercion){
            for (auto it = cola->clientes.begin(); it != cola->clientes.end();){
                if ((*it)->t_salidaCola == tiempo){
                    Cliente *aux = *it;
                    cola->clientesDesertados++;
                    it = cola->clientes.erase(it);
                    delete aux;
                }else{
                    ++it;
                }
            }
        }
    }
    //LIBERAR PS
    for (auto ps : direccionPS){
        if(ps->ocupado && ps->t_FinDeServicio <= tiempo ){
        ps->TransferirCliente();
        ps->ocupado = false;
        }
    }
    //LIBERAR ZS
    for (auto zs : direccionZS){
        if(zs->ocupado && zs->t_salida <= tiempo ){
                if(zs->PSAsignado != nullptr && !zs->PSAsignado->ocupado && zs->PSAsignado->Activo){
            zs->TransferirCliente(tiempo); //actualizar el roundrobin de las funciones
            zs->ocupado = false;
            zs->clienteActual = nullptr;
            zs->PSAsignado = nullptr;
        }
        }
    }
    //DESCANSO
    for(auto ps : direccionPS){
        if(ps->descanso){
            if(ps->t_InicioDescanso == tiempo) ps->cantidadDescansos++;
            if(ps->t_InicioDescanso == tiempo && ps->Activo){
                ps->Activo = false;
                unsigned int tDescanso = hora(descansan);
                ps->t_FinDescanso = ps->t_InicioDescanso + tDescanso;
                if(ps->ocupado && !ps->descansoAplicado){
                     ps->t_FinDeServicio += tDescanso;
                     ps->descansoAplicado = true;
                }
            }
            if (ps->t_FinDescanso == tiempo){ //FIN DESCANSO
                ps->Activo = true;
                ps->t_InicioDescanso = tiempo + hora(descan);
                ps->descansoAplicado = false;
            }
        }
    }
    //TRANSFERENCIA DE COLAS
    for(auto cola : direccionDeColas){
    cola->TransferirCliente(tiempo);
    }
}


void mostrarHora(unsigned int segundos)//11 no es necesario modificar
{
     int h = segundos / 3600;
     int m = (segundos % 3600) / 60;
    int s = segundos % 60;

    printf("%03d:%02d:%02d|", h, m, s);}



bool establecerPrioridad(int probVal){

    int probabilidad = rand() % 101;

    if(probabilidad > probVal){
        return false; // Cliente tipo B
    }else{
        return true; // Cliente tipo A
    }
}
Cola* inicializarCola(string nombre, bool p, bool zs, bool desercion){
        Cola* nueva = new Cola(nombre, p, zs, desercion);
        direccionDeColas.push_back(nueva);
        contColas++;
        return nueva;
    }
FuenteClientes* inicializarFC(unsigned int tiempoLlegada, int probVal){
        FuenteClientes* nueva = new FuenteClientes(tiempoLlegada, probVal);
        contFC++;
        direccionFC.push_back(nueva);
        return nueva;
    }
PuestoServicio* inicializarPS(string n,bool o, bool act,bool des,bool fin, Cliente* actual, unsigned int tiempo){
        PuestoServicio* nueva = new PuestoServicio(n, o, act, des, fin, actual, tiempo);
        direccionPS.push_back(nueva);
        contPS++;
        return nueva;
    }
    ZonaSeguridad* InicializarZS(string n, bool o, Cliente* c){
        ZonaSeguridad* nuevo = new ZonaSeguridad(n, o, c);
        direccionZS.push_back(nuevo);
        contZS++;
        return nuevo;
    }
void calcularEstadisticasGlobales() {
    // 1. Limpiar e inicializar contadores para esta ejecucion
    contGlobal = 0;
    contDesertoresGlobal = 0;
    long long totalTiempoOcupadoZS = 0;
    int totalClientesEnColasActuales = 0;

    // 2. Recopilar datos de todas las Colas
    for (auto cola : direccionDeColas) {
        contDesertoresGlobal += cola->clientesDesertados;
        totalClientesEnColasActuales += cola->clientes.size();
    }

    // 3. Recopilar datos de todos los Puestos de Servicio (PS)
    for (auto ps : direccionPS) {
        if (ps->esFin == true){
        contGlobal += ps->contClientes;
    }
    }
    // 4. Recopilar datos de todas las Zonas de Seguridad (ZS)
    for (auto zs : direccionZS) {
        totalTiempoOcupadoZS += zs->tiempoOcupada;
    }

}
