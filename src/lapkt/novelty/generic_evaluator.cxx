
#include <cassert>
#include <algorithm>

#include "generic_evaluator.hxx"

namespace lapkt { namespace novelty {
	

template <typename FeatureValueT>
bool
GenericNoveltyEvaluator<FeatureValueT>::evaluate_width_1_tuples(const ValuationT& valuation, const std::vector<unsigned>& novel) {
	bool exists_novel_tuple = false;
	for (unsigned idx:novel) {
		auto res = _width_1_tuples.insert(std::make_pair(idx, valuation[idx]));
		if (res.second) exists_novel_tuple = true; // The tuple is new, hence the novelty of the state is 1
	}
	return exists_novel_tuple;
}

template <typename FeatureValueT>
bool
GenericNoveltyEvaluator<FeatureValueT>::evaluate_width_2_tuples(const ValuationT& valuation, const std::vector<unsigned>& novel) {
	bool exists_novel_tuple = false;
	for (unsigned i = 0; i < novel.size(); ++i) {
		unsigned novel_idx = novel[i];
		int novel_val = valuation[novel_idx];
		
		for (unsigned j = 0; j < novel_idx; ++j) {
			auto res = _width_2_tuples.insert(std::make_tuple(j, valuation[j], novel_idx, novel_val));
			exists_novel_tuple |= res.second;
		}
		
		for (unsigned j = novel_idx+1; j < valuation.size(); ++j) {
			auto res = _width_2_tuples.insert(std::make_tuple(novel_idx, novel_val, j, valuation[j]));
			exists_novel_tuple |= res.second;
		}
	}
	return exists_novel_tuple;
}


template <typename FeatureValueT>
unsigned
GenericNoveltyEvaluator<FeatureValueT>::_evaluate(const ValuationT& valuation, const std::vector<unsigned>& novel, unsigned k) {
	assert(!valuation.empty());

	unsigned novelty = std::numeric_limits<unsigned>::max();
	
	if (k == 0) { // We're actually computing nothing, novelty will always be MAX
		return novelty;
	
		
	} else if (k == 1) {
		if (evaluate_width_1_tuples(valuation, novel)) return 1;
	
		
	} else if (k == 2) {
		if (evaluate_width_2_tuples(valuation, novel)) return 2;
	
		
	} else {
		std::vector<bool> novel_idx(valuation.size(), false);
		for (unsigned idx:novel) { novel_idx[idx] = true; }

		TupleIterator<FeatureValueT> it(k, valuation, novel_idx);
		while (!it.ended()) {
			auto res = _tables[k].insert(it.next());
			if (res.second) novelty = k;
		}
		
	}

	return novelty;
}

// explicit template instantiation
template class GenericNoveltyEvaluator<bool>;
template class GenericNoveltyEvaluator<int>;


} } // namespaces
