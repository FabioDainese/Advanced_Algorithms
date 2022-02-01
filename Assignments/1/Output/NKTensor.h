#ifndef TENSORLIBRARY_NKTensor_H
#define TENSORLIBRARY_NKTensor_H

#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Utility.h"
#include "NKIterator.h"
#include "NKIteratorOneDim.h"

using namespace std;

template <class T, int R>
class ROTensor;

template <class T, int... D>
class RDTensor;

template <class T>
class NKTensor {
private:
    int rank_;
    vector<int> dimensions_;
    vector<int> strides_;
    shared_ptr<vector<T>> array_;
    int start_pointer_;
    int end_pointer_;
    friend class NKIterator<T>;
    friend class NKIteratorOneDim<T>;
    template <class T1, int R>
    friend class ROTensor;
    template <class T1, int...D>
    friend class RDTensor;

    /**
     * It throws and exception if the element poitend to indexes does not exisit
     * @param indexes : tupla of element
     */
    inline void checkExistence(const vector<int>& indexes) const{
        if(indexes.size() != rank_)
            throw invalid_argument("Indexes number not equal to rank");
        for(int i = 0; i< rank_; ++i){
            if(indexes.at(i)<0 || indexes.at(i)>=dimensions_.at(i)){
                throw out_of_range ("Indexes out of range");
            }
        }
    }

    inline void checkExistence(const int indexes[], const int dim) const{
        if(dim != rank_)
            throw invalid_argument("Indexes number not equal to rank");
        for(int i = 0; i< dim; ++i){
            if(indexes[i]<0 || indexes[i]>=dimensions_.at(i)){
                throw out_of_range ("Indexes out of range");
            }
        }
    }

    /**
     * It thows an exception if the index does not be in the correct range
     * @param idx : index number
     * @param val : index value
     */
    inline void checkIndex(const int idx, const int val) const{
        if(idx<0 || idx>=rank_ || val<0 || val>=dimensions_.at(idx)){
            throw out_of_range ("Indexes out of range");
        }
    }

    /**
     * It thows an exception if the index does not be in the correct range
     * @param idx : index number
     */
    inline void checkIndex(const int idx) const{
        if(idx<0 || idx>=rank_){
            throw out_of_range ("Indexes out of range. No possible operations.");
        }
    }

    /**
     * A private constructor used to create a precise new tensor (used for slicing)
     * @param array : pointer to the shared memory where datas are stored
     * @param dimensions : dimensions
     * @param strides : strides
     * @param start_pointer : start pointer
     * @param end_pointer : end pointer
     * @return
     */
    NKTensor(
            const shared_ptr<vector<T>>& array,
            const vector<int>& dimensions,
            const vector<int>& strides,
            const int start_pointer,
            const int end_pointer
    ) : rank_(dimensions.size()),
        dimensions_(vector<int>(dimensions)),
        strides_(vector<int>(strides)),
        array_ (array),
        start_pointer_(start_pointer),
        end_pointer_(end_pointer)
    {}

    /**
     * Convert tupla into int to access the array
     * @param tupla : element indexes
     * @return position index of the element pointed by tupla in the array
     */
    inline int calculatePosition(const vector<int>& tupla) const {
        int position = start_pointer_;
        for(int i = 0 ; i<rank_; ++i){
            position += strides_.at(i)*tupla.at(i);
        }
        return position;
    }

    inline int calculatePosition(const int tupla[], const int dim) const {
        int position = start_pointer_;
        for(int i = 0 ; i < dim; ++i){
            position += strides_.at(i)*tupla[i];
        }
        return position;
    }

    /**
     * Single Flattening
     * @param idx : inex on which applying the flattening operation
     * @return new Tensor on which has been applied the Flattening operation
     */
    inline NKTensor flattening(const int idx){
        try{
            checkIndex(idx);
        }
        catch(out_of_range e){
            throw (e);
        }
        if (idx>=dimensions_.size()-1){
            throw out_of_range("Out of range exception! No possible operations.");
        }
        vector<int> new_dimensions_(dimensions_);
        new_dimensions_.at(idx+1) = new_dimensions_.at(idx) * new_dimensions_.at(idx+1);
        new_dimensions_.erase(new_dimensions_.begin()+idx);
        vector<int> new_strides_(strides_);
        new_strides_.erase(new_strides_.begin()+idx);
        NKTensor new_tensor(array_,new_dimensions_,new_strides_,start_pointer_,end_pointer_);
        return new_tensor;
    }

public:
    /**
     * Default constructor
     * @return new tensor or rank 0 (like a variable)
     */
    NKTensor() : NKTensor(vector<int>({})){}

    /**
     * Constructor with parameter pack
     * @param DIM : parameter pack of int correspondig to tensor dimensions
     * @return new tensor with dimensions DIM
     */
    template<typename ...Ints>
    NKTensor(Ints... DIM) :  NKTensor(vector<int>({DIM...})){}

    /**
     * Constructor with vector
     * @param dimensions : vector of int corresponding to tensor dimensions
     * @return new tensor with dimensions dimensions
     */
    NKTensor(const vector<int>& dimensions) :
            rank_(dimensions.size()),
            dimensions_(vector<int>(dimensions)),
            strides_(vector<int>(rank_))
    {
        int element_number = 1;
        for (int i = rank_ - 1; i >= 0 ; --i) {
            if(dimensions_.at(i) <= 0)
                throw invalid_argument("Dimensions value must not be equal to 0!");
            strides_.at(i) = element_number;
            element_number *= dimensions_.at(i);
        }
        array_ = make_shared<vector<T>>(element_number);
        start_pointer_= 0;
        if(rank_==0){
            end_pointer_=0;
        }
        else{
            end_pointer_= element_number;
        }
    }

    /**
     * Sharing Constructor
     * @param ten : tensor to be copied
     * @return a copy of ten
     */
    NKTensor(const NKTensor& ten) = default;

    /**
     * Move Constructor
     * @param ten : must not be const because it will be changed
     * @return
     */
    NKTensor(NKTensor&& ten) = default;

    /**
     * Destructor
     */
    ~NKTensor() = default;

    /**
     * Sharing Assignment
     * @param ten : tensor to be copied
     * @return reference to the new copied tensor
     */
    NKTensor& operator=(const NKTensor& ten) = default;

    /**
     * Move Assignment
     * @param ten
     * @return
     */
    NKTensor& operator=(NKTensor&& ten) = default;

    /**
     * This method create a tensor's copy duplicating the array, which contains only those fields concerning the array's portion used by the tensor
     * @return : A tensor's copy
     */
    NKTensor copy(){
        vector<int> new_strides_(rank_);
        int element_number = 1;
        for (int i = rank_ - 1; i >= 0 ; --i) {
            new_strides_.at(i) = element_number;
            element_number *= dimensions_.at(i);
        }
        shared_ptr<vector<T>> new_array = make_shared<vector<T>>(element_number);
        if(rank_ != 0){
            NKTensor copied_tensor_(new_array, dimensions_, new_strides_, 0, dimensions_.at(0)*new_strides_.at(0));
            auto j = copied_tensor_.begin();
            for(auto i = begin(); i < end(); ++i){
                copied_tensor_.set(*i, j.currentPosition());
                ++j;
            }
            return copied_tensor_;
        } else{
            NKTensor copied_tensor_(new_array, dimensions_, strides_, 0, 0);
            copied_tensor_.set(get());
            return copied_tensor_;
        }
    }

    /**
     * Get an element
     * @param INDEXES : position to be read
     * @return value pointed by INDEXES
     */
    template<typename ...Ints>
    inline T get(Ints... INDEXES) const{
        int idxs[] = {INDEXES...};
        return getWithArray(idxs, (int)sizeof...(INDEXES));
    }

    /**
     * Get an element
     * @param indexes : position to be read
     * @return value pointed by indexes
     */
    inline T getWithArray(const int indexes[], const int dim) const{
        try{
            checkExistence(indexes, dim);
        }
        catch(invalid_argument e){
            throw e;
        }
        catch(out_of_range e){
            throw e;
        }
        return array_->at(calculatePosition(indexes, dim));
    }

    /**
     * Get an element
     * @param indexes : position to be read
     * @return value pointed by indexes
     */
    inline T get(const vector<int>& indexes) const{
        try{
            checkExistence(indexes);
        }
        catch(invalid_argument e){
            throw e;
        }
        catch(out_of_range e){
            throw e;
        }
        return array_->at(calculatePosition(indexes));
    }

    /**
     * Like get
     * @param INDEXES : position to be read
     * @return value pointed by indexes
     */
    template<typename ...Ints>
    inline T operator()(Ints... INDEXES){
        return get(INDEXES...);
    }

    /**
     * Set an element value
     * @param value : new value of the element
     * @param INDEXES : tupla of element to be setted
     */
    template<typename ...Ints>
    inline void set(const T& value, Ints... INDEXES){
        int idxs[] = {INDEXES...};
        //vector<int> idxs ={INDEXES...};
        setWithArray(value, idxs, (int)sizeof...(INDEXES));
    }

    /**
     * Set an element value
     * @param value : new value of the element
     * @param indexes : tupla of element to be setted
     */
    inline void setWithArray(const T& value, const int indexes[], const int dim){
        try{
            checkExistence(indexes, dim);
        }
        catch(invalid_argument e){
            throw e;
        }
        catch(out_of_range e){
            throw e;
        }
        array_->at(calculatePosition(indexes, dim)) = value;
    }

    /**
     * Set an element value
     * @param value : new value of the element
     * @param indexes : tupla of element to be setted
     */
    inline void set(const T& value, const vector<int>& indexes){
        try{
            checkExistence(indexes);
        }
        catch(invalid_argument e){
            throw e;
        }
        catch(out_of_range e){
            throw e;
        }
        array_->at(calculatePosition(indexes)) = value;
    }

    /**
     * It slices the current tensor on idx = value
     * @param idx : is the index to be fix,
     * @param val : is the value assigned to the index
     * @return new sliced tensor
     */
    inline NKTensor slicing(const int idx, const int val){
        try{
            checkIndex(idx,val);
        }
        catch(out_of_range e){
            throw (e);
        }
        vector<int> new_dimensions_(dimensions_);
        new_dimensions_.erase(new_dimensions_.begin() + idx);
        vector<int> new_strides_(strides_);
        new_strides_.erase(new_strides_.begin() + idx);
        shared_ptr<vector<T>> new_array_ = array_;
        int new_start_pointer_ = start_pointer_ + val * strides_.at(idx);
        int new_end_pointer_;
        if(new_dimensions_.size() == 0){
            new_end_pointer_ = new_start_pointer_;
        }
        else {
            new_end_pointer_ = new_start_pointer_ + new_dimensions_.at(0) * new_strides_.at(0);
        }
        NKTensor new_tensor(new_array_,new_dimensions_,new_strides_,new_start_pointer_,new_end_pointer_);
        return new_tensor;
    }

    /**
     * Slicing
     * @param val
     * @return
     */
    NKTensor operator[](const int val){
        try{
            return slicing(0, val);
        }
        catch (out_of_range e){
            throw e;
        }
    }

    /**
     * Multiple slicing
     * @param idx_val : array of indexes and respective value to be sliced
     * Be aware that in idx_val user pass index number as in the original tensor.
     * As the tensor is sliced the remaining indexes number must be decrease of one because the slice rank decrease.
     * @return new sliced tensor
     */
    inline NKTensor slicing(const vector<pair<int,int>>& idx_val){
        vector<pair<int,int>> idx_val_tmp(idx_val);
        NKTensor tmp(*this);
        for(int i=0;i<idx_val_tmp.size();++i){
            tmp = tmp.slicing(idx_val_tmp[i].first, idx_val_tmp[i].second);
            for(int j = i + 1; j < idx_val_tmp.size(); ++j){
                if(idx_val_tmp[j].first > idx_val_tmp[i].first){
                    idx_val_tmp[j].first--;
                }
            }
        }
        return tmp;
    }

    /**
     * Single Windowing
     * @param idx : index to be windowed
     * @param min : min index value used for windowing
     * @param max : max index value used for windowing
     * @return New windowed Tensor
     */
    inline NKTensor windowing(const int idx, const int min, const int max){
        try{
            checkIndex(idx,min);
            checkIndex(idx,max);
        }
        catch(out_of_range e){
            throw (e);
        }
        if(min>max){
            throw invalid_argument("Min value is greater than max value. No possible operations.");
        }
        vector<int> new_dimensions_(dimensions_);
        new_dimensions_.at(idx) = (max - min + 1);
        vector<int> new_strides_(strides_);
        shared_ptr<vector<T>> new_array_ = array_;
        int new_start_pointer_ = start_pointer_ + strides_.at(idx) * min;
        int new_end_pointer_ = new_start_pointer_ + strides_.at(0) * new_dimensions_.at(0);
        NKTensor new_tensor(new_array_,new_dimensions_,new_strides_,new_start_pointer_,new_end_pointer_);
        return new_tensor;
    }

    /**
     * Multiple Windowing
     * @param idx_val : vector of indexes to be windowed with min and max value
     * Be aware that it's different by the slicing because in t user pass original index number that remains always the
     * same because windowing doesn't decrease the rank.
     * @return New Tensor windowed multiple times
     */
    inline NKTensor windowing(const vector<tuple<int,int,int>>& t){
        NKTensor tmp (*this);
        for(int i=0;i<t.size();++i){
            tmp = tmp.windowing(std::get<0>(t[i]), std::get<1>(t[i]), std::get<2>(t[i]));
        }
        return tmp;
    }

    /**
     * Multiple flattening
     * @param t : vector of index on whose flattening will be applied
     * @return new Tensor on which it has been applied the flattening operation multiple times
     */
    inline NKTensor flattening(const int min, const int max){
        if(min>=max || min<0 || max>=rank_){
            throw invalid_argument("Min value greater than max value. Invalid argumetn, no possible opretion.");
        }
        NKTensor tmp (*this);
        try{
            for(int i=min;i<max;++i){
                    tmp = tmp.flattening(min);
            }
        }
        catch(out_of_range e){
            throw (e);
        }

        return tmp;
    }

    /**
     * Begin iterator
     * @return iterator to the begin point
     */
    NKIterator<T> begin(){
        NKIterator<T> iter(*this, true);
        return iter;
    }

    /**
     * End iterator
     * @return iterator to the end point
     */
    NKIterator<T> end(){
        NKIterator<T> iter(*this, false);
        return iter;
    }

    /**
     * Begin iterator of the tensor on only one dimension
     * @param idx : dimension to iterate
     * @param fixed_values : fixed values of other dimensions
     * @return iterator on dimension idx
     */
    NKIteratorOneDim<T> begin(const int idx, const vector<int>& fixed_values){
        try{
            return NKIteratorOneDim<T>(*this, true, idx, fixed_values);
        }
        catch (out_of_range e){
            throw e;
        }
        catch (invalid_argument e){
            throw e;
        }
    }

    template <typename... Ints>
    NKIteratorOneDim<T> begin(const int idx, Ints... fixed_values){
        vector<int> fv = {fixed_values...};
        try{
            return begin(idx, fv);
        }
        catch (out_of_range e){
            throw e;
        }
        catch (invalid_argument e){
            throw e;
        }
    }

    /**
     * End iterator of the tensor on only one dimension
     * @param idx : dimension to iterate
     * @param fixed_values : fixed values of other dimensions
     * @return iterator on dimension idx
     */
    NKIteratorOneDim<T> end(const int idx, const vector<int>& fixed_values){
        try{
            return NKIteratorOneDim<T>(*this, false, idx, fixed_values);
        }
        catch (out_of_range e){
            throw e;
        }
        catch (invalid_argument e){
            throw e;
        }
    }

    template <typename... Ints>
    NKIteratorOneDim<T> end(const int idx, Ints... fixed_values){
        vector<int> fv = {fixed_values...};
        try{
            return end(idx, fv);
        }
        catch (out_of_range e){
            throw e;
        }
        catch (invalid_argument e){
            throw e;
        }
    }

    /**
     * Simply print some information on the tensor, just to help debugging
     * @param name : name to be showed
     */
    void printTensor(const string name = "Tensor"){
        cout << endl;
        cout << "Name: " << name << endl;
        cout << "Rank: " << rank_ << endl;
        cout << "Dimensions: " << &dimensions_ << " --> ";
        showVector(dimensions_);
        cout << endl;
        cout << "Strides: " << &strides_ << " --> ";
        showVector(strides_);
        cout << endl;
        cout << "Array pointer: " << array_ << endl;
        cout << "Start pointer: " << start_pointer_ << endl;
        cout << "End pointer: " << end_pointer_ << endl;
        cout << "Elements: {";
        if(array_== nullptr) {
            cout << "nullptr";
        }
        else if(rank_ == 0){
            cout << get();
        }
        else{
            for(auto i = begin(); i < end(); ++i){
                cout << *i;
                if(i + 1 != end()){
                    cout << ", ";
                }
            }
        }
        cout << "}" << endl;
        cout << endl;
    }
};

#endif //TENSORLIBRARY_NKTensor_H
