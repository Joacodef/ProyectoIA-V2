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
        //o se asigno porque antes habia un depot, o no se cumplian restricciones
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
    for(int i = dominio.len()-1; i>=0;i--){
        if(dominio.getNodo(i).tipo == 'c'){
            dominio.remove(i);
        }
    }
    domSinClientes = dominio;
    return domSinClientes;
}

ListaNodos Variable::quitarEstacionesDominio(){
    ListaNodos domSinEstaciones;
    for(int i = dominio.len()-1; i>=0;i--){
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
        void append(Variable vari); 
        void remove(unsigned int pos);
        void pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void moveToPos(unsigned int pos);
        void clear();
        Variable getVariable(unsigned int pos);
        Variable getCurr();
        Variable getLast();
        unsigned int getPos();
        unsigned int len();
        void free();
        ListaNodos clientesVisitados();
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
    ptr = vect.end()-1;
}

void ListaVariables::prev(){
    int dist = distance(vect.begin(),ptr);
    ptr = vect.begin();
    for(int i=0;i<dist-1;i++) advance(ptr,1);
}

void ListaVariables::next(){
    if(ptr!=vect.end())advance(ptr,1);
}

void ListaVariables::moveToPos(unsigned int pos){
    ptr = vect.begin();
    if(pos>len()) return;
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

Variable ListaVariables::getVariable(unsigned int pos){
    if(pos<len() && len()>0) return vect[pos];
    Variable varAux;
    return varAux;
}

Variable ListaVariables::getCurr(){
    if(len()!=0) return vect[distance(vect.begin(),ptr)];
    Variable varAux;
    return varAux;
}

Variable ListaVariables::getLast(){
    if(len()!=0){
        moveToEnd();
    }
    return getCurr();
}


unsigned int ListaVariables::getPos(){return distance(vect.begin(),ptr);}

unsigned int ListaVariables::len(){return vect.size();}



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
        moveToPos(posAux+largo);
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
    if(pos!=-1)moveToPos(abs(pos));
    else{ 
        return vehi;
    }
    if(len()==1){vehi.agregarParada(var.nodoAsignado);return vehi;}
    if(pos!=0) anterior = getVariable(pos-1);
    if(pos!=len()-1) siguiente = getVariable(pos+1);
    if(var.nodoAsignado.tipo=='d'){
        //Si estamos en un depot debemos determinar si hay que avanzar o retroceder
        //Comprobar si se está al final de la lista o detras de un depot(estamos al final de un recorrido)
        if((abs(pos)==len()-1 || siguiente.nodoAsignado.tipo=='d') && getPos() > 0){
            prev();
            while(getCurr().nodoAsignado.tipo!='d'){
                //Retroceder hasta llegar a un depot, para que la lista esté ordenada
                prev();
            }
            //agregar depot
            vehi.agregarParada(getCurr().nodoAsignado);
            do{// luego avanzar hasta llegar al depot y agregarlo
                next();
                vehi.agregarParada(getCurr().nodoAsignado);
            }while(getCurr().nodoAsignado.tipo!='d'&& getPos()!=len()-1  );             
        }//Comprobar si se está al principio de la lista o el anterior es un depot (estamos en el incio de un recorrido)
        else if(getPos()==0 || anterior.nodoAsignado.tipo=='d'){
            vehi.agregarParada(getCurr().nodoAsignado); 
            do{//avanzar agregando paradas
                next();
                vehi.agregarParada(getCurr().nodoAsignado);                    
            }while(getCurr().nodoAsignado.tipo!='d'&& getPos()!=len()-1 );  
        }
    }
    else{//Si no estamos en un depot, se retrocede hasta llegar a uno
        while(getCurr().nodoAsignado.tipo!='d'){
            prev();
        }
        //agregar depot
        vehi.agregarParada(getCurr().nodoAsignado);
        do{//avanzar hasta llegar a un depot o hasta el final
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
