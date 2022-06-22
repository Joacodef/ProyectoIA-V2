#include <iostream>
#include "Nodo.h"

using namespace std;

// Clases y métodos relativas a vehículos:

class Vehiculo{
    public:
        ListaNodos recorrido;
        int tiempoTranscurrido;
        double distanciaTotalRecorrida;
        double distanciaDesdeRecarga;
        unsigned int cantClientesVisitados;

        Vehiculo();
        void agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga);
        void reiniciarRecorrido();
};

Vehiculo::Vehiculo(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
}

void Vehiculo::agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga){
    recorrido.insertInFront(nodo);
    tiempoTranscurrido += distancia/velocidad;
    distanciaTotalRecorrida += distancia;
    distanciaDesdeRecarga += distancia;
    if(nodo.tipo=='f') {
        distanciaDesdeRecarga = 0.0;
        tiempoTranscurrido += tiempoRecarga;
    }
    else if(nodo.tipo=='c'){
        tiempoTranscurrido += tiempoServicio;
        cantClientesVisitados++;
    }
}



typedef struct tVehi{
    Vehiculo data;
    struct tVehi *next;
} tVehi;

void Vehiculo::reiniciarRecorrido(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
    recorrido.clear();
}

//ListaVehiculos tambien es una lista enlazada

class ListaVehiculos{
    tVehi *head;
    tVehi *tail;
    tVehi *curr; // curr apunta al nodo anterior al actual
    unsigned int listSize;
    unsigned int pos;
    public:
        ListaVehiculos();
        void insertInFront(Vehiculo item);
        void append(Vehiculo vehi); 
        void remove(unsigned int pos);
        void removeNext();
        Vehiculo pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
        Vehiculo getVehiculo(unsigned int pos);
        unsigned int getPos();
        unsigned int len();
        void goToPos(unsigned int pos);
};

ListaVehiculos::ListaVehiculos(){
    head = tail = curr = (tVehi*)malloc(sizeof(tVehi)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
}

void ListaVehiculos::insertInFront(Vehiculo item){
    tVehi *aux = curr->next;
    curr->next = (tVehi*)malloc(sizeof(tVehi));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
}

void ListaVehiculos::append(Vehiculo vehi){
    moveToEnd();
    insertInFront(vehi);
    moveToStart();
}

void ListaVehiculos::remove(unsigned int pos){
    if(pos>listSize) return;
    goToPos(pos);
    prev();
    removeNext();
    moveToStart();
}

void ListaVehiculos::removeNext(){
    if(curr==tail) return;
    if(curr->next == tail) tail = curr;
    tVehi *aux = curr->next;
    curr->next = curr->next->next;
    listSize--;
    free(aux);
}

Vehiculo ListaVehiculos::pop(){
    Vehiculo vehiAux;
    if(listSize==0) return vehiAux;
    moveToEnd();
    vehiAux = curr->data;
    prev();
    removeNext();
    moveToStart();
    return vehiAux;
}

void ListaVehiculos::moveToStart(){
    curr=head;
    pos=0;
}

void ListaVehiculos::moveToEnd(){
    curr=tail;
    pos=listSize;
}

void ListaVehiculos::prev(){
    tVehi *temp;
    if (curr==head) return;
    temp = head;
    while (temp->next != curr) temp = temp->next;
    curr = temp;
    pos--;
}

void ListaVehiculos::next(){
    if (curr != tail){
        curr = curr->next; 
        pos++;
    } 
}

void ListaVehiculos::clear(){
    moveToStart();
    while(listSize>0){
        removeNext();
    }
}

Vehiculo ListaVehiculos::getVehiculo(unsigned int pos){
    //El head es posicion -1, el siguiente es posicion 0
    Vehiculo vehiAux;
    if(pos>=listSize) return vehiAux;
    moveToStart();
    next();
    for(unsigned int i = 0;i<pos;i++){
        next();
    }
    vehiAux = curr->data;
    return vehiAux;
}

unsigned int ListaVehiculos::getPos(){return pos;}

unsigned int ListaVehiculos::len(){return listSize;}

void ListaVehiculos::goToPos(unsigned int pos){
    moveToStart();
    if(pos>=listSize) return;
    next();
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}