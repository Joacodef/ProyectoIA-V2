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
    if(vehiculoDelNodo.distanciaDesdeRecarga>inst.maxDistancia){
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
    Nodo nodoAux = Nodo();
    Variable variableActual;
    double distancia = 0.0;
    bool backtracking = false;
    bool variableSeAsigno = false;
    while(contadorIter < numIteraciones){
        /*
        Verificar si se está entrando al loop "avanzando" o por backtracking. Si es lo primero,
        se crea una variable nueva. Si es lo segundo, se utiliza variable anterior.

        Si el vehículo asociado a la variable anterior no terminó su recorrido, se le asigna 
        ese vehículo. Si lo terminó, se mantiene el vehículo generado en constructor y se asigna
        el depot como proximo nodo.

        while(variable no ha encontrado un nodo factible)
            Probar nuevos valores dentro de su dominio, comprobar condiciones.
            Si variable

            if(dominio esta vacío)
                backtracking = true
                borrar variable actual (puede molestar más adelante)
                si está asociada a un vehículo que no tiene nodos asociados, se borra el vehículo
                (puede molestar también) 
                volver a variable anterior, quitando el valor de su asignación actual de su dominio
                variable anterior pasa a ser variable actual, y se realiza el loop de nuevo

        Si se terminan iteraciones, se retorna la mejor solucion encontrada hasta el momento.        
        */
        
        /*
        if(!backtracking){
            variableActual = Variable(clientes,estaciones,depot);
        }
        else{
            backtracking = false;
        }
        variables.moveToEnd();
        if(!variables.getCurr().vehiculo.terminoRecorrido()){ 
            variableActual.asignarVehiculo();
            while(!variableSeAsigno && !variableActual.dominioVacio()){
                nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominio,&distancia);
                if(verificarRestricciones(variableActual.getVehiculo,variables.clientesVisitados(),
                                            nodoAux,inst,depot)){
                    variableActual.asignar();
                    variables.append(variableActual); (o algun otro metodo que haga mas cosas)
                    variableSeAsigno = true;
                }
            }
        }
        else{
            
        }
        
        */


    }
    return solucion;
}


/*

*/