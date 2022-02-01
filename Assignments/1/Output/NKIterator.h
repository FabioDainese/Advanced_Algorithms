#ifndef TENSORLIBRARY_NKTENSORITERATOR_H
#define TENSORLIBRARY_NKTENSORITERATOR_H

#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Utility.h"

using namespace std;

template <class T>
class NKTensor;

template<class T>
class NKIterator{
private:
    friend class NKTensor<T>;
    const NKTensor<T>* tensor_; // reference to the tensor_ that has  to be iterated
    vector<int> cur_ref_; // tupla of the next element to be read

    /**
     * Private Costructor
     * @param ten : tensor_ to be iterated
     * @param start : true for begin iterator, false otherwise
     * @return the begin iterator on tensor_ if start = true, the end iterator otherwise
     */
    NKIterator(const NKTensor<T>& ten, bool start) : tensor_(&ten), cur_ref_((int)ten.rank_){
        if(ten.rank_ == 0)
            throw invalid_argument("Unsupported operation on tensor with rank 0");
        if(!start){
            cur_ref_[0] = ten.dimensions_.at(0);
        }
    }

public:
    /**
     * It only shows the current tupla
     */
    void show(){
        showVector(cur_ref_);
    }

    /**
     * For tensor copy
     * @return copy of the internal tupla
     */
    vector<int> currentPosition(){
        return cur_ref_;
    }

    /**
     * Copy constructor
     * @param ite : is the iterator to be copied
     * @return new iterator that is a copy of ite
     */
    NKIterator(const NKIterator& ite) = default;

    /**
     * Move constructor
     * @param ite
     * @return
     */
    NKIterator(NKIterator&& ite) = default;

    /**
     * Destructor
     */
    ~NKIterator() = default;

    /**
     * Copy Assignment
     * @param ite : is the iterator to be copied
     * @return the reference to the operator assigned
     */
    NKIterator& operator=(const NKIterator& ite) = default;

    /**
     * Move Assignment
     * @param ite
     * @return
     */
    NKIterator& operator=(NKIterator&& ite) = default;

    /**
     * Prefix ++ operator
     * @return reference to the updated iterator
     */
    NKIterator& operator++ (){
        int max_dim = cur_ref_[0];
        bool updated = false;
        int i = (int)tensor_->rank_ - 1;
        while(!updated && i >= 0 ){
            if(cur_ref_[i] == tensor_->dimensions_.at(i)-1){
                cur_ref_[i]=0;
                --i;
            }
            else{
                ++cur_ref_[i];
                updated = true;
            }
        }
        if(!updated){
            cur_ref_[0] = max_dim + 1;
        }
        return *this;
    }

    /**
     * Postfix ++ operator
     * @param n : imposed to 1
     * @return
     */
    NKIterator operator++(const int n){
        return ++(*this);
    }

    /**
     * Equality comparison
     * @param ite : iterator to be compared with this
     * @return true if they point to the same element in tensor_
     */
    bool operator== (const NKIterator& ite){
        return tensor_->calculatePosition(cur_ref_) == tensor_->calculatePosition(ite.cur_ref_);
    }

    /**
     * Disequality comparison
     * @param ite : iterator to be compared with this
     * @return true if they don't point to the same element in tensor_
     */
    bool operator!= (const NKIterator& ite){
        return tensor_->calculatePosition(cur_ref_) != tensor_->calculatePosition(ite.cur_ref_);
    }

    /**
     * Dereference operator
     * @return the value of type T pointed
     */
    T operator* () {
        try{
            return tensor_->get(cur_ref_);
        }
        catch (invalid_argument e){
            throw e;
        }
        catch (out_of_range e){
            throw e;
        }
    }

};


#endif //TENSORLIBRARY_NKTENSORITERATOR_H
