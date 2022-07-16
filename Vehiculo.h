#include "Nodo.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

using namespace std;

// Clases y métodos relativas a vehículos:

class Vehiculo{
    public:
        int ID;
        ListaNodos recorrido;
        unsigned int cantClientesVisitados;
        double velocidad;
        int tiempoServicio;
        int tiempoRecarga;
        static int id_actual;

        Vehiculo();
        Vehiculo(double vel, int tServicio, int tRecarga);
        Vehiculo(Nodo depot);
        void agregarParada(Nodo nodo);
        bool recorridoTerminado();
        int tiempoTranscurrido();
        double distanciaTotalRecorrida();
        double distanciaDesdeRecarga();
        void free();
        bool checkEstaciones(Nodo paradaNueva);
};

Vehiculo::Vehiculo(){
    ID = id_actual++;
    cantClientesVisitados = 0;
    recorrido = ListaNodos();
}

Vehiculo::Vehiculo(double vel, int tServicio, int tRecarga){
    ID = id_actual++;
    cantClientesVisitados = 0;
    velocidad = vel;
    tiempoServicio = tServicio;
    tiempoRecarga = tRecarga;
    recorrido = ListaNodos();
}

Vehiculo::Vehiculo(Nodo depot){
    ID = id_actual++;
    cantClientesVisitados = 0;
    recorrido.append(depot);
}

void Vehiculo::agregarParada(Nodo nodo){
    recorrido.append(nodo);
    if(nodo.tipo=='c')cantClientesVisitados++;
}

bool Vehiculo::recorridoTerminado(){
    if(recorrido.len() < 1) return false;
    recorrido.moveToEnd();
    Nodo nodoFinal = recorrido.getCurr();
    recorrido.prev();
    //La segunda condicion nunca debería pasar, pero por si acaso
    if(nodoFinal.tipo=='d' && recorrido.getCurr().tipo != 'd') return true;
    else return false;
}

int Vehiculo::tiempoTranscurrido(){
    return distanciaTotalRecorrida()/velocidad;
}

double Vehiculo::distanciaTotalRecorrida(){
    double distancia = 0.0;
    for(unsigned int i = 0; i < recorrido.len()-1;i++){
        distancia += calcularDistancia(recorrido.getNodo(i),recorrido.getNodo(i+1));
    }
    return distancia;
}

double Vehiculo::distanciaDesdeRecarga(){
    double distancia = 0.0;
    recorrido.moveToEnd();
    //Vemos si hay una estacion en el recorrido. Si no hay, calcula la distancia total del recorrido
    while(recorrido.getCurr().tipo!='f'){
        recorrido.prev();
        if(compararNodos(recorrido.getCurr(),recorrido.getNodo(0))) return distanciaTotalRecorrida();
    }
    //Si hay una estacion, se calcula la distancia de todos los pares de nodos hasta llegar al final
    for(unsigned int i = recorrido.getPos(); i < recorrido.len()-1;i++){
        distancia += calcularDistancia(recorrido.getNodo(i),recorrido.getNodo(i+1));
    }
    return distancia;
}

bool Vehiculo::checkEstaciones(Nodo paradaNueva){
    //Verifica si, con la parada nueva, se estarían visitando 3 nodos seguidos, en cuyo caso aceptada es false.
    bool aceptada = true;
    if(recorrido.len() > 3){
        recorrido.moveToEnd();
        if(recorrido.getCurr().tipo == 'f'){
            recorrido.prev();
            if(recorrido.getCurr().tipo == 'f' && paradaNueva.tipo == 'f') aceptada = false; 
        }
    }
    return aceptada;
}

void Vehiculo::free(){
    recorrido.free();
}

typedef struct tVehi{
    Vehiculo data;
    struct tVehi *next;
} tVehi;

//ListaVehiculos tambien es una lista enlazada

class ListaVehiculos{
    vector<Vehiculo> vect;
    vector<Vehiculo>::iterator ptr;

    public:
        ListaVehiculos();
        void insertInFront(Vehiculo item);
        void append(Vehiculo vehi); 
        void remove(int pos);
        void pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void moveToPos(unsigned int pos);
        void clear();
        Vehiculo getVehiculo(unsigned int pos);
        Vehiculo getCurr();
        unsigned int getPos();
        unsigned int len();
        void free();
        double calcularDistTotal();
        void mostrar();
};

ListaVehiculos::ListaVehiculos(){
    vect;
    ptr = vect.begin();
}

void ListaVehiculos::append(Vehiculo vehi){
    if(len()==1) ptr = vect.begin();
    vect.push_back(vehi);
}

void ListaVehiculos::remove(int pos){
    ptr = vect.begin();
    advance(ptr,pos);
    vect.erase(ptr++);
}

void ListaVehiculos::pop(){
    vect.pop_back();
}

void ListaVehiculos::moveToStart(){
    ptr = vect.begin();
}

void ListaVehiculos::moveToEnd(){
    ptr = vect.end()-1;
}

void ListaVehiculos::prev(){
    int dist = distance(vect.begin(),ptr);
    ptr = vect.begin();
    for(int i=0;i<dist-1;i++) advance(ptr,1);
}

void ListaVehiculos::next(){
    if(ptr!=vect.end()-1)advance(ptr,1);
}

void ListaVehiculos::moveToPos(unsigned int pos){
    ptr = vect.begin();
    if(pos>len()) return;
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

Vehiculo ListaVehiculos::getVehiculo(unsigned int pos){
    if(pos<len() && len()>0) return vect[pos];
    Vehiculo vehiAux;
    return vehiAux;
    
}

Vehiculo ListaVehiculos::getCurr(){
    Vehiculo vehiAux;
    if(len()==0) return vehiAux;
    else vehiAux = vect[distance(vect.begin(),ptr)];
    return vehiAux;
}

unsigned int ListaVehiculos::getPos(){return distance(vect.begin(),ptr);}

unsigned int ListaVehiculos::len(){return vect.size();}


void ListaVehiculos::free(){
    for (unsigned int i = 0; i < vect.size(); i++){
        vect[i].recorrido.free();
    }
    vect.clear();
}

double ListaVehiculos::calcularDistTotal(){
    double distancia = 0.0;
    if(vect.size() < 1) return distancia;
    for(unsigned int i = 0; i<vect.size();i++){
        distancia += getVehiculo(i).distanciaTotalRecorrida();
    }
    return distancia;
}


void ListaVehiculos::mostrar(){
    for(unsigned int i=0;i<vect.size();i++){
        Vehiculo vehi = getVehiculo(i);
        cout << vehi.recorrido.to_string() <<"\n";
    }
}