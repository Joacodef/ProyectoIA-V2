#include <iostream>
#include <fstream>
#include <cstring>
#include <list>
#include <bits/stdc++.h>
#include <math.h>
#include <string.h>
#include "GeneradorSoluciones.h"
#define MY_PI 3.14159265358979323846

using namespace std;

int Variable::id_actual = 1;
int Vehiculo::id_actual = 1;

ListaVariables casoPrueba(ListaNodos nodes, Nodo depot, ListaNodos clientes, ListaNodos estaciones);

int main() {
    /********Extraer datos de archivo*******/
    ListaVariables vars;
    Instancia inst = Instancia();
    Nodo depot = Nodo();
    ListaNodos nodos = ListaNodos();
    ListaNodos estaciones = ListaNodos();
    ListaNodos clientes = ListaNodos();
    inst = extraerArchivo(&nodos,"AB101");
    depot = nodos.getNodo(1);
    for(int i=0;i<inst.numEstaciones;i++){
        estaciones.append(nodos.getNodo(i+1));                
    }
    for(int i=0;i<inst.numClientes;i++){
        clientes.append(nodos.getNodo(i+inst.numEstaciones+1));                
    }
    vars = casoPrueba(nodos, depot, clientes, estaciones);

    /***Probar cosas***/
    ListaVehiculos lv;
    //vars.getVariable(0).nodoAsignado.mostrar();

    Vehiculo vehi = vars.recorridoDeVariable(vars.getVariable(2),inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);

    vehi.velocidad = inst.velocidad;
    vehi.tiempoRecarga = inst.tiempoRecarga;
    vehi.tiempoServicio = inst.tiempoServicio;

    //cout<< <<"\n";
    //vehi.recorrido.to_string()

    return 0;
}


ListaVariables casoPrueba(ListaNodos nodes, Nodo depot, ListaNodos clientes, ListaNodos estaciones){  
    ListaVariables vars;
    Variable variables[60];
    Nodo nodoAux, nodoAux2;
    string casoDePrueba = "00d-37c-08c-09c-32c-12c-16c-17c-41c-06c-18c-30c-00d-";
    casoDePrueba+="00d-19c-50c-22c-21c-45c-23c-29c-44c-39c-11c-38c-20c-42c-40c-33c-02c-34c-13c-14c-00d-";
    casoDePrueba+="00d-15c-05c-04c-46c-24c-25c-43c-03c-00d-";
    casoDePrueba+="00d-28c-10c-26c-49c-27c-48c-36c-35c-47c-07c-00d-";
    casoDePrueba+="00d-01c-31c-00d-";
    for(int i = 0; i < 60; i++){
        string ID = casoDePrueba.substr(i*4,2);
        string tipo = casoDePrueba.substr(i*4+2,1);
        nodoAux.ID=stoi(ID);
        nodoAux.tipo=tipo[0];
        for(int j=0; j<nodes.len();j++){
            nodoAux2 = nodes.getNodo(j);
            if(compararNodos(nodoAux,nodoAux2)){
                variables[i].asignarNodo(nodoAux2);
                break;
            }
        }
    }
    for(int i = 0; i < 60; i++)vars.append(variables[i]);

    return vars;
}



