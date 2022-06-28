#include <iostream>
#include <fstream>
#include <cstring>
#include <list>
#include <bits/stdc++.h>
#include <math.h>
#include <string.h>
#include <chrono>
#include <map>
#include "GeneradorSoluciones.h"
#define MY_PI 3.14159265358979323846

using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

int Variable::id_actual = 1;
int Vehiculo::id_actual = 1;

ListaVariables casoPrueba(ListaNodos nodes, Nodo depot, ListaNodos clientes, ListaNodos estaciones);



int main(){
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
    vector<vector<double>> matriz = matrizDistancias(nodos);
    auto t1 = high_resolution_clock::now();

    /*****PROBAR ALGO AQUÍ*****/
    
    
    Nodo nodoAux = clientes.getNodo(25);
    
    for(int i = 0; i<100000;i++){
        //calcularDistancia(nodoAux,depot);
        
    }

    /**************************/

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    duration<double, std::milli> ms_double = t2 - t1;
    double tiempoEjecucion = ms_double.count();
    cout<<"\n Tiempo de ejecucion: "<<tiempoEjecucion<<"\n";
    

    return 0;
}



int probar() {
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



/*Calculo de distancias entre todos los nodos + ordenamiento de menor a mayor por nodo

void treeInsert(vector<tuple<double,string>> *nodos, tuple<double,string> nodoInsertar, int a, int b){
    if(a==b){
        auto itPos = nodos->begin()+4;
        nodos->insert(itPos,nodoInsertar);
    }
    else{
        int i = (int)((a+b)/2);
        tuple<double,string> nodoMedio = (*nodos)[i];
        if(get<0>(nodoInsertar) < get<0>(nodoMedio)){
            treeInsert(nodos,nodoInsertar,a,i);
        }
        else if(get<0>(nodoInsertar) > get<0>(nodoMedio)){
            treeInsert(nodos,nodoInsertar,i,b);
        }
        else{
            treeInsert(nodos,nodoInsertar,i,i);
        }
    }
}


void calcDistancias(map<string,vector<tuple<double,string>>> *dictClientesDist, //map<Nodo,vector<Nodo>> *dictEstacionesDist(ListaNodos nodos),
                    Nodo depot, ListaNodos clientes, ListaNodos estaciones){
    //Diccionario asocia a cada nodo un vector de pares (distancia, Nodo)
    map<string,vector<tuple<double,string>>> dictClientesDistTemp;
    vector<tuple<double,string>>::iterator ptr;
    string stringAux1,stringAux2;
    clientes.moveToStart();
    for(int i=0;i<clientes.len();i++){
        stringAux1 = to_string(clientes.getCurr().ID) + clientes.getCurr().tipo;
        ptr = dictClientesDistTemp[stringAux1].begin();
        for(int j=0;j<clientes.len();j++){
            //En cada iteracion sobre un nodo se debe insertar el nuevo par (distancia, nodo2) de manera queden ordenados por distancia ascendente
            stringAux2 = to_string(clientes.getNodo(j).ID) + clientes.getNodo(j).tipo;
            tuple<double,string> tuplaAux = make_tuple(calcularDistancia(clientes.getCurr(),clientes.getNodo(j)),stringAux2);
            
            if(dictClientesDistTemp[stringAux2].size()>0) treeInsert(&dictClientesDistTemp[stringAux2],tuplaAux,0,dictClientesDistTemp.size());
            else{dictClientesDistTemp[stringAux2].push_back(tuplaAux);}
        }
        clientes.next();
    }
    *dictClientesDist = dictClientesDistTemp;
}

//Testeo para ver si funciona
map<string,vector<tuple<double,string>>> dictClientesDist;
    calcDistancias(&dictClientesDist,depot,clientes,estaciones);
    for(int i = 0; i<dictClientesDist["10c"].size();i++) cout << get<0>(dictClientesDist["10c"][i]) << "  ";
*/



