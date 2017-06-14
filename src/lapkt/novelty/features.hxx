
#pragma once

#include <vector>
#include <iostream>
#include <memory>


namespace lapkt { namespace novelty {

//! Features are (ATM) integer values
using FeatureValueT = int;

//! A feature valuation is an ordered set of feature values
using FeatureValuation = std::vector<FeatureValueT>;

//! Base interface. A single novelty feature basically takes a state of a given type and returns a feature value.
template <typename StateT>
class NoveltyFeature {
public:
	virtual ~NoveltyFeature() = default;
	virtual NoveltyFeature* clone() const = 0;
	
	//!
	virtual FeatureValueT evaluate(const StateT& s) const = 0;
	
	//! Prints a representation of the object to the given stream.
	friend std::ostream& operator<<(std::ostream &os, const NoveltyFeature& o) { return o.print(os); }
	virtual std::ostream& print(std::ostream& os) const = 0;
};


//! An GenericFeatureSetEvaluator works for any type of state, for any combination of features
//! that we want to use. In case we want to use only the values of state variables, however, it 
//! might be more performant to use one of the evaluators below.
template <typename StateT>
class GenericFeatureSetEvaluator {
public:
	using FeatureT = NoveltyFeature<StateT>;
	using FeaturePT = std::unique_ptr<FeatureT>;
	
	GenericFeatureSetEvaluator() = default;
	~GenericFeatureSetEvaluator() = default;
	
	GenericFeatureSetEvaluator(const GenericFeatureSetEvaluator&) = delete;
	GenericFeatureSetEvaluator(GenericFeatureSetEvaluator&&) = default;
	GenericFeatureSetEvaluator& operator=(const GenericFeatureSetEvaluator&) = delete;
	GenericFeatureSetEvaluator& operator=(GenericFeatureSetEvaluator&&) = default;
	
	//!
	void add(FeatureT* feature) {
		_features.push_back(FeaturePT(feature));
	}
	
	//!
	FeatureValuation evaluate(const StateT& state) const {
	// 	LPT_INFO("novelty-evaluations", "Evaluating state " << state);
		FeatureValuation values;
		
		values.reserve(_features.size());
		for (const auto& feature:_features) {
			values.push_back(feature->evaluate(state));
			// LPT_INFO("novelty-evaluations", "\t" << _featureMap.feature(k) << ": " << values[k]);
		}

	// 	LPT_DEBUG("heuristic", "Feature evaluation: " << std::endl << print::feature_set(varnames, values));
		return values;
	}
	
	//! The number of features in the set
	unsigned size() const { return _features.size(); }
	
	bool uses_extra_features() const { return true; }
	
protected:
	//! The features in the set
	std::vector<FeaturePT> _features;
};

//! A "straight" evaluator simply returns as a set of features _a const reference_ to the
//! vector of values that underlies the representation of the state. This should work
//! whenever the state has values of one single type (int / bool) and we're not interested
//! in using additional features. It _is_ a somewhat ugly abstraction leak, but I think
//! well-justified for the sake of performance.
template <typename FeatureValueT>
class StraightFeatureSetEvaluator {
public:
	//!
	template <typename StateT>
	const std::vector<FeatureValueT>& evaluate(const StateT& state) const {
		return state.template dump<FeatureValueT>(); //@ see https://stackoverflow.com/a/613132
	}
	
	bool uses_extra_features() const { return false; }
};



} } // namespaces
