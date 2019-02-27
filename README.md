# Freestanding Extentions Library

## Supported compilers

* GCC > 8
* Clang > 5
* MSVC++ 2018 (must support C++17)
  * require heavy config file tweaking

If you have specific needs for a compiler, post an issue and we'll look into that, but it must support C++17!

## Iterators and ranges

3 types of iterators:

* Contiguous iterator for cases where substraction of iterators provides a size
* Non contiguous iterators for where it doesn't but the iterator operate on available data only
* Lazy for all other cases

An iterator MUST have a member with the signature `constexpr iterator_type_t iterator_type;`

A range must provide:

* A typedef of name `associated_iterator`
* Functions with signatures `associated_iterator begin()` and `associated_iterator end()`

## Containers

Features the following containers:

* unordered_map
* unordered_set
* vector
* array
* buffer
* unique_buffer
* pair
* variant
* optional

## Tools for writing allocators

* arena

## Tools for formatted displays

* display_ranges + copy

## Current WIP

* In-memory filesystem
* map
* set
* better allocators