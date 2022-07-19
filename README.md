# ProyectoIA

Utilizar comando "make instancia=<instancia elegida>", donde se debe especificar en "<instancia elegida>" el nombre de la instancia que se revisará, sin la extensión de archivo. Ejemplos: "make instancia=AB101", "make instancia=AB220".

Utilizar "make clean" para limpiar el archivo de output.

Se necesita que estén todos los archivos ".h" y el ".cpp" en la misma carpeta.

Se necesita que estén las carpetas "Instancias" con la instancia especificada en el argumento del comando

Se necesita que esté la carpeta "Soluciones" (vacía o no vacía) para generar las soluciones.


Algunas explicaciones sobre el código:

Nodo.h - Contiene la clase Nodo y ListaNodos. La clase Nodo almacena la información de ID, tipo y coordenadas de los nodos que se reciben en los archivos. ListaNodos provee una interfaz para manipular un arreglo de Nodos (utilizando la clase "Vector" de C++). Se incluyen también funciones que facilitan calcular la distancia entre dos nodos o buscar el nodo que este a menor distancia de un nodo central, respecto de una lista de nodos.

Vehiculo.h - Contiene la clase Vehiculo y ListaVehiculos. La clase Vehiculo almacena un ID, un recorrido (representado en un objeto ListaNodos), la cantidad de clientes visitados en su recorrido, su velocidad, tiempo de servicio y tiempo de recarga. Además, posee funciones para agregar nodos a su recorrido, verificar si su recorrido terminó, calcular cuánto tiempo ha durado su recorrido, calcular la distancia total que ha recorrido, calcular la distancia desde que recargó combustible y verificar si los ultimos dos nodos en su recorrido han sido paradas en estaciones. La clase ListaVehiculos provee una interfaz para manipular un arreglo de Vehiculos, ademas de permitir calcular la distancia total recorrida entre todos los vehículos.

Variable.h - Contiene la clase Variable y ListaVariables. La clase Variable permite almacenar un Nodo (nodoAsignado) junto con un dominio (entendido desde la perspectiva del backtracking, las variables poseen un dominio); esta clase permite separar la lógica del problema, en cuanto a vehículos que realizan recorridos pasando por nodos, respecto de la lógica del método para resolver el problema, es decir, la manera en que opera el método del backtracking. Posee funciones para quitar nodos especificos del dominio (como los clientes, las estaciones o un nodo específico), considerar sólo algunos nodos sin modificar el dominio, o verificar si el dominio posee solo clientes. La clase ListaVariables provee una interfaz para manipular un arreglo de Variables, pero también tiene importancia en que representa la manera en que se almacena la solución al problema.

GeneradorSoluciones.h - Realiza el proceso de backtracking, junto con la verificación de restricciones. Posee la funcion generarSoluciones, que es la que recibe toda la información de la instancia, y realiza los loops principal y secundario, donde se asignan variables, se acotan dominios, se hacen saltos atrás y, finalmente, se genera la solución que será entregada en el output. Es asistida por la función verificarRestricciones, que se encarga de verificar cuándo una variable puede ser asignada y agregada a la solución, según las reglas establecidas en el GVRP.

Instancia.h - Contiene la clase Instancia, que almacena datos sobre la instancia recibida en el input, esto es, su nombre, número de clientes, número de estaciones, máximo tiempo permitido para los recorridos, máxima distancia que pueden recorrer los vehículos antes de tener que recargar combustible, velocidad a la que se mueven los vehículos, y los tiempos de demora para el servicio al cliente y la recarga de combustible.

ArchivosIO.h - Contiene funciones para leer el archivod e input y generar un archivo de output.