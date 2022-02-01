# Assignment #2: Einstein's Notation
##### Due date: 23/12/2019

Augment the tensor library with the ability to perform tensor operations using Einstein's notation.

Einstein's notation, introduced in 1916 to simplify the notation used for general relativity, expresses tensor operations by rendering implicit the summation operations. According to this notation, repeated indices in a tensorial expression are implicitly summed over, so the expression:

a_{ijk} \cdot b_{j}

Represents a rank 2 tensor ***c*** indexed by *i* and *k* such that:

c_{ik} = Σ_{j} ( a_{ijk} b_{j} )

The notation allows for simple contractions, like:

Tr(a) = a_{ii}

As well as additions (subtractions) and multiplications.

The operations should work with dynamically ranked tensors as well as fixed-rank tensors by maintaining and checking as much static information as possible.

**Hint**: Define index types. Indexing operations with index types should return proxy objects that allow deferred operations and conversion to the correct tensor type.

# Solution
The developed solution is available inside the [*output*](https://github.com/FabioDainese/Advanced_Algorithms/tree/master/Assignments/2/Output/) folder. For a more detailed explanation about it, feel free to take a look at the [documentation](https://github.com/FabioDainese/Advanced_Algorithms/tree/master/Assignments/2/Output%20documentation.pdf) file.
