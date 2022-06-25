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
    if(recorrido.len() < 2) return false;
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
    for(unsigned int i = 1; i < recorrido.len();i++){
        distancia += calcularDistancia(recorrido.getNodo(i),recorrido.getNodo(i+1));
    }
    return distancia;
}

double Vehiculo::distanciaDesdeRecarga(){
    double distancia = 0.0;
    recorrido.moveToEnd();
    while(recorrido.getCurr().tipo!='f'){
        recorrido.prev();
        if(compararNodos(recorrido.getCurr(),recorrido.getNodo(1))) return distanciaTotalRecorrida();
    }
    for(unsigned int i = recorrido.getPos(); i < recorrido.len();i++){
        distancia += calcularDistancia(recorrido.getNodo(i),recorrido.getNodo(i+1));
    }
    return distancia;
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
        void remove(unsigned int pos);
        void removeNext();
        void pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
        Vehiculo getVehiculo(unsigned int pos);
        unsigned int getPos();
        unsigned int len();
        void goToPos(unsigned int pos);
        void free();
        double calcularDistTotal();
        Vehiculo getCurr();
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

void ListaVehiculos::remove(unsigned int pos){
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
    ptr = vect.end();
}

void ListaVehiculos::prev(){
    int dist = distance(vect.begin(),ptr);
    ptr = vect.begin();
    for(int i=0;i<dist;i++) advance(ptr,1);
}

void ListaVehiculos::next(){
    if(ptr!=vect.end())advance(ptr,1);
}

Vehiculo ListaVehiculos::getVehiculo(unsigned int pos){
    return vect[pos];
}

unsigned int ListaVehiculos::getPos(){return distance(vect.begin(),ptr);}

unsigned int ListaVehiculos::len(){return vect.size();}

void ListaVehiculos::goToPos(unsigned int pos){
    ptr = vect.begin();
    if(pos>len()) return;
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

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

Vehiculo ListaVehiculos::getCurr(){
   return *ptr;
}

void ListaVehiculos::mostrar(){
    for(unsigned int i=0;i<vect.size();i++){
        Vehiculo vehi = getVehiculo(i);
        cout << vehi.recorrido.to_string() <<"\n";
    }
}


/*
void Vehiculo::quitarUltimaParada(int velocidad, int tiempoServicio, int tiempoRecarga){
    if(recorrido.len()<=1) return; //no se puede quitar el depot
    recorrido.moveToEnd();
    Nodo nodoAux = recorrido.getCurr();
    recorrido.prev();
    double distancia = 0.0;

    distancia = calcularDistancia(nodoAux, recorrido.getCurr());

    tiempoTranscurrido -= distancia/velocidad;
    distanciaTotalRecorrida -= distancia;
    distanciaDesdeRecarga -= distancia;

    recorrido.moveToEnd();

    if(recorrido.getCurr().tipo=='f'){
        //Se debe calcular la distancia que se había recorrido desde el depot u otra estacion
        //antes de llegar a la estacion actual.
        distancia = 0.0;
        do{
            nodoAux = recorrido.getCurr();
            recorrido.prev();
            distancia += calcularDistancia(nodoAux,recorrido.getCurr());

        }while(recorrido.getCurr().tipo=='c');
        tiempoTranscurrido -= tiempoRecarga;
        distanciaDesdeRecarga -= distancia;
    }
    else if(recorrido.getCurr().tipo=='c'){
        tiempoTranscurrido-=tiempoServicio;
        cantClientesVisitados--;
    }
    else{
        cout << "\nError, se está intentando quitar una parada del recorrido, pero no hay nodos para quitar.\n";
        return;
    }
    recorrido.pop();
}*/
