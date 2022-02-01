#ifndef TENSORLIBRARY_ROITERATORONEDIM_H
#define TENSORLIBRARY_ROITERATORONEDIM_H

#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Utility.h"

using namespace std;

template <class T, int R>
class ROTensor;

template <class T, int R>
class ROIteratorOneDim {
private:
    friend class ROTensor<T, R>;
    const ROTensor<T, R>* tensor_; // reference to the tensor_ that has  to be iterated
    int cur_ref_[R]; // tupla of the next element to be read
    int idx_to_scan_;

    /**
     * Check value for begin Iterator on only one dimension
     * @param idx dimension to iterate
     * @param fixed_values : fixed values of other dimensionsl,,
     * @return
     */
    void checkOnDimIterator(const int idx, const int fixed_values[], const int dim){
        if(idx < 0 || idx >= R)
            throw out_of_range("Index to scan doesn't exist.");
        if(dim != R - 1)
            throw invalid_argument("Indexes number not equal to rank - 1.");
        for(int i = 0; i< R; ++i) {
            if(i != idx){
                if(i < idx) {
                    if (fixed_values[i] < 0 || fixed_values[i] >= tensor_->dimensions_[i]) {
                        throw out_of_range("Indexes out of range");
                    }
                    cur_ref_[i] = fixed_values[i];
                }
                else {
                    if (fixed_values[i - 1] < 0 || fixed_values[i - 1] >= tensor_->dimensions_[i]) {
                        throw out_of_range("Indexes out of range");
                    }
                    cur_ref_[i] = fixed_values[i - 1];
                }
            }
        }
        cur_ref_[idx] = 0;
    }

    /**
     * Private Costructor
     * @param ten : tensor_ to be iterated
     * @param start : true for begin iterator, false otherwise
     * @return the begin iterator on tensor_ if start = true, the end iterator otherwise
     */
    ROIteratorOneDim(const ROTensor<T, R>& ten, const bool start, const int idx, const int fixed_values[], const int dim) :
            tensor_(&ten),
            idx_to_scan_(idx)
    {
        try{
            checkOnDimIterator(idx, fixed_values, dim);
            if(!start){
                cur_ref_[idx] = ten.dimensions_[idx];
            }
        }
        catch (out_of_range e){
            throw out_of_range("Index out of range. No possible operations.");
        }
        catch (invalid_argument e){
            throw invalid_argument("Invalid argument. No possible operations.");
        }
    }

public:
    /**
     * It only shows the current tupla
     */
    void show(){
        showVector(cur_ref_, R);
    }

    /**
     * For copy of a tensor
     * @return pointer to the internal tupla
     */
    int* currentPosition(){
        return cur_ref_;
    }

    /**
     * Copy constructor
     * @param ite : is the iterator to be copied
     * @return new iterator that is a copy of ite
     */
    ROIteratorOneDim(const ROIteratorOneDim& ite) = default;

    /**
     * Move constructor
     * @param ite
     * @return
     */
    ROIteratorOneDim(ROIteratorOneDim&& ite) = default;

    /**
     * Destructor
     */
    ~ROIteratorOneDim() = default;

    /**
     * Copy Assignment
     * @param ite : is the iterator to be copied
     * @return the reference to the operator assigned
     */
    ROIteratorOneDim& operator=(const ROIteratorOneDim& ite) = default;

    /**
     * Move Assignment
     * @param ite
     * @return
     */
    ROIteratorOneDim& operator=(ROIteratorOneDim&& ite) = default;

    /**
     * Prefix ++ operator
     * @return reference to the updated iterator
     */
    ROIteratorOneDim& operator++ (){
        cur_ref_[idx_to_scan_]++;
        return *this;
    }

    /**
     * Postfix ++ operator
     * @param n : imposed to 1
     * @return
     */
    ROIteratorOneDim operator++(const int n){
        return ++(*this);
    }

    /**
     * Equality comparison
     * @param ite : iterator to be compared with this
     * @return true if they point to the same element in tensor_
     */
    bool operator== (const ROIteratorOneDim& ite){
        return tensor_->calculatePosition(cur_ref_, R) == tensor_->calculatePosition(ite.cur_ref_, R);
    }

    /**
     * Disequality comparison
     * @param ite : iterator to be compared with this
     * @return true if they don't point to the same element in tensor_
     */
    bool operator!= (const ROIteratorOneDim& ite){
        return tensor_->calculatePosition(cur_ref_, R) != tensor_->calculatePosition(ite.cur_ref_, R);
    }

    /**
     * Dereference operator
     * @return the value of type T pointed
     */
    T operator* () {
        return tensor_->getWithArray(cur_ref_, R);
    }

};


#endif //TENSORLIBRARY_ROITERATORONEDIM_H
