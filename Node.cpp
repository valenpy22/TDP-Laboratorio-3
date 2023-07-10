#include "Node.h"

/// @brief Método constructor que inicializa un nodo
/// @param simplex Simplex asociado al nodo
Node::Node(Simplex *simplex) {
    this->simplex = simplex;
    this->left = nullptr;
    this->right = nullptr;
    this->parent = nullptr;
}

/// @brief Método destructor que elimina un nodo
Node::~Node() {
    delete simplex;
    delete left;
    delete right;
    delete parent;
}

/// @brief Método que imprime el upper bound de un nodo
void Node::printUpperBound(){
    cout << "Upper bound: ";
    for(int i = 0; i < this->upperBound.size(); i++){
        cout << this->upperBound[i] << " ";
    }
    cout << endl;
}

/// @brief Método que imprime el lower bound de un nodo
void Node::printLowerBound(){
    cout << "Lower bound: ";
    for(int i = 0; i < this->lowerBound.size(); i++){
        cout << this->lowerBound[i] << " ";
    }
    cout << endl;
}

/// @brief Método que determina si se cumple el criterio de las variables
///        que deben ser enteras.
/// @return Booleano que indica si se cumple el criterio.
bool Node::areFloat(){
    vector<int> integers = this->simplex->cb;
    integers.insert(integers.begin(), 0);
    for(int i = 1; i < this->upperBound.size(); i++){
        //Para ver si es flotante, debe aproximarse su número hacia abajo y restarse.
        //Si el resto es igual a 0, entonces es entero.
        for(int j = 0; j < integers.size(); j++){
            if(i == integers[j] && (this->upperBound[i] - floor(this->upperBound[i]) != 0)){
                return true;
            }
        }
    }
    return false;
}

/// @brief Método que encuentra el índice de la variable que está más cerca
///        de ser entera.
/// @return Índice de la variable que está más cerca de ser entera.
int Node::findIndex(){
    vector<float> averagePoints;

    //Por cada variable, se obtiene el punto medio entre el valor 
    //de la aproximación por abajo y la aproximación por arriba
    for(int i = 0; i < this->upperBound.size(); i++){
        float down = floor(this->upperBound[i]);
        float up = ceil(this->upperBound[i]);
        float mid = (down + up)/2;
        averagePoints.insert(averagePoints.end(), mid);
    }

    float resta = 1000000;
    int index = 1;

    for(int i = 1; i < averagePoints.size(); i++){
        float error = abs(averagePoints[i] - this->upperBound[i]);
        if(error < resta && error != 0){
            resta = error;
            index = i;
        }
    }

    return index;
}

/// @brief Método que calcula el upper bound de un nodo.
void Node::calculateUpperBound(){
    this->upperBound = this->simplex->solve();
}

/// @brief Método que calcula el lower bound de un nodo.
void Node::calculateLowerBound(){
    vector<float> LB;

    //Si hay valores flotantes en el UPPER BOUND, se mantiene el lower bound.
    //Para eso, se deben ver qué variables deben ser enteras.

    for(int i = 0; i < this->upperBound.size(); i++){
        LB.insert(LB.end(), floor(upperBound[i]));
    }

    LB[0] = 0;

    for(int i = 1; i <= this->simplex->n; i++){
        LB[0] += LB[i]*this->simplex->initialA[0][i];
    }

    this->lowerBound = LB;
}
