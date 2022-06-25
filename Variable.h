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
        Variable(ListaNodos dominioRestringido); //para Backtracking
        void eliminarClientesDominio();
        void eliminarNodosMasLejosDepot(double distancia);
        void resetearDominio(ListaNodos clientes, ListaNodos estaciones);
        void quitarDelDominio(Nodo node);
        bool dominioVacio();
        void asignarNodo(Nodo node);
        ListaNodos dominioSoloClientes();
        bool dominioTieneCliente();
        ListaNodos quitarClientesDominio();
        ListaNodos quitarEstacionesDominio();
        void free();
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

Variable::Variable(ListaNodos dominioRestringido){
    ID = id_actual++;
    nodoAsignado = Nodo();
    dominio = dominioRestringido;
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
    ListaNodos domCli;
    Nodo nodoAux;
    if(dominio.len()<1) return domCli;
    for(unsigned int i=0;i<dominio.len();i++){
        nodoAux = dominio.getNodo(i);
        if(nodoAux.tipo=='c'){
            domCli.append(nodoAux);
        }
    }
    return domCli;
}

bool Variable::dominioTieneCliente(){
    bool tieneCliente=false;
    Nodo nodoAux;
    if(dominio.len()<1) return tieneCliente;
    for(unsigned int i=0;i<dominio.len();i++){
        nodoAux = dominio.getNodo(i);
        if(nodoAux.tipo=='c'){
            return true;
        }
    }
    return tieneCliente;
}

ListaNodos Variable::quitarClientesDominio(){
    ListaNodos domSinClientes;
    for(unsigned int i = dominio.len()-1; i>=0;i--){
        if(dominio.getNodo(i).tipo == 'c'){
            dominio.remove(i);
        }
    }
    domSinClientes = dominio;
    return domSinClientes;
}

ListaNodos Variable::quitarEstacionesDominio(){
    ListaNodos domSinEstaciones;
    for(unsigned int i = dominio.len()-1; i>=0;i--){
        if(dominio.getNodo(i).tipo == 'f'){
            dominio.remove(i);
        }
    }
    domSinEstaciones = dominio;
    return domSinEstaciones;
}

void Variable::free(){
    delete(&nodoAsignado);
    dominio.free();
}

typedef struct tVar{
    Variable data;
    struct tVar *next;
} tVar;


class ListaVariables{
    vector<Variable> vect;
    vector<Variable>::iterator ptr;

    public:
        ListaVariables();
        void insertInFront(Variable item);
        void append(Variable vari); 
        void remove(unsigned int pos);
        void pop();
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
        Variable getVariable(unsigned int pos);
        void printNodos();
        int find(Variable var);
        ListaVehiculos extraerSolucionActual(double velocidad, int tiempoRecarga, int tiempoServicio);
        Vehiculo recorridoDeVariable(Variable var, double velocidad, 
                            int tiempoServicio, int tiempoRecarga);
};

ListaVariables::ListaVariables(){
    vect;
    ptr = vect.begin();
}

void ListaVariables::append(Variable var){
    if(len()==1) vect.begin();
    vect.push_back(var);
}

void ListaVariables::remove(unsigned int pos){
    ptr = vect.begin();
    advance(ptr,pos);
    vect.erase(ptr++);
}

void ListaVariables::pop(){
    vect.pop_back();
}

void ListaVariables::moveToStart(){
    ptr = vect.begin();
}

void ListaVariables::moveToEnd(){
    ptr = vect.end();
}

void ListaVariables::prev(){
    int dist = distance(vect.begin(),ptr);
    ptr = vect.begin();
    for(int i=0;i<dist;i++) advance(ptr,1);
}

void ListaVariables::next(){
    if(ptr!=vect.end())advance(ptr,1);
}

void ListaVariables::clear(){
    vect.clear();
}

Variable ListaVariables::getCurr(){
    Variable varAux;
    if(len()==0) return varAux;
    else varAux = vect[distance(vect.begin(),ptr)];
    return varAux;
}


Variable ListaVariables::getVariable(unsigned int pos){
    Variable varAux;
    if(pos<0 || pos>len()-1) return varAux;
    return vect[pos];
}

unsigned int ListaVariables::getPos(){return distance(vect.begin(),ptr);}

unsigned int ListaVariables::len(){return vect.size();}

void ListaVariables::goToPos(unsigned int pos){
    ptr = vect.begin();
    if(pos>len()) return;
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

void ListaVariables::free(){
    for (unsigned int i = 0; i < vect.size(); i++){
        vect[i].dominio.free();
    }
    vect.clear();
}

ListaNodos ListaVariables::clientesVisitados(){
    ListaNodos clientes = ListaNodos();
    if(len()<1) {
        return clientes;
    }
    for(unsigned int i = 0;i<len();i++){
        if(getVariable(i).nodoAsignado.tipo=='c'){
            clientes.append(getVariable(i).nodoAsignado);
        }
    }
    return clientes;
}

ListaVehiculos ListaVariables::extraerSolucionActual(double velocidad, int tiempoServicio,int tiempoRecarga){
    ListaVehiculos sol = ListaVehiculos();
    Vehiculo vehiAux;
    moveToStart();
    unsigned int largo = 0;
    unsigned int posAux = 0;

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
    for(unsigned int i=0;i<len();i++){
        varAux = getVariable(i);
        cout << varAux.nodoAsignado.ID << varAux.nodoAsignado.tipo << "-";
    }
    cout << "\n";
}

Vehiculo ListaVariables::recorridoDeVariable(Variable var, double velocidad, int tiempoServicio, int tiempoRecarga){
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

        if((abs(pos)==len() || siguiente.nodoAsignado.tipo=='d') && getPos() > 0){
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
        else if(getPos()==0 || anterior.nodoAsignado.tipo=='d'){
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
        }while(getCurr().nodoAsignado.tipo!='d' && getPos()!=len()-1 );
    }
    return vehi;
}

int ListaVariables::find(Variable var){
    if(len()==0) return -1;
    for(unsigned int i=0;i<len();i++){
        if(var.ID == getVariable(i).ID){
            return i;
        }
    }
    return -1;
}
