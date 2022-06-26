#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "GeneradorSoluciones.h"

using namespace std;

int Variable::id_actual = 1;
int Vehiculo::id_actual = 1;

int main() {
    time_t start, end;
    string nombreArchivo = "";
    double tiempoEjecucion = 0.0;
    for(int i=1;i<=1;i++){
        for(int j=1;j<=1;j++){
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

            for(int i=0;i<inst.numEstaciones;i++){
                estaciones.append(nodos.getNodo(i+1));                
            }
            for(int i=0;i<inst.numClientes;i++){
                clientes.append(nodos.getNodo(i+inst.numEstaciones+1));                
            }
            time(&start);
            ListaVehiculos vehiculos = generarSoluciones(10000,inst,clientes,estaciones,depot);
            cout<<"-----SOLUCION FINAL-----\n";
            vehiculos.mostrar();
            time(&end);
            tiempoEjecucion = double(end-start)/ double(CLOCKS_PER_SEC);
            
            //generarOutput(vehiculos,nombreArchivo,&inst,tiempoEjecucion);


            cout<<"Procesada instancia "<<nombreArchivo<<"\n";
        }
    }
    return 0;
}