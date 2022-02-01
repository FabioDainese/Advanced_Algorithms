#include "Utility.h"

/*Used to print vectors content*/
void showVector(vector<int>& v){
    cout << "{";
    for(auto i = v.begin(); i < v.end(); ++i){
        cout <<  *i;
        if(i != v.end() - 1)
            cout << ", ";
    }
    cout << "}";
}

void showVector(int v[], int dim){
    cout << "{";
    for(int i = 0; i < dim; ++i){
        cout <<  v[i];
        if(i != dim - 1)
            cout << ", ";
    }
    cout << "}";
}
