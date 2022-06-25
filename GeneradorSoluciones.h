#include "ArchivosIO.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

// Funcion para generar recorridos para un vehiculo, de acuerdo con restricciones del GVRP

string verificarRestricciones(Vehiculo vehiculoDelNodo, ListaNodos clientesVisitados, Nodo nodoPorAsignar, 
                              Nodo depot, Nodo anterior, Instancia inst){
    string porQueNoCumple = "siCumple";
    //cout << vehiculoDelNodo.distanciaDesdeRecarga<<"\n";
    //Verificar si se tiene combustible para llegar al nodo asignado:
    double distRecarga = vehiculoDelNodo.distanciaDesdeRecarga();
    if(distRecarga>inst.maxDistancia){
        //Agregar cosas para quitar del dominio a otros clientes
        return "combustible";
    }
    //Verificar si se tiene tiempo para volver al depósito:
    double distanciaDeposito = calcularDistancia(nodoPorAsignar,depot);
    double tiempoAlDepot = distanciaDeposito/inst.velocidad;
    if(vehiculoDelNodo.tiempoTranscurrido()+tiempoAlDepot > inst.maxTiempo){
        //Agregar cosas para descartar nodos que esten mas lejos del depot que el actual
        return "tiempo";
    }
    //Verificar si el cliente ya fue asignado
    if(nodoPorAsignar.tipo == 'c'){
        if(clientesVisitados.find(nodoPorAsignar)!=-1){
            //Cliente esta en la lista de clientes visitados
            return "asignado";
        }
    }
    if(nodoPorAsignar.ID == 0 && anterior.ID == 0){
        return "sinSentido";
    }
    
    
    /*
    else if(nodoPorAsignar.tipo == 'd'){
        //Que no se pueda ir a dos estaciones seguidas (quizá no es correcta esta restriccion)
        if(anterior.tipo == 'd'){
            return "dosEstaciones";
        }
    }*/

    return porQueNoCumple;
}


ListaVehiculos generarSoluciones(int maxIteraciones, Instancia inst, ListaNodos clientes, 
                                  ListaNodos estaciones, Nodo depot){
    ListaVehiculos mejorSolucion;
    ListaVehiculos solucionCandidata;
    int contadorIter = 0;
    ListaVariables variables;
    Nodo nodoAux;
    Variable variableActual;
    ListaNodos dominioAcotado;
    Vehiculo vehiAux = Vehiculo(inst.velocidad, inst.tiempoServicio, inst.tiempoRecarga);
    bool backtracking = false;
    bool variableSeAsigno = false;
    double distMejorSolucion = INFINITY;
    double distActual = 0.0;
    string restriccion;

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
        variableSeAsigno = false;
        vehiAux = variables.recorridoDeVariable(variables.getCurr(),inst.velocidad, inst.tiempoServicio,inst.tiempoRecarga);
        

        //******Verificar si se visitaron todos los clientes (nueva solución candidata)******//
        if(variables.clientesVisitados().len()>=abs(inst.numClientes)){

            //Determinar si nueva solución es mejor
            Variable depotFinal;
            depotFinal.asignarNodo(depot);
            variables.append(depotFinal);

            solucionCandidata = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
            distActual = solucionCandidata.calcularDistTotal();

            //cout << "---------SOLUCION CANDIDATA-----------\n";
            //solucionCandidata.mostrar();
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
            dominioAcotado = variables.getCurr().dominio; 
            variables.pop();          
            variables.moveToEnd();
            contadorIter++; 
            continue;
        }
        

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
            //Si hicimos backtracking, la variable auxiliar "variableActual" parte con el dominio (acotado)
            //de la variable que se tenía antes al final de la lista.
            backtracking = false;
            variableActual = Variable(dominioAcotado);
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
                variables.moveToEnd();
                //Si quedan clientes se busca más cercano:
                if(variableActual.dominioTieneCliente()){
                    nodoAux = nodoMenorDistancia(variables.getCurr().nodoAsignado, variableActual.dominioSoloClientes());
                    restriccion = verificarRestricciones(vehiAux,variables.clientesVisitados(),nodoAux,depot,variables.getCurr().nodoAsignado,inst);
                    //Se verifica si el cliente a asignar cumple las restricciones
                    if(restriccion == "siCumple"){
                        //Si cumple las restricciones, se asigna el cliente a la variable
                        variableActual.asignarNodo(nodoAux);
                        variableActual.quitarDelDominio(variableActual.nodoAsignado);
                        variables.append(variableActual);
                        variableSeAsigno = true;
                    }
                    else if(restriccion == "combustible"){
                        //Si no cumple restriccion de combustible, se deben quitar clientes del dominio, para ir estacion
                        variableActual.dominio = variableActual.quitarClientesDominio();
                    }
                    else if(restriccion == "tiempo"){
                        //Si no cumple restriccion de tiempo, debe volver al depot
                        variableActual.asignarNodo(depot);
                        variables.append(variableActual);
                        variableSeAsigno = true;
                    }
                    else if(restriccion == "asignado"){
                        //Si no cumple porque esta asignado a otro cliente, se quita de su dominio
                        variableActual.quitarDelDominio(nodoAux);
                    }
                }
                else{ //Si no quedan clientes en el dominio de variableActual
                    
                    nodoAux = nodoMenorDistancia(variables.getCurr().nodoAsignado,variableActual.dominio);
                    restriccion = verificarRestricciones(vehiAux,variables.clientesVisitados(),nodoAux,depot,variables.getCurr().nodoAsignado,inst);
                    variables.moveToEnd();
                    
                    if(restriccion == "siCumple"){
                        //Si la estación cumple todas las restricciones, se pasa por ella.
                        variableActual.asignarNodo(nodoAux);
                        variableActual.quitarDelDominio(variableActual.nodoAsignado);
                    }
                    else if(restriccion == "combustible" || restriccion == "tiempo" || restriccion == "sinSentido"){
                        //Si hay alguna que no cumpla, no se necesita intentar con niguna otra
                        variableActual.dominio = variableActual.quitarEstacionesDominio();
                        break;  
                    }
                    /*
                    else if(restriccion == "dosEstaciones"){
                        //
                    }*/
                    variables.append(variableActual);
                    variableSeAsigno = true;
                }  
            }

            //****CONDICIONES DE BACKTRACKING****//
            distActual = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga).calcularDistTotal();
            if((variableActual.dominioVacio() && variableSeAsigno==false) || distActual > distMejorSolucion){

                //1 Si el dominio de una variable queda vacío, sin haberle asignado un valor factible, significa que el vehiculo 
                //no cumple ninguna restriccion y debe hacerse backtracking
                //
                //2 Si la distancia de la solucion actual es mayor que la mejor solucion, se hace backtracking
                backtracking = true;
                variableSeAsigno = false;
                //se quita variable de la lista, pero se guarda su dominio para darselo a la variableActual en la sgte iteracion
                variables.moveToEnd();
                dominioAcotado = variables.getCurr().dominio;
                variables.pop();
                variables.moveToEnd();              
            }
        }//***FIN DEL LOOP CHICO***
        contadorIter ++;
        //vehiAux.free();
    }//***FIN DEL LOOP PRINCIPAL***
    //variables.free();
    return mejorSolucion;
}


/*

*///Si todavía hay clientes, se busca busca solo dentro de los clientes el mas cercano
                    