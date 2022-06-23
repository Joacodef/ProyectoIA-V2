#include "Vehiculo.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

class Variable{
    public:
        int ID;
        Nodo nodoAsignado;
        ListaNodos dominio;
        static int id_actual;

        Variable();
        Variable(ListaNodos clientes, ListaNodos estaciones);
        void eliminarClientesDominio();
        void eliminarNodosMasLejosDepot(double distancia);
        void resetearDominio(ListaNodos clientes, ListaNodos estaciones);
        void quitarDelDominio(Nodo node);
        bool dominioVacio();
        void asignarNodo(Nodo node);
        ListaNodos dominioSoloClientes();
};

Variable::Variable(){
    ID = id_actual++;
    nodoAsignado = Nodo();
    dominio = ListaNodos();
}

Variable::Variable(ListaNodos clientes, ListaNodos estaciones){
    ID = id_actual++;
    nodoAsignado = Nodo();
    dominio = concatenar(clientes,estaciones);
}

void Variable::resetearDominio(ListaNodos clientes, ListaNodos estaciones){
    dominio = concatenar(clientes,estaciones);
}

void Variable::quitarDelDominio(Nodo node){
    if(dominio.len() < 1) return;
    int pos = dominio.find(node);
    if(pos != -1){
        dominio.remove(pos);
    }
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
        void printNodos();
        int find(Variable var);
        ListaVehiculos extraerSolucionActual(double velocidad, int tiempoRecarga, int tiempoServicio);
        Vehiculo recorridoDeVariable(Variable var, double velocidad, 
                            int tiempoServicio, int tiempoRecarga);
};

ListaVariables::ListaVariables(){
    head = tail = curr = (tVar*)malloc(sizeof(tVar)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
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
    insertInFront(var);
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
    goToPos(pos);
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

ListaVehiculos ListaVariables::extraerSolucionActual(double velocidad, int tiempoServicio,int tiempoRecarga){
    ListaVehiculos sol = ListaVehiculos();
    Vehiculo vehiAux;
    moveToStart();
    unsigned int largo = 0;
    unsigned int posAux = 0;
    next();

    while(posAux+largo<len()){
        posAux = getPos();
        vehiAux = recorridoDeVariable(getCurr(), velocidad, tiempoServicio, tiempoRecarga);
        sol.append(vehiAux);
        largo = vehiAux.recorrido.len();
        goToPos(posAux+largo);
    }
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

Vehiculo ListaVariables::recorridoDeVariable(Variable var, double velocidad, int tiempoServicio, int tiempoRecarga){
    double dist = 0.0;
    Vehiculo vehi = Vehiculo(velocidad, tiempoServicio, tiempoRecarga);
    Variable siguiente;
    Variable anterior;
    int pos = find(var);
    if(pos!=-1)goToPos(abs(pos));
    else{ 
        return vehi;
    }

    if(var.nodoAsignado.tipo=='d'){
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
                prev();
            }
            //agregar depot
            vehi.agregarParada(getCurr().nodoAsignado);
            do{//avanzar hasta llegar a un depot y agregarlo
                next();
                vehi.agregarParada(getCurr().nodoAsignado);
            }while(getCurr().nodoAsignado.tipo!='d'&& getPos()!=len()  );             
        }
        else if(getPos()==1 || anterior.nodoAsignado.tipo=='d'){
            vehi.agregarParada(getCurr().nodoAsignado); 
            do{//avanzar agregando paradas
                next();
                vehi.agregarParada(getCurr().nodoAsignado);                    
            }while(getCurr().nodoAsignado.tipo!='d'&& getPos()!=len() );  
        }
    }
    else{
        while(getCurr().nodoAsignado.tipo!='d'){
            //Retroceder hasta llegar a un depot
            prev();
        }
        //agregar depot
        vehi.agregarParada(getCurr().nodoAsignado);
        do{//avanzar hasta llegar a un depot y agregarlo
            next();
            vehi.agregarParada(getCurr().nodoAsignado);            
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