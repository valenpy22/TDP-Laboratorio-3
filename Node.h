#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "Simplex.h"

using namespace std;

#define EPS 1.0e-6

#ifndef NODE_H
#define NODE_H

/*
    * Clase Node
    * Esta clase representa un nodo del árbol de branch and bound.
    * Cada nodo tiene un simplex asociado, un upper bound y un lower bound.
    * Además, tiene punteros a sus hijos izquierdo y derecho, y a su padre.
    * El upper bound y el lower bound son vectores de floats.
    * El simplex es un puntero a un objeto de la clase Simplex.
    * Los punteros a los hijos y al padre son de tipo Node.
    * Los nodos se crean con un simplex asociado.
*/

class Node{
    public:
        //Atributos
        vector<float> upperBound;
        vector<float> lowerBound;
        Simplex* simplex;
        Node* left;
        Node* right;
        Node* parent;

        //Métodos
        Node(Simplex *simplex);
        ~Node();
        void printUpperBound();
        void printLowerBound();
        bool areFloat();
        int findIndex();
        void calculateUpperBound();
        void calculateLowerBound();
};

#endif