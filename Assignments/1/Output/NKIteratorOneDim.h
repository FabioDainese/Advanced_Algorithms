#ifndef TENSORLIBRARY_NKTENSORITERATORONEDIM_H
#define TENSORLIBRARY_NKTENSORITERATORONEDIM_H

#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Utility.h"

using namespace std;

template <class T>
class NKTensor;

template<class T>
class NKIteratorOneDim {
private:
    friend class NKTensor<T>;
    const NKTensor<T>* tensor_; // reference to the tensor_ that has  to be iterated
    vector<int> cur_ref_; // tupla of the next element to be read
    int idx_to_scan_;

    /**
     * Check value for begin Iterator on only one dimension
     * @param idx dimension to iterate
     * @param fixed_values : fixed values of other dimensionsl,,
     * @return
     */
    void checkOnDimIterator(const int idx, vector<int>& fixed_values){
        if(idx < 0 || idx >= tensor_->rank_)
            throw out_of_range("Index to scan doesn't exist.");
        if(fixed_values.size() != tensor_->rank_ - 1)
            throw invalid_argument("Indexes number not equal to rank - 1.");
        for(int i = 0; i< tensor_->rank_; ++i) {
            if(i != idx){
                if(i < idx) {
                    if (fixed_values.at(i) < 0 || fixed_values.at(i) >= tensor_->dimensions_.at(i)) {
                        throw out_of_range("Indexes out of range");
                    }
                }
                else {
                    if (fixed_values.at(i - 1) < 0 || fixed_values.at(i - 1) >= tensor_->dimensions_.at(i)) {
                        throw out_of_range("Indexes out of range");
                    }
                }
            }
        }
        fixed_values.insert(fixed_values.begin() + idx, 0);
    }

    /**
     * Private Costructor
     * @param ten : tensor_ to be iterated
     * @param start : true for begin iterator, false otherwise
     * @return the begin iterator on tensor_ if start = true, the end iterator otherwise
     */
    NKIteratorOneDim(const NKTensor<T>& ten, const bool start, const int idx, const vector<int>& fixed_values) :
            tensor_(&ten),
            cur_ref_(fixed_values),
            idx_to_scan_(idx)
    {
        try{
            if(ten.rank_ == 0)
                throw invalid_argument("Unsupported operation on tensor with rank 0");
            checkOnDimIterator(idx, cur_ref_);
            if(!start){
                cur_ref_[idx] = ten.dimensions_.at(idx);
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
        showVector(cur_ref_);
    }

    vector<int> currentPosition(){
        return cur_ref_;
    }

    /**
     * Copy constructor
     * @param ite : is the iterator to be copied
     * @return new iterator that is a copy of ite
     */
    NKIteratorOneDim(const NKIteratorOneDim& ite) = default;

    /**
     * Move constructor
     * @param ite
     * @return
     */
    NKIteratorOneDim(NKIteratorOneDim&& ite) = default;

    /**
     * Destructor
     */
    ~NKIteratorOneDim() = default;

    /**
     * Copy Assignment
     * @param ite : is the iterator to be copied
     * @return the reference to the operator assigned
     */
    NKIteratorOneDim& operator=(const NKIteratorOneDim& ite) = default;

    /**
     * Move Assignment
     * @param ite
     * @return
     */
    NKIteratorOneDim& operator=(NKIteratorOneDim&& ite) = default;

    /**
     * Prefix ++ operator
     * @return reference to the updated iterator
     */
    NKIteratorOneDim& operator++ (){
        cur_ref_.at(idx_to_scan_)++;
        return *this;
    }

    /**
     * Postfix ++ operator
     * @param n : imposed to 1
     * @return
     */
    NKIteratorOneDim operator++(const int n){
        return ++(*this);
    }

    /**
     * Equality comparison
     * @param ite : iterator to be compared with this
     * @return true if they point to the same element in tensor_
     */
    bool operator== (const NKIteratorOneDim& ite){
        return tensor_->calculatePosition(cur_ref_) == tensor_->calculatePosition(ite.cur_ref_);
    }

    /**
     * Disequality comparison
     * @param ite : iterator to be compared with this
     * @return true if they don't point to the same element in tensor_
     */
    bool operator!= (const NKIteratorOneDim& ite){
        return tensor_->calculatePosition(cur_ref_) != tensor_->calculatePosition(ite.cur_ref_);
    }

    /**
     * Dereference operator
     * @return the value of type T pointed
     */
    T operator* () {
        return tensor_->get(cur_ref_);
    }
   
};


#endif //TENSORLIBRARY_NKTENSORITERATORONEDIM_H
