#ifndef TENSORLIBRARY_ROITERATOR_H
#define TENSORLIBRARY_ROITERATOR_H

#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Utility.h"

using namespace std;

template <class T, int R>
class ROTensor;

template<class T, int R>
class ROIterator {
private:
    friend class ROTensor<T, R>;
    const ROTensor<T, R>* tensor_; // reference to the tensor_ that has  to be iterated
    int cur_ref_[R]; // tupla of the next element to be read

    /**
     * Private Costructor
     * It's not necessary to control if tensor rank is equal to 0 because rank 0 tensor specialization will not provide method begin()/end()
     * @param ten : tensor_ to be iterated
     * @param start : true for begin iterator, false otherwise
     * @return the begin iterator on tensor_ if start = true, the end iterator otherwise
     */
    ROIterator(const ROTensor<T, R>& ten, const bool start) : tensor_(&ten){
        for(int i = 0; i < R; ++i)
            cur_ref_[i] = 0;
        if(!start){
            cur_ref_[0] = ten.dimensions_[0];
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
    ROIterator(const ROIterator& ite) = default;

    /**
     * Move constructor
     * @param ite
     * @return
     */
    ROIterator(ROIterator&& ite) = default;

    /**
     * Destructor
     */
    ~ROIterator() = default;

    /**
     * Copy Assignment
     * @param ite : is the iterator to be copied
     * @return the reference to the operator assigned
     */
    ROIterator& operator=(const ROIterator& ite) = default;

    /**
     * Move Assignment
     * @param ite
     * @return
     */
    ROIterator& operator=(ROIterator&& ite) = default;

    /**
     * Prefix ++ operator
     * @return reference to the updated iterator
     */
    ROIterator& operator++ (){
        int max_dim = cur_ref_[0];
        bool updated = false;
        int i = R - 1;
        while(!updated && i >= 0 ){
            if(cur_ref_[i] == tensor_->dimensions_[i]-1){
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
    ROIterator operator++(const int n){
        return ++(*this);
    }

    /**
     * Equality comparison
     * @param ite : iterator to be compared with this
     * @return true if they point to the same element in tensor_
     */
    bool operator== (const ROIterator& ite){
        return tensor_->calculatePosition(cur_ref_, R) == tensor_->calculatePosition(ite.cur_ref_, R);
    }

    /**
     * Disequality comparison
     * @param ite : iterator to be compared with this
     * @return true if they don't point to the same element in tensor_
     */
    bool operator!= (const ROIterator& ite){
        return tensor_->calculatePosition(cur_ref_, R) != tensor_->calculatePosition(ite.cur_ref_, R);
    }

    /**
     * Dereference operator
     * @return the value of type T pointed
     */
    T operator* () {
        try{
            T value = tensor_->getWithArray(cur_ref_, R);
            return value;
        }
        catch (invalid_argument e){
            throw e;
        }
        catch (out_of_range e){
            throw e;
        }
    }
    
};


#endif //TENSORLIBRARY_ROITERATOR_H
