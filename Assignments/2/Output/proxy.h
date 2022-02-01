#ifndef ES2_PROXY_H
#define ES2_PROXY_H

#include "algorithm"
#include "tensor.h"
#include "label.h"

template<typename T, typename K>
class proxy_tensor;

template<typename T, typename K>
class proxy_op_tensor;

template<typename T, typename K>
class proxy_sum_tensor;

template<typename T, typename K>
class proxy_prod_tensor;

template<typename T, typename K>
class proxy_labeled_tensor;

/**
 * Generic class of a proxy tensor.
 * @tparam T Type of the tensors
 * @tparam K Type of the labels
 */
template<typename T, typename K>
class proxy_tensor {

    friend class proxy_op_tensor<T, K>;

    friend class proxy_sum_tensor<T, K>;

    friend class proxy_prod_tensor<T, K>;

    friend class proxy_labeled_tensor<T, K>;

private:

    std::vector<label<K>> positions_list;

    proxy_tensor(std::vector<label<K>> labels) : positions_list(labels) {
        assert(labels.size() > static_cast<size_t>(0));
    }

    proxy_tensor() {
    }

    /**
     * Last visitable position test: it checks if the current tensor position is the last visitable one of the current tensor.
     * @param point Current tensor position
     * @param dimensions Pointer to current tensor dimensions vector
     * @return true current position is the last one, false otherwise
     */
    bool last_visitable_point(const std::vector<size_t> *point, const std::vector<size_t> *dimensions) {
        bool is_last = true;
        for (size_t i = static_cast<size_t>(0); i < point->size(); ++i) {
            if (point->at(i) == std::numeric_limits<size_t>::max() || point->at(i) < dimensions->at(i) - 1) {
                is_last = false;
                break;
            }
        }
        return is_last;
    }

    /**
     * Compute next position
     * @param pointer Current position Vector
     * @param origin Sizes Vector
     */
    void next_position(std::vector<size_t> *pointer, std::vector<size_t> *origin) {
        int current_dim = 0;
        do {
            if (++pointer->at(current_dim) == origin->at(current_dim)) {
                pointer->at(current_dim) = static_cast<size_t>(0);
                ++current_dim;
            }
            else {
                break;
            }
        } while (current_dim < pointer->size());
    }

public:

    virtual operator proxy_labeled_tensor<T, K>() = 0;
};

/**
 * Subclass of proxy tensor. It represents a singular tensor.
 * @tparam T Type of the tensors
 * @tparam K Type of the labels
 */
template<typename T, typename K>
class proxy_labeled_tensor : public proxy_tensor<T, K> {

    friend class proxy_tensor<T, K>;

    friend class proxy_op_tensor<T, K>;

    friend class proxy_sum_tensor<T, K>;

    friend class proxy_prod_tensor<T, K>;

private:

    tensor::tensor<T> data;

    /**
     * Create a vector of labels starting from a list of names
     * @param info Tensor
     * @param names List of the label's names
     * @return Vector of label's objects
     */
    static std::vector<label<K>> labels_from_names(tensor::tensor<T> &info, std::initializer_list<K> &names) {
        std::vector<label<K>> result;
        typename std::vector<label<K>>::iterator iter_result;

        size_t current_dim = static_cast<size_t>(0);

        for (const K &name : names) {
            iter_result = result.begin();
            while (iter_result != result.end()) {
                if ((*iter_result).name == name) {
                    (*iter_result).positions.push_back(current_dim);
                    break;
                }
                ++iter_result;
            }
            if (iter_result == result.end()) {
                result.push_back(label<K>(name, info.get_dimension(current_dim), current_dim));
            }
            ++current_dim;
        }
        return result;
    }

    /**
     * Copy a labels list.
     * @param info Tensor
     * @param labels List of label's objects
     * @return Vector of labels
     */
    static std::vector<label<K>> copy_labels(tensor::tensor<T> &info, std::vector<label<K>> &labels) {
        std::vector<label<K>> result;
        typename std::vector<label<K>>::iterator iter = labels.begin();
        for (size_t index = 0; index < labels.size(); ++index) {
            result.push_back(label<K>(iter[index].name, info.get_dimension(index), index));
        }

        return result;
    }

    /**
     * Associates the labels to the actual dimension on the tensor and moves to the specified position.
     * @throw 1 Tried to compose position with not all the labels in common label list
     * @param cm_label_pointer Positions of the common labels list
     * @param cm_labels Common labels list
     * @param [uq_label_pointer] Positions of the unique labels list
     * @param [uq_labels] Unique labels list
     * @return Vector of the position on the tensor
     */
    std::vector<size_t> get_position(std::vector<size_t> *cm_label_pointer, std::vector<label<K>> *cm_labels, std::vector<size_t> *uq_label_pointer = nullptr, std::vector<label<K>> *uq_labels = nullptr) {
        std::vector<size_t> position(data.get_rank());
        size_t order = static_cast<size_t>(0);
        for (auto label_item = proxy_tensor<T, K>::positions_list.begin(); label_item < proxy_tensor<T, K>::positions_list.end(); ++label_item) {
            size_t index = label_item->find_label(cm_labels);
            if (index != std::numeric_limits<size_t>::max()) {
                for (size_t pos = static_cast<size_t>(0); pos < label_item->positions.size(); ++pos)
                    position[label_item->positions[pos]] = cm_label_pointer->at(index);
            }
            else {
                if (uq_labels == nullptr || uq_label_pointer == nullptr) throw 1;
                size_t index = label_item->find_label(uq_labels);
                if (index != std::numeric_limits<size_t>::max()) {
                    for (size_t pos = static_cast<size_t>(0); pos < label_item->positions.size(); ++pos)
                        position[label_item->positions[pos]] = uq_label_pointer->at(index);
                }
            }
            ++order;
        }
        return position;
    }

    proxy_labeled_tensor(tensor::tensor<T> info, std::vector<label<K>> labels) : data(info), proxy_tensor<T, K>::proxy_tensor(copy_labels(info, labels)) {
        assert(info.get_rank() == labels.size());
    }

public:

    proxy_labeled_tensor(tensor::tensor<T> info, std::initializer_list<K> names) : data(info), proxy_tensor<T, K>::proxy_tensor(labels_from_names(info, names)) {
        assert(info.get_rank() == names.size());
    }

    proxy_sum_tensor<T, K> operator+(const proxy_sum_tensor<T, K> &other) {
        /* return other + *this; */
        other.positions_list.push_back(*this);
        return other;
    }

    proxy_sum_tensor<T, K> operator+(const proxy_labeled_tensor<T, K> &other) {
        return proxy_sum_tensor<T, K>(*this, other);
    }

    proxy_prod_tensor<T, K> operator*(const proxy_prod_tensor<T, K> &other) {
        /* return other * *this; */
        other.positions_list.push_back(*this);
        return other;
    }

    proxy_prod_tensor<T, K> operator*(const proxy_labeled_tensor<T, K> &other) {
        return proxy_prod_tensor<T, K>(*this, other);
    }

    operator tensor::tensor<T> () {
        bool to_implode = false;
        auto dead = proxy_tensor<T, K>::positions_list.end();
        for (auto current_label = proxy_tensor<T, K>::positions_list.begin(); current_label < dead; ++current_label) {
            if (current_label->positions.size() > 1)
                to_implode = true;
        }
        if (!to_implode)
            return data;
        else {
            // There is a trace to compute
            return static_cast<tensor::tensor<T>>(static_cast<proxy_labeled_tensor<T, K>>(proxy_prod_tensor<T, K>(*this)));
        }
    }

    operator proxy_labeled_tensor<T, K>() {
        return *this;
    }


};

/**
 * Subclass of proxy tensor. It's an aggregator for all the operation classes.
 * @tparam T Type of the tensors
 * @tparam K Type of the labels
 */
template<typename T, typename K>
class proxy_op_tensor : public proxy_tensor<T, K> {

    friend class proxy_tensor<T, K>;

    friend class proxy_sum_tensor<T, K>;

    friend class proxy_prod_tensor<T, K>;

    friend class proxy_labeled_tensor<T, K>;

private:

    std::vector<label<K>> unique_labels;
    std::vector<label<K>> common_labels;
    std::vector<proxy_labeled_tensor<T, K>> positions_list;

    proxy_op_tensor(proxy_labeled_tensor<T, K> st, proxy_labeled_tensor<T, K> nd) {
        positions_list.push_back(st);
        positions_list.push_back(nd);
    }

    proxy_op_tensor() {}

    /**
     * From the labels list it separate the common labels to the unique labels.
     */
    void divide_labels() {
        typename std::vector<proxy_labeled_tensor<T, K>>::iterator current, succ, dead = positions_list.end();
        for (current = positions_list.begin(); current < dead; ++current) {
            for (label<K> &l: current->positions_list) {
                if (l.find_label(&common_labels) == std::numeric_limits<size_t>::max()) {
                    proxy_tensor<T, K>::positions_list.push_back(l);

                    for (succ = current + 1; succ < dead; ++succ) {
                        if (l.find_label(&succ->proxy_tensor<T, K>::positions_list) != std::numeric_limits<size_t>::max()) {
                            common_labels.push_back(l);
                            break;
                        }
                    }
                    if (succ == dead) {
                        unique_labels.push_back(l);
                    }
                }
            }
        }
    }

    /**
     * This function check if a label is in every item of the labeled positions list.
     * @param l Label to search
     * @return true if the label is in all the labeled positions, otherwise false
     */
    bool is_label_in_all_positions(label<K> *l) {
        for (proxy_labeled_tensor<T, K> &proxy_tensor : positions_list) {
            if (l->find_label(&proxy_tensor.positions_list) == std::numeric_limits<size_t>::max())
                return false;
        }
        return true;
    }

    virtual proxy_labeled_tensor<T, K> eval() {};

public:
    operator tensor::tensor<T> () {
        return static_cast<tensor::tensor<T>>(static_cast<proxy_labeled_tensor<T, K>>(*this));
    }

    virtual operator proxy_labeled_tensor<T, K>() = 0;

};

/**
 * Subclass to manage the product of tensors.
 * @tparam T Type of the tensors
 * @tparam K Type of the labels
 */
template<typename T, typename K>
class proxy_prod_tensor : public proxy_op_tensor<T, K> {

    friend class proxy_tensor<T, K>;

    friend class proxy_op_tensor<T, K>;

    friend class proxy_sum_tensor<T, K>;

    friend class proxy_labeled_tensor<T, K>;

private:

    proxy_prod_tensor(proxy_labeled_tensor<T, K> st, proxy_labeled_tensor<T, K> nd) : proxy_op_tensor<T, K>::proxy_op_tensor(st, nd) {
    }

    proxy_prod_tensor(proxy_labeled_tensor<T, K> st) : proxy_op_tensor<T, K>::proxy_op_tensor() {
        proxy_op_tensor<T, K>::positions_list.push_back(st);
    }

    /**
     * Calculates the final result of the product of a list of tensors.
     * @return Proxy labeled tensor
     */
    proxy_labeled_tensor<T, K> eval() override {
        proxy_op_tensor<T, K>::divide_labels();
        std::vector<size_t> pointer(proxy_op_tensor<T, K>::unique_labels.size());
        if (pointer.size() && proxy_op_tensor<T, K>::positions_list.size() > 1) {
            // There is at least one unique label in the labeled positions list and it isn't from only one tensor
            std::vector<size_t> unique_labels_size = label<K>::get_labels_sizes(&(proxy_op_tensor<T, K>::unique_labels));
            std::vector<size_t> common_labels_size = label<K>::get_labels_sizes(&(proxy_op_tensor<T, K>::common_labels));
            tensor::tensor<T> computed_tensor(unique_labels_size);
            --pointer[0];
            int current_dim;
            while (!proxy_tensor<T, K>::last_visitable_point(&pointer, &unique_labels_size)) {
                // Going trough the final tensor positions
                proxy_tensor<T, K>::next_position(&pointer, &unique_labels_size);
                std::vector<size_t> common_label_pointer(proxy_op_tensor<T, K>::common_labels.size());
                --common_label_pointer[0];

                while (!proxy_tensor<T, K>::last_visitable_point(&common_label_pointer, &common_labels_size)) {
                    // Going trough the common labels like they were a tensor pointer
                    proxy_tensor<T, K>::next_position(&common_label_pointer, &common_labels_size);
                    T product = 1;
                    for (proxy_labeled_tensor<T, K> &proxy_tensor_ptr : proxy_op_tensor<T, K>::positions_list) {
                        // Going trough the proxy_tensor
                        product *= proxy_tensor_ptr.data.at(proxy_tensor_ptr.get_position(&common_label_pointer,
                                                                                          &(proxy_op_tensor<T, K>::common_labels),
                                                                                          &pointer,
                                                                                          &(proxy_op_tensor<T, K>::unique_labels)));
                    }
                    computed_tensor.at(pointer) += product;
                }
            }
            return proxy_labeled_tensor<T, K>(computed_tensor, proxy_op_tensor<T, K>::unique_labels);
        }
        else {
            // There is a trace to compute
            return trace();
        }
    }

    /**
     * This function calculates the trace of a tensor or a list of tensors by calling the compute_trace
     * @return Proxy labeled tensor of the trace
     */
    proxy_labeled_tensor<T, K> trace() {
        if (proxy_op_tensor<T, K>::positions_list.size() > 1)
            return compute_trace(&(proxy_op_tensor<T, K>::common_labels));
        else
            return compute_trace(&(proxy_op_tensor<T, K>::unique_labels));
    }

    /**
     * Calculates the trace.
     * @param label_list List of the labels to "merge"
     * @return Proxy labeled tensor of the trace
     */
    proxy_labeled_tensor<T, K> compute_trace(std::vector<label<K>> *label_list) {
        std::vector<size_t> common_label_pointer(label_list->size());
        std::vector<size_t> common_labels_size = label<K>::get_labels_sizes(label_list);
        tensor::tensor<T> computed_tensor(1);
        --common_label_pointer[0];

        while (!proxy_tensor<T, K>::last_visitable_point(&common_label_pointer, &common_labels_size)) {
            proxy_tensor<T, K>::next_position(&common_label_pointer, &common_labels_size);
            T product = 1;
            for (proxy_labeled_tensor<T, K> &proxy_tensor_ptr : proxy_op_tensor<T, K>::positions_list) {
                product *= proxy_tensor_ptr.data.at(proxy_tensor_ptr.get_position(&common_label_pointer, label_list));
            }
            computed_tensor.at(static_cast<size_t>(0)) += product;
        }
        K new_label = ++(*std::max_element(label_list->begin(), label_list->end())).name;
        return proxy_labeled_tensor<T, K>(computed_tensor, {new_label});
    }

    operator proxy_labeled_tensor<T, K> () override {
        return eval();
    }

public:
    proxy_sum_tensor<T, K> operator+(const proxy_labeled_tensor<T, K> &other) {
        return proxy_sum_tensor<T, K>(*this, other);
    }

    proxy_sum_tensor<T, K> operator+(const proxy_prod_tensor<T, K> &other) {
        return proxy_sum_tensor<T, K>(*this, other);
    }

    proxy_prod_tensor<T, K> operator*(const proxy_labeled_tensor<T, K> &other) {
        proxy_op_tensor<T, K>::positions_list.push_back(other);
        return *this;
    }

    proxy_prod_tensor<T, K> operator*(const proxy_prod_tensor<T, K> &other) {
        proxy_op_tensor<T, K>::positions_list.insert(
                proxy_op_tensor<T, K>::positions_list.end(),
                other.proxy_op_tensor<T, K>::positions_list.begin(),
                other.proxy_op_tensor<T, K>::positions_list.end());
        return *this;
    }

};

/**
 * Subclass to manage the sum of tensors.
 * @tparam T Type of the tensors
 * @tparam K Type of the labels
 */
template<typename T, typename K>
class proxy_sum_tensor : public proxy_op_tensor<T, K> {

    friend class proxy_tensor<T, K>;

    friend class proxy_op_tensor<T, K>;

    friend class proxy_prod_tensor<T, K>;

    friend class proxy_labeled_tensor<T, K>;

private:

    std::vector<proxy_prod_tensor<T, K>> prod_positions_list;

    proxy_sum_tensor(proxy_labeled_tensor<T, K> st, proxy_labeled_tensor<T, K> nd) : proxy_op_tensor<T, K>::proxy_op_tensor(st, nd) {
    }

    proxy_sum_tensor(proxy_prod_tensor<T, K> st, proxy_labeled_tensor<T, K> nd) : proxy_op_tensor<T, K>::proxy_op_tensor() {
        prod_positions_list.push_back(st);
        proxy_op_tensor<T, K>::positions_list.push_back(nd);
    }

    proxy_sum_tensor(proxy_labeled_tensor<T, K> st, proxy_prod_tensor<T, K> nd) : proxy_sum_tensor(nd, st) {}

    /**
     * Calculates the final result of the product of a list of tensors or product of tensors.
     * @return Proxy labeled tensor of the final tensor
     */
    proxy_labeled_tensor<T, K> eval() override {
        for (proxy_prod_tensor<T, K> proxy_tensor : prod_positions_list) {
            proxy_op_tensor<T, K>::positions_list.push_back(static_cast<proxy_labeled_tensor<T, K>>(proxy_tensor));
        }
        proxy_op_tensor<T, K>::divide_labels();

        assert((proxy_op_tensor<T, K>::unique_labels.size() == static_cast<size_t> (0)));
        for (label<K> &l : proxy_op_tensor<T, K>::common_labels) {
            assert((proxy_op_tensor<T, K>::is_label_in_all_positions(&l)));
        }

        std::vector<size_t> labels_size = label<K>::get_labels_sizes(&(proxy_op_tensor<T, K>::common_labels));
        std::vector<size_t> pointer(proxy_op_tensor<T, K>::common_labels.size());
        --pointer[0];
        tensor::tensor<T> computed_tensor(labels_size);
        while (!proxy_tensor<T, K>::last_visitable_point(&pointer, &labels_size)) {
            proxy_tensor<T, K>::next_position(&pointer, &labels_size);
            for (proxy_labeled_tensor<T, K> &proxy_tensor_ptr : proxy_op_tensor<T, K>::positions_list) {
                computed_tensor.at(pointer) += proxy_tensor_ptr.data.at(
                        proxy_tensor_ptr.get_position(&pointer, &(proxy_op_tensor<T, K>::common_labels)));
            }
        }
        return proxy_labeled_tensor<T, K>(computed_tensor, proxy_op_tensor<T, K>::common_labels);
    }

public:

    proxy_sum_tensor<T, K> operator+(const proxy_labeled_tensor<T, K> &other) {
        proxy_op_tensor<T, K>::positions_list.push_back(other);
        return *this;
    }

    proxy_sum_tensor<T, K> operator+(const proxy_prod_tensor<T, K> &other) {
        prod_positions_list.push_back(other);
        return *this;
    }

    proxy_sum_tensor<T, K> operator+(const proxy_sum_tensor<T, K> &other) {
        proxy_op_tensor<T, K>::positions_list.insert(
                proxy_op_tensor<T, K>::positions_list.end(),
                other.proxy_op_tensor<T, K>::positions_list.begin(),
                other.proxy_op_tensor<T, K>::positions_list.end());

        prod_positions_list.insert(
                prod_positions_list.end(),
                other.prod_positions_list.begin(),
                other.prod_positions_list.end());
        return *this;
    }

    operator proxy_labeled_tensor<T, K> () override {
        return eval();
    }

};

#endif //ES2_PROXY_H
