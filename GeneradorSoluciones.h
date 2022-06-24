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
    double distRecarga = vehiculoDelNodo.distanciaDesdeRecarga();
    if(distRecarga>inst.maxDistancia){
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
    ListaVehiculos mejorSolucion;
    ListaVehiculos solucionCandidata;
    int contadorIter = 0;
    ListaVariables variables;
    Nodo nodoAux;
    Variable variableActual;
    ListaNodos dominioRestringido;
    Vehiculo vehiAux = Vehiculo(inst.velocidad, inst.tiempoServicio, inst.tiempoRecarga);
    bool backtracking = false;
    bool variableSeAsigno = false;
    double distMejorSolucion = INFINITY;
    double distActual = 0.0;

    //***************LOOP PRINCIPAL******************//
    while(contadorIter < maxIteraciones){
        /****Explicación general****/
        /*
        Verificar si ya se llego a una solución (todos los clientes visitados), ver si es mejor que la actual
        Hacer backtracking si se llegó a la solución

        Verificar si se está entrando al loop "avanzando" o por backtracking. Si es lo primero,
        se crea una variable "totalmente" nueva. Si es lo segundo, se mantiene el dominio anterior para
        no perder los valores que se han probado.

        Si el vehículo asociado a la variable anterior terminó su recorrido, se asigna un depot a la variable actual.

        while(LOOP "CHICO" - variable no ha encontrado un nodo factible)
            Probar nuevos valores dentro de su dominio, comprobar condiciones.

            if(dominio esta vacío)
                backtracking = true       
        */

        //vehiAux guardará al principio de cada iteración el recorrido en el que se esté en esta asignación
        variables.moveToEnd();
        vehiAux = variables.recorridoDeVariable(variables.getCurr(),inst.velocidad, inst.tiempoServicio,inst.tiempoRecarga);
        distActual = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga).calcularDistTotal();
        if(distActual > distMejorSolucion){
            //Si la distancia de la solucion actual es mayor que la mejor solucion, se hace backtracking
            backtracking = true;
            variableSeAsigno = false;
            //se quita variable de la lista, pero se guarda su dominio para darselo a la variableActual en la sgte iteracion
            dominioRestringido = variableActual.dominio;
            variables.pop();
            variables.moveToEnd();
            variableActual = variables.getCurr();
            contadorIter++;
            continue;
        }

        //******Verificar si se visitaron todos los clientes (nueva solución candidata)******//
        if(variables.clientesVisitados().len()>=abs(inst.numClientes)){

            //Determinar si nueva solución es mejor
            Variable depotFinal;
            depotFinal.asignarNodo(depot);
            variables.append(depotFinal);

            solucionCandidata = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
            distActual = solucionCandidata.calcularDistTotal();

            cout << "---------SOLUCION CANDIDATA-----------\n";
            solucionCandidata.mostrar();
            //return solucionCandidata;//En caso de tener problemas con backtracking

            if(distActual < distMejorSolucion){
                mejorSolucion = solucionCandidata;
                distMejorSolucion = distActual;
            } 
            
            //Ahora borramos el depot de la solución, restringimos y guardamos el dominio de la 
            //última variable, y hacemos backtracking
            variables.pop();
            variables.moveToEnd();
            backtracking = true;
            variableSeAsigno = false; 
            dominioRestringido = variables.getCurr().dominio; 
            variables.pop();          
            variables.moveToEnd();
            variableActual = variables.getCurr();
            contadorIter++; 
            continue;
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
            //Si estamos haciendo backtracking, la variable auxiliar "variableActual" parte con el dominio
            //de la variable que se tenía antes. En teoría es la misma variable, pero era más simple hacerlo así.
            backtracking = false;
            variableActual = Variable(dominioRestringido);
        }

        //Se verifica si el recorrido actual ya terminó, en cuyo caso se debe asignar un depot a la nueva variable
        if(vehiAux.recorridoTerminado()){
            //variableActual = Variable(clientes,estaciones);
            variableActual.asignarNodo(depot);
            variables.append(variableActual);
        }
        else{//*******************LOOP CHICO*******************//


            //Se buscan asignaciones factibles para la nueva variable en un loop. Si el dominio esta vacío o se encontró
            //la asignación, se termina el loop, habiendo asignado la variable.

            //Verificar que no se haya asignado un nodo a la variable, y que su dominio no esté vacío
            while(!variableSeAsigno && !variableActual.dominioVacio()){

                //Si quedan clientes se busca más cercano:
                if(variableActual.dominioTieneCliente()){
                    nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominioSoloClientes());
                    variables.moveToEnd();

                    //Se verifica si el cliente a asignar cumple las restricciones
                    if(verificarRestricciones(vehiAux,variables.clientesVisitados(),nodoAux,inst,depot)){

                        //Si cumple las restricciones, se asigna el cliente a la variable
                        variableActual.asignarNodo(nodoAux);
                        variables.append(variableActual); 
                        variables.moveToEnd();
                        variableSeAsigno = true;
                        variables.getCurr().quitarDelDominio(variableActual.nodoAsignado);
                    }
                    else{

                        //Si no cumple restricciones, se debe quitar el cliente del dominio
                        variables.moveToEnd();
                        variableActual.quitarDelDominio(nodoAux);
                    }
                }
                else{ //Si no quedan clientes en el dominio de "variableActual"
                    
                    nodoAux = nodoMenorDistancia(variableActual.nodoAsignado,variableActual.dominio);
                    //Se debe verificar si ya se pasó antes a una estación de recarga, en cuyo caso se devuelve al depot
                    variables.moveToEnd();
                    if(variables.getCurr().nodoAsignado.tipo == 'f'){
                        variableActual.asignarNodo(depot);
                        variables.append(variableActual);
                        variableSeAsigno = true;
                        break;
                    }
                    if(verificarRestricciones(vehiAux,variables.clientesVisitados(),nodoAux,inst,depot)){
                        //Si cumple las restricciones, se asigna la estación a la variable
                        variableActual.asignarNodo(nodoAux);
                        variables.append(variableActual); 
                        variables.moveToEnd();
                        variableSeAsigno = true;
                        variables.getCurr().quitarDelDominio(variableActual.nodoAsignado);
                    }
                    else{
                        //Si ninguna estación cumple las restricciones, se devuelve al deposito 
                        //pueden infringirse reglas de distancia!!
                        variableActual.asignarNodo(depot);
                        variables.append(variableActual);
                        variables.moveToEnd();
                        variableSeAsigno = true;
                    }
                }  
            }

            //****CONDICIÓN DE BACKTRACKING****//
            if(variableActual.dominioVacio() && variableSeAsigno==false){

                //Si el dominio de una variable queda vacío, sin haberle asignado un valor factible, significa que el vehiculo 
                //no cumple ninguna restriccion y debe hacerse backtracking
                backtracking = true;
                variableSeAsigno = false;
                //se quita variable de la lista, pero se guarda su dominio para darselo a la variableActual en la sgte iteracion
                dominioRestringido = variableActual.dominio;
                variables.pop();
                variables.moveToEnd();
                variableActual = variables.getCurr();                
            }

        }//***FIN DEL LOOP CHICO***
        contadorIter ++;

    }//***FIN DEL LOOP PRINCIPAL***
    return mejorSolucion;
}


/*

*///Si todavía hay clientes, se busca busca solo dentro de los clientes el mas cercano
                    