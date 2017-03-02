
#include <iostream>

#include "tuples.hxx"

namespace aptk
{

	ValuesTuple::ValuesTuple() {}

	ValuesTuple::ValuesTuple( size_t sz, bool preallocate ) {
		if (preallocate)elements.resize( sz );
		else elements.reserve( sz );
	}


	std::ostream&
	operator<<( std::ostream& stream, const ValuesTuple& t ) {
		stream << "[";
		for ( const ValuesTuple::Entry& e : t.elements ) {
			VariableIndex x;
			ValueIndex v;
			std::tie( x, v ) = e;
			stream << "(" << x << "," << v << ")";
		}
		stream << "]";
		return stream;
	}

	ValuesTupleIterator::ValuesTupleIterator( const std::vector<VariableIndex>& X, const std::vector<ValueIndex>& v, size_t sz )
	: vars(X), values(v), tuple_sz(sz), count(0), _indexes(v.size()) {
		assert( vars.size() >= tuple_sz );
		reset();
	}
}
