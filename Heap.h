#include <iostream>
#include "Node.h"
using namespace std;

#ifndef HEAP_H
#define HEAP_H

/*
    * Clase Heap
    * Esta clase representa un heap de nodos
    * que se utiliza para el algoritmo branch and bound.
    * El heap está implementado como un arreglo de punteros
    * a nodos.
*/

class Heap {
    public:
        // Atributos
        Node **data;
        int size; // current size
        int capacity; // max size

        // Métodos
        Heap(int capacity);
        ~Heap();
        bool empty();
        void swap(int i, int j);
        int left_idx(int i);
        int right_idx(int i);
        int parent_idx(int i);
        Node* pop();
        void push(Node *s);
        void heapify(int i);
};

#endif