# Assignment #1: Tensor Library
## Goal: Type semantics and ownership policies
##### Due date: 17/11/2019

Implement a templated library handling tensors, i.e., multidimensional arrays.

Tensors are specified by the type they are holding, their rank, i.e., number of dimensions (or indices), and the size of each dimension.

The tensor class must provide various data-access functions:

1. **Direct access**: the user must be able to directly access any entry of the tensor by providing all the indexes. I suggest the use of an operator ()
2. **Slicing**: the user can fix one (or more) index producing a lower rank tensor **sharing** the same data
3. **Iterators**: the class must provide **forward** iterators to the **full content** of the tensor or to the content along any one index, keeping the other indices fixed 

The library must also provide services for **flattening** two or more indices into one and to generate sub-windows of any given tensor changing the starting-point and end-point of each index. For efficiency reasons, flattening can be restricted to **consecutive** ranges of indices.

The library must allow data to be shared in all slicing, flattening and windowing operations, and also must provide a sharing construction and assignment, but must allow a way to force copying on construction and assignment. It is up to you to decide how each behavior should be invoked. Consider also what move semantics are reasonable for this type semantics.

The tensors, once created, should be unmodifiable in their dimensions (while modifiable in their content), but the library must provide for 2 different levels of compile time knowledge of the tensor's shape:

1. **No knowledge**: there is no compile time information about rank or dimensions of the tensor, only type
2. **Rank only**: the tensor has rank information in its type, but no dimensional information

The various types of tensors should be able to share data if compatible.

## Implementation notes
The data should be maintained in a consecutive area. The template specification must maintain an array of the strides and width of each dimension.

* **Stride**: distance in the next element of this index
* **Width**: number of entries for this index

For example a rank 3 tensor of size (3,4,5) represented in right-major order will have strides (20,5,1) and width (3,4,5). Entry (i,j,k) will be at index (20 \* i + 5 \* j + 1 \* k) in the flat storage.

# Solution
The developed solution is available inside the [*output*](https://github.com/FabioDainese/Advanced_Algorithms/tree/master/Assignments/1/Output/) folder. For a more detailed explanation about it, feel free to take a look at the [documentation](https://github.com/FabioDainese/Advanced_Algorithms/tree/master/Assignments/1/Output%20documentation.pdf) file.
