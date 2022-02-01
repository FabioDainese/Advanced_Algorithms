#ifndef TENSORLIBRARY_ROTESTER_H
#define TENSORLIBRARY_ROTESTER_H

#include "ROTensor.h"
#include <time.h>

using namespace std;

class ROTester {
public:

    void testPerformance(){
        ROTensor<char,2> tensor(1000, 1000);
        char matrix[1000][1000];

        double num_test = 20;
        double sum = 0;

        cout << "ROTENSOR : Access test 1 --> Using get function"<< endl;

        for(int i = 0; i < num_test; ++i){
            const clock_t start1 = clock();
            for(int r= 0; r < 1000; ++r)
                for(int c = 0; c < 1000; ++c)
                    matrix[r][c] = 'a';
            const clock_t end1 = clock();
            sum += 1000.0 * (end1-start1) / CLOCKS_PER_SEC;
        }
        cout << "Average time with char[1000][1000]: " << sum/num_test << " ms" << endl;

        sum = 0;
        for(int i = 0; i < num_test; ++i){
            const clock_t start = clock();
            for(int r= 0; r < 1000; ++r)
                for(int c = 0; c < 1000; ++c)
                    tensor.set('a', r, c);
            const clock_t end = clock();
            sum += 1000.0 * (end-start) / CLOCKS_PER_SEC;
        }

        cout << "Average time with ROTensor<char,2> tensor(1000, 1000): " << sum/num_test << " ms" << endl;

        cout << endl;
        cout << "ROTENSOR : Access test 2 --> Using operator [] (slicing)"<< endl;

        sum = 0;
        for(int i = 0; i < num_test; ++i){
            const clock_t start1 = clock();
            for(int r= 0; r < 1000; ++r)
                for(int c = 0; c < 1000; ++c)
                    matrix[r][c] = 'a';
            const clock_t end1 = clock();
            sum += 1000.0 * (end1-start1) / CLOCKS_PER_SEC;
        }
        cout << "Average time with char[1000][1000]: " << sum/num_test << " ms" << endl;

        sum = 0;
        for(int i = 0; i < num_test; ++i){
            const clock_t start = clock();
            for(int r= 0; r < 1000; ++r)
                for(int c = 0; c < 1000; ++c)
                    tensor[r][c] = 'a';
            const clock_t end = clock();
            sum += 1000.0 * (end-start) / CLOCKS_PER_SEC;
        }

        cout << "Average time with ROTensor<char,2> tensor(1000, 1000): " << sum/num_test << " ms" << endl;

        cout << endl;
        cout << "ROTENSOR : Access test 3 --> Using iterators"<< endl;

        sum = 0;
        for(int i = 0; i < num_test; ++i){
            const clock_t start1 = clock();
            for(char*  r= &(matrix[0][0]); r < &(matrix[0][0]) + 1000*1000; r = r + 1)
                *r = 'a';
            const clock_t end1 = clock();
            sum += 1000.0 * (end1-start1) / CLOCKS_PER_SEC;
        }
        cout << "Average time with char[1000][1000]: " << sum/num_test << " ms" << endl;

    }
};


#endif //TENSORLIBRARY_ROTESTER_H
