#include <iostream>
#include <list>
#include <stack>
#include <thread>
#include <time.h> 
#include <stdlib.h>
#include <cstdlib>
#include <math.h> 
#include <sstream>
#include <atomic>
#include <mutex>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

#define A 0
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

#define X 32

using namespace std;

int *mejor_path;
int mejor_tamaño = -1;
int mejor_index;
atomic<int> ready_flag = 0;
mutex g_mejor;

class Ruta
{
	int v;
	int peso;
public:
	Ruta(int _v, int _w) { v = _v; peso = _w;}
	int getV()	 { return v; }
	int getPeso() { return peso; }
};

class Grafo
{
	int V;

	list<Ruta> *rutas;

    void Recorre_Util(int u, int d, int path[], int& path_index, int peso[]);

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
	int* path = new int[V];
	int* peso = new int[V];
	int path_index = 0;

	while((ready_flag.load(memory_order_acquire) == 0))
        Recorre_Util(s, d, path, path_index, peso);
}

void Grafo::Recorre_Util(int u, int d, int path[], int& path_index, int peso[])
{
    path[path_index] = u;
	path_index++;
    //cout << "Index: " << path_index << endl;
	int tamaño;
    if(u == d)
    {
        tamaño = 0;
		for (int i = 1; i < path_index; i++)
		{
			tamaño += peso[i];
		}
        //cout << tamaño << endl;

		/*if (mejor_tamaño > tamaño || mejor_tamaño < 0)
		{
			mejor_tamaño = tamaño;
			mejor_index = path_index;
			mejor_path = new int[path_index];
			for (int i = 0; i < path_index; i++)
				{
					mejor_path[i] = path[i];
				}
		}*/
    }

    else
    {
        int c = 0;
        //list<Ruta>::iterator i;
        int arr[V];
        int weight[V];
        for (list<Ruta>::iterator i = rutas[u].begin(); i != rutas[u].end(); ++i)
        {
            arr[c] = i->getV();
            weight[c] = i->getPeso();
            c++;
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
        int r = (rand())%c;
        #endif
        //cout << "E: 8" << endl;
        peso[path_index] = weight[r];
        //cout << "E: 9" << endl;
        Recorre_Util(arr[r], d, path, path_index, peso);
    }
    path_index--;
}

void Iniciar(Grafo *g, int s, int d)
{
    g->Recorre(s, d);
}

void Terminar()
{
    #ifdef _WIN32
    cout << "Presione una tecla para terminar . . ." << endl;
    _getch();
    //system("pause");
    #else
    cout << "Presione <Enter> para terminar . . ." << endl;
    cin.get();
    #endif
    ready_flag.store(1, memory_order_release);
}

int main()
{
	Grafo g(18);

    g.AddRuta(A, B, 6);
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
        myThreads[i] = thread(Iniciar, &g, A, KH);
    }

    for (int i = 0; i < X; i++)
    {
        myThreads[i].join();
    }
    terminar.join();

	/*cout << "Mejor Ruta de " << A << " (A)" << " a " << KH << " (KH)" << ": ";
	for (int i = 0; i < mejor_index; i++)
				cout << mejor_path[i] << " ";
	cout << endl;
	cout << "Mejor peso: " << mejor_tamaño << endl;*/

	return 0;
}