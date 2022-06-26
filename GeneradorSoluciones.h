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
    return porQueNoCumple;
}


ListaVehiculos generarSoluciones(int maxIteraciones, Instancia inst, ListaNodos clientes, 
                                  ListaNodos estaciones, Nodo depot){
    int contadorIter = 0;   
    Nodo nodoAux;
    Variable variableActual;
    ListaNodos dominioAcotado,clientesRestantes,clientesVisitados;
    ListaVehiculos mejorSolucion,solucionCandidata;
    ListaVariables variables;
    bool backtracking = false;
    bool variableSeAsigno = false;
    double distMejorSolucion = INFINITY;
    double distActual = 0.0;
    string restriccion;

    Vehiculo vehiAux = Vehiculo(inst.velocidad, inst.tiempoServicio, inst.tiempoRecarga);
    variableActual.asignarNodo(depot);
    variables.append(variableActual);
    clientesRestantes = concatenar(clientes,estaciones);

    /***************LOOP PRINCIPAL******************/
    while(contadorIter < maxIteraciones){
        /****Explicación general****/
        /*
        Verificar si ya se llego a una solución (todos los clientes visitados), ver si es mejor que la actual
        Hacer backtracking si se llegó a la solución

        Verificar si se está entrando al loop "avanzando" o por backtracking. Si es lo primero,
        se crea una variable "totalmente" nueva. Si es lo segundo, se mantiene el dominio anterior para
        no perder los valores que se han probado.

        Si el vehículo asociado a la variable anterior terminó su recorrido, se asigna un depot a la variable actual.

        while(LOOP SECUNDARIO - variable no ha encontrado un nodo factible)
            Probar nuevos valores dentro de su dominio, comprobar condiciones.

            if(dominio esta vacío)
                backtracking = true       
        */

        //vehiAux guardará al principio de cada iteración el recorrido en el que se esté en esta asignación
        variables.moveToEnd();
        variableSeAsigno = false;
        vehiAux = variables.recorridoDeVariable(variables.getCurr(),inst.velocidad, 
                                                    inst.tiempoServicio,inst.tiempoRecarga);

        /*********PARA RUTEO**********/

        variables.printNodos();

        /******Verificar si se visitaron todos los clientes (nueva solución candidata)******/
        if(clientesVisitados.len()>=abs(inst.numClientes)){

            //Determinar si nueva solución es mejor
            Variable depotFinal;
            depotFinal.asignarNodo(depot);
            variables.append(depotFinal);

            solucionCandidata = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
            distActual = solucionCandidata.calcularDistTotal();

            cout << "\n\n---------SOLUCION CANDIDATA-----------\n";
            solucionCandidata.mostrar();

            if(distActual < distMejorSolucion){
                mejorSolucion = solucionCandidata;
                distMejorSolucion = distActual;
            }
            //Quitar todos las variables correspondientes al ultimo recorrido, más el depot del último recorrido (backtracking hasta recorrido anterior)
            for(int i=0;i<vehiAux.recorrido.len()+1;i++){
                variables.moveToEnd();
                if(variables.getCurr().nodoAsignado.tipo == 'c'){ 
                    clientesRestantes.append(variables.getCurr().nodoAsignado);
                    clientesVisitados.remove(clientesVisitados.find(variables.getCurr().nodoAsignado));
                }
                variables.pop();
            }
            variables.moveToEnd();
            //Luego se guarda el dominio de la última variable del recorrido anterior, y se elimina de la lista
            backtracking = true;
            variableSeAsigno = false; 
            dominioAcotado = variables.getCurr().dominio;
            variables.moveToEnd();
            //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
            if(variables.getCurr().nodoAsignado.tipo == 'c'){ 
                clientesRestantes.append(variables.getCurr().nodoAsignado);
                clientesVisitados.remove(clientesVisitados.find(variables.getCurr().nodoAsignado));
            }
            variables.pop();          
            contadorIter++; 
            continue;
        }
        

        //Se comprueba si estamos entrando al loop por backtracking o no
        if(!backtracking){
            //Si no es backtracking, se declara la nueva variable que se asignará en esta iteración
            variableActual = Variable(clientesRestantes);
        }
        else{
            //Si hicimos backtracking, la variable auxiliar "variableActual" parte con el dominio (acotado)
            //de la variable que se tenía antes al final de la lista.
            variableActual = Variable(dominioAcotado);
            backtracking = false;
        }

        //Se verifica si el recorrido actual ya terminó, en cuyo caso se debe asignar un depot a la nueva variable
        if(vehiAux.recorridoTerminado()){
            //variableActual = Variable(clientes,estaciones);
            variableActual.asignarNodo(depot);
            variables.append(variableActual);
        }
        else{/*******************LOOP SECUNDARIO*******************/


            //Se buscan asignaciones factibles para la nueva variable en un loop. Si el dominio esta vacío o se encontró
            //la asignación, se termina el loop, habiendo asignado la variable.

            //Verificar que no se haya asignado un nodo a la variable, y que su dominio no esté vacío
            while(!variableSeAsigno && !variableActual.dominioVacio()){
                variables.moveToEnd();
                //Si quedan clientes se busca más cercano:
                if(variableActual.dominioTieneCliente()){
                    nodoAux = nodoMenorDistancia(variables.getCurr().nodoAsignado, variableActual.dominioSoloClientes());
                    restriccion = verificarRestricciones(vehiAux,clientesVisitados,nodoAux,depot,variables.getCurr().nodoAsignado,inst);
                    //Se verifica si el cliente a asignar cumple las restricciones
                    if(restriccion == "siCumple"){
                        //Si cumple las restricciones, se asigna el cliente a la variable
                        variableActual.asignarNodo(nodoAux);
                        variableActual.quitarDelDominio(variableActual.nodoAsignado);
                        variables.append(variableActual);
                        variables.moveToEnd();
                        ////Actualizar listas que hacen seguimiento de clientes visitados y restantes 
                        if(variables.getCurr().nodoAsignado.tipo == 'c'){
                            clientesRestantes.remove(clientesRestantes.find(variables.getCurr().nodoAsignado));
                            clientesVisitados.append(variables.getCurr().nodoAsignado);
                        }
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
                    restriccion = verificarRestricciones(vehiAux,clientesVisitados,nodoAux,depot,variables.getCurr().nodoAsignado,inst);
                    variables.moveToEnd();
                    
                    if(restriccion == "siCumple"){
                        //Si la estación cumple todas las restricciones, se pasa por ella.
                        variableActual.asignarNodo(nodoAux);
                        variableActual.quitarDelDominio(variableActual.nodoAsignado);
                        variables.append(variableActual);
                        variableSeAsigno = true;
                    }
                    else if(restriccion == "combustible" || restriccion == "tiempo" || restriccion == "sinSentido"){
                        variableActual.dominio = variableActual.quitarEstacionesDominio();
                        break;  
                    }                    
                }  
            }

            /****CONDICIONES DE BACKTRACKING****/
            distActual = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga).calcularDistTotal();
            if((variableActual.dominioVacio() && variableSeAsigno==false) || distActual > distMejorSolucion){

                //1 Si el dominio de una variable queda vacío, sin haberle asignado un valor factible, significa que el vehiculo 
                //no cumple ninguna restriccion y debe hacerse backtracking
                //
                //2 Si la distancia de la solucion actual es mayor que la mejor solucion, se hace backtracking

                //Chequeamos ademas si la penúltima variable en la lista tienen un depot, volvemos 2 veces hacia atras para evitar loops
                if(variables.getVariable(variables.len()-1).nodoAsignado.tipo=='d' &&
                variables.getVariable(variables.len()-2).nodoAsignado.tipo=='d'){
                    variables.pop();
                    variables.pop();
                }
                backtracking = true;
                variableSeAsigno = false;
                //se quita variable de la lista, pero se guarda su dominio para darselo a la variableActual en la sgte iteracion
                variables.moveToEnd();
                //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
                if(variables.getCurr().nodoAsignado.tipo == 'c'){
                    clientesRestantes.append(variables.getCurr().nodoAsignado);
                    clientesVisitados.remove(clientesVisitados.find(variables.getCurr().nodoAsignado));
                }
                dominioAcotado = variables.getCurr().dominio;
                variables.pop();
                variables.moveToEnd();              
            }
        }/***FIN DEL LOOP SECUNDARIO***/
        contadorIter ++;
        //vehiAux.free();
    }/***FIN DEL LOOP PRINCIPAL***/
    //variables.free();
    return mejorSolucion;
}


/*

*///Si todavía hay clientes, se busca busca solo dentro de los clientes el mas cercano
                    