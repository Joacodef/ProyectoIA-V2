#include "ArchivosIO.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

vector<vector<double>> matrizDistancias(ListaNodos nodos);

string verificarRestricciones(Vehiculo vehiculoDelNodo, ListaNodos clientesVisitados, Nodo nodoPorAsignar, 
                              Nodo depot, Nodo anterior, Instancia inst, vector<vector<double>> matrizDist){
    string porQueNoCumple = "siCumple";
    //cout << vehiculoDelNodo.distanciaDesdeRecarga<<"\n";
    //Verificar si se tiene combustible para llegar al nodo asignado:
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


ListaVehiculos generarSoluciones(int maxIteraciones, Instancia inst, ListaNodos clientes, 
                                  ListaNodos estaciones, Nodo depot, ListaNodos nodos){
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

        /*********PARA RUTEO**********/

        //variables.printNodos();

        /******Verificar si se visitaron todos los clientes (nueva solución candidata)******/
        if(clientesVisitados.len()>=abs(inst.numClientes)){

            //Determinar si nueva solución es mejor
            Variable depotFinal;
            depotFinal.asignarNodo(depot);
            variables.append(depotFinal);

            solucionCandidata = variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
            distActual = solucionCandidata.calcularDistTotal();

            //cout << "\n\n---------SOLUCION CANDIDATA-----------\n";
            //solucionCandidata.mostrar();

            if(distActual < distMejorSolucion){
                //cout << "\n\nSolucion mejorada\n";
                mejorSolucion = solucionCandidata;
                distMejorSolucion = distActual;
            }
            //Quitar todos las variables correspondientes al ultimo recorrido, más el depot del último recorrido (backtracking hasta recorrido anterior)
            for(int i=0;i<vehiAux.recorrido.len()+1;i++){
                if(variables.getLast().nodoAsignado.tipo == 'c'){ 
                    clientesRestantes.append(variables.getLast().nodoAsignado);
                    clientesVisitados.remove(clientesVisitados.find(variables.getLast().nodoAsignado));
                }
                variables.pop();
            }
            //Luego se guarda el dominio de la última variable del recorrido anterior, y se elimina de la lista
            backtracking = true;
            variableSeAsigno = false; 
            dominioAcotado = variables.getLast().dominio;
            //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
            if(variables.getLast().nodoAsignado.tipo == 'c'){ 
                clientesRestantes.append(variables.getLast().nodoAsignado);
                clientesVisitados.remove(clientesVisitados.find(variables.getLast().nodoAsignado));
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
                //Si quedan clientes se busca más cercano:
                if(variableActual.dominioTieneCliente()){
                    nodoAux = nodoMenorDistancia(variables.getLast().nodoAsignado, variableActual.dominioSoloClientes(),inst.numEstaciones,matrizDist);
                    restriccion = verificarRestricciones(vehiAux,clientesVisitados,nodoAux,depot,variables.getLast().nodoAsignado,inst, matrizDist);
                    //Se verifica si el cliente a asignar cumple las restricciones
                    if(restriccion == "siCumple"){
                        //Si cumple las restricciones, se asigna el cliente a la variable
                        variableActual.asignarNodo(nodoAux);
                        variableActual.quitarDelDominio(variableActual.nodoAsignado);
                        variables.append(variableActual);
                        //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
                        if(variables.getLast().nodoAsignado.tipo == 'c'){
                            clientesRestantes.remove(clientesRestantes.find(variables.getLast().nodoAsignado));
                            clientesVisitados.append(variables.getLast().nodoAsignado);
                        }
                        variableSeAsigno = true;
                    }
                    else if(restriccion == "combustible"){
                        //Si no cumple restriccion de combustible, se deben quitar clientes del dominio, para ir estacion
                        variableActual.dominio = variableActual.quitarClientesDominio();
                    }
                    else if(restriccion == "tiempo"){
                        //Si no cumple restriccion de tiempo, se revisa si el depot está a una distancia tal que no se exceda
                        //la distanciaMax permitida por más de 30 millas (dando un rango razonable de error). Si es así, se debe 
                        //hacer BT y probar con nodos que estén más cerca del depot.
                        //AGREGAR ALGO PARA QUE SE LIMITEN EN LA VARIABLE ANTERIOR LOS NODOS QUE ESTÉN MÁS LEJOS DEL DEPOT
                        int posMatriz = variables.getLast().nodoAsignado.ID;
                        if(variables.getLast().nodoAsignado.tipo=='f') posMatriz+=1;
                        else if(variables.getLast().nodoAsignado.tipo=='c') posMatriz+=inst.numEstaciones;
                        double distAlDepot = matrizDist[posMatriz][0];
                        
                        if(vehiAux.distanciaDesdeRecarga() + distAlDepot - inst.maxDistancia > 30){
                            variableActual.dominio = ListaNodos();
                            break;                        
                        }
                        //Chequear si alcanza el tiempo para volver al depot directo, de lo contrario hace BT
                        if(distAlDepot/inst.velocidad + vehiAux.tiempoTranscurrido() > inst.maxTiempo){
                            variableActual.dominio = ListaNodos();
                            break;  
                        }
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
                    
                    nodoAux = nodoMenorDistancia(variables.getLast().nodoAsignado,variableActual.dominio,inst.numEstaciones, matrizDist);
                    restriccion = verificarRestricciones(vehiAux,clientesVisitados,nodoAux,depot,variables.getLast().nodoAsignado,inst, matrizDist);
                    
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
                //Actualizar listas que hacen seguimiento de clientes visitados y restantes 
                if(variables.getLast().nodoAsignado.tipo == 'c'){
                    clientesRestantes.append(variables.getLast().nodoAsignado);
                    clientesVisitados.remove(clientesVisitados.find(variables.getLast().nodoAsignado));
                }
                dominioAcotado = variables.getLast().dominio;
                variables.pop();
                variables.moveToEnd();              
            }
        }/***FIN DEL LOOP SECUNDARIO***/
        contadorIter ++;
    }/***FIN DEL LOOP PRINCIPAL***/
    if(mejorSolucion.len()==0){
        variableActual.asignarNodo(depot);
        variables.append(variableActual);
        return variables.extraerSolucionActual(inst.velocidad,inst.tiempoServicio,inst.tiempoRecarga);
    }
    return mejorSolucion;
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