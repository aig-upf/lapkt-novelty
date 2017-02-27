

#pragma once

#include <limits>

#include <lapkt/search/components/open_lists.hxx>


namespace lapkt { namespace novelty {
	
template <typename NodeT, typename FeatureSetT, typename NoveltyEvaluatorT>
class NoveltyBasedAcceptor : public lapkt::QueueAcceptorI<NodeT> {
protected:
	//! The set of features used to compute the novelty
	const FeatureSetT& _features;

	//! A single novelty evaluator will be in charge of evaluating all nodes
	std::unique_ptr<NoveltyEvaluatorT> _evaluator;

public:
	NoveltyBasedAcceptor(const FeatureSetT& features, NoveltyEvaluatorT* evaluator) :
		_features(features),
		_evaluator(evaluator)
	{}

	~NoveltyBasedAcceptor() = default;

	//! Returns false iff we want to prune this node during the search
	unsigned evaluate(const NodeT& node) {
		unsigned nov;
		
		if (node.parent) {
			// Important: the novel-based computation works only when the parent has the same novelty type and thus goes against the same novelty tables!!!
			nov = _evaluator->evaluate(_features.evaluate(node.state), _features.evaluate(node.parent->state));
		} else {
			nov = _evaluator->evaluate(_features.evaluate(node.state));
		}
		
		return nov;
	}
	
	bool accept(NodeT& n) override {
		return evaluate(n) < std::numeric_limits<unsigned>::max();
	}		
};

} } // namespaces
