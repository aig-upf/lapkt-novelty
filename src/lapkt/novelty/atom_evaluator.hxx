
#pragma once

#include "w1_atom_evaluator.hxx"
#include "w2_atom_evaluator.hxx"


namespace lapkt { namespace novelty {

//!	
template <typename FeatureValueT, typename ValuationIndexerT>
class CompoundAtomEvaluator : public NoveltyEvaluatorI<FeatureValueT> {
public:
	using Base = NoveltyEvaluatorI<FeatureValueT>;
	using ValuationT = typename Base::ValuationT;
	
	using W1EvaluatorT = W1AtomEvaluator<FeatureValueT, ValuationIndexerT>;
	using W2EvaluatorT = W2AtomEvaluator<FeatureValueT, ValuationIndexerT>;


protected:
	
	//!
	W1EvaluatorT _w1evaluator;
	W2EvaluatorT _w2evaluator;


public:
		
	CompoundAtomEvaluator(const ValuationIndexerT& indexer, bool ignore_negative) :
		Base(2),
		_w1evaluator(indexer, ignore_negative),
		_w2evaluator(indexer, ignore_negative)
	{
// 		LPT_INFO("cout", "Built a compound atom evaluator");
	}


	CompoundAtomEvaluator(const CompoundAtomEvaluator&) = default;
	CompoundAtomEvaluator(CompoundAtomEvaluator&&) = default;
	CompoundAtomEvaluator& operator=(const CompoundAtomEvaluator&) = default;
	CompoundAtomEvaluator& operator=(CompoundAtomEvaluator&&) = default;
	CompoundAtomEvaluator* clone() const override { return new CompoundAtomEvaluator(*this); }
	

	//! Return the approx. expected size of the evaluator
	uint64_t expected_size() {
		return _w1evaluator.expected_size() + _w2evaluator.expected_size();
	}
	static uint64_t expected_size(uint32_t num_atom_indexes) { 
		return W1EvaluatorT::expected_size(num_atom_indexes) + W2EvaluatorT::expected_size(num_atom_indexes);
	}
	
	
	unsigned evaluate(const ValuationT& valuation, unsigned k) override {
		assert(!valuation.empty());

		if (k==0) return std::numeric_limits<unsigned>::max();
		if (k==1) return _w1evaluator.evaluate(valuation, k);
		if (k==2) return _w2evaluator.evaluate(valuation, k);
		
		throw std::runtime_error("Unexpected width value");
	}
	
	unsigned _evaluate(const ValuationT& valuation, const std::vector<unsigned>& novel, unsigned k) override {
		assert(!valuation.empty());
		
		if (k==0) return std::numeric_limits<unsigned>::max();
		if (k==1) return _w1evaluator._evaluate(valuation, novel, k);
		if (k==2) return _w2evaluator._evaluate(valuation, novel, k);
		
		throw std::runtime_error("Unexpected width value");
	}
	
	void mark_atoms_in_novelty1_table(std::vector<bool>& atoms) const override {
		_w1evaluator.mark_atoms_in_novelty1_table(atoms);
	}
	
	void mark_nov2atoms_from_last_state(std::vector<std::pair<unsigned, unsigned>>& atoms) const override {
		_w2evaluator.mark_nov2atoms_from_last_state(atoms);
	}
	
	void reset() override {
		_w1evaluator.reset();
		_w2evaluator.reset();
	}	
	
};

} } // namespaces
