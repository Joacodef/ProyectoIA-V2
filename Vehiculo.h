#include <iostream>
#include "Nodo.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

// Clases y métodos relativas a vehículos:

class Vehiculo{
    public:
        int ID;
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
    srand (time(NULL));
    ID = rand() % 99999999 + 1;
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
}

Vehiculo::Vehiculo(Nodo depot){
    srand (time(NULL));
    ID = rand() % 99999999 + 1;
    tiempoTranscurrido = 0;
    distanciaTotalRecorrida = 0.0;
    distanciaDesdeRecarga = 0.0;
    cantClientesVisitados = 0;
    recorrido.append(depot);
}

void Vehiculo::agregarParada(Nodo nodo, double velocidad, double distancia, 
                            int tiempoServicio, int tiempoRecarga){
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
        Vehiculo getCurr();
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
        distancia += getVehiculo(i+1).distanciaTotalRecorrida;
    }
    return distancia;
}

Vehiculo ListaVehiculos::getCurr(){
    return curr->data;
}

// Clase que se asocia al concepto de variable en la representación

class Variable{
    public:
        int ID;
        Nodo nodoAsignado;
        Vehiculo vehiculo;
        ListaNodos dominio;
        static int id_actual;

        Variable();
        Variable(ListaNodos clientes, ListaNodos estaciones, Nodo depot);
        void eliminarClientesDominio();
        void eliminarNodosMasLejosDepot(double distancia);
        void resetearDominio(ListaNodos clientes, ListaNodos estaciones, Nodo depot);
        void quitarDelDominio(Nodo node);
        void asignarVehiculo(Vehiculo vehi);
        bool dominioVacio();
        void asignarNodo(Nodo node);
        ListaNodos dominioSoloClientes();
};

Variable::Variable(){
    ID = id_actual++;
    nodoAsignado = Nodo();
    vehiculo = Vehiculo();
    dominio = ListaNodos();
}

Variable::Variable(ListaNodos clientes, ListaNodos estaciones, Nodo depot){
    id_actual++;
    nodoAsignado = Nodo();
    vehiculo = Vehiculo();
    dominio = concatenar(clientes,estaciones);
    dominio.append(depot);
}

void Variable::resetearDominio(ListaNodos clientes, ListaNodos estaciones, Nodo depot){
    dominio = concatenar(clientes,estaciones);
    dominio.append(depot);
}

void Variable::quitarDelDominio(Nodo node){
    if(dominio.len() < 1) return;
    int pos = dominio.find(node);
    if(pos != -1){
        dominio.remove(pos);
    }
}

void Variable::asignarVehiculo(Vehiculo vehi){
    vehiculo = vehi;
}

bool Variable::dominioVacio(){
    if(dominio.len()>0) return false;
    else return true;
}

void Variable::asignarNodo(Nodo node){
    if(node.tipo=='d'){
        //si se asigna depot es porque no hay otra alternativa
        dominio.clear();
    }
    nodoAsignado=node;
}

ListaNodos Variable::dominioSoloClientes(){
    ListaNodos domCli = ListaNodos();
    if(dominio.len()<1) return domCli;
    for(unsigned int i=0;i<dominio.len();i++){
        if(dominio.getNodo(i+1).tipo=='c'){
            domCli.append(dominio.getNodo(i+1));
        }
    }
    return domCli;
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
    //ListaVehiculos vehiculosEnUso;
    

    public:
        ListaVehiculos mejorSolucion;
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
        //void agregarVehiculo(Vehiculo vehi);
        Variable getVariable(unsigned int pos);
        ListaVehiculos extraerSolucionActual();
        void printNodos();
        int find(Variable var);
        Vehiculo rutaActual(Variable var, double velocidad, 
                            int tiempoServicio, int tiempoRecarga);
};

ListaVariables::ListaVariables(){
    head = tail = curr = (tVar*)malloc(sizeof(tVar)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
    //vehiculosEnUso = ListaVehiculos();
    mejorSolucion = ListaVehiculos();
}

void ListaVariables::insertInFront(Variable item){
    tVar *aux = curr->next;
    curr->next = (tVar*)malloc(sizeof(tVar));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
}

void ListaVariables::append(Variable var){
    moveToEnd();
    Variable aux = Variable();
    aux.ID = var.ID;
    aux.nodoAsignado = var.nodoAsignado;
    aux.vehiculo = var.vehiculo;
    aux.dominio = var.dominio;
    insertInFront(aux);
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

Variable ListaVariables::getCurr(){
    return curr->data;
}


Variable ListaVariables::getVariable(unsigned int pos){
    Variable varAux;
    if(pos>listSize) return varAux;
    moveToStart();
    for(unsigned int i = 0;i<pos;i++){
        next();
    }
    varAux = curr->data;
    return varAux;
}

unsigned int ListaVariables::getPos(){return pos;}

unsigned int ListaVariables::len(){return listSize;}

void ListaVariables::goToPos(unsigned int pos){
    moveToStart();
    if(pos>listSize) return;
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

void ListaVariables::free(){
    clear();
    std::free(head);
}

ListaNodos ListaVariables::clientesVisitados(){
    ListaNodos clientes = ListaNodos();
    if(listSize<1) {
        return clientes;
    }
    for(unsigned int i = 0;i<listSize;i++){
        clientes.append(getVariable(i+1).nodoAsignado);
    }
    return clientes;
}

ListaVehiculos ListaVariables::extraerSolucionActual(){
    ListaVehiculos sol = ListaVehiculos();
    moveToStart();
    Variable varAux;
    Vehiculo vehiAux;
    int contadorDepots = 0;
    for(unsigned int i = 0; i<listSize;i++){
        varAux = getVariable(i+1);
        if(varAux.nodoAsignado.tipo=='d'){
            contadorDepots++;
            if(i==0){
                vehiAux = Vehiculo();
                vehiAux.recorrido.append(varAux.nodoAsignado);
            }
            else if(contadorDepots == 2){
                vehiAux.recorrido.append(varAux.nodoAsignado);
                sol.append(vehiAux);
                contadorDepots = 0;
                vehiAux = Vehiculo();
            }
            else{
                vehiAux.recorrido.append(varAux.nodoAsignado);
            }
        }
        else{
            vehiAux.recorrido.append(varAux.nodoAsignado);
        }

    }
    if(vehiAux.recorrido.len()!=0) sol.append(vehiAux);
    return sol;
}

void ListaVariables::printNodos(){
    Variable varAux;
    for(unsigned int i=0;i<listSize;i++){
        varAux = getVariable(i+1);
        cout << varAux.nodoAsignado.ID << varAux.nodoAsignado.tipo << "-";
    }
    cout << "\n";
}

Vehiculo ListaVariables::rutaActual(Variable var, double velocidad, 
                            int tiempoServicio, int tiempoRecarga){

    Vehiculo vehi = Vehiculo();
    double dist = 0.0;
    Nodo nodoAux;
    Variable siguiente;
    Variable anterior;
    int pos = find(var);
    //cout<<pos<<"\n";
    if(pos!=-1)goToPos(abs(pos));
    else{ 
        return vehi;
    }


    if(getCurr().nodoAsignado.tipo=='d'){
        //si estamos en un depot debemos determinar si hay que avanzar o retroceder
        
        siguiente = getVariable(pos+1);
        goToPos(abs(pos));
        anterior = getVariable(pos-1);
        goToPos(abs(pos));

        if((pos==listSize || siguiente.nodoAsignado.tipo=='d') && getPos() > 1){
             //Retroceder hasta llegar a un depot 
            
            prev();
            while(getCurr().nodoAsignado.tipo!='d'){
                //Retroceder hasta llegar a un depot
                nodoAux = getCurr().nodoAsignado;
                prev();
            }
            //agregar depot
            dist = calcularDistancia(nodoAux,getCurr().nodoAsignado);
            vehi.agregarParada(getCurr().nodoAsignado,0.0,0.0,0,0);
            do{//avanzar hasta llegar a un depot y agregarlo
                nodoAux = getCurr().nodoAsignado;
                next();
                dist = calcularDistancia(nodoAux,getCurr().nodoAsignado);
                vehi.agregarParada(getCurr().nodoAsignado,velocidad,dist,
                                                tiempoServicio,tiempoRecarga);
            }while(getCurr().nodoAsignado.tipo!='d'&& getPos()!=len()  );             
        }
        else if(getPos()==1 || anterior.nodoAsignado.tipo=='d'){
            //cout<<"HOLA\n";
            vehi.agregarParada(getCurr().nodoAsignado,0.0,0.0,0,0); 
            do{//avanzar agregando paradas
                nodoAux = getCurr().nodoAsignado;
                next();
                dist = calcularDistancia(nodoAux,getCurr().nodoAsignado);
                vehi.agregarParada(getCurr().nodoAsignado,velocidad,dist,
                                                tiempoServicio,tiempoRecarga);                    
            }while(getCurr().nodoAsignado.tipo!='d'&& getPos()!=len() );  
            
        }
        
    }
    else{
        while(getCurr().nodoAsignado.tipo!='d'){
            //Retroceder hasta llegar a un depot
            nodoAux = getCurr().nodoAsignado;
            prev();
        }
        
        //agregar depot
        dist = calcularDistancia(nodoAux,getCurr().nodoAsignado);
        vehi.agregarParada(getCurr().nodoAsignado,0.0,0.0,0,0);
        do{//avanzar hasta llegar a un depot y agregarlo
            nodoAux = getCurr().nodoAsignado;
            next();
            dist = calcularDistancia(nodoAux,getCurr().nodoAsignado);
            vehi.agregarParada(getCurr().nodoAsignado,velocidad,dist,
                                            tiempoServicio,tiempoRecarga);
            
        }while(getCurr().nodoAsignado.tipo!='d' && getPos()!=len() );
    }

    return vehi;
}


int ListaVariables::find(Variable var){
    if(len()==0) return -1;
    for(unsigned int i=0;i<len();i++){
        if(var.ID == getVariable(i+1).ID){
            return i+1;
        }
    }
    return -1;
}


/*
void ListaVariables::agregarVehiculo(Vehiculo vehi){
    for(unsigned int i; i<vehiculosEnUso.len(); i++){
        if(vehiculosEnUso.getVehiculo(i).ID == vehi.ID){
            return;
        }
    }
    vehiculosEnUso.append(vehi);
}

   ListaVehiculos sol = ListaVehiculos();
    Variable varAux = Variable();
    Vehiculo vehiAux = Vehiculo();
    for(unsigned int i=0;i<listSize;i++){
        varAux = getVariable(i+1);
        //cout << varAux.nodoAsignado.ID<<"\n";
        //Los vehiculos se repiten entre variables, por lo tanto
        //se debe buscar los vehiculos distintos que hayan en la solucion
        if(varAux.vehiculo.ID != sol.getCurr().ID){
            if(vehiAux.recorrido.len() > 0) sol.append(vehiAux);
            vehiAux = varAux.vehiculo;
            vehiAux.recorrido.append(varAux.nodoAsignado);
            //cout << varAux.nodoAsignado.ID<<"\n";
        }
        else{
            vehiAux.recorrido.append(varAux.nodoAsignado);
            //cout << varAux.nodoAsignado.ID<<"\n";
        }        
    }
    if(vehiAux.recorrido.len() > 0) sol.append(vehiAux);
    
    for(unsigned int i=0;i<sol.len();i++){
            cout << sol.getVehiculo(i).recorrido.to_string()<<"\n";;
        }*/
