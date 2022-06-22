#include <iostream>
#include <fstream>
#include <cstring>
#include <bits/stdc++.h>
#include "Funciones.h"

using namespace std;

int main() {
    double eficienciaEsperada = 0.02;

    for(int i=1;i<2;i++){
        for(int j=1;j<20;j++){
            //-----EXTRACCIÓN DE DATOS DE ARCHIVOS (SE RESUELVEN AB101-AB220)-----
            string nombreArchivo = "";
            if(j<10){
                nombreArchivo = "AB"+to_string(i)+"0"+to_string(j);
            }
            else{
                nombreArchivo = "AB"+to_string(i)+to_string(j);
            }
            string archivoInput = "Instancias/"+nombreArchivo+".dat";
            time_t start, end;
            ifstream input(archivoInput);
            Instancia inst;
            Nodo *nodos;
            if(input.is_open()){
                inst = extraerInstancia(input);
                if(inst.nombre == ""){
                    cout << "\nERROR EN EXTRACCION DE INSTANCIA\n";
                    exit(-1);
                }
                //Almacenar nodos en variable de heap:
                nodos = (Nodo*)malloc(sizeof(Nodo)*(inst.numClientes+inst.numClientes+1));
                extraerNodos(input,inst.numEstaciones,inst.numClientes,nodos);
            }
            else{
                cout << "\nERROR EN LECTURA DE ARCHIVO\n";
                exit(-1);
            }
            time(&start);
            //Almacenar depot, estaciones y clientes en variables de stack
            Nodo depot = nodos[0];
            Nodo estaciones[inst.numEstaciones];
            Nodo clientes[inst.numClientes];
            //cout << depot.to_string();
            for(int i=0;i<inst.numEstaciones;i++){
                estaciones[i] = nodos[i+1];
                //cout << estaciones[i].to_string();
            }
            for(int i=0;i<inst.numClientes;i++){
                clientes[i] = nodos[i+1+inst.numEstaciones];
                //cout << clientes[i].to_string();
            }
            input.close();
            free(nodos);

            //-----EJECUCION DEL GENERADOR DE SOLUCIONES Y OBTENCIÓN DE RESULTADOS------
            
            ListaVehiculos vehiculos = loopGeneracionSolucion(depot, estaciones, clientes, inst, eficienciaEsperada);


            /*
            vehiculos.moveToStart();
            vehiculos.next();
            for(unsigned int i = 0;i<vehiculos.listSize;i++){
                cout << vehiculos.curr->data.recorrido.to_string();
                cout << "Distancia recorrida: "<< vehiculos.curr->data.distanciaTotalRecorrida<<"\n";
                cout << "Tiempo transcurrido: "<< vehiculos.curr->data.tiempoTranscurrido<<"\n";
                vehiculos.next();
            }*/

            time(&end);
            
            //-----GENERAR ARCHIVO PARA SOLUCIÓN DE LA INSTANCIA ACTUAL-----

            string archivoOutput = "Soluciones/"+nombreArchivo+".out";
            ofstream output(archivoOutput);
            double tiempoEjecucion = double(end-start)/ double(CLOCKS_PER_SEC);

            //Calcular valores para mostrar:

            float sumaDistancias = 0.0;
            int sumaClientes = 0;
            float distanciasExcedidas[vehiculos.listSize];

            vehiculos.moveToStart();
            vehiculos.next();

            for(unsigned int i = 0; i < vehiculos.listSize; i++){
                sumaDistancias += vehiculos.curr->data.distanciaTotalRecorrida;
                sumaClientes += vehiculos.curr->data.cantClientesVisitados;
                if(vehiculos.curr->data.distanciaTotalRecorrida > inst.maxDistancia){
                    distanciasExcedidas[i] = vehiculos.curr->data.distanciaTotalRecorrida - inst.maxDistancia;
                }
                else{
                    distanciasExcedidas[i] = 0.0;
                }
                vehiculos.next();
            } 
            //Escribir en el archivo de output
            vehiculos.moveToStart();
            vehiculos.next();
            output << std::fixed;
            output << std::setprecision(2);
            output << std::fixed << sumaDistancias << "    " << sumaClientes << "     " << vehiculos.listSize << "     ";
            output << std::setprecision(10);
            output << tiempoEjecucion <<"\n";
            for(unsigned int i = 0; i < vehiculos.listSize; i++){ 
                output << std::setprecision(6);
                output << vehiculos.curr->data.recorrido.to_string() << "     " << vehiculos.curr->data.distanciaTotalRecorrida << "    " 
                            << vehiculos.curr->data.tiempoTranscurrido << "     ";
                output << std::setprecision(2);
                output << distanciasExcedidas[i] <<"\n";
                vehiculos.next();
            }
            output.close();
            vehiculos.clear();
        }
        
    }
  return 0;
} 


