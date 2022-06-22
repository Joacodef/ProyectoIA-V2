#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include <math.h>
#include "Funciones.h"

using namespace std;

void probando_concatenar();

int main() {
    
    ListaVariables vars = ListaVariables();

    Variable var1 = Variable();
    Variable var2 = Variable();
    Variable var3 = Variable();
    Variable var4 = Variable();


    Nodo nodo1 = Nodo();
    Nodo nodo2 = Nodo();
    Nodo nodo3 = Nodo();
    Nodo nodo4 = Nodo();

    nodo1.ID = 1;
    nodo2.ID = 2;
    nodo3.ID = 3;
    nodo4.ID = 4;

    var1.asignarNodo(nodo1);
    var2.asignarNodo(nodo2);
    var3.asignarNodo(nodo3);
    var4.asignarNodo(nodo4);

    vars.append(var1);
    vars.append(var2);
    vars.append(var3);
    vars.append(var4);

    vars.moveToEnd();
    cout << vars.getVariable(1).nodoAsignado.ID << "\n"; 
    cout << vars.len() << "\n"; 

    vars.printNodos();

    ListaVehiculos ve = vars.extraerSolucionActual();

    ve.moveToEnd();

    cout << "Largo lista vehiculos " << ve.len() << "\n";
    cout << "recorrido de vehiculo: " << ve.getVehiculo(0).recorrido.to_string() << "\n";

    return 0;
}

void probando_concatenar(){
    //probar la funcion concatenar, y depasada
    ListaNodos lista = ListaNodos();
    ListaNodos lista2 = ListaNodos();

    Nodo nodo1 = Nodo();
    Nodo nodo2 = Nodo();
    Nodo nodo3 = Nodo();

    nodo1.ID=1;
    nodo2.ID=2;
    nodo3.ID=3;
    nodo1.tipo='c';
    nodo2.tipo='d';
    nodo3.tipo='e';

    Nodo nodo4 = Nodo();
    Nodo nodo5 = Nodo();
    Nodo nodo6 = Nodo();

    nodo4.ID=4;
    nodo5.ID=5;
    nodo6.ID=6;
    nodo4.tipo='f';
    nodo5.tipo='g';
    nodo6.tipo='h';

    lista.append(nodo1);
    lista.append(nodo2);
    lista.append(nodo3);

    lista2.append(nodo4);
    lista2.append(nodo5);
    lista2.append(nodo6);

    ListaNodos lista3;

    lista3 = concatenar(lista,lista2);


    cout<<lista3.to_string()<<"\n";
    cout<<lista2.to_string()<<"\n";
    cout<<lista.to_string()<<"\n";

}


void probando_clearRemoveNextPrev(){
    ListaNodos lista = ListaNodos();

    Nodo nodo1 = Nodo();
    Nodo nodo2 = Nodo();
    Nodo nodo3 = Nodo();
    Nodo nodo4 = Nodo();
    Nodo nodo5 = Nodo();
    Nodo nodo6 = Nodo();


    nodo1.ID=1;
    nodo2.ID=2;
    nodo3.ID=3;
    nodo1.tipo='c';
    nodo2.tipo='d';
    nodo3.tipo='e';
    nodo4.ID=4;
    nodo5.ID=5;
    nodo6.ID=6;
    nodo4.tipo='f';
    nodo5.tipo='g';
    nodo6.tipo='h';

    
    lista.insertInFront(nodo6);
    lista.insertInFront(nodo5);
    lista.insertInFront(nodo4);
    lista.insertInFront(nodo3);
    lista.insertInFront(nodo2);
    lista.insertInFront(nodo1);

    lista.moveToStart();
    lista.next();
    lista.next();
    lista.next();
    lista.next();
    lista.next();
    lista.next();
    cout<<lista.len()<<"\n";

    //Estando en el tail, se debe hacer prev para eliminarla
    lista.prev();
    lista.removeNext();
    cout<<lista.len()<<"\n";

    lista.clear();
    cout<<lista.len()<<"\n";
}


void probando_appendPopGet(){
        ListaNodos lista = ListaNodos();

    Nodo nodo1 = Nodo();
    Nodo nodo2 = Nodo();
    Nodo nodo3 = Nodo();
    Nodo nodo4 = Nodo();
    Nodo nodo5 = Nodo();
    Nodo nodo6 = Nodo();


    nodo1.ID=1;
    nodo2.ID=2;
    nodo3.ID=3;
    nodo1.tipo='c';
    nodo2.tipo='d';
    nodo3.tipo='e';
    nodo4.ID=4;
    nodo5.ID=5;
    nodo6.ID=6;
    nodo4.tipo='f';
    nodo5.tipo='g';
    nodo6.tipo='h';

    
    lista.append(nodo6);
    lista.append(nodo5);
    lista.append(nodo4);
    lista.append(nodo3);
    lista.append(nodo2);
    lista.append(nodo1);


    Nodo nodoAux = lista.getNodo(4);

    cout<<"Nodo obtenido:"<<nodoAux.ID<<"\n";


    cout << lista.to_string() << "\n";

    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";
    lista.pop();
    cout << lista.to_string() << "\n";

}