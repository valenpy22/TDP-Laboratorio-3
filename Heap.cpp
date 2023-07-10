#include "Heap.h"
#include "Node.h"

/// @brief Constructor de la clase Heap
/// @param capacity Capacidad inicial del heap
Heap::Heap(int capacity) {
    this->capacity = capacity;
    data = new Node*[capacity];
    size = 0;
}

/// @brief Destructor de la clase Heap
Heap::~Heap() {
    delete[] data;
}

/// @brief Método que determina si un heap está vacío.
/// @return Booleano que indica si el heap está vacío.
bool Heap::empty(){
    return size == 0;
}

/// @brief Método que intercambia dos nodos del heap.
/// @param i Índice del primer nodo.
/// @param j Índice del segundo nodo.
void Heap::swap(int i, int j) {
    Node *temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}

/// @brief Método que retorna el índice del hijo izquierdo de un nodo.
/// @param i Índice del nodo.
/// @return Índice del hijo izquierdo.
int Heap::left_idx(int i) {
    return 2*i + 1;
}

/// @brief Método que retorna el índice del hijo derecho de un nodo.
/// @param i Índice del nodo.
/// @return Índice del hijo derecho.
int Heap::right_idx(int i) {
    return 2*i + 2;
}

/// @brief Método que retorna el índice del padre de un nodo.
/// @param i Índice del nodo.
/// @return Índice del padre.
int Heap::parent_idx(int i) {
    return (i-1)/2;
}

/// @brief Método que retorna el nodo con mayor upper bound del heap.
/// @return Nodo con mayor upper bound.
Node* Heap::pop() {
    if (size == 0) {
        return nullptr;
    }
    Node *s = data[0];
    data[0] = data[size-1];
    size--;
    heapify(0);
    return s;
}

/// @brief Método que agrega un nodo al heap.
/// @param s Nodo a agregar.
void Heap::push(Node *s) {
    if (size == capacity) {
        Node **temp= new Node*[capacity*2];
        for(int i=0; i<size; i++){
            temp[i] = data[i]; 
        }
        delete[] data;
        data = temp;
        capacity = capacity*2;
    }
    data[size] = s;
    size++;
    int i = size-1;
    while (i > 0) {
        int p = parent_idx(i);
        if (data[i]->upperBound[0] > data[p]->upperBound[0]) {
            swap(i, p);
            i = p;
        } else {
            break;
        }
    }
}

/// @brief Método que reordena el heap.
/// @param i Índice del nodo a reordenar.
void Heap::heapify(int i) {
    int l = left_idx(i);
    int r = right_idx(i);
    int smallest = i;
    if (l < size && data[l]->upperBound[0] > data[i]->upperBound[0]) {
        smallest = l;
    }
    if (r < size && data[r]->upperBound[0] > data[smallest]->upperBound[0]) {
        smallest = r;
    }
    if (smallest != i) {
        swap(i, smallest);
        heapify(smallest);
    }
}