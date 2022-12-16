#include <iostream>
#include <list>
#include <stack>
#include <thread>
#include <time.h> 
#include <stdlib.h>
#include <cstdlib>
#include <math.h> 
#include <sstream>
#include <semaphore>
#include <atomic>
#include <mutex>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <map>
#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

#define A 0 // Nodos
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define I 8
#define J 9
#define K 10
#define L 11
#define M 12
#define N 13
#define O 14
#define P 15
#define Q 16
#define KH 17

#define X 32 // Threads a crear

using namespace std;

int *mejor_path; // Un arreglo que contiene todos los nodos de la mejor ruta
int mejor_mi_size = -1;
int mejor_index;

atomic<int> ready_flag = 0; // Si es 1 los threads se detendran
mutex g_mejor; // Lo usare para actualizar las variables mejor_ en una zona critica
vector<unique_ptr<counting_semaphore<3>>> sem; // Vector de semaforos para controlar cuantos threads pueden usar cada ruta

int vec[KH][KH]; // Para evitar usar un vector en 2D, usare un arreglo en 2D con indices en el vector para cada ruta
// Cada ruta tiene una direccion del arreglo vec[A][B] con la direccion del vector que contiene el semaforo especifico de esa ruta

void Acquire_Sem(int u, int p) // Redusco el contador del semaforo
{
    int i = vec[u][p]; // Consigo el indice de la ruta entre los nodos u y p
    sem[i]->acquire(); // Este es el semaforo perteneciente a la ruta entre u y p
}

void Release_Sem(int a, int u) // Incremento el contador del semaforo
{
    int i = vec[a][u]; // Consigo el indice de la ruta entre los nodos a y u
    sem[i]->release(); // Este es el semaforo perteneciente a la ruta entre a y u
}

class Ruta // Las rutas quedan en una lista
{
	int v; // El nodo al que lleva esta ruta
	int peso; // El peso de la ruta
public:
	Ruta(int _v, int _w) { v = _v; peso = _w;}
	int getV()	 { return v; }
	int getPeso() { return peso; }
};

class Grafo
{
	int V; // Cuantos nodos tengo en total

	list<Ruta> *rutas; // Todas las rutas

    void Recorre_Util(int u, int d, int path[], int& path_index, int peso[], int a);

public:
	Grafo(int V);

	void AddRuta(int u, int v, int peso);

    void Recorre(int s, int d);
};

Grafo::Grafo(int V)
{
	this->V = V;
	rutas = new list<Ruta>[V];
}

void Grafo::AddRuta(int u, int v, int peso)
{
	Ruta nodo(v, peso);
	rutas[u].push_back(nodo);
}

void Grafo::Recorre(int s, int d)
{
	int* path = new int[V]; // path es el arreglo con todos los nodos por los que he pasado
	int* peso = new int[V]; // peso es el arreglo con el peso de todas las rutas por las que hemos pasado
	int path_index = 0; // path_index es un contador con la cantidad de nodos por los cuales hemos pasado
    int a = s;

	while((ready_flag.load(memory_order_acquire) == 0)) // Si la variable atomica es cero entonces sigo buscando rutas, si no es cero entonces me detengo
        Recorre_Util(s, d, path, path_index, peso, a);
}

void Grafo::Recorre_Util(int u, int d, int path[], int& path_index, int peso[], int a)
{ // El nodo actual es u, el nodo anterior es a
    path[path_index] = u;
	path_index++; // Incremento por cuantos nodos e pasado
	int mi_size; // Tamaño es el peso total de todas las rutas
    if(u == d) // Para cuando llegue al destino
    {
        mi_size = 0;
		for (int i = 1; i < path_index; i++)
		{
			mi_size += peso[i]; // Tamaño tomara la suma del peso de todas las rutas
		}

        if (mejor_mi_size > mi_size || mejor_mi_size < 0) // Si tengo una ruta mejor entrare a la zona critica, si no entonces sigo de largo
		{
            g_mejor.lock(); // Para hacer cambios al mejor entro a la zona critica
            if (mejor_mi_size > mi_size || mejor_mi_size < 0) // Vuelvo a correr el if para acegurar de que en lo que me demore en entrar a la zona critica otro thread no halla cambiado por un valor mejor
            {
                mejor_mi_size = mi_size;
                mejor_index = path_index;
                mejor_path = new int[path_index];
                for (int i = 0; i < path_index; i++)
                    {
                        mejor_path[i] = path[i]; // Agregare la ruta al arreglo de mejor ruta
                    }
                cout << "Mejor Ruta encontrada: ";
                for (int i = 0; i < mejor_index; i++)
                            cout << mejor_path[i] << " ";
                cout << endl;
                cout << "Mejor peso: " << mejor_mi_size << endl;
            }
            g_mejor.unlock(); // Salgo de la zona critica
		}
        //cout << "E: 3" << endl;
        Release_Sem(a, u);  // Libero la ruta que acabo de usar
        //cout << "E: 4" << endl;
    }

    else // Si todabia no llego al destino entonces entro aca
    {
        int c = 0; // Un contador simple
        //list<Ruta>::iterator i;
        int arr[V]; // Guardare los nodos a los que puedo avanzar aca
        int weight[V]; // Guardare los pesos de las rutas que puedo tomar aca
        for (list<Ruta>::iterator i = rutas[u].begin(); i != rutas[u].end(); ++i) // Veo las rutas disponibles del nodo actual
        {
            arr[c] = i->getV(); // Guardo los nodos para los cuales tengo rutas
            weight[c] = i->getPeso(); // Guardo los pesos de las rutas
            c++; // Incremento el contador
        }

        //cout << "E: 7" << endl;
        #ifdef _WIN32
        thread::id t_id = this_thread::get_id();
        stringstream ss;
        ss << t_id;
        int id = stoi(ss.str());
        srand (time(0));
        int r = (id*rand())%c; // Escojo de manera aleatoria a que nodo viajare
        #else
        srand (time(0));
        int r = (rand())%c; // Escojo de manera aleatoria a que nodo viajare
        #endif

        //cout << "E: 8" << endl;
        peso[path_index] = weight[r]; // Guardo el peso de esta ruta en el arreglo
        //cout << "E: 9" << endl;
        Acquire_Sem(u, arr[r]); // Antes de entrar a la ruta la reservo en el semaforo
        //cout << "E: 10" << endl;
        Release_Sem(a, u); // Tras reservar la nueva ruta libero la ruta anterior

        a = u; // El nodo actual en que estoy se volvera el nodo anterior

        //cout << "E: 11" << endl;
        Recorre_Util(arr[r], d, path, path_index, peso, a); // Procedo con la recursion buscando llegar al destino KH
    }
    path_index--; // Esto no deberia servir, pero si lo quito aveces deja de funcionar
}

void Iniciar(Grafo *g, int s, int d) // Esta funcion es solo para iniciar el recorrido
{
    g->Recorre(s, d); // Inicio el recorrido
}

void Terminar() // Esta funcion es para terminar los threads
{
    #ifdef _WIN32
    g_mejor.lock();
    cout << endl << "Presione una tecla para terminar . . ." << endl << endl;
    g_mejor.unlock();
    _getch();
    //system("pause");
    #else
    g_mejor.lock();
    cout << endl << "Presione <Enter> para terminar . . ." << endl << endl;
    g_mejor.unlock();
    cin.get();
    #endif
    ready_flag.store(1, memory_order_release);
}

int main()
{
    for (int i = 0; i < KH; i++)
        for (int j = 0; j < KH; j++)
            vec[i][j] = j + (i * KH); // En este arreglo guardare un indice para todas las posibles rutas entre nodos
            // Por ejemplo la ruta entre 0 (A) y 1 (B) tendra un indice 1
            // Mientras que una ruta teorica entre B y A tendra un indice de 18 si KH es 17

    sem.reserve(KH*KH); // Reservo espacio para los semaforos de todas las rutas posibles

    while (sem.size() < sem.capacity()) // Mientras el vector tenga espacio sigo agregando vectores
        sem.emplace_back(make_unique<counting_semaphore<3>>(3)); // Agrego un nuevo semaforo al vector
    
	Grafo g(18); // Declaro cuantos nodos tengo

    g.AddRuta(A, B, 6); // Agrego las rutas entre los nodos
    g.AddRuta(B, C, 2);
    g.AddRuta(B, D, 10);
    g.AddRuta(B, E, 3);
    g.AddRuta(C, F, 4);
    g.AddRuta(D, H, 10);
    g.AddRuta(E, H, 8);
    g.AddRuta(F, G, 3);
    g.AddRuta(F, I, 6);
    g.AddRuta(G, L, 5);
    g.AddRuta(H, J, 7);
    g.AddRuta(H, K, 5);
    g.AddRuta(I, L, 7);
    g.AddRuta(J, M, 8);
    g.AddRuta(K, M, 9);
    g.AddRuta(K, N, 1);
    g.AddRuta(K, O, 9);
    g.AddRuta(L, P, 7);
    g.AddRuta(M, Q, 4);
    g.AddRuta(N, P, 2);
    g.AddRuta(O, Q, 8);
    g.AddRuta(P, Q, 1);
    g.AddRuta(Q, KH, 4);

    thread terminar;
    terminar = thread(Terminar);

    thread myThreads[X];
    for (int i = 0; i < X; i++)
    {
        myThreads[i] = thread(Iniciar, &g, A, KH); // Inicio los threads
    }

    for (int i = 0; i < X; i++)
    {
        myThreads[i].join(); // Uno los threads
    }
    terminar.join();

	cout << endl;
    cout << "Mejor Ruta de " << A << " (A)" << " a " << KH << " (KH)" << ": ";
	for (int i = 0; i < mejor_index; i++)
				cout << mejor_path[i] << " ";
	cout << endl;
	cout << "Mejor peso: " << mejor_mi_size << endl;

	return 0;
}