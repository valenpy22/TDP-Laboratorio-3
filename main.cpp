#include "Simplex.h"
#include "Heap.h"
#include "Node.h"
#include "BB.h"

int main(){
    string filename;

    cout << "Ingrese el nombre del archivo a resolver: ";
    cin >> filename;

    Simplex *simplex1 = new Simplex(filename);

    Node *node = new Node(simplex1);
    BB *bb = new BB();
    bb->branchAndBound(node);
}
