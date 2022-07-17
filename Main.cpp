#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "GeneradorSoluciones.h"
#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

int Variable::id_actual = 1;
int Vehiculo::id_actual = 1;

int main(int argc, char* argv[]) {
    string nombreArchivo(argv[1]);
    double tiempoEjecucion = 0.0;
    
    cout << nombreArchivo <<"\n";
    Instancia inst = Instancia();
    Nodo depot = Nodo();
    ListaNodos nodos = ListaNodos();
    ListaNodos estaciones = ListaNodos();
    ListaNodos clientes = ListaNodos();

    inst = extraerArchivo(&nodos,nombreArchivo);

    depot = nodos.getNodo(0);

    for(int i=0;i<inst.numEstaciones;i++){
        estaciones.append(nodos.getNodo(i+1));                
    }
    for(int i=0;i<inst.numClientes;i++){
        clientes.append(nodos.getNodo(i+inst.numEstaciones+1));                
    }
    //time(&start);
    auto t1 = high_resolution_clock::now();
    
    ListaVehiculos vehiculos = generarSoluciones(4000,inst,clientes,estaciones,depot);

    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    duration<double, std::milli> ms_double = t2 - t1;

    //cout<<"-----SOLUCION FINAL-----\n";
    //vehiculos.mostrar();
    cout<<"Procesada instancia "<<nombreArchivo<<"\n";

    tiempoEjecucion = ms_double.count()/1000;
    generarOutput(vehiculos,nombreArchivo,&inst,tiempoEjecucion);
        
    
    return 0;
}