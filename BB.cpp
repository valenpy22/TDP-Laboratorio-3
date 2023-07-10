#include "BB.h"

using namespace std;

/// @brief Método constructor que inicializa el heap
BB::BB() {
    this->pending = new Heap(1);
}

/// @brief Método destructor que elimina el heap
BB::~BB() {
    delete pending;
}

/// @brief Método que ejecuta el algoritmo branch and bound
/// @param n Nodo raíz
void BB::branchAndBound(Node *n){

    n->calculateUpperBound();
    n->calculateLowerBound();

    pending->push(n);

    while(!pending->empty()){
        Node *node = pending->pop();

        int cont = 0;

        if(!node->areFloat()){
            cout << "SOLUCIÓN ÓPTIMA ENCONTRADA: " << node->upperBound[0] << endl;
            node->simplex->printSolution();
            cout << "Variables: " << endl;
            for(int i = 1; i < node->upperBound.size(); i++){
                cout << "x" << i << " = " << node->upperBound[i] << endl;
            }
            exit(1);    
        }

        int index = node->findIndex();

        Simplex *simplex2 = node->simplex->copy();
        Simplex *simplex3 = node->simplex->copy();

        simplex2->insertConstraint(floor(node->upperBound[index]), index, 1);
        vector<float> a = simplex2->solve();

        if(a.size() == 0){
            cout << "Infeasible" << endl;
        }else{
            Node *left = new Node(simplex2);
            left->parent = node;
            left->upperBound = a;
            left->calculateLowerBound();

            if(left->areFloat()){
                left->lowerBound = node->lowerBound;
            }

            if(areEqual(node, left)){
                cout << "SOLUCIÓN ÓPTIMA ENCONTRADA: " << node->upperBound[0] << endl;
                node->simplex->printSolution();
                cout << "Variables: " << endl;
                for(int i = 1; i < node->upperBound.size(); i++){
                    cout << "x" << i << " = " << node->upperBound[i] << endl;
                }
                exit(1);
            }

            pending->push(left);
        }

        simplex3->insertConstraint(ceil(node->upperBound[index]), index, 2);
        vector<float> b = simplex3->solve();
        
        if(b.size() == 0){
            cout << "Infeasible" << endl;
        }else{
            Node *right = new Node(simplex3);
            right->upperBound = b;
            right->parent = node;
            right->calculateLowerBound();

            if(right->areFloat()){
                right->lowerBound = node->lowerBound;
            }

            if(areEqual(node, right)){
                cout << "SOLUCIÓN ÓPTIMA ENCONTRADA: " << node->upperBound[0] << endl;
                node->simplex->printSolution();
                cout << "Variables: " << endl;
                for(int i = 1; i < node->upperBound.size(); i++){
                    cout << "x" << i << " = " << node->upperBound[i] << endl;
                }
                exit(1);
            }
            
            pending->push(right);
        }
    }
    cout << "No existe solución" << endl;
}

/// @brief Método que verifica si dos nodos son iguales
/// @param n Nodo 1
/// @param p Nodo 2
/// @return Booleano que indica si los nodos son iguales
bool BB::areEqual(Node *n, Node *p){
    for(int i = 0; i < n->upperBound.size(); i++){
        if(n->upperBound[i] != p->upperBound[i]){
            return false;
        }
        if(n->lowerBound[i] != p->lowerBound[i]){
            return false;
        }
    }
    return true;
}