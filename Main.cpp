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

int main() {
    string nombreArchivo = "";
    double tiempoEjecucion = 0.0;
    for(int i=1;i<=1;i++){
        for(int j=2;j<=2;j++){
            nombreArchivo = "";
            if(j<10){
                nombreArchivo = "AB"+to_string(i)+"0"+to_string(j);
            }
            else{
                nombreArchivo = "AB"+to_string(i)+to_string(j);
            }
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
            
            ListaVehiculos vehiculos = generarSoluciones(20000,inst,clientes,estaciones,depot);

            auto t2 = high_resolution_clock::now();
            auto ms_int = duration_cast<milliseconds>(t2 - t1);
            duration<double, std::milli> ms_double = t2 - t1;

            //cout<<"-----SOLUCION FINAL-----\n";
            vehiculos.mostrar();

            tiempoEjecucion = ms_double.count()/1000;
            generarOutput(vehiculos,nombreArchivo,&inst,tiempoEjecucion);
        }
    }
    return 0;
}