#include "Simplex.h"

/*
Se genera el simplex a partir de un archivo de texto. 
El archivo debe tener la siguiente estructura:

m1 m2 m3
0 c1 c2 ... cn
b1 a11 a12 ... a1n
b2 a21 a22 ... a2n
...
bm am1 am2 ... amn

Donde:
m1: numero de restricciones de tipo 1 (<=)
m2: numero de restricciones de tipo 2 (>=)
m3: numero de restricciones de tipo 3 (==)
c1, c2, ..., cn: coeficientes de la funcion a maximizar
b1, b2, ..., bm: constantes de las restricciones
ai1, ai2, ..., ain: coeficientes de la restriccion i-esima
con i=1,..,m1+m2+m3
Las m1 primeras restricciones son de tipo 1 (<=)
Las m2 siguientes son de tipo 2 (>=)
Las m3 ultimas son de tipo 3 (==)
*/

/// @brief Constructor de la clase Simplex
/// @param fileName Nombre del archivo de texto que contiene el problema
Simplex::Simplex(string fileName){
    string line;
    fstream file(fileName);
    stringstream charStream;
    string token;

    if(!file.is_open()){
        cout << "No se pudo abrir el archivo." << endl;
        exit(1);
    }
    // leyendo m1,m2,m3 en primera linea
    getline(file, line);
    charStream << line;
    getline(charStream, token, ' ');
    this->m1 = stoi(token);
    getline(charStream, token, ' ');
    this->m2 = stoi(token);
    getline(charStream, token, ' ');
    this->m3 = stoi(token);

    //Leyendo la segunda línea que incluye los índices de las variables restringidas a enteros
    getline(file, line);
    charStream.clear();
    charStream << line;
    while(getline(charStream, token, ' ')){
        this->cb.push_back(stoi(token));
    }

    // leyendo la matrix que incluye c,a,b: (m+1) x (n+1), donde n es el numero de variables y m=m1+m2+m3 el numero de restricciones
    while(getline(file, line)){
        charStream.clear();
        charStream << line;
        vector<float> row;
        while(getline(charStream, token, ' ')){
            row.push_back(stof(token));
        }
        this->a.push_back(row);
    }

    this->n = a[0].size() - 1;
    this->m = m1 + m2 + m3;
    // si hay una cantidad distinta de resticciones a las indicadas: Error!
    if(m != (int)a.size() - 1){
        cout<<"Error en la cantidad de restricciones."<<endl;
        exit(1);
    }

    // si hay una cantidad distinta de variables a las indicadas: Error!
    for(int i = 1; i <= m; i++){
        if(a[i][0] < 0.0 || (int)a[i].size() != n + 1){
            cout<<"Error en la cantidad de variables o constante menor que 0:" << i <<endl;
            exit(1);
        }
    }

    // EL metodo requiere una fila adicional al final de la matriz para calculos intermedios
    vector<float> row;
    for(int i = 0; i <= n; i++){
        row.push_back(0.0);
    }

    this->a.push_back(row);
    this->initialA = a;
    this->isSolved = false; // inicialmente aún no se ha resuelto
    file.close();
}

/// @brief Constructor de la clase Simplex
/// @param a Matriz que contiene el problema
/// @param m1 Restricciones de tipo 1 (<=)
/// @param m2 Restricciones de tipo 2 (>=)
/// @param m3 Restricciones de tipo 3 (==)
Simplex::Simplex(vector<vector<float>> a, int m1, int m2, int m3){
    int m = m1 + m2 + m3;
    for(int i = 1; i <= m; i++){
        if(a[i][0] < 0.0){
            // Error al crear el Simplex, ya que no se permiten
            // constantes negativas para las restricciones.
            throw invalid_argument("La matriz ingresada no es válida.");
        }
    }

    this->initialA = a;
    this->a = a;
    this->m1 = m1;
    this->m2 = m2;
    this->m3 = m3;
    this->m = m;
    this->n = a[0].size() - 1;
    this->isSolved = false;
}

/// @brief Destructor de la clase Simplex
Simplex::~Simplex(){
}


/// @brief Método que ejecuta el algoritmo simplex
/// @return Vector con la solución del problema
vector<float> Simplex::solve(){
    icase = simplx();
    isSolved = true;
    if(icase != 0){ // No hay solucion
        vector<float> emptyVector;
        return emptyVector; 
    }

    vector<float> parameters(n + 1, 0.0); // vector de tamaño n+1 inicializado en 0
    parameters[0] = a[0][0]; // valor de Z resultante
    for(int i = 0; i < m; i++){
        if(iposv[i] < n){ // si el indice corresponde a una variable (y no a una slack variable)
            parameters[iposv[i] + 1] = a[i + 1][0]; // se guarda el valor de la variable
        }
    }
    solution = parameters; // se guarda la solucion en el atributo solution

    return parameters;
}

/// @brief Método que ejecuta el algoritmo simplex
/// @return Estado de la solución: 0 si se encontró una solución, 1 si no hay solución, 2 si hay infinitas soluciones
int Simplex::simplx(){
    int i, ip, is, k, kh, kp, nl1;
    float q1, bmax;
    vector<int> l1;
    vector<int> l3;
    nl1 = n;
    // inicializacion de l1, l3, izrov, iposv
    izrov.clear();
    iposv.clear();
    for(k = 0; k < n; k++){
        l1.push_back(k);
        izrov.push_back(k);
    }
    for(i = 0; i < m; i++){
        iposv.push_back(n + i);
    }
    // Caso que origen no es factible
    if (m2 + m3){
        for(i = 0; i < m2; i++){ // inicializando m2 variables de holgura
            l3.push_back(1);
        }
        for(k = 0; k < n + 1; k++){ // computando nueva funcion Z
            q1 = 0.0;
            for(i = m1; i < m; i++){
                q1 += a[i + 1][k];
            }
            a[m + 1][k] = -q1;
        }
        for (;;){
            maxValue(m + 1, l1, nl1, 0, &kp, &bmax); // buscando la variable con mayor coeficiente en la funcion Z
            if(bmax <= EPS && a[m + 1][0] < -EPS){ // no hay solucion factible (Z auxiliar < 0)
                return -1; // icase -1
            
            }else if(bmax <= EPS && a[m + 1][0] <= EPS){ // solucion factible (Z auxiliar = 0)
                for(ip = m1 + m2; ip < m; ip++){
                    if(iposv[ip] == ip + n){ // se busca la variable de holgura correspondiente
                        maxValue(ip, l1, nl1, 1, &kp, &bmax);
                        if(bmax > EPS){
                            goto one;
                        }
                    }
                }
                for(i = m1; i < m1 + m2; i++){ // se cambia el signo de la restriccion de variables aun presentes
                    if(l3[i - m1] == 1){
                        for(k = 0; k < n + 1; k++){
                            a[i + 1][k] = -a[i + 1][k];
                        }
                    }
                }
                break;
            }
            locatePivot(&ip, kp);  // se busca el pivote
            if(ip == -1){ // caso no acotado (Z es infinito)
                return -1; // icase -1
            }
        one:
            exchangeParameters(m + 1, n, ip, kp); // intercambia variables
            if(iposv[ip] >= (n + m1 + m2)){
                for(k = 0; k < nl1; k++){
                    if(l1[k] == kp){
                        break;
                    }
                }
                --nl1;
                for(is = k; is < nl1; is++){
                    l1[is] = l1[is + 1];
                }
            }
            else {
                kh = iposv[ip] - m1 - n;
                if(kh >= 0 && l3[kh]){
                    l3[kh] = 0;
                    ++a[m + 1][kp + 1];
                    for(i = 0; i < m + 2; i++){
                        a[i][kp + 1] = -a[i][kp + 1];
                    }
                }
            }
            is = izrov[kp];
            izrov[kp] = iposv[ip];
            iposv[ip] = is;
        }
    }

    for (;;){
        maxValue(0, l1, nl1, 0, &kp, &bmax);
        if (bmax <= EPS){
            return 0; // icase = 0
        }
        locatePivot(&ip, kp);
        if (ip == -1){
            return 1; // icase  = 1
        }
        exchangeParameters(m, n, ip, kp);
        is = izrov[kp];
        izrov[kp] = iposv[ip];
        iposv[ip] = is;
    }
}

/// @brief Método que determina el máximo valor de una fila de la matriz a
/// @param mm Fila de la matriz a
/// @param ll Vector de índices de las variables que son cero en la solución
/// @param nll Número de variables que son cero en la solución
/// @param iabf Indica si se busca el máximo valor de la fila o el máximo valor absoluto
/// @param kp Índice de la variable con el máximo valor
/// @param bmax Máximo valor de la fila
void Simplex::maxValue(int mm, vector<int> ll, int nll, int iabf, int *kp, float *bmax){
    int k;
    float test;

    if(nll <= 0){
        *bmax = 0.0;

    }else{
        *kp = ll[0];
        *bmax = a[mm][*kp + 1];
        for(k = 1; k < nll; k++){
            if(iabf == 0){
                test = a[mm][ll[k] + 1] - (*bmax);
            }else{
                test = fabs(a[mm][ll[k] + 1]) - fabs(*bmax);
            }

            if(test > 0.0){
                *bmax = a[mm][ll[k] + 1];
                *kp = ll[k];
            }
        }
    }
}

/// @brief Método que encuentra la fila en donde se encuentra el elemento que sirve como pivote
/// @param ip Entero que indica la fila en donde se encuentra el elemento que sirve como pivote
/// @param kp Entero que indica la columna en donde se encuentra el elemento que sirve como pivote
void Simplex::locatePivot(int *ip, int kp){

    int k, i;
    float qp, q0, q, q1;

    kp = kp + 1;
    *ip = -1;
    for(i = 0; i < m; i++){
        if(a[i + 1][kp] < -EPS){
            break;
        }
    }

    if(i + 1 > m){
        return;
    }

    q1 = -a[i + 1][0] / a[i + 1][kp];
    *ip = i;
    for(i = *ip + 1; i < m; i++){
        if(a[i + 1][kp] < -EPS){
            q = -a[i + 1][0] / a[i + 1][kp];
            if(q < q1){
                *ip = i;
                q1 = q;
            }else if(q == q1){
                qp = -1;
                q0 = -1;
                for(k = 0; k < n; k++){
                    qp = -a[*ip + 1][k + 1] / a[*ip + 1][kp];
                    q0 = -a[i + 1][k + 1] / a[i + 1][kp];
                    if(q0 != qp){
                        break;
                    }
                }
                if(q0 < qp){
                    *ip = i;
                }
            }
        }
    }
}

/// @brief Método que intercambia variables izq y der
/// @param i1 Fila de la matriz a
/// @param k1 Columna de la matriz a
/// @param ip Fila de la matriz a
/// @param kp Columna de la matriz a
void Simplex::exchangeParameters(int i1, int k1, int ip, int kp){
    int kk, ii;
    float piv;

    piv = 1.0 / a[ip + 1][kp + 1];
    for(ii = 0; ii < i1 + 1; ii++){
        if(ii - 1 != ip){
            a[ii][kp + 1] *= piv;
            for(kk = 0; kk < k1 + 1; kk++){
                if(kk - 1 != kp){
                    a[ii][kk] -= a[ip + 1][kk] * a[ii][kp + 1];
                }
            }
        }
    }

    for(kk = 0; kk < k1 + 1; kk++){
        if(kk - 1 != kp){
            a[ip + 1][kk] *= -piv;
        }
    }

    a[ip + 1][kp + 1] = piv;
}

/// @brief Método que inserta una restricción en la matriz a
/// @param b Constante de la restricción
/// @param var Índice de la variable
/// @param type Tipo de la restricción
/*
Inserta una restricción en la matriz A del tipo
var <=b tipo 1
var >=b tipo 2
var = b tipo 3
*/
void Simplex::insertConstraint(float b, int var, int type){
    if(var == 0 || var > n || b < 0.0){
        return;
    }

    vector<float> constraint(n + 1, 0.0);
    constraint[0] = b;
    constraint[var] = -1.0;

    switch(type){
        case 1:
            this->initialA.insert(this->initialA.begin() + m1 + 1, constraint);
            m1++;
            break;
        case 2:
            this->initialA.insert(this->initialA.begin() + m1 + m2 + 1, constraint);
            m2++;
            break;
        case 3:
            this->initialA.insert(this->initialA.begin() + m + 1, constraint);
            m3++;
            break;
        default:
            return;
            break;
    }
    m++;
    isSolved = false; // La solucion ya no es valida
    a = initialA;    // Se copia la matriz inicial a la matriz de trabajo
    solution.clear(); // Se limpia la solucion
}

/// @brief Método que copia el simplex
/// @return Simplex copiado
Simplex* Simplex::copy(){
    Simplex *s = new Simplex(initialA, m1, m2, m3);
    s->a = a;
    s->cb = cb;
    s->izrov = izrov;
    s->iposv = iposv;
    s->isSolved = isSolved;
    s->icase = icase;
    s->solution = solution;
    return s;
}

/// @brief Método que retorna la solución del problema
/// @return Vector con la solución del problema
vector<float> Simplex::getSolution(){
    if(isSolved && icase == 0){
        return solution;
    }
    vector<float> empty;
    return empty;
}

/// @brief Método que imprime la matriz del problema
void Simplex::printProblemMatrix(){
    cout << "Variables restringidas a enteros: ";
    for(int i = 0; i < cb.size(); i++){
        cout << cb[i] << " ";
    }
    cout << endl;
    
    cout << "Matriz del problema: " << endl;
    for(size_t i = 0; i < initialA.size(); i++) {
        for(size_t j = 0; j < initialA[0].size(); j++){
            string aij = to_string(initialA[i][j]);
            aij = aij.substr(0, aij.find(".") + 3);
            aij.insert(aij.begin(), 10 - aij.length(), ' ');
            cout << aij;
        }
        cout << endl;
    }
}

/// @brief Método que imprime la solución del problema
void Simplex::printSolution(){
    if(isSolved){
        int nm1m2;
        if(icase == 1){
            cout << "Caso no acotado -> solucion infinita." << endl;
        
        }else if(icase == -1){
            cout << "Solucion vacia." << endl;
        
        }else{
            nm1m2 = n + m1 + m2;
            string txt[nm1m2];
            for(int i = 0; i < n; i++){
                txt[i] = "x" + to_string(i + 1);
            }
            for(int i = n; i < nm1m2; i++){
                txt[i] = "y" + to_string(i + 1 - n);
            }

            cout << string(11, ' ');
            for(int i = 0; i < n; i++){
                if(izrov[i] < nm1m2){
                    txt[izrov[i]].insert(txt[izrov[i]].begin(), 10 - txt[izrov[i]].length(), ' ');
                    cout << txt[izrov[i]];
                }
            }

            cout << endl;
            for(int i = 0; i < m + 1; i++){
                if(i == 0 || iposv[i - 1] < nm1m2){
                    if(i > 0){
                        cout << txt[iposv[i - 1]];
                    }else{
                        cout << "  ";
                    }

                    string ai0 = to_string(a[i][0]);
                    ai0 = ai0.substr(0, ai0.find(".") + 3);
                    ai0.insert(ai0.begin(), 10 - ai0.length(), ' ');
                    cout << ai0;
                    for(int j = 1; j < n + 1; j++){
                        if(izrov[j - 1] < nm1m2){
                            string aij = to_string(a[i][j]);
                            aij = aij.substr(0, aij.find(".") + 3);
                            aij.insert(aij.begin(), 10 - aij.length(), ' ');
                            cout << aij;
                        }
                    }
                    cout << endl;
                }
            }
        }
    }else{
        cout << "No está resuelto." << endl;
    }
} 
