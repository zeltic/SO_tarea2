#include<iostream>
#include <list>
#include <stack>

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

using namespace std;

int *mejor_path;
int mejor_tamaño = -1;
int mejor_index;

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

	void Mejor_Ruta_Util(int u, int d, bool visitado[], int path[], int& path_index, int peso[]);

public:
	Grafo(int V);

	void AddRuta(int u, int v, int peso);

	void Mejor_Ruta(int s, int d);
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

void Grafo::Mejor_Ruta(int s, int d)
{
	bool* visitado = new bool[V];

	int* path = new int[V];
	int* peso = new int[V];
	int path_index = 0;

	for (int i = 0; i < V; i++)
		visitado[i] = false;

	Mejor_Ruta_Util(s, d, visitado, path, path_index, peso);
}

void Grafo::Mejor_Ruta_Util(int u, int d, bool visitado[], int path[], int& path_index, int peso[])
{
	visitado[u] = true;
	path[path_index] = u;
	path_index++;
	int tamaño;

	if (u == d)
	{
		tamaño = 0;
		for (int i = 1; i < path_index; i++)
		{
			tamaño += peso[i];
			//cout << peso[i] << " ";
		}

		if (mejor_tamaño > tamaño || mejor_tamaño < 0)
		{
			mejor_tamaño = tamaño;
			mejor_index = path_index;
			mejor_path = new int[path_index];
			for (int i = 0; i < path_index; i++)
				{
					cout << path[i] << " ";
					mejor_path[i] = path[i];
				}
		}

		else
			for (int i = 0; i < path_index; i++)
				cout << path[i] << " ";

		cout << endl;

		cout << "Peso: " << tamaño << endl;
		cout << endl;
	}
	else
	{
		list<Ruta>::iterator i;
		for (i = rutas[u].begin(); i != rutas[u].end(); ++i)
		{
			peso[path_index] = i->getPeso();
			Ruta nodo = *i;
			if (!visitado[nodo.getV()])
				Mejor_Ruta_Util(nodo.getV(), d, visitado, path, path_index, peso);
		}
	}

	path_index--;
	visitado[u] = false;
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

	cout << "Rutas de " << A << " a " << KH << ":" << endl << endl;
    g.Mejor_Ruta(A, KH);

	cout << "Mejor ruta: ";
	for (int i = 0; i < mejor_index; i++)
				cout << mejor_path[i] << " ";
	cout << endl;
	cout << "Mejor peso: " << mejor_tamaño << endl << endl;

	return 0;
}