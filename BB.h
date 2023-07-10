#include "Node.h"
#include "Simplex.h"
#include "Heap.h"

#define EPS 1.0e-6

using namespace std;

/*
    * Clase BB (Branch And Bound)
    * Esta clase representa el algoritmo branch and bound, el cual
    * se encarga de resolver problemas de programación lineal entera
    * mediante la técnica de ramificación y acotación.
*/

class BB{
    public:
        //Atributos
        Heap *pending;

        //Métodos
        BB();
        ~BB();
        void branchAndBound(Node *node);
        bool areEqual(Node *n, Node *p);
};