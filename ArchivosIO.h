#include "Variable.h"
#include "Instancia.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//Funciones auxiliares para manejo de archivos:

Instancia extraerInstancia(ifstream& archivo){
    Instancia inst;
    archivo >> inst.nombre;
    archivo >> inst.numClientes;
    archivo >> inst.numEstaciones;
    archivo >> inst.maxTiempo;
    archivo >> inst.maxDistancia;
    archivo >> inst.velocidad;
    archivo >> inst.tiempoServicio;
    archivo >> inst.tiempoRecarga;
    return inst;
}

void extraerNodos(ifstream& archivo, int numEstaciones, int numClientes, ListaNodos* nodos){
    Nodo nodoAux = Nodo();
    //Siguientes lineas del archivo:
    for(int i = 1; i <= numClientes+numEstaciones+1; i++){
        //Inicializar nodo para que valgrind no alegue:
        archivo >> nodoAux.ID;
        archivo >> nodoAux.tipo;
        archivo >> nodoAux.longitud;
        archivo >> nodoAux.latitud;
        nodos->append(nodoAux);
    }
}

Instancia extraerArchivo(ListaNodos* nodos, string nombreArchivo){ 
    string archivoInput = "Instancias/"+nombreArchivo+".dat";
    ifstream input(archivoInput);
    Instancia inst = Instancia();
    if(input.is_open()){
        inst = extraerInstancia(input);
        if(inst.nombre == ""){
            cout << "\nERROR EN EXTRACCION DE INSTANCIA\n";
            exit(-1);
        }
        //Almacenar nodos en variable de heap:
        extraerNodos(input,inst.numEstaciones,inst.numClientes,nodos);
    }
    else{
        cout << "\nERROR EN LECTURA DE ARCHIVO\n";
        exit(-1);
    }
    
    input.close();

    return inst;
}

void generarOutput(ListaVehiculos vehiculos,string nombreArchivo, Instancia *inst, double tiempoEjecucion){
    string archivoOutput = "Soluciones/"+nombreArchivo+".out";
    ofstream output(archivoOutput);

    //Calcular valores para mostrar:

    float sumaDistancias = 0.0;
    int sumaClientes = 0;
    float distanciasExcedidas[vehiculos.len()];

    vehiculos.moveToStart();
    vehiculos.next();

    for(unsigned int i = 0; i < vehiculos.len(); i++){
        sumaDistancias += vehiculos.getVehiculo(i+1).distanciaTotalRecorrida();
        sumaClientes += vehiculos.getVehiculo(i+1).cantClientesVisitados;
        if(vehiculos.getVehiculo(i+1).distanciaTotalRecorrida() > inst->maxDistancia){
            distanciasExcedidas[i] = vehiculos.getVehiculo(i+1).distanciaTotalRecorrida() - inst->maxDistancia;
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
    output << std::fixed << sumaDistancias << "    " << sumaClientes << "     " << vehiculos.len() << "     ";
    output << std::setprecision(10);
    output << tiempoEjecucion <<"\n";
    for(unsigned int i = 0; i < vehiculos.len(); i++){ 
        output << std::setprecision(6);
        output << vehiculos.getVehiculo(i+1).recorrido.to_string() <<"  "<<vehiculos.getVehiculo(i+1).distanciaTotalRecorrida() << "    " 
                    << vehiculos.getVehiculo(i+1).tiempoTranscurrido() << "     ";
        output << std::setprecision(2);
        output << distanciasExcedidas[i] <<"\n";
        vehiculos.next();
    }
    output.close();
}