#define RADIO_TIERRA 4182.44949
#define MY_PI 3.14159265358979323846
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;


class Nodo{
    public:
        int ID;
        char tipo;
        double longitud;
        double latitud;

        Nodo();
        void mostrar();   
};

Nodo::Nodo(){
    ID = -1;
    tipo = '\0';
    longitud = 0.0;
    latitud = 0.0;
}

void Nodo::mostrar(){
    if(ID!=-1){
        cout << "\nID: " << ID;
        cout << "\nTipo: " << tipo;
        cout << "\nLongitud: " << longitud;
        cout << "\nLatitud: " << latitud << "\n";
    }
}


class ListaNodos{
    vector<Nodo> vect;
    vector<Nodo>::iterator ptr;

    public:
        ListaNodos();
        void setVect(vector<Nodo> vect1);
        void setPtr(vector<Nodo>::iterator ptr1);
        vector<Nodo> getVect();
        vector<Nodo>::iterator getPtr();
        void append(Nodo node);
        void remove(int pos);
        void pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void moveToPos(unsigned int pos);
        void clear();
        Nodo getCurr();
        Nodo getNodo(unsigned int pos);
        int find(Nodo node);
        unsigned int getPos();
        unsigned int len();
        void free();
        string to_string();
};


ListaNodos::ListaNodos(){
    vect;
    ptr = vect.begin();
}

void ListaNodos::setVect(vector<Nodo> vect1){vect = vect1;}

void ListaNodos::setPtr(vector<Nodo>::iterator ptr1){ptr = ptr1;}

vector<Nodo> ListaNodos::getVect(){return vect;}

vector<Nodo>::iterator ListaNodos::getPtr(){return ptr;}

void ListaNodos::append(Nodo node){
    if(len()==1) ptr = vect.begin();
    vect.push_back(node);
}

void ListaNodos::remove(int pos){
    ptr = vect.begin();
    advance(ptr,pos);
    vect.erase(ptr++);
}

void ListaNodos::pop(){
    vect.pop_back();
}

void ListaNodos::moveToStart(){
    ptr = vect.begin();
}

void ListaNodos::moveToEnd(){
    ptr = vect.end()-1;
}

void ListaNodos::prev(){
    int dist = distance(vect.begin(),ptr);
    ptr = vect.begin();
    for(int i=0;i<dist-1;i++) advance(ptr,1);
}

void ListaNodos::next(){
    if(ptr!=vect.end()-1)advance(ptr,1);
}

void ListaNodos::moveToPos(unsigned int pos){
    ptr = vect.begin();
    if(pos>len()) return;
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

void ListaNodos::clear(){
    vect.clear();
}


Nodo ListaNodos::getNodo(unsigned int pos){
    if(pos<len() && len()>0) return vect[pos];
    Nodo nodoAux;
    return nodoAux;
}

Nodo ListaNodos::getCurr(){
    if(len()!=0) return vect[distance(vect.begin(),ptr)];
    Nodo nodoAux;
    return nodoAux;
}

bool compararNodos(Nodo nodo1, Nodo nodo2);

int ListaNodos::find(Nodo node){
    for(int i = 0; i < len();i++){
        if(compararNodos(vect[i],node)){
            return i;
        }
    }
    return -1;
}

unsigned int ListaNodos::getPos(){return distance(vect.begin(),ptr);}

unsigned int ListaNodos::len(){return vect.size();}



void ListaNodos::free(){
    vect.clear();
}

string ListaNodos::to_string(){
    string output = "";
    moveToStart();
    if(len() != 0){
        for(unsigned int i=0;i<len()-1;i++){
            output += getNodo(i).tipo + std::to_string(getNodo(i).ID) + "-";
            next();
        }
        output += getNodo(len()-1).tipo + std::to_string(getNodo(len()-1).ID);
        while(output.length()<90){
            output += " ";
        }
    }
    else{
        output = "No hay nodos en la lista";
    }
    moveToStart();
    return output;
}

ListaNodos concatenar(ListaNodos lista1, ListaNodos lista2){
    //Con esto quedan los elementos de la lista1 seguidos de los de la lista2
    ListaNodos concatenacion = ListaNodos();
    if(lista1.len()>0 && lista2.len()>0){
        for(unsigned int i=0;i<lista1.len();i++){
            concatenacion.append(lista1.getNodo(i));
        }
        for(unsigned int i=0;i<lista2.len();i++){
            concatenacion.append(lista2.getNodo(i));
        }   
    }
    else if(lista1.len()>0){
        concatenacion = lista1;
    }
    else if(lista2.len()>0){
        concatenacion = lista2;
    }
    return concatenacion;
}

bool nodosIguales(Nodo nodo1, Nodo nodo2){
    if(nodo1.ID == nodo2.ID && nodo1.tipo == nodo2.tipo) return true;
    return false;
}


double aRadianes(double num){
    double rad = MY_PI * (num/180.0);
    return rad;
}

double semiVerseno(float num){
    return pow(sin(num/2.0),2);
}


double distanciaHaversine(double lon1, double lat1, double lon2, double lat2){
    double distancia = 0.0;

    double deltaLat = aRadianes(lat1 - lat2);
    double deltaLon = aRadianes(lon1 - lon2);

    double a = semiVerseno(deltaLat) + cos(aRadianes(lat1)) * cos(aRadianes(lat2)) * semiVerseno(deltaLon);
    
    double c = 2.0 * atan2(sqrt(a),sqrt(1-a));

    distancia = RADIO_TIERRA * c;

    return distancia;
}

double calcularDistancia(Nodo nodo1, Nodo nodo2){
    double distancia = 0.0;
    distancia = distanciaHaversine(nodo1.longitud,nodo1.latitud,
                                    nodo2.longitud,nodo2.latitud);
    return distancia;
}

Nodo nodoMenorDistancia(Nodo central, ListaNodos conjunto,int numEstaciones, vector<vector<double>> matrizDist){
    double menor = 999999999.9;
    Nodo menorNodo, nodoAux;
    double distancia = 0.0;
    for(unsigned int i=0; i<conjunto.len();i++){
        nodoAux = conjunto.getNodo(i);
        if(central.ID == nodoAux.ID && central.tipo == nodoAux.tipo) continue;
        
        int posMatriz1 = central.ID;
        if(central.tipo=='f') posMatriz1+=1;
        else if(central.tipo=='c') posMatriz1+=numEstaciones;

        int posMatriz2 = nodoAux.ID;
        if(nodoAux.tipo=='f') posMatriz2+=1;
        else if(nodoAux.tipo=='c') posMatriz2+=numEstaciones;

        distancia = matrizDist[posMatriz1][posMatriz2];
        if(distancia<menor){
            menor = distancia;
            menorNodo = nodoAux;
        }      
    }
    return menorNodo;
}

bool compararNodos(Nodo nodo1, Nodo nodo2){
    if(nodo1.ID == nodo2.ID && nodo1.tipo==nodo2.tipo) return true;
    return false;
}

bool compararListasNodos(ListaNodos list1, ListaNodos list2){
    bool respuesta = true;
    if (list1.len()!=list2.len()) return !respuesta;
    for(unsigned int i=0;i<list1.len();i++){
        if(!compararNodos(list1.getNodo(i),list2.getNodo(i))){
            respuesta = false;
        }
    }
    return respuesta;
}