# Assignment #3: Concurrency and Parallelism
##### Due date: no limit (September 2020)

Parallelize the operations on tensors (additions/subtractions and contractions).

Make so that any deferred expression lunches a given number of worker threads and distribute the operations over those threads.

Each thread computes the operations for a given range of non-repeated indices, writing the result on the target tensor.

## Explanation of "*Concurrency and Parallelism*"

> Extend the matrix library so that the operations can be performed concurrently.
> 
> There are two forms of concurrency to be developed:
> 
> 1. **Concurrent operations**:  In multiple matrix operations like $(A+B) \cdot (C+D)$ the addition $A+B$ and $C+D$ can be performed asynchronously in any order (or in parallel) before the final multiplication. Same goes for sequence of multiplications like $A \cdot B \cdot C \cdot D$ if the optimized multiplication order happens to be $(A \cdot B) \cdot (C \cdot D)$.
> 
> 2. **Parallel matrix multiplication**: With large matrix multiplication the access times through the polymorphic wrapper can induce an sizable overhead. One way around this is to access big fixed-size sub-matrices with each call.
With this optimization matrix $A$ is composed of several sub-matrices ($A_{ij}$), each fetched with a single polymorphic call. Matrix multiplication can be expressed easily in this form, so $C=A \cdot B$ becomes $C_{ij} = ∑_{k} (A_{ik} \cdot C_{kj})$, where now '$\cdot$' denotes the usual matrix multiplication for the sub-matrices. 
Now, each $C_{ij}$ can be computed independently from the other and in parallel.
>

# Solution
The developed solution is available inside the [*output*](https://github.com/FabioDainese/Advanced_Algorithms/tree/master/Assignments/3/Output/) folder. For a more detailed explanation about it, feel free to take a look at the [documentation](https://github.com/FabioDainese/Advanced_Algorithms/tree/master/Assignments/3/Output%20documentation.pdf) file.
