#ifndef TENSORLIBRARY_ROTENSOR_H
#define TENSORLIBRARY_ROTENSOR_H

#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Utility.h"
#include "ROIterator.h"
#include "ROIteratorOneDim.h"
#include "NKTensor.h"

using namespace std;

template <class T, int... D>
class RDTensor;

template <class T, int R = 0>
class ROTensor {
private:
    int dimensions_[R];
    int strides_[R];
    shared_ptr<vector<T>> array_;
    int start_pointer_;
    int end_pointer_;
    friend class ROIterator<T, R>;
    friend class ROIteratorOneDim<T, R>;
    // I must impose that tensor with rank R + 1 is a friend because otherwise tensor with rank R + 1 could not do slicing
    friend class ROTensor<T, R + 1>;
    template <class T1, int ... D>
    friend class RDTensor;

    /**
     * It throws and exception if the element poitend to indexes does not exisit
     * @param indexes : tupla of element
     */
    inline void checkExistence(const int indexes[], const int dim) const{
        if(dim != R)
            throw invalid_argument("Indexes number not equal to rank");
        for(int i = 0; i< dim; ++i){
            if(indexes[i]<0 || indexes[i]>=dimensions_[i]){
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
        if(idx<0 || idx>=R || val<0 || val>=dimensions_[idx]){
            throw out_of_range ("Indexes out of range");
        }
    }

    /**
     * It thows an exception if the index does not be in the correct range
     * @param idx : index number
     */
    inline void checkIndex(const int idx) const{
        if(idx<0 || idx>=R){
            throw out_of_range ("Indexes out of range. No possible operations.");
        }
    }

    /**
     * Convert tupla into int to access the array
     * @param tupla : element indexes
     * @return position index of the element pointed by tupla in the array
     */
    inline int calculatePosition(const int tupla[], const int dim) const {
        int position = start_pointer_;
        for(int i = 0 ; i < dim; ++i){
            position += strides_[i]*tupla[i];
        }
        return position;
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
    ROTensor(
            const shared_ptr<vector<T>>& array,
            int dimensions[],
            int strides[],
            const int start_pointer,
            const int end_pointer
    ) : array_ (array),
        start_pointer_(start_pointer),
        end_pointer_(end_pointer)
    {
        for(int i = 0; i < R; ++i){
            dimensions_[i] = dimensions[i];
            strides_[i] = strides[i];
        }
    }

public:
    /**
     * Default constructor
     * @return new tensor with R dimensions imposed to 1
     */
    ROTensor() : ROTensor(vector<int>(R,1)){}

    /**
     * Constructor with parameter pack
     * @param DIM : parameter pack of int correspondig to tensor dimensions
     * @return new tensor with dimensions DIM
     */
    template<typename ...Ints>
    ROTensor(Ints... DIM) :  ROTensor(vector<int>({DIM...})){}

    /**
     * Constructor with vector
     * @param dimensions : vector of int corresponding to tensor dimensions
     * @return new tensor with dimensions dimensions
     */
    ROTensor(const vector<int>& dimensions) {
        if (dimensions.size() != R)
            throw invalid_argument("Number of dimensions not equal to " + to_string(R));
        int element_number = 1;
        for (int i = R - 1; i >= 0; --i) {
            if(dimensions[i] <= 0)
                throw invalid_argument("Dimensions value must not be equal to 0!");
            dimensions_[i] = dimensions[i];
            strides_[i] = element_number;
            element_number *= dimensions_[i];
        }
        array_ = make_shared<vector<T>>(element_number);
        start_pointer_ = 0;
        end_pointer_ = element_number;
    }

    /**
     * Sharing Constructor
     * @param ten : tensor to be copied
     * @return a copy of ten
     */
    ROTensor(const ROTensor& ten) = default;

    /**
     * Move Constructor
     * @param ten : must not be const because it will be changed
     * @return
     */
    ROTensor(ROTensor&& ten) = default;

    /**
     * Destructor
     */
    ~ROTensor() = default;

    /**
     * Sharing Assignment
     * @param ten : tensor to be copied
     * @return reference to the new copied tensor
     */
    ROTensor& operator=(const ROTensor& ten) = default;

    /**
     * Move Assignment
     * @param ten
     * @return
     */
    ROTensor& operator=(ROTensor&& ten) = default;

    /**
     * This method create a tensor's copy duplicating the array, which contains only those fields concerning the array's portion used by the tensor
     * @return : A tensor's copy
     */
    ROTensor copy(){
        int element_number = 1;
        int new_strides_[R];
        for (int i = R - 1; i >= 0 ; --i) {
            new_strides_[i] = element_number;
            element_number *= dimensions_[i];
        }
        shared_ptr<vector<T>> new_array = make_shared<vector<T>>(element_number);
        ROTensor<T,R> copied_tensor_(new_array, dimensions_, new_strides_, 0, dimensions_[0]*strides_[0]);
        auto j = copied_tensor_.begin();
        for(auto i = begin(); i < end(); ++i){
            copied_tensor_.setWithArray(*i, j.currentPosition(), R);
            ++j;
        }
        return copied_tensor_;
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
     * It slices the current tensor on idx = value
     * @param idx : is the index to be fix,
     * @param val : is the value assigned to the index
     * @return new sliced tensor
     */
    inline ROTensor<T, R - 1> slicing(const int idx, const int val) {
        try{
            checkIndex(idx,val);
        }
        catch(out_of_range e){
            throw (e);
        }
        if(R - 1 == 0){
            return ROTensor<T,R - 1>(array_,(int *)NULL,(int *)NULL,start_pointer_ + val * strides_[idx],(int)NULL);
        }
        else{
            int new_dimensions_[R - 1];
            int new_strides_[R - 1];
            for(int i = 0; i < R; ++i){
                if(i < idx){
                    new_dimensions_[i] = dimensions_[i];
                    new_strides_[i] = strides_[i];
                }else if(i > idx){
                    new_dimensions_[i - 1] = dimensions_[i];
                    new_strides_[i - 1] = strides_[i];
                }
            }
            int new_start_pointer_ = start_pointer_ + val * strides_[idx];
            int new_end_pointer_ = new_start_pointer_ + new_dimensions_[0] * new_strides_[0];
            return ROTensor<T, R - 1>(array_,new_dimensions_,new_strides_,new_start_pointer_,new_end_pointer_);
        }
    }

    /**
     * Slicing
     * @param val
     * @return
     */
    ROTensor<T, R - 1> operator[](const int val) {
        try{
            return slicing(0, val);
        }
        catch (out_of_range e){
            throw e;
        }
    }

    /**
     * Multiple slicing
     * @param idx_val
     * @return new sliced NKTensor
     */
    inline NKTensor<T> slicing(const vector<pair<int,int>>& idx_val) const {
        vector<int> new_dimensions_;
        vector<int> new_strides_;
        for(int i = 0; i < R; ++i){
            new_dimensions_.push_back(dimensions_[i]);
            new_strides_.push_back(strides_[i]);
        }
        NKTensor<T> new_tensor_(array_, new_dimensions_, new_strides_, start_pointer_, end_pointer_);
        return new_tensor_.slicing(idx_val);
    }

    /**
     * Single Windowing
     * @param idx : index to be windowed
     * @param min : min index value used for windowing
     * @param max : max index value used for windowing
     * @return New windowed Tensor
     */
    inline ROTensor<T,R> windowing(const int idx, const int min, const int max){
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
        int new_dimensions_[R];
        int new_strides_[R];
        for(int i = 0; i < R; ++i){
            new_dimensions_[i] = dimensions_[i];
            new_strides_[i] = strides_[i];
        }
        new_dimensions_[idx] = max - min + 1;
        shared_ptr<vector<T>> new_array_ = array_;
        int new_start_pointer_ = start_pointer_ + strides_[idx] * min;
        int new_end_pointer_ = new_start_pointer_ + strides_[0] * new_dimensions_[0];
        ROTensor<T,R> new_tensor(new_array_,new_dimensions_,new_strides_,new_start_pointer_,new_end_pointer_);
        return new_tensor;
    }

    /**
     * Multiple Windowing
     * @param idx_val : vector of indexes to be windowed with min and max value
     * Be aware that it's different by the slicing because in t user pass original index number that remains always the
     * same because windowing doesn't decrease the rank.
     * @return New Tensor windowed multiple times
     */
    inline ROTensor<T,R> windowing(const vector<tuple<int,int,int>>& t){
        ROTensor<T,R> tmp (*this);
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
    inline NKTensor<T> flattening(const int min, const int max){
        vector<int> new_dimensions_;
        vector<int> new_strides_;
        for(int i = 0; i < R; ++i){
            new_dimensions_.push_back(dimensions_[i]);
            new_strides_.push_back(strides_[i]);
        }
        NKTensor<T> new_tensor_(array_, new_dimensions_, new_strides_, start_pointer_, end_pointer_);
        return new_tensor_.flattening(min, max);
    }

    /**
     * Begin iterator
     * @return iterator to the begin point
     */
    ROIterator<T, R> begin(){
        ROIterator<T, R> iter(*this, true);
        return iter;
    }

    /**
     * End iterator
     * @return iterator to the end point
     */
    ROIterator<T, R> end(){
        ROIterator<T, R> iter(*this, false);
        return iter;
    }

    /**
     * Begin iterator of the tensor on only one dimension
     * @param idx : dimension to iterate
     * @param fixed_values : fixed values of other dimensions
     * @return iterator on dimension idx
     */
    ROIteratorOneDim<T, R> beginWithArray(const int idx, const int fixed_values[], const int dim) const {
        try{
            return ROIteratorOneDim<T, R>(*this, true, idx, fixed_values, dim);
        }
        catch (out_of_range e){
            throw e;
        }
        catch (invalid_argument e){
            throw e;
        }
    }

    template <typename... Ints>
    ROIteratorOneDim<T, R> begin(const int idx, Ints... INDEXES) const {
        int fv[] = {INDEXES...};
        try{
            return beginWithArray(idx, fv, sizeof...(INDEXES));
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
    ROIteratorOneDim<T, R> endWithArray(const int idx, const int fixed_values[], const int dim){
        try{
            return ROIteratorOneDim<T, R>(*this, false, idx, fixed_values, dim);
        }
        catch (out_of_range e){
            throw e;
        }
        catch (invalid_argument e){
            throw e;
        }
    }

    template <typename... Ints>
    ROIteratorOneDim<T, R> end(const int idx, Ints... fixed_values){
        int fv[] = {fixed_values...};
        try{
            return endWithArray(idx, fv, sizeof...(fixed_values));
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
        cout << "Rank: " << R << endl;
        cout << "Dimensions: " << dimensions_ << " --> ";
        showVector(dimensions_, R);
        cout << endl;
        cout << "Strides: " << &strides_ << " --> ";
        showVector(strides_, R);
        cout << endl;
        cout << "Array pointer: " << array_ << endl;
        cout << "Start pointer: " << start_pointer_ << endl;
        cout << "End pointer: " << end_pointer_ << endl;
        cout << "Elements: {";
        if(array_== nullptr) {
            cout << "nullptr";
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

template <class T>
class ROTensor<T,0>{
private:
    shared_ptr<vector<T>> array_;
    int start_pointer_ = 0;
    // I must impose that tensor with rank 1 is a friend because otherwise tensor with rank 1 could not do slicing
    friend class ROTensor<T, 1>;
    template <class T1, int ... D>
    friend class RDTensor;

    /**
     * Private Constructor
     * @param array
     * @param start_pointer
     * @return
     */
    ROTensor(const shared_ptr<vector<T>> array, const int start_pointer) : array_(array), start_pointer_(start_pointer) {}

    /**
     * A private constructor used to create a precise new tensor (used for slicing)
     * @param array : pointer to the shared memory where datas are stored
     * @param dimensions : dimensions
     * @param strides : strides
     * @param start_pointer : start pointer
     * @param end_pointer : end pointer
     * @return
     */
    ROTensor(
            const shared_ptr<vector<T>>& array,
            const int dimensions[],
            const int strides[],
            int start_pointer,
            int end_pointer
    ) : array_(array), start_pointer_(start_pointer) {}

public:
    /**
     * Default constructor
     * @return new tensor with rank 0 (like a variable)
     */
    ROTensor() {
        array_ = make_shared<vector<T>>(1);
    }

    /**
     * Constructor of one element
     * @param element
     * @return
     */
    ROTensor(const T element) : ROTensor() {
        array_->at(0) = element;
    }

    /**
     * Sharing Constructor
     * @param ten : tensor to be copied
     * @return a copy of ten
     */
    ROTensor(const ROTensor& ten) = default;

    /**
     * Move Constructor
     * @param ten : must not be const because it will be changed
     * @return
     */
    ROTensor(ROTensor&& ten) = default;

    /**
     * Destructor
     */
    ~ROTensor() = default;

    /**
     * Sharing Assignment
     * @param ten : tensor to be copied
     * @return reference to the tensor
     */
    ROTensor& operator=(const ROTensor& ten) = default;

    /**
     * Assignment
     * @param value : value to be copied in the tensor
     * @return reference to the tensor
     */
    ROTensor& operator=(const T value) {
        array_->at(start_pointer_) = value;
        return *this;
    }

    /**
     * Move Assignment
     * @param ten
     * @return
     */
    ROTensor& operator=(ROTensor&& ten) = default;

    /**
     * This method create a tensor's copy duplicating the array, which contains only those fields concerning the array's portion used by the tensor
     * @return : A tensor's copy
     */
    ROTensor<T> copy(){
        ROTensor<T,0> new_tensor_;
        new_tensor_.set(this->get());
        return new_tensor_;
    }

    /**
     * Get an element
     * @param INDEXES : position to be read
     * @return value pointed by INDEXES
     */
    inline T get() const{
        if (array_ == nullptr){
            throw out_of_range ("Indexes out of range");
        }
        return array_->at(start_pointer_);
    }

    /**
     * Set the element
     * @param value
     */
    inline void set(const T value){
        if (array_ == nullptr){
            throw out_of_range ("Indexes out of range");
        }
        array_->at(start_pointer_) = value;
    }

    /**
     * Only for debug
     * @param name
     */
    void printTensor(const string name){
        if(array_ == nullptr){
            cout << "Empty Tensor Variable!!!" << endl;
        } else{
            T element = array_->at(start_pointer_);
            cout << "Tensor Variable " << name << ": " << element << endl;
        }
        cout << endl;
    }
};

#endif //TENSORLIBRARY_ROTENSOR_H
