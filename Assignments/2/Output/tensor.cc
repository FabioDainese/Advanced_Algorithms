#include<iostream>
#include<chrono>

#include"tensor.h"
#include"proxy.h"


/**
 * Test function of the product between two tensors with one common dimension
 */

/**
 * Test function of the product between two tensors with one common dimension
 */
void tensor_product_test()
{
    tensor::tensor<int> t23(2, 3);
    tensor::tensor<int> t32(3, 2);

    int count = 0;
    for (auto iter = t23.begin(); iter != t23.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    count = 0;
    for (auto iter = t32.begin(); iter != t32.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    proxy_labeled_tensor<int, char> sp_t1(t23, {'n', 'm'});
    proxy_labeled_tensor<int, char> sp_t2(t32, {'m', 'p'});

    tensor::tensor<int> result = static_cast<tensor::tensor<int>>(sp_t1 * sp_t2);
    std::cout << "result: ";
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;
}

/**
 * Test function of the product between two tensors with two common dimensions
 */
void multi_dimension_tensor_product_test()
{
    tensor::tensor<int> t232(2, 3, 2);
    tensor::tensor<int> t32(3, 2);

    int count = 0;
    for (auto iter = t232.begin(); iter != t232.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    count = 0;
    for (auto iter = t32.begin(); iter != t32.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    proxy_labeled_tensor<int, char> sp_t1(t232, {'n', 'm', 'o'});
    proxy_labeled_tensor<int, char> sp_t2(t32, {'m', 'n'});

    tensor::tensor<int> result = static_cast<tensor::tensor<int>>(sp_t1 * sp_t2);
    std::cout << "result: ";
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;
}

void tensor_sum_test()
{
    tensor::tensor<int> t23(2, 3);
    tensor::tensor<int> t32(3, 2);

    int count = 0;
    for (auto iter = t23.begin(); iter != t23.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    count = 6;
    for (auto iter = t32.begin(); iter != t32.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    proxy_labeled_tensor<int, char> sp_t1(t23, {'n', 'm'});
    proxy_labeled_tensor<int, char> sp_t2(t32, {'m', 'n'});

    tensor::tensor<int> result = static_cast<tensor::tensor<int>>(sp_t1 + sp_t2);
    std::cout << "result: ";
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;
}

/**
 * Test function of a product of two tensor summed to a tensor. The result will be a multi tensor trace.
 */
void tensor_multi_operations_test()
{
    tensor::tensor<int> t23(2, 3);
    tensor::tensor<int> t32(3, 2);
    tensor::tensor<int> t22(2, 2);

    int count = 0;
    for (auto iter = t23.begin(); iter != t23.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    count = 0;
    for (auto iter = t32.begin(); iter != t32.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    count = 0;
    for (auto iter = t22.begin(); iter != t22.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    proxy_labeled_tensor<int, char> sp_t1(t23, {'n', 'm'});
    proxy_labeled_tensor<int, char> sp_t2(t32, {'m', 'p'});
    proxy_labeled_tensor<int, char> sp_t3(t22, {'n', 'p'});

    tensor::tensor<int> result = static_cast<tensor::tensor<int>>(sp_t1 * sp_t2 * sp_t3);
    std::cout << "result: ";
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;
}

/**
 * Test function of the trace of a 3x3 tensor.
 */
void tensor_trace_test()
{
    // t33 e' un oggetto di tipo tensor
    tensor::tensor<int> t33(3, 3);
    int count = 0;
    for (auto iter = t33.begin(); iter != t33.end(); ++iter)
    {
        *iter = ++count;
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
    // lo popolo con numeri da 1 a 9 (3x3)

    //gli attacco le labels
    proxy_labeled_tensor<int, char> sp_t1(t33, {'n', 'n'});

    //static conversion da labeled tensor a normal tensor
    tensor::tensor<int> result = static_cast<tensor::tensor<int>>(sp_t1);
    std::cout << "result: ";
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;
}

int main() {

    std::cout << "Tensor product test\n";
    tensor_product_test();

    std::cout << "\n\nTensor product multi dimensional test\n";
    multi_dimension_tensor_product_test();

    std::cout << "\n\nTensor sum test\n";
    tensor_sum_test();

    std::cout << "\n\nTensor multi operations test\n";
    tensor_multi_operations_test();

    std::cout << "\n\nTensor trace test\n";
    tensor_trace_test();

    return 0;

}
