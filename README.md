# dac
Distributed Algebraic Computations

## Target use case
The focus lies in numerical problems requiring many algebraic operations, where the overhead of a function call for a mathematical operation may be neglected.

E.g. if only a few dozen vector/matrix operations in a few dimensions is required, one will be better off using Eigen or some BLAS-like library.

## Features
### Standard
- Automatic derivations
- Global optimzation over whole computational graph

### Unique
- Everything is a n-Tensor, greatly simplifying derivative handling (e.g. derivative of a 2-Tensor = Matrix is a 3-Tensor)
- Easy access to special objects (e.g. Kronecker delta, ...) with special implementations for operations using them.
- Interface is closer to standard mathematical literature, e.g. one defines a vector space and may then declare something an element of that space
- This enables group representations on that space which in turn optimizes the numerics: E.g. one may then use objects from the rotations group acting on that space.
- The computations use C-Code only with no external libraries and may thus be run on any hardware (see TODO)

## Design Decisions
### Why generate code?
- 
- Generating custom code is obviously the fastest possible solution
- Makes the code less complicated
- Enables customization, e.g. loop-unrolling according to how many floating point units, ...

### Why C?
- Closest to hardware and supported on all architectures. In particular embedded.

### Why no external libraries?
- 

## TODO
- [ ]	Rethink that vector space pointer business.
- [ ]	Introduce "scaling" to vectors, so that e.g. for scalar multiplication not the whole vector needs to be multiplied
- [ ]   Introduce optimized memory handling: Not every node needs to be statically allocated. Memory may actually be globally optimized on graph
- [ ]   How to deal with special vectors? E.g. starting with Kronecker? They all require a special implementation.
- [ ]   To enable DAC on non-unix single-core targets, it should be possible to build without threads.