#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#define EPS 1.0e-6

using namespace std;

#ifndef SIMPLEX_H
#define SIMPLEX_H

/*
    * Clase Simplex
    * Esta clase representa las restricciones de un problema 
    * de ecuaciones 

*/

class Simplex{ 
public:
    int m; // Numero total de restricciones
    int n; // Numero de variables
    bool isSolved; // Indica si el metodo solve ha sido llamado.
    int icase; // Indica el estado de la solucion del problema despues de solve, 0:Solucion encontrada 1:Solucion no acotada -1:Solucion no factible
    vector<int> izrov; // indice de variables que son cero en la solucion
    vector<int> iposv; // indice de variables que constituyen la solucion. Si indice >=n entonces la variable es artificial y no se cuenta en la solucion
    vector<float> solution;
    int simplx(); // Metodo que ejecuta el algoritmo simplex
    void maxValue(int mm, vector<int> ll, int nll, int iabf, int *kp, float *bmax);
    void locatePivot(int *ip, int kp);
    void exchangeParameters(int i1, int k1, int ip, int kp);

    vector<vector<float>> initialA; // Matriz inicial  --> de aqui pueden calcular el LB
    vector<vector<float>> a; //***** Matriz del problema: incluye c,a,b inicialmente se transforma a la solucion por piveoteo*****
    vector<int> cb; // indice de variables basicas
    int m1; // numero de restricciones <=
    int m2; // numero de restricciones >=
    int m3; // numero de restricciones =

    Simplex(string filename); // Constructor con archivo de entrada
    Simplex(vector<vector<float>> a, int m1, int m2, int m3); // Constructor con matriz de entrada
    ~Simplex();
    vector<float> solve(); // Metodo que ejecuta metodo simplex
    void insertConstraint(float b, int var, int type); // Agrega una restriccion var<=b (tipo 1), var>=b (tipo 2), var=b (tipo 3) 
    Simplex* copy();
    vector<float> getSolution(); // Retorna la solucion del problema de tamaño n+1 [Z,x1,...,xn] Z: valor funcion objetivo, xi: valor de la variable i
    void printProblemMatrix(); // Imprime la matriz del problema
    void printSolution(); // Imprime la solucion del problema)
};

#endif // SIMPLEX_H