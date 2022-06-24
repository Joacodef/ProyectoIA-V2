#include "ArchivosIO.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

// Funcion para generar recorridos para un vehiculo, de acuerdo con restricciones del GVRP

bool verificarRestricciones(Vehiculo vehiculoDelNodo, ListaNodos clientesVisitados, Nodo nodoAsignado, 
                            Instancia inst, Nodo depot/*,ListaNodos dominio*/){
    bool cumple = true;
    //cout << vehiculoDelNodo.distanciaDesdeRecarga<<"\n";
    //Verificar si se tiene combustible para llegar al nodo asignado:
    if(vehiculoDelNodo.distanciaDesdeRecarga()>inst.maxDistancia){
        //Agregar cosas para quitar del dominio a otros clientes
        return false;
    }
    //Verificar si se tiene tiempo para volver al depósito:
    double distanciaDeposito = calcularDistancia(nodoAsignado,depot);
    double tiempoAlDepot = distanciaDeposito/inst.velocidad;
    if(vehiculoDelNodo.tiempoTranscurrido()+tiempoAlDepot > inst.maxTiempo){
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
    ListaVariables variables;
    Nodo nodoAux;
    Variable variableActual;
    Vehiculo vehiAux = Vehiculo(inst.velocidad, inst.tiempoServicio, inst.tiempoRecarga);
    bool backtracking = false;
    bool variableSeAsigno = false;

    //***************LOOP PRINCIPAL (se asignan nuevas variables o se hace backtracking)******************
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

        //vehiAux guardará al principio de cada iteración el recorrido en el que se esté en esta asignación
        variables.moveToEnd();
        vehiAux = variables.recorridoDeVariable(variables.getCurr(),inst.velocidad, inst.tiempoServicio,inst.tiempoRecarga);

        //Se comprueba si se han visitado todos los clientes 
        if(variables.clientesVisitados().len()>=abs(inst.numClientes)){
            //(Por ahora se detiene con la primera solucion encontrada)
            Variable depotFinal;
            depotFinal.asignarNodo(depot);
            variables.append(depotFinal);
            return variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga); 
        }
        variableSeAsigno = false;
        //Se comprueba si estamos entrando al loop por backtracking o no
        if(!backtracking){
            //Si no es backtracking, se declara la nueva variable que se asignará en esta iteración
            variableActual = Variable(clientes,estaciones);
            if(contadorIter == 0){
                //Si es la primera iteración, se asigna el depot de la instancia a la variable actual.
                //variableActual = Variable(clientes,estaciones);
                variableActual.asignarNodo(depot);
                variables.append(variableActual);
                contadorIter++;
                continue;
            }
        }
        else{
            //El punto de detectar si hay backtracking es no declarar una nueva variable, por lo tanto se asigna
            //el flag como false en seguida. (puede que haya que hacer otras cosas más adelante para este caso)
            backtracking = false;
        }

        //Se verifica si el recorrido actual ya terminó, en cuyo caso se debe asignar un depot a la nueva variable
        if(vehiAux.recorridoTerminado()){
            //variableActual = Variable(clientes,estaciones);
            variableActual.asignarNodo(depot);
            variables.append(variableActual);
        }
        else{//**************LOOP "CHICO" (se buscan asignaciones factibles)****************

            //Se buscan asignaciones factibles para la nueva variable en un loop. Si el dominio esta vacío o se encontró
            //la asignación, se termina el loop, habiendo asignado la variable.
            while(!variableSeAsigno && !variableActual.dominioVacio()){
                //Se verifica si a la variable no se le puede asignar ningun cliente, para que busque cualquier nodo cercano (estaciones de recarga)
                if(!variableActual.dominioTieneCliente()){
                    nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominio);
                    if(verificarRestricciones(vehiAux,variables.clientesVisitados(),nodoAux,inst,depot)){
                        //Si las cumple, se asigna el cliente a la variable
                        variableActual.asignarNodo(nodoAux);
                        variables.append(variableActual); 
                        variables.moveToEnd();
                        variableSeAsigno = true;
                    }
                    else{
                        //Si ninguna estación cumple las restricciones, se devuelve al deposito
                        variableActual.asignarNodo(depot);
                        variables.append(variableActual);
                        variables.moveToEnd();
                        variableSeAsigno = true;
                    }
                }
                else{
                    //Si todavía hay clientes, se busca busca solo dentro de los clientes el mas cercano
                    nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominioSoloClientes());
                    variables.moveToEnd();
                    //Se verifica si el cliente a asignar cumple las restricciones
                    if(verificarRestricciones(vehiAux,variables.clientesVisitados(),nodoAux,inst,depot)){
                        //Si las cumple, se asigna el cliente a la variable
                        variableActual.asignarNodo(nodoAux);
                        variables.append(variableActual); 
                        variables.moveToEnd();
                        variableSeAsigno = true;
                    }
                    else{
                        //Si no las cumple, se debe quitar el cliente del dominio
                        variableActual.quitarDelDominio(nodoAux);
                    }
                } 
                
            }
            if(variableActual.dominioVacio() && variableSeAsigno==false){
                //Si el dominio de una variable queda vacío, sin haberle asignado un valor factible, significa que el vehiculo 
                //no cumple ninguna restriccion y debe hacerse backtracking
                backtracking = true;
                
                //Se vuelve a la ultima variable que se había asignado, se le borra el nodo asignado y se quita ese nodo del dominio
                variables.moveToEnd();
                variableActual = variables.getCurr();
                variableActual.quitarDelDominio(variableActual.nodoAsignado);
                variableActual.nodoAsignado = Nodo();
            }
        }
        
        contadorIter ++;
    }

    
    /*if(variables.mejorSolucion.len() == 0)*/ return variables.extraerSolucionActual(inst.velocidad, inst.tiempoServicio, inst.tiempoRecarga); 
    //return variables.mejorSolucion;
}


/*

*/