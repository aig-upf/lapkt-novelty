
#include <iostream>

#include "tuples.hxx"
#include <boost/functional/hash.hpp>


namespace lapkt { namespace novelty {

	
	
template <typename ValueT>
std::size_t
Width2TupleHasher<ValueT>::operator()(const Width2Tuple<ValueT>& tuple) const {
	assert(std::get<0>(tuple) < std::get<2>(tuple));
	std::size_t seed = 0;
	boost::hash_combine(seed, std::get<0>(tuple));
	boost::hash_combine(seed, std::get<1>(tuple));
	boost::hash_combine(seed, std::get<2>(tuple));
	boost::hash_combine(seed, std::get<3>(tuple));
	return seed;
}

template <typename ValueT>
std::size_t
Width1TupleHasher<ValueT>::operator()(const Width1Tuple<ValueT>& tuple) const {
	return boost::hash<Width1Tuple<ValueT>>()(tuple);
}

Tuple::Tuple(std::size_t sz) {
	elements.reserve(sz*2);
}

std::ostream& 
Tuple::print(std::ostream& os) const {
	assert(elements.size() % 2 == 0);
	os << "[";
	for (unsigned i = 0; i < elements.size(); i = i+2) {
		os << "(" << elements[i] << "," << elements[i+1] << ")";
	}
	os << "]";
	return os;
}

std::size_t Tuple::Hasher::operator()(const Tuple& tuple) const {
	assert(tuple._check_ordered());
	return boost::hash_range(tuple.elements.begin(), tuple.elements.end());
}

// explicit template instantiation
template struct Width1TupleHasher<bool>;
template struct Width1TupleHasher<int>;
template struct Width2TupleHasher<bool>;
template struct Width2TupleHasher<int>;

} } // namespaces
