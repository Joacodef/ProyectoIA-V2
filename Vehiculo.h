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
        Vehiculo(Nodo depot);
        void agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga);
        void quitarUltimaParada(int velocidad, int tiempoServicio, int tiempoRecarga);
        void reiniciarRecorrido();
        bool terminoRecorrido();
};

Vehiculo::Vehiculo(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
}

Vehiculo::Vehiculo(Nodo depot){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
    recorrido.append(depot);
}

void Vehiculo::agregarParada(Nodo nodo, double velocidad, double distancia, int tiempoServicio, int tiempoRecarga){
    recorrido.append(nodo);
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
}

void Vehiculo::reiniciarRecorrido(){
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
    recorrido.clear();
}

bool Vehiculo::terminoRecorrido(){
    if(recorrido.len() < 2) return false;
    recorrido.moveToEnd();
    if(recorrido.getCurr().tipo=='d') return true;
    else return false;
}

typedef struct tVehi{
    Vehiculo data;
    struct tVehi *next;
} tVehi;


//ListaVehiculos tambien es una lista enlazada

class ListaVehiculos{
    tVehi *head;
    tVehi *tail;
    tVehi *curr; 
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
        void free();
        double calcularDistTotal();
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
    std::free(aux);
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

void ListaVehiculos::free(){
    clear();
    std::free(head);
}

double ListaVehiculos::calcularDistTotal(){
    double distancia = 0.0;
    if(listSize < 1) return distancia;
    for(unsigned int i = 0; i<listSize;i++){
        distancia += getVehiculo(i).distanciaTotalRecorrida;
    }
    return distancia;
}

// Clase que se asocia al concepto de variable en la representación

class Variable{
    public:
        Nodo nodoAsignado;
        Vehiculo vehiculo;
        ListaNodos dominio;

        Variable();
        Variable(Nodo nodo, Vehiculo vehi, ListaNodos clientes, 
                 ListaNodos estaciones, Nodo depot);
        void eliminarClientesDominio();
        void eliminarNodosMasLejosDepot(double distancia);
        void resetearDominio(ListaNodos clientes, ListaNodos estaciones, Nodo depot);
};

Variable::Variable(){
    nodoAsignado = Nodo();
    vehiculo = Vehiculo();
    dominio = ListaNodos();
}

Variable::Variable(ListaNodos clientes, 
                   ListaNodos estaciones, Nodo depot){
    nodoAsignado = Nodo();
    vehiculo = Vehiculo();
    dominio = concatenar(clientes,estaciones);
    dominio.append(depot);
}

void Variable::resetearDominio(ListaNodos clientes, ListaNodos estaciones, Nodo depot){
    dominio = concatenar(clientes,estaciones);
    dominio.append(depot);
}

typedef struct tVar{
    Variable data;
    struct tVar *next;
} tVar;


class ListaVariables{
    tVar *head;
    tVar *tail;
    tVar *curr;
    unsigned int listSize;
    unsigned int pos;
    ListaVehiculos vehiculosEnUso;
    ListaVehiculos mejorSolucion;

    //quiza se puede restringir el dominio de una variable en seguida
    //con clientesVisitados

    public:
        ListaVariables();
        void insertInFront(Variable item);
        void append(Variable vari); 
        void remove(unsigned int pos);
        void removeNext();
        Variable pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
        Variable getCurr();
        unsigned int getPos();
        unsigned int len();
        void goToPos(unsigned int pos);
        void free();
        ListaNodos clientesVisitados();
};

ListaVariables::ListaVariables(){
    head = tail = curr = (tVar*)malloc(sizeof(tVar)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
    clientesVisitados = ListaNodos();
    vehiculosEnUso = ListaNodos();
    mejorSolucion = ListaNodos();
}

void ListaVariables::insertInFront(Variable item){
    tVar *aux = curr->next;
    curr->next = (tVar*)malloc(sizeof(tVar));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
}

void ListaVariables::append(Variable vehi){
    moveToEnd();
    insertInFront(vehi);
    moveToStart();
}

void ListaVariables::remove(unsigned int pos){
    if(pos>listSize) return;
    goToPos(pos);
    prev();
    removeNext();
    moveToStart();
}

void ListaVariables::removeNext(){
    if(curr==tail) return;
    if(curr->next == tail) tail = curr;
    tVar *aux = curr->next;
    curr->next = curr->next->next;
    listSize--;
    std::free(aux);
}

Variable ListaVariables::pop(){
    Variable vehiAux;
    if(listSize==0) return vehiAux;
    moveToEnd();
    vehiAux = curr->data;
    prev();
    removeNext();
    moveToStart();
    return vehiAux;
}

void ListaVariables::moveToStart(){
    curr=head;
    pos=0;
}

void ListaVariables::moveToEnd(){
    curr=tail;
    pos=listSize;
}

void ListaVariables::prev(){
    tVar *temp;
    if (curr==head) return;
    temp = head;
    while (temp->next != curr) temp = temp->next;
    curr = temp;
    pos--;
}

void ListaVariables::next(){
    if (curr != tail){
        curr = curr->next; 
        pos++;
    } 
}

void ListaVariables::clear(){
    moveToStart();
    while(listSize>0){
        removeNext();
    }
}

Variable ListaVariables::getCurr() return curr->data;

/*
Variable ListaVariables::getVariable(unsigned int pos){
    //El head es posicion -1, el siguiente es posicion 0
    Variable varAux;
    if(pos>=listSize) return vehiAux;
    moveToStart();
    next();
    for(unsigned int i = 0;i<pos;i++){
        next();
    }
    varAux = curr->data;
    return varAux;
}*/

unsigned int ListaVariables::getPos(){return pos;}

unsigned int ListaVariables::len(){return listSize;}

void ListaVariables::goToPos(unsigned int pos){
    moveToStart();
    if(pos>=listSize) return;
    next();
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

void ListaVariables::free(){
    clear();
    std::free(head);
}

ListaNodos ListaVariables::clientesVisitados(){
    ListaNodos = ListaNodos();
    
}
