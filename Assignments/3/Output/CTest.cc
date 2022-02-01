#include <iostream>
#include <chrono>
#include <unistd.h>
#include"tensor.h"

using namespace Tensor;

std::ostream & operator << (std::ostream& out, Index_Set<>) { return out; }
template<unsigned id, unsigned... ids>
std::ostream & operator << (std::ostream& out, Index_Set<id, ids...>) {
    return out << id << ' ' << Index_Set<ids...>();
}

void test_multiplication(int nw = 1){
    set_workers_number(nw);
    auto start = std::chrono::steady_clock::now();

    auto i=new_index;
    auto j=new_index;
    auto k=new_index;
    tensor<int> t23(2,3),t32(3,2);

    int count = 0;
    for(auto iter=t23.begin(); iter!=t23.end(); ++iter)
        *iter = count++;
    count=0;
    for(auto iter=t32.begin(); iter!=t32.end(); ++iter)
        *iter = count++;

    tensor<int,rank<2>> tres=t23(i,k)*t32(k,j);
    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "Elapsed time: " << std::chrono::duration<double, std::milli>(time).count() << " ms.\n";
    std::cout << std::endl;
}

void test_long_multiplication(int nw = 1){
    set_workers_number(nw);
    auto start = std::chrono::steady_clock::now();

    auto i=new_index;
    auto j=new_index;
    auto k=new_index;
    tensor<int,rank<2>> t1(2,3), t2(3,2);
    tensor<int> tres(2);

    for(auto iter=t1.begin(); iter!=t1.end(); ++iter)
        *iter = 1;
    for(auto iter=t2.begin(); iter!=t2.end(); ++iter)
        *iter = 1;

    tres(k)=t1(k,j)*t1(i,i)*t2(j,j)*t2(j,j)*t2(j,j)*t1(i,i)*t2(j,j)*t2(j,j)*t1(i,i)*t2(j,j)*t1(i,i)*t2(j,j)*t1(i,i)*t2(j,j)*t1(i,i);
    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "Elapsed time: " << std::chrono::duration<double, std::milli>(time).count() << " ms.\n";
    std::cout << std::endl;
}

void test_long_data_multiplication(int nw = 1){
    set_workers_number(nw);
    auto start = std::chrono::steady_clock::now();

    auto i=new_index;
    auto j=new_index;
    auto k=new_index;
    tensor<int,rank<2>> t1(1000,3), t2(3,1000);

    for(auto iter=t1.begin(); iter!=t1.end(); ++iter)
        *iter = 1;
    for(auto iter=t2.begin(); iter!=t2.end(); ++iter)
        *iter = 1;

    tensor<int,rank<2>> tres=t1(i,k)*t2(k,j);
    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "Elapsed time: " << std::chrono::duration<double, std::milli>(time).count() << " ms.\n";
    std::cout << std::endl;
}


void test_sum(int nw = 1){
    set_workers_number(nw);
    auto start = std::chrono::steady_clock::now();

    auto i=new_index;
    auto j=new_index;
    tensor<int> t23(2,3), t32(3,2), tsum(2,3);

    int count = 0;
    for(auto iter=t32.begin(); iter!=t32.end(); ++iter)
        *iter = count++;
    count = 0;
    for(auto iter=tsum.begin(); iter!=tsum.end(); ++iter)
        *iter = count++;

    tsum(i,j) = t23(i,j)+t32(j,i);
    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "Elapsed time: " << std::chrono::duration<double, std::milli>(time).count() << " ms.\n";
    std::cout << std::endl;
}

void test_long_sum(int nw = 1){
    set_workers_number(nw);
    auto start = std::chrono::steady_clock::now();

    auto i=new_index;
    auto j=new_index;
    auto k=new_index;
    tensor<int,rank<5>> t1(2,2,8,200,100);
    tensor<int,rank<4>> t3(2,2,8,300);

    for(auto iter=t1.begin(); iter!=t1.end(); ++iter)
        *iter = 1;
    for(auto iter=t3.begin(); iter!=t3.end(); ++iter)
        *iter = 1;

    tensor<int> t4 = t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k)+t3(i,j,k,k)+t1(i,j,k,k,k);
    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "Elapsed time: " << std::chrono::duration<double, std::milli>(time).count() << " ms.\n";
    std::cout << std::endl;
}

void test_invert(int nw = 1){
    set_workers_number(nw);
    auto start = std::chrono::steady_clock::now();
    tensor<int,rank<3>> t1(4,40,3), t2(3,40,4);
    int val = 0;
    int pcount = 0;
    for(auto iter=t1.begin(); iter!=t1.end(); ++iter)
        *iter = val++;
    std::cout << '\n';
    auto i=new_index;
    auto j=new_index;
    auto k=new_index;

    t2(k,j,i) = t1(i,j,k);
    pcount = 0;

    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "Elapsed time: " << std::chrono::duration<double, std::milli>(time).count() << " ms.\n";
    std::cout << std::endl;

}

// Aggregation of tests per operation type

void sum_tests(){
//    // sum
//    std::cout << "Sum test: " << std::endl;
//    for (int i = 1; i <=8  ; ++i) {
//        test_sum(i);
//        sleep(1);
//    }
    std::cout << "________________________________ " << std::endl;
    std::cout << "Big sum tests: " << std::endl;
    for (int i = 1; i <=8  ; ++i) {
        test_long_sum(i);
        sleep(1);
    }
    std::cout << "________________________________ " << std::endl;
}

void multiplication_tests(){
    std::cout << "Multiplication test: " << std::endl;
    for (int i = 1; i <=8  ; ++i) {
        test_multiplication(i);
        sleep(1);
    }
    std::cout << "________________________________ " << std::endl;
    std::cout << "Long Multiplication test: " << std::endl;
    for (int i = 1; i <=8  ; ++i) {
        test_long_multiplication(i);
        sleep(1);
    }
    std::cout << "________________________________ " << std::endl;

//    for (int i = 1; i <=8  ; ++i) {
//        test_long_data_multiplication(i);
//        sleep(1);
//    }
//    std::cout << "________________________________ " << std::endl;
}

void inversion_tests(){
    std::cout << "Inversion test: " << std::endl;
    for (int i = 1; i <=8  ; ++i) {
        test_invert(i);
        sleep(1);
    }
    std::cout << "________________________________ " << std::endl;
}

int main(){
//    inversion_tests();
    sum_tests();
    multiplication_tests();
    return 0;
}