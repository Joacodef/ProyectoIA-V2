#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "GeneradorSoluciones.h"

using namespace std;

int main() {
    //time_t start, end;
    string nombreArchivo = "";
    //double tiempoEjecucion = 0.0;
    for(int i=1;i<2;i++){
        for(int j=1;j<2;j++){
            nombreArchivo = "";
            //tiempoEjecucion = 0.0;
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
            for(int i=1;i<inst.numEstaciones+1;i++){
                estaciones.append(nodos.getNodo(i));                
            }
            for(int i=1;i<inst.numClientes+1;i++){
                clientes.append(nodos.getNodo(i+inst.numEstaciones));                
            }

            //cout << estaciones.to_string()<<"\n";
            //cout << clientes.to_string()<<"\n";

            //time(&start);

            ListaVehiculos vehiculos = generarSoluciones(10,inst,clientes,estaciones,depot);
            
            //time(&end);
            //tiempoEjecucion = double(end-start)/ double(CLOCKS_PER_SEC);

            //generarOutput(vehiculos,nombreArchivo,tiempoEjecucion);
            
            for(unsigned int i=0;i<vehiculos.len();i++){
                cout << vehiculos.getVehiculo(i).recorrido.to_string()<<"\n";
            }
            estaciones.free();
            clientes.free();
            nodos.free();
            return 0;
        }
    }
}





