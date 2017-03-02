
#pragma once

#include <cassert>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include <boost/functional/hash.hpp>

#include <lapkt/novelty/tuples.hxx>
#include "features.hxx"
#include "base.hxx"

namespace lapkt { namespace novelty {


//! A generic novelty evaluator that accepts all sorts of feature valuations as long as
//! their type is (homogeneously) FeatureValueT
//! The evaluator has somewhat optimized data structures for computing novelty 1 and 2,
//! but is also able to compute higher novelty values.
template <typename FeatureValueT>
class GenericNoveltyEvaluator : public NoveltyEvaluatorI<FeatureValueT> {
public:
	using Base = NoveltyEvaluatorI<FeatureValueT>;
	using ValuationT = typename Base::ValuationT;
	
	GenericNoveltyEvaluator() = delete;
	GenericNoveltyEvaluator(unsigned max_novelty) :
		Base(max_novelty), _tables(max_novelty + 1)
	{}
	
	GenericNoveltyEvaluator(const GenericNoveltyEvaluator&) = default;
	GenericNoveltyEvaluator(GenericNoveltyEvaluator&&) = default;
	GenericNoveltyEvaluator& operator=(const GenericNoveltyEvaluator&) = default;
	GenericNoveltyEvaluator& operator=(GenericNoveltyEvaluator&&) = default;
	GenericNoveltyEvaluator* clone() const override { return new GenericNoveltyEvaluator(*this);	}

	
	//! Evaluate assuming all elements in the valuation can be novel
	unsigned evaluate(const ValuationT& valuation, unsigned k) override {
		this->setup_all_features_novel(valuation);
		return _evaluate(valuation, this->_all_features_novel, k);
	}	
	
protected:
	
	//! Check only if the valuation contains a width-'k' tuple which is novel; return k if that is the case, or MAX if not
	unsigned _evaluate(const ValuationT& valuation, const std::vector<unsigned>& novel, unsigned k) override;
	
	bool evaluate_width_1_tuples(const ValuationT& current, const std::vector<unsigned>& novel);
	bool evaluate_width_2_tuples(const ValuationT& current, const std::vector<unsigned>& novel);	
	
protected:
	//! Maximum novelty value to be computed
	unsigned _max_novelty;
	
	using NoveltyTable = std::unordered_set<Tuple, Tuple::Hasher>;
	using NoveltyTables = std::vector<NoveltyTable>;
	
	std::unordered_set<Width1Tuple<FeatureValueT>, Width1TupleHasher<FeatureValueT>> _width_1_tuples;
	std::unordered_set<Width2Tuple<FeatureValueT>, Width2TupleHasher<FeatureValueT>> _width_2_tuples;
	
	NoveltyTables _tables;
};

} } // namespaces
