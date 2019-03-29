# FEL 2019

## Iterators and ranges

### Expectations

`range.begin()` and `range.end()` are expected from any range.

`range.begin()>range.end()` is always false.

`range.end()<range.begin()` is always false.

`range.begin()` and `range.end()` return a `range_type::associated_iterator`.

A range may implement `range.size()`.

`range.size()` returns a `range_type::associated_iterator::difference_type`.

`range_type::associated_iterator` have a type member `iterator_type` of type `iterator_type_t`. 

If `range_type::associated_iterator::iterator_type` is equal to :
* `iterator_type_t::contiguous_iterator`: 
	* `range.size()` must be defined
	* equal iterators must compare equal to `true`
* `iterator_type_t::non_contiguous_iterator`:
	* `range.size()` may be define
	* equal iterators must compare equal to `true`
* `iterator_type_t::lazy_iterator`: `range.size()` may be defined; comparing two iterators may always return `false`

## Containers


