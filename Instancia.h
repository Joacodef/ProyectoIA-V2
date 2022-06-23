using namespace std;

//Clase que representa a la instancia:

class Instancia{
    public:
        string nombre;
        int numClientes;
        int numEstaciones;
        int maxTiempo;
        double maxDistancia;
        double velocidad;
        int tiempoServicio;
        int tiempoRecarga;

        Instancia();
        void mostrar();     
};

Instancia::Instancia(){
    nombre = "";
    numClientes = 0;
    numEstaciones = 0;
    maxTiempo = 0;
    maxDistancia = 0.0;
    velocidad = 0.0;
    tiempoServicio = 0;
    tiempoRecarga = 0;
}

void Instancia::mostrar(){
    cout << "\nNombre: " << nombre;
    cout << "\nNumClientes: " << numClientes;
    cout << "\nNumEstaciones: " << numEstaciones;
    cout << "\nMaxTiempo: " << maxTiempo;
    cout << "\nMaxDistancia: " << maxTiempo;
    cout << "\nVelocidad: " << velocidad;
    cout << "\nTiempoServicio: " << tiempoServicio;
    cout << "\nTiempoRecarga: " << tiempoRecarga << "\n";
}
