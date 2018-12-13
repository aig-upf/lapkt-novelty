
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

	using Base::evaluate; // Give visibility to the base class evaluate method

	//! Evaluate assuming all elements in the valuation can be novel
	unsigned evaluate(const ValuationT& valuation, unsigned k) override {
		setup_all_features_novel(valuation);
		return _evaluate(valuation, _all_features_novel, k);
	}

	virtual void mark_tuples_in_novelty1_table(std::vector<Width1Tuple<int>>& tuples) const override {
		tuples.assign( _width_1_tuples.begin(), _width_1_tuples.end());
	}

	void reset() override {
		_width_1_tuples.clear();
		_width_2_tuples.clear();
		for (auto& table:_tables) table.clear();
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

	//! This is used to cache a vector <0,1,...,k> of appropriate length and spare the creation of one each time we need it.
	mutable std::vector<unsigned> _all_features_novel;

	void setup_all_features_novel(const ValuationT& valuation) {
		std::size_t num_features = valuation.size();
		if (_all_features_novel.size() != num_features) {
			_all_features_novel.resize(num_features);
			std::iota(_all_features_novel.begin(), _all_features_novel.end(), 0);
		}
	}

	bool check_w1_table(unsigned variable, const FeatureValueT& value) const {
		return _width_1_tuples.find(std::make_pair(variable, value)) == _width_1_tuples.end();
	}

	bool update_w1_table(unsigned variable, const FeatureValueT& value) {
		auto res = _width_1_tuples.insert(std::make_pair(variable, value));
		return res.second;
	}
};

} } // namespaces
