## AVL Tree (non-recursive)

The goal has been to implement an AVL tree that coudl be used in real-time safety-critical systems (automotive domain for example). Therefore:

* Adherance to the C11 standard
* No recursion
* No dynamic memory allocation / deallocation
* 100% code coverage with tests
* Rather rigorous static checking done with Clang-tidy (no MISRA-C checker yet available)

### Build instructions

```sh
mkdir build
cmake -S . -B build -DBUILD_UNIT_TESTS=yes
cmake --build build
```
