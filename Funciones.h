#include <math.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "Vehiculo.h"
#include "Instancia.h"


#define RADIO_TIERRA 4182.44949

using namespace std;

double aRadianes(double num){
    double rad = 0.0;
    rad = M_PI * (num/180.0);
    return rad;
}

double haversine(float num){
    return pow(sin(num/2.0),2);
}


//Funcion que calcula la distancia entre 2 puntos segun la formula de Haversine.

double calcularDistancia(double lon1, double lat1, double lon2, double lat2){
    double distancia = 0.0;

    double deltaLat = aRadianes(lat1 - lat2);
    double deltaLon = aRadianes(lon1 - lon2);

    double a = haversine(deltaLat) + cos(aRadianes(lat1)) * cos(aRadianes(lat2)) * haversine(deltaLon);
    
    double c = 2.0 * atan2(sqrt(a),sqrt(1-a));

    distancia = RADIO_TIERRA * c;

    return distancia;
}


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


//Funcion que retorna el nodo que esté mas cerca del nodo "centro", chequeando si esta restringido.

Nodo* nodoMenorDistancia(Nodo centro, Nodo *demasNodos, int size, ListaNodos nodosRestringidos, double *distPtr){
    double menor = 999999999.9;
    Nodo *menorNodo;
    double distancia = 0.0;
    for(int i=0; i<size;i++){
        if(centro.ID == demasNodos[i].ID && centro.tipo == demasNodos[i].tipo) continue;
        if(nodosRestringidos.find(demasNodos[i])!=-1) continue;
        distancia = calcularDistancia(centro.longitud,centro.latitud,
                                    demasNodos[i].longitud,demasNodos[i].latitud);
        if(distancia<menor){
            menor = distancia;
            *distPtr = distancia;
            menorNodo = &demasNodos[i];
        }      
    }
    return menorNodo;
}

//Funciones que extraen datos de archivos:
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
        sumaDistancias += vehiculos.getVehiculo(i).distanciaTotalRecorrida;
        sumaClientes += vehiculos.getVehiculo(i).cantClientesVisitados;
        if(vehiculos.getVehiculo(i).distanciaTotalRecorrida > inst->maxDistancia){
            distanciasExcedidas[i] = vehiculos.getVehiculo(i).distanciaTotalRecorrida - inst->maxDistancia;
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
        output << vehiculos.getVehiculo(i).recorrido.to_string() << "     " << vehiculos.getVehiculo(i).distanciaTotalRecorrida << "    " 
                    << vehiculos.getVehiculo(i).tiempoTranscurrido << "     ";
        output << std::setprecision(2);
        output << distanciasExcedidas[i] <<"\n";
        vehiculos.next();
    }
    output.close();
}
