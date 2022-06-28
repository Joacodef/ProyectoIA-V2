#include "ArchivosIO.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

string verificarRestricciones(Vehiculo vehiculoDelNodo, ListaNodos clientesVisitados, Nodo nodoPorAsignar, 
                              Nodo depot, Nodo anterior, Instancia inst,vector<vector<double>> matrizDist);

void verificarSolEncontrada(Nodo depot, Instancia inst, ListaVariables *variables, double *distMejorSolucion,ListaVehiculos *mejorSolucion, 
                            Vehiculo vehiAux, ListaNodos *clientesRestantes, ListaNodos *clientesVisitados, ListaNodos *dominio);

void buscarAsignacionVariable(bool *variableSeAsigno, Variable *variableActual,ListaVariables *variables, Instancia inst,
                            ListaNodos *clientesVisitados, ListaNodos *clientesRestantes, Vehiculo vehiAux, Nodo depot, 
                            vector<vector<double>> matrizDist);

vector<vector<double>> matrizDistancias(ListaNodos nodos);


// Funcion para generar recorridos para un vehiculo, de acuerdo con restricciones del GVRP

ListaVehiculos generarSoluciones(int maxIteraciones, Instancia inst, ListaNodos clientes, 
                                ListaNodos estaciones, Nodo depot, ListaNodos nodos){
    int contadorIter = 0;   
    Variable variableActual;
    ListaNodos dominioAcotado,clientesRestantes,clientesVisitados;
    ListaVariables variables;
    ListaVehiculos mejorSolucion;
    bool backtracking = false;
    bool variableSeAsigno = false;
    double distMejorSolucion = INFINITY;
    double distActual = 0.0;
    vector<vector<double>> matrizDist = matrizDistancias(nodos);

    Vehiculo vehiAux = Vehiculo(inst.velocidad, inst.tiempoServicio, inst.tiempoRecarga);
    variableActual.asignarNodo(depot);
    variables.append(variableActual);
    clientesRestantes = concatenar(clientes,estaciones);

    /***************LOOP PRINCIPAL******************/
    while(contadorIter < maxIteraciones){

        //vehiAux guardará al principio de cada iteración el recorrido en el que se esté en esta asignación
        variableSeAsigno = false;
        vehiAux = variables.recorridoDeVariable(variables.getLast(),inst.velocidad, 
                                                inst.tiempoServicio,inst.tiempoRecarga);

        /*PARA RUTEO*/
        //variables.printNodos();
        
        //Si se visitaron todos los clientes:
        if(clientesVisitados.len()>=abs(inst.numClientes)){
            verificarSolEncontrada(depot,inst,&variables,&distMejorSolucion,&mejorSolucion,vehiAux,
                                &clientesRestantes,&clientesVisitados,&dominioAcotado);
            contadorIter++;
            backtracking = true;
            variableSeAsigno = false;  
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
        else{
            /**LOOP SECUNDARIO**/
            buscarAsignacionVariable(&variableSeAsigno,&variableActual,&variables,inst,&clientesVisitados,&clientesRestantes,
                                    vehiAux,depot, matrizDist);

            /****CONDICIONES DE BACKTRACKING****/
            distActual = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga).calcularDistTotal();
            if((variableActual.dominioVacio() && variableSeAsigno==false) || distActual > distMejorSolucion){
                if(variables.getVariable(variables.len()-1).nodoAsignado.tipo=='d' &&
                variables.getVariable(variables.len()-2).nodoAsignado.tipo=='d'){
                    variables.pop();
                    variables.pop();
                }
                backtracking = true;
                variableSeAsigno = false;
                //se quita variable de la lista, pero se guarda su dominio para darselo a la variableActual en la sgte iteracion
                //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
                if(variables.getLast().nodoAsignado.tipo == 'c'){
                    clientesRestantes.append(variables.getLast().nodoAsignado);
                    clientesVisitados.remove(clientesVisitados.find(variables.getLast().nodoAsignado));
                }
                dominioAcotado = variables.getLast().dominio;
                variables.pop();
                variables.moveToEnd();              
            }
        }
        contadorIter ++;
    }/***FIN DEL LOOP PRINCIPAL***/

    if(mejorSolucion.len()==0){
        variableActual.asignarNodo(depot);
        variables.append(variableActual);
        return variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
    }

    return mejorSolucion;
}



//Funciones utilizadas por GeneradorSoluciones


string verificarRestricciones(Vehiculo vehiculoDelNodo, ListaNodos clientesVisitados, Nodo nodoPorAsignar, 
                            Nodo depot, Nodo anterior, Instancia inst, vector<vector<double>> matrizDist){
    string porQueNoCumple = "siCumple";
    //Verificar si se tiene combustible para llegar al nodo asignado
    double distRecarga = vehiculoDelNodo.distanciaDesdeRecarga();
    if(distRecarga>inst.maxDistancia){
        //Agregar cosas para quitar del dominio a otros clientes
        return "combustible";
    }
    //Verificar si se tiene tiempo para volver al depósito:
    int posMatriz = nodoPorAsignar.ID;
    if(nodoPorAsignar.tipo=='f') posMatriz+=1;
    else if(nodoPorAsignar.tipo=='c') posMatriz+=inst.numEstaciones;
    double distanciaDeposito = matrizDist[posMatriz][0];
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



void verificarSolEncontrada(Nodo depot, Instancia inst, ListaVariables *variables, double *distMejorSolucion, ListaVehiculos *mejorSolucion,
                            Vehiculo vehiAux, ListaNodos *clientesRestantes, ListaNodos *clientesVisitados, ListaNodos *dominio){
    //Determinar si nueva solución es mejor
    Variable depotFinal;
    double distActual;
    depotFinal.asignarNodo(depot);
    variables->append(depotFinal);
    ListaVehiculos solucionCandidata;

    solucionCandidata = variables->extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
    distActual = solucionCandidata.calcularDistTotal();

    //cout << "\n\n---------SOLUCION CANDIDATA-----------\n";
    //solucionCandidata.mostrar();
    if(distActual < *distMejorSolucion){
        //cout << "\n\nSolucion mejorada\n";
        *mejorSolucion=solucionCandidata;
        *distMejorSolucion = distActual;
    }
    //Quitar todos las variables correspondientes al ultimo recorrido, más el depot del último recorrido (backtracking hasta recorrido anterior)
    for(int i=0;i<vehiAux.recorrido.len()+1;i++){
        if(variables->getLast().nodoAsignado.tipo == 'c'){ 
            clientesRestantes->append(variables->getLast().nodoAsignado);
            clientesVisitados->remove(clientesVisitados->find(variables->getLast().nodoAsignado));
        }
        variables->pop();
    }
    //Luego se guarda el dominio de la última variable del recorrido anterior, y se elimina de la lista
    
    *dominio = variables->getLast().dominio;
    //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
    if(variables->getLast().nodoAsignado.tipo == 'c'){ 
        clientesRestantes->append(variables->getLast().nodoAsignado);
        clientesVisitados->remove(clientesVisitados->find(variables->getLast().nodoAsignado));
    }
    variables->pop();      
}



void buscarAsignacionVariable(bool *variableSeAsigno, Variable *variableActual,ListaVariables *variables, Instancia inst,
                            ListaNodos *clientesVisitados, ListaNodos *clientesRestantes, Vehiculo vehiAux, Nodo depot, 
                            vector<vector<double>> matrizDist){
    //Verificar que no se haya asignado un nodo a la variable, y que su dominio no esté vacío
    Nodo nodoAux;
    string restriccion;
    while(!(*variableSeAsigno) && !variableActual->dominioVacio()){
        //Si quedan clientes se busca más cercano:
        if(variableActual->dominioTieneCliente()){
            nodoAux = nodoMenorDistancia(variables->getLast().nodoAsignado, variableActual->dominioSoloClientes());
            restriccion = verificarRestricciones(vehiAux,*clientesVisitados,nodoAux,depot,variables->getLast().nodoAsignado,inst,matrizDist);
            //Se verifica si el cliente a asignar cumple las restricciones
            if(restriccion == "siCumple"){
                //Si cumple las restricciones, se asigna el cliente a la variable
                variableActual->asignarNodo(nodoAux);
                variableActual->quitarDelDominio(variableActual->nodoAsignado);
                variables->append(*variableActual);
                //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
                if(variables->getLast().nodoAsignado.tipo == 'c'){
                    clientesRestantes->remove(clientesRestantes->find(variables->getLast().nodoAsignado));
                    clientesVisitados->append(variables->getLast().nodoAsignado);
                }
                *variableSeAsigno = true;
            }
            else if(restriccion == "combustible"){
                //Si no cumple restriccion de combustible, se deben quitar clientes del dominio, para ir estacion
                variableActual->dominio = variableActual->quitarClientesDominio();
            }
            else if(restriccion == "tiempo"){
                //Si no cumple restriccion de tiempo, se revisa si el depot está a una distancia tal que no se exceda
                //la distanciaMax permitida por más de 30 millas (dando un rango razonable de error). Si es así, se debe 
                //hacer BT y probar con nodos que estén más cerca del depot.
                //AGREGAR ALGO PARA QUE SE LIMITEN EN LA VARIABLE ANTERIOR LOS NODOS QUE ESTÉN MÁS LEJOS DEL DEPOT
                double distAlDepot= calcularDistancia(variables->getLast().nodoAsignado, depot);
                if(vehiAux.distanciaDesdeRecarga() + distAlDepot - inst.maxDistancia > 30){
                    variableActual->dominio = ListaNodos();
                    break;                        
                }
                //Chequear si alcanza el tiempo para volver al depot directo, de lo contrario hace BT
                if(distAlDepot/inst.velocidad + vehiAux.tiempoTranscurrido() > inst.maxTiempo){
                    variableActual->dominio = ListaNodos();
                    break;  
                }
                variableActual->asignarNodo(depot);
                variables->append(*variableActual);
                *variableSeAsigno = true;
            }
            else if(restriccion == "asignado"){
                //Si no cumple porque esta asignado a otro cliente, se quita de su dominio
                variableActual->quitarDelDominio(nodoAux);
            }
        }
        else{ //Si no quedan clientes en el dominio de variableActual
            
            nodoAux = nodoMenorDistancia(variables->getLast().nodoAsignado,variableActual->dominio);
            restriccion = verificarRestricciones(vehiAux,*clientesVisitados,nodoAux,depot,variables->getLast().nodoAsignado,inst,matrizDist);
            
            if(restriccion == "siCumple"){
                //Si la estación cumple todas las restricciones, se pasa por ella.
                variableActual->asignarNodo(nodoAux);
                variableActual->quitarDelDominio(variableActual->nodoAsignado);
                variables->append(*variableActual);
                *variableSeAsigno = true;
            }
            else if(restriccion == "combustible" || restriccion == "tiempo" || restriccion == "sinSentido"){
                variableActual->dominio = variableActual->quitarEstacionesDominio();
                break;  
            }                    
        }  
    }
}


vector<vector<double>> matrizDistancias(ListaNodos nodos){
    vector<vector<double>> matriz;
    vector<double> vect;
    for(int i=0;i<nodos.len();i++){
        matriz.push_back(vect);
        for(int j=0;j<nodos.len();j++){
            //if(i<j){
                matriz[i].push_back(calcularDistancia(nodos.getNodo(i),nodos.getNodo(j)));
            /*}
            else{
                matriz[i].push_back(0);
            }*/
        }
    }
    return matriz;
}
