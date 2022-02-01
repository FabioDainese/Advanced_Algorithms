#include "NKTester.h"
#include "ROTester.h"

int main() {
    cout << "-------------------------------------" << endl;
    cout << "| RUNNING A SIMPLE PERFORMANCE TEST |" << endl;
    cout << "-------------------------------------" << endl << endl;
    cout << ">> Keep in mind that it may take a while!" << endl << endl;
    
    try {
        NKTester test;
        cout << "------------------------" << endl;
        cout << "[1] Performance NKTensor" << endl;
        cout << "------------------------" << endl << endl;
        test.testPerformance();

        ROTester rotest;
        cout << endl << "------------------------" << endl;
        cout << "[2] Performance ROTensor" << endl;
        cout << "------------------------" << endl << endl;
        rotest.testPerformance();

        cout << endl << ">> Nessun errore!"<< endl;
    }
    catch(exception e){
        cout << ">> Qualche errore: " << e.what() << endl;
    }

    return 0;
}