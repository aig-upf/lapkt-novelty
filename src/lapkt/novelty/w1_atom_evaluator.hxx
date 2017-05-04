
#pragma once

#include <cassert>
#include <vector>

#include "base.hxx"
#include <lapkt/tools/logging.hxx>
#include <lapkt/tools/utils.hxx>



namespace lapkt { namespace novelty {

//! An optimized novelty-1 evaluator
//! The novelty-1 table is represented as a vector of bools.
template <typename FeatureValueT, typename ValuationIndexerT>
class W1AtomEvaluator : public NoveltyEvaluatorI<FeatureValueT> {
public:
	using Base = NoveltyEvaluatorI<FeatureValueT>;
	using ValuationT = typename Base::ValuationT;


protected:
	//! The indexer that maps each pair of (feature, value) to a feature-index.
	ValuationIndexerT _indexer;

	//! Whether we want to ignore "negative" values, i.e. values of 0.
	bool _ignore_negative;

	//! The total number of possible feature-indexes that can be given by the feature valuation indexer.
	uint32_t _num_atom_indexes;

	//! The tuples of size 1 that we have seen so far
	std::vector<bool> _seen_tuples_sz_1;


public:
	
	W1AtomEvaluator(const ValuationIndexerT& indexer, bool ignore_negative) :
		Base(1),
		_indexer(indexer),
		_ignore_negative(ignore_negative),
		_num_atom_indexes(_indexer.num_indexes()),
		_seen_tuples_sz_1(_num_atom_indexes, false)
	{
// 		LPT_INFO("cout", "Built a novelty-1 atom evaluator");
	}	

	W1AtomEvaluator(const W1AtomEvaluator&) = default;
	W1AtomEvaluator(W1AtomEvaluator&&) = default;
	W1AtomEvaluator& operator=(const W1AtomEvaluator&) = default;
	W1AtomEvaluator& operator=(W1AtomEvaluator&&) = default;
	W1AtomEvaluator* clone() const override { return new W1AtomEvaluator(*this); }
	
	//! Return the approx. expected size (in bytes) of novelty-1 table.
	uint64_t expected_size() { return expected_size(_num_atom_indexes); }
	static uint64_t expected_size(uint32_t num_atom_indexes) {
		return num_atom_indexes / 8; // Assuming a vector of bools packs 8 bools into a single byte.
	}	


	//! Evaluate assuming all elements in the valuation can be novel
	unsigned evaluate(const ValuationT& valuation, unsigned k) override {
		assert(!valuation.empty());
		assert(k==0 || k==1);

		if (k==0) return std::numeric_limits<unsigned>::max();
		return evaluate_width_1_tuples(valuation) ? 1 : std::numeric_limits<unsigned>::max();
	}

	
	unsigned _evaluate(const ValuationT& valuation, const std::vector<unsigned>& novel, unsigned k) override {
		assert(!valuation.empty());
		assert(k==0 || k==1);
		
		if (k==0) return std::numeric_limits<unsigned>::max();
		return evaluate_width_1_tuples(valuation, novel) ? 1 : std::numeric_limits<unsigned>::max();
	}
	
	void mark_atoms_in_novelty1_table(std::vector<bool>& atoms) const override {
		atoms = _seen_tuples_sz_1; // Copy the vector
	}
	
	void reset() override {
		std::vector<bool> _(_seen_tuples_sz_1.size(), false);
		_seen_tuples_sz_1.swap(_);
	}	
	
protected:
	bool evaluate_width_1_tuples(const ValuationT& valuation, const std::vector<unsigned>& novel) {
		bool exists_novel_tuple = false;
		for (unsigned var_index:novel) {
			exists_novel_tuple |= update_tuple1(var_index, valuation[var_index]);
		}
		return exists_novel_tuple;
	}
	
	//! Assume all elements in the valuation can be new.
	//! NOTE this closely mirrors the code of the method with the `novel` parameter,
	//! but we favor here performance over avoiding code duplication.
	bool evaluate_width_1_tuples(const ValuationT& valuation) {
		bool exists_novel_tuple = false;
		for (unsigned var_index = 0; var_index < valuation.size(); ++var_index) {
			exists_novel_tuple |= update_tuple1(var_index, valuation[var_index]);
		}
		return exists_novel_tuple;
	}	
	//! Helper. Returns true if the given feature is novel in the index of 1-tuples.
	bool update_tuple1(unsigned index, const FeatureValueT& value) {
		if (_ignore_negative && value == 0) return false;

		unsigned atom_index = _indexer.to_index(index, value);
		std::vector<bool>::reference ref = _seen_tuples_sz_1[atom_index];
		if (!ref) { // The tuple is new
			ref = true;
			return true;
		}
		return false;
	}
};

} } // namespaces
