#include <iostream>
#define RADIO_TIERRA 4182.44949

using namespace std;

//Clases y métodos relativas a los NODOS:

class Nodo{
    public:
        int ID;
        char tipo;
        double longitud;
        double latitud;
        Nodo *next;

        Nodo();
        void mostrar();   
};

Nodo::Nodo(){
    ID = -1;
    tipo = '\0';
    longitud = 0.0;
    latitud = 0.0;
    //restringidoTemp = false;
}

void Nodo::mostrar(){
    cout << "\nID: " << ID;
    cout << "\nTipo: " << tipo;
    cout << "\nLongitud: " << longitud;
    cout << "\nLatitud: " << latitud << "\n";
}

typedef struct tPaso{
    Nodo data;
    struct tPaso *next;
} tPaso;

//ListaNodos es una lista enlazada

class ListaNodos{
    tPaso *head;
    tPaso *tail;
    tPaso *curr;
    unsigned int listSize;
    unsigned int pos;

    public:
        ListaNodos();
        void insertInFront(Nodo item); //insertar en pos actual
        void append(Nodo node);
        void remove(int pos);
        void removeNext();
        Nodo pop();
        void moveToStart();
        void moveToEnd();
        void prev();
        void next();
        void clear();
        Nodo getNodo(unsigned int pos);
        Nodo getCurr();
        int find(Nodo node);
        unsigned int getPos();
        unsigned int len();
        void goToPos(unsigned int pos);
        void free();
        string to_string();
};

ListaNodos::ListaNodos(){
    head = tail = curr = (tPaso*)malloc(sizeof(tPaso)); // Siempre es la cabecera
    listSize = 0;
    pos = 0;
}

void ListaNodos::insertInFront(Nodo item){
    tPaso *aux = curr->next;
    curr->next = (tPaso*)malloc(sizeof(tPaso));
    curr->next->data = item;
    curr->next->next = aux;
    if(curr == tail) tail = curr->next;
    listSize++;
}

void ListaNodos::append(Nodo node){
    moveToEnd();
    insertInFront(node);
    moveToStart();
}

void ListaNodos::remove(int pos){
    if(pos<0) return;
    if(abs(pos)>listSize) return;
    goToPos(pos);
    prev();
    removeNext();
    moveToStart();
}

void ListaNodos::removeNext(){
    if(curr==tail) return;
    if(curr->next == tail) tail = curr;
    tPaso *aux = curr->next;
    curr->next = curr->next->next;
    listSize--;
    std::free(aux);
}

Nodo ListaNodos::pop(){
    Nodo nodoAux;
    if(listSize==0) return nodoAux;
    moveToEnd();
    nodoAux = curr->data;
    prev();
    removeNext();
    moveToStart();
    return nodoAux;
}

void ListaNodos::moveToStart(){
    curr=head;
    pos=0;
}

void ListaNodos::moveToEnd(){
    curr=tail;
    pos=listSize;
}

void ListaNodos::prev(){
    tPaso *temp;
    if (curr==head) return;
    temp = head;
    while (temp->next != curr) temp = temp->next;
    curr = temp;
    pos--;
}

void ListaNodos::next(){
    if (curr != tail) {
        curr = curr->next;
        pos++;
    }
}

void ListaNodos::clear(){
    if(listSize!=0){
        moveToStart();
        while(listSize>0){
            removeNext();
        }
    }
}

Nodo ListaNodos::getNodo(unsigned int pos){
    //El head es posicion -1, el siguiente es posicion 0
    Nodo nodoAux;
    if(pos>=listSize) return nodoAux;
    moveToStart();
    next();
    for(unsigned int i = 0;i<pos;i++){
        next();
    }
    nodoAux = curr->data;
    return nodoAux;
}

Nodo ListaNodos::getCurr(){
    return curr->data;
}

int ListaNodos::find(Nodo node){
    if(listSize==0) return -1;
    for(unsigned int i=0;i<listSize;i++){
        if(node.ID == getNodo(i).ID && node.tipo == getNodo(i).tipo){
            return i;
        }
    }
    return -1;
}

unsigned int ListaNodos::getPos(){return pos;}

unsigned int ListaNodos::len(){return listSize;}

void ListaNodos::goToPos(unsigned int pos){
    moveToStart();
    if(pos>=listSize) return;
    next();
    for(unsigned int i=0;i<pos;i++){
        next();
    }
}

void ListaNodos::free(){
    clear();
    std::free(head);
}

string ListaNodos::to_string(){
    string output = "\n";
    moveToStart();
    next();
    if(listSize != 0){
        for(unsigned int i=0;i<listSize-1;i++){
            output += std::to_string(curr->data.ID) + curr->data.tipo + "-";
            next();
        }
        output += std::to_string(curr->data.ID) + curr->data.tipo;
        while(output.length()<50){
            output += " ";
        }
    }
    else{
        output = "No hay nodos en la lista";
    }
    moveToStart();
    return output;
}

// OTRAS FUNCIONES RELATIVAS A NODOS:

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

//Funciones para calcular distancia entre nodos:

double aRadianes(double num){
    double rad = 0.0;
    rad = M_PI * (num/180.0);
    return rad;
}

double haversine(float num){
    return pow(sin(num/2.0),2);
}


double distanciaHaversine(double lon1, double lat1, double lon2, double lat2){
    double distancia = 0.0;

    double deltaLat = aRadianes(lat1 - lat2);
    double deltaLon = aRadianes(lon1 - lon2);

    double a = haversine(deltaLat) + cos(aRadianes(lat1)) * cos(aRadianes(lat2)) * haversine(deltaLon);
    
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



Nodo nodoMenorDistancia(Nodo actual, ListaNodos dominio, double *distPtr){
    double menor = 999999999.9;
    Nodo menorNodo;
    double distancia = 0.0;
    for(int i=0; i<dominio.len();i++){
        if(actual.ID == dominio.getNodo(i).ID && actual.tipo == dominio.getNodo(i).tipo) continue;
        distancia = calcularDistancia(dominio.getNodo(i),actual);
        if(distancia<menor){
            menor = distancia;
            *distPtr = distancia;
            menorNodo = dominio.getNodo(i);
        }      
    }
    return menorNodo;
}


/*
//Funcion para calcular todas las distancias desde un nodo central a un conjunto de nodos.

double *calcularTodasDistancias(Nodo centro, Nodo *demasNodos, int size){
    double *distancias = (double*)malloc(sizeof(double)*size);
    for(int i=0; i<size;i++){
        distancias[i] = 0.0;
        distancias[i] = calcularDistancia(centro.longitud,centro.latitud,
                                    demasNodos[i].longitud,demasNodos[i].latitud);
    }
    return distancias;
}
*/

//Funcion que chequea si el nodo "actual" está en el arreglo de "nodosRestringidos".
/*
bool nodoRestringido(Nodo actual,ListaNodos nodosRestringidos){
    nodosRestringidos.moveToStart();
    nodosRestringidos.next();
    if(actual.ID == -1) return true;
    if(nodosRestringidos.len() == 0) return false;
    //if(actual.restringidoTemp) return true;
    for(unsigned int i = 0;i<nodosRestringidos.len()+1;i++){
        if(actual.ID == nodosRestringidos.curr->data.ID && actual.tipo == nodosRestringidos.curr->data.tipo){
            return true;
        }
        nodosRestringidos.next();
    }
    return false;
}*/