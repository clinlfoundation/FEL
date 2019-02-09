# Freestanding Extentions Library

## Supported compilers

* GCC > 8
* Clang > 5
* MSVC++ 2018 (must support C++17)
  * require heavy config file tweaking

## Iterators and ranges

3 types of iterators:

* Contiguous iterator for cases where substraction of iterators provides a size
* Non contiguous iterators for where it doesn't but the iterator operate on available data only
* Lazy for all other cases

An iterator MUST have a member with the signature `constexpr iterator_type_t iterator_type;`

A range must provide:

* A typedef of name `associated_iterator`
* Functions with signatures `associated_iterator begin()` and `associated_iterator end()`
