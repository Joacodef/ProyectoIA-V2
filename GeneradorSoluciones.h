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
    cout<<"HOLA 612"<<"\n";
    bool cumple = true;
    //cout << vehiculoDelNodo.distanciaDesdeRecarga<<"\n";
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


ListaVehiculos generarSoluciones(int maxIteraciones, Instancia inst, ListaNodos clientes, 
                                  ListaNodos estaciones, Nodo depot){
    int contadorIter = 0;
    ListaVariables variables = ListaVariables();
    Nodo nodoAux = Nodo();
    Variable variableActual;
    double distancia = 0.0;
    bool backtracking = false;
    bool variableSeAsigno = false;
    while(contadorIter < maxIteraciones){

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
                si la variable ya existe en la listaVariables, se borra de ella.
                volver a variable anterior, quitando el valor de la asignación actual, de su dominio
                variable anterior pasa a ser variable actual, y se realiza el loop de nuevo

        Si se visitaron todos los clientes, se llego a una solucion
        Si se terminan iteraciones, se retorna la mejor solucion encontrada hasta el momento.        
        */
        cout<<"HOLA 1"<<"\n";
        if(variables.clientesVisitados().len()>=abs(inst.numClientes)){
            //Por ahora se detiene con la primera solucion encontrada
            return variables.extraerSolucionActual(); 
        }
        variableSeAsigno = false;
        if(!backtracking){
            cout<<"HOLA 2"<<"\n";
            variableActual = Variable(clientes,estaciones,depot);
            if(contadorIter == 0){
                //Vehiculo vehi = Vehiculo();
                //variableActual.asignarVehiculo(vehi);
                variableActual.asignarNodo(depot);
                cout<<variables.getCurr().nodoAsignado.tipo<<"\n";
                variables.append(variableActual);
                contadorIter++;
                continue;
            }
        }
        else{
            cout<<"HOLA 3"<<"\n";
            backtracking = false;
        }
        variables.moveToEnd();
        cout<<"HOLA 4"<<"\n";
        if(!variables.getCurr().vehiculo.terminoRecorrido()){
            cout<<"HOLA 5"<<"\n";
            while(!variableSeAsigno && !variableActual.dominioVacio()){
                cout<<"HOLA 6"<<"\n";
                if(variableActual.dominioSoloClientes().len() < 1){
                    cout<<"HOLA 89"<<"\n";
                    nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominio,&distancia);
                }
                else{
                    cout<<"HOLA 61"<<"\n";
                    nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominioSoloClientes(),&distancia);
                    cout<<nodoAux.ID<<nodoAux.tipo<<"\n";
                } 
                variables.moveToEnd();
                cout<<variables.getCurr().nodoAsignado.tipo<<"\n";
                if(verificarRestricciones(variables.rutaActual(variables.getCurr(),inst.velocidad, inst.tiempoServicio,
                                            inst.tiempoRecarga),variables.clientesVisitados(),nodoAux,inst,depot)){
                    
                    variableActual.asignarNodo(nodoAux);
                    variables.append(variableActual); 
                    variables.moveToEnd();
                    variableSeAsigno = true;
                }
                else{
                    variableActual.quitarDelDominio(nodoAux);
                }
            }
            if(variableActual.dominioVacio()){
                cout<<"HOLA 7"<<"\n";
                //cout << "REALIZANDO BACKTRACKING " << "\n";
                backtracking = true;

                variables.moveToEnd();
                if(variables.getCurr().ID == variableActual.vehiculo.ID){
                    variables.pop(); 
                }

                variables.moveToEnd();

                variableActual = variables.getCurr();
                cout<<"HOLA 8"<<"\n";
                cout<<variableActual.dominio.getCurr().ID<<"\n";
                cout<<"HOLA 9"<<"\n";
                cout<<variableActual.nodoAsignado.ID<<"\n";
                variableActual.quitarDelDominio(variableActual.nodoAsignado);
                
                variableActual.nodoAsignado = Nodo();

            }
        }
        else{
            //cout<<"HOLA 8"<<"\n";
            //Vehiculo vehi = Vehiculo();
            //variableActual.asignarVehiculo(vehi);
            variableActual.asignarNodo(depot);
            variables.append(variableActual);
        }
        //cout<< "\ntamaño lista variables:"<<variables.len()<<"\n";
        contadorIter ++;
        //cout<<"HOLA 9"<<"\n";
    }

    
    /*if(variables.mejorSolucion.len() == 0)*/ return variables.extraerSolucionActual(); 
    //return variables.mejorSolucion;
}


/*

*/