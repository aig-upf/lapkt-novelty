
#pragma once

#include <set>
#include <utility>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>
#include <lapkt/tools/jenkins_12bit.hxx>

namespace aptk
{

	typedef		unsigned 	VariableIndex;
	typedef		unsigned	ValueIndex;

	class ValuesTuple {
	public:

		typedef std::tuple< VariableIndex, ValueIndex > Entry;
		typedef std::vector< Entry >  Container;

		ValuesTuple();
		~ValuesTuple() = default;
		ValuesTuple( std::size_t sz, bool preallocate = true );
		ValuesTuple(const ValuesTuple&) = default;
		ValuesTuple(ValuesTuple&&) = default;
		ValuesTuple& operator=(const ValuesTuple& other) = default;
		ValuesTuple& operator=(ValuesTuple&& other) = default;


		void	add( VariableIndex x, ValueIndex v ) {
			elements.push_back( std::make_tuple(x,v) );
		}

		void	set( unsigned i, VariableIndex x, ValueIndex v ) {
			elements[i] = std::make_tuple(x,v);
		}

		void	finish() {
			std::sort( elements.begin(), elements.end() );
		}

		//! Comparison operators
		inline bool operator==( const ValuesTuple& t ) const { return elements == t.elements; }
		inline bool operator!=( const ValuesTuple& t ) const { return !(operator==(t)); }
		inline bool operator< ( const ValuesTuple& t ) const { return elements < t.elements; }
		inline bool operator> ( const ValuesTuple& t ) const { return t.operator<(*this); }
		inline bool operator<=( const ValuesTuple& t ) const { return !(operator>(t)); }
		inline bool operator>=( const ValuesTuple& t ) const { return !(operator<(t)); }

		typename Container::iterator
		begin()	{ return elements.begin(); }

		typename Container::iterator
		end() { return elements.end(); }

		typename Container::const_iterator
		begin() const { return elements.begin(); }

		typename Container::const_iterator
		end() const { return elements.end(); }

		friend std::ostream& 	operator<<( std::ostream& stream, const ValuesTuple& t );

		class Hasher {
		public:
			std::size_t operator()( const ValuesTuple& t ) const {
				std::size_t hashcode = 0;
				for ( const Entry& e : t.elements ) {
					VariableIndex x;
					ValueIndex v;
					std::tie( x, v ) = e;
					hashcode = jenkins_hash( (ub1*)(&x), sizeof(VariableIndex), hashcode );
					hashcode = jenkins_hash( (ub1*)(&v), sizeof(ValueIndex), hashcode );
				}

				return hashcode;
			}
		};

	protected:

		Container	elements;
	};

	std::ostream& 	operator<<( std::ostream& stream, const ValuesTuple& t );


	typedef	std::set< ValuesTuple >	Tuple;

	class ValuesTupleIterator {
	public:

		ValuesTupleIterator( const std::vector<VariableIndex>& X, const std::vector<ValueIndex>& v, std::size_t sz );

		~ValuesTupleIterator() = default;

		void reset() {
			std::fill(_indexes.begin(), _indexes.begin() + tuple_sz, true);
			count = 0;
		}

		bool next( ValuesTuple& t ) {
			// Check http://stackoverflow.com/a/9430993
			unsigned i = 0, k = 0; 
			for (; i < vars.size(); ++i) {
				if (_indexes[i]) {
					// std::cout << "(" << i << ", " << values[i] << ") ";
					t.set(k, i, values[i]);
					++k;
				}
			}
			++count;
			return std::prev_permutation(_indexes.begin(), _indexes.end());
		}

		unsigned current_tuple_index() const { return count; }

	protected:
		const std::vector<VariableIndex>& vars;
		const std::vector<ValueIndex>& values;
		std::size_t tuple_sz;
		unsigned count;
		
		std::vector<bool> _indexes;
	};



}
