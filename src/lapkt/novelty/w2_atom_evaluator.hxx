
#pragma once

#include <cassert>
#include <vector>

#include "base.hxx"
#include <lapkt/tools/logging.hxx>
#include <lapkt/tools/utils.hxx>



namespace lapkt { namespace novelty {

uint32_t _combine_indexes(uint32_t index1, uint32_t index2, uint32_t num_atom_indexes);


template <typename FeatureValueT, typename ValuationIndexerT>
class W2AtomEvaluator : public NoveltyEvaluatorI<FeatureValueT> {
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

	std::vector<bool> _seen_tuples_sz_2;

public:
		
	W2AtomEvaluator(const ValuationIndexerT& indexer, bool ignore_negative) :
		Base(2),
		_indexer(indexer),
		_ignore_negative(ignore_negative),
		_num_atom_indexes(_indexer.num_indexes()),
		_seen_tuples_sz_2(num_combined_indexes(_num_atom_indexes), false)
	{
// 		LPT_INFO("cout", "Built a novelty-2 atom evaluator");
	}


	W2AtomEvaluator(const W2AtomEvaluator&) = default;
	W2AtomEvaluator(W2AtomEvaluator&&) = default;
	W2AtomEvaluator& operator=(const W2AtomEvaluator&) = default;
	W2AtomEvaluator& operator=(W2AtomEvaluator&&) = default;
	W2AtomEvaluator* clone() const override { return new W2AtomEvaluator(*this); }
	

	static unsigned num_combined_indexes(unsigned num_atom_indexes) {
		// If we can have atom indexes in the range [0.._num_atom_indexes-1], then the highest combined index
		// we can have is combine_indexes(_num_atom_indexes-1, _num_atom_indexes-2)
		return _combine_indexes(num_atom_indexes-1, num_atom_indexes-2, num_atom_indexes);
	}


	//! Return the approx. expected size (in bytes) of novelty-2 table.
	uint64_t expected_size() { return expected_size(_num_atom_indexes); }
	
	static uint64_t expected_size(uint32_t num_atom_indexes) {
		unsigned n_combined_indexes = num_combined_indexes(num_atom_indexes);
		return n_combined_indexes / 8;
	}
	
	
	unsigned evaluate(const ValuationT& valuation, unsigned k) override {
		assert(!valuation.empty());

		if (k==0) return std::numeric_limits<unsigned>::max();
			
		if (k!=2) throw std::runtime_error("Unexpected width value");
		
		return evaluate_pairs(valuation) ? 2 : std::numeric_limits<unsigned>::max();
	}

	
	unsigned _evaluate(const ValuationT& valuation, const std::vector<unsigned>& novel, unsigned k) override {
		assert(!valuation.empty());
		
		if (k==0) return std::numeric_limits<unsigned>::max();

		if (k!=2) throw std::runtime_error("Unexpected width value");
		
		return evaluate_pairs(valuation, novel) ? 2 : std::numeric_limits<unsigned>::max();
	}
	
	void mark_nov2atoms_from_last_state(std::vector<std::pair<unsigned, unsigned>>& atoms) const override {
		atoms = _nov2_pairs;
	}	

protected:
	std::vector<std::pair<unsigned, unsigned>> _nov2_pairs;
	
		
	bool evaluate_pairs(const ValuationT& valuation, const std::vector<unsigned>& novel) {
		assert(valuation.size() >= novel.size());
		if (valuation.size() == novel.size()) return evaluate_pairs(valuation); // Just in case
		
		auto all_indexes = index_valuation(valuation);
		auto novel_indexes = index_valuation(novel, valuation);
		
		_nov2_pairs.clear();
		
		for (unsigned feat_index1:novel_indexes) {
			for (unsigned feat_index2:all_indexes) {
				if (feat_index1==feat_index2) continue;
				if (update_sz2_table(feat_index1, feat_index2)) {
					_nov2_pairs.push_back(std::make_pair(feat_index1, feat_index2));
				}
			}
		}
		return !_nov2_pairs.empty();
	}	
	

	bool evaluate_pairs(const ValuationT& valuation) {
		auto all_indexes = index_valuation(valuation);
		unsigned sz = all_indexes.size();
		
		_nov2_pairs.clear();
		
		for (unsigned i = 0; i < sz; ++i) {
			unsigned index_i = all_indexes[i];

			for (unsigned j = i+1; j < sz; ++j) {
				if (update_sz2_table(index_i, all_indexes[j])) {
					_nov2_pairs.push_back(std::make_pair(index_i, all_indexes[j]));
				}
			}
		}
		return !_nov2_pairs.empty();
	}	

	//! Helper. Map a feature valuation into proper indexes. Ignore negative values if so requested.
	std::vector<unsigned> index_valuation(const ValuationT& valuation) {
		unsigned sz = valuation.size();
		std::vector<unsigned> indexes;
		indexes.reserve(sz);
		for (unsigned i = 0; i < sz; ++i) {
			const auto& value = valuation[i];
			if (_ignore_negative && value == 0) continue;
			indexes.push_back(_indexer.to_index(i, value));
		}
		return indexes;
	}
	
	//! This one performs the same mapping but assuming we only want the values given by the indexes in 'novel'
	std::vector<unsigned> index_valuation(const std::vector<unsigned>& novel, const ValuationT& valuation) {
		std::vector<unsigned> indexes;
		indexes.reserve(novel.size());
		for (unsigned i:novel) {
			const auto& value = valuation[i];
			if (_ignore_negative && value == 0) continue;
			indexes.push_back(_indexer.to_index(i, value));
		}
		return indexes;
	}	
	
	bool update_sz2_table(unsigned atom1_index, unsigned atom2_index) {
		uint32_t combined = _combine_indexes(atom1_index, atom2_index, _num_atom_indexes);
		assert(combined < _seen_tuples_sz_2.size());
		auto&& value = _seen_tuples_sz_2[combined]; // see http://stackoverflow.com/a/8399942
		if (!value) { // The tuple is new
			value = true;
			return true;
		}
		return false;
	}	
};

} } // namespaces
