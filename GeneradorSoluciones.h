#include <cstring>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "Funciones.h"

using namespace std;

// Funcion para generar recorridos para un vehiculo, de acuerdo con restricciones del GVRP

bool verificarRestricciones(Vehiculo vehiculoDelNodo, ListaNodos clientesVisitados, Nodo nodoAsignado, 
                            Instancia inst, Nodo depot/*,ListaNodos dominio*/){
    bool cumple = true;
    //Verificar si se tiene combustible para llegar al nodo asignado:
    if(vehi.distanciaDesdeRecarga>inst.maxDistancia){
        //Agregar cosas para quitar del dominio a otros clientes
        return false;
    }
    //Verificar si se tiene tiempo para volver al depósito:
    double distanciaDeposito = calcularDistancia(nodoAsignado,depot);
    double tiempoAlDepot = distanciaDeposito/inst.velocidad;
    if(vehiculoDelNodo.tiempoTranscurrido+tiempoAlDepot > inst.maxTiempo){
        //Agregar cosas para descartar nodos que esten mas lejos del depot que el actual
        return false;
    }
    //Verificar si el cliente ya fue asignado
    if(nodoAsignado.tipo == 'c'){
        if(clientesVisitados.find(nodoAsignado)!=-1){
            //Cliente esta en la lista de clientes visitados
            return false;
        }
    }

    return cumple;
}


ListaVehiculos generarSoluciones(int numIteraciones, Instancia inst, ListaNodos clientes, 
                                  ListaNodos estaciones, Nodo depot){
    int contadorIter = 0;
    ListaVariables variables = ListaVariables();
    Vehiculo vehiculoActual = Vehiculo();
    Nodo nodoActual = Nodo();
    while(contadorIter < numIteraciones){
        /*
        -Asignar una nueva variable, es decir, asociar un nodo y un vehículo en el último paso
        de su recorrido hasta el momento, luego:
        while(variable no ha encontrado un valor factible)
            -Probar nuevos valores dentro de su dominio, comprobar condiciones
            if(dominio esta vacío)
                
        */
        Variable varActual = Variable();
    }
    return solucion;
}


/*
Se debe manejar la lista de vehiculos y las variables al mismo tiempo, de manera conjunta
*/