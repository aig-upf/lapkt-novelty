#pragma once

#include <limits>

#include <lapkt/search/components/open_lists.hxx>


namespace lapkt { namespace novelty {

    template <typename NodeT, typename FeatureMapT, typename NoveltyEvaluatorT>
    class NoveltyFilter : public lapkt::QueueAcceptorI<NodeT> {

    protected:
        typedef typename NoveltyEvaluatorT::ValuationT ValuationT;

        //! The feature map
        std::unique_ptr<FeatureMapT>        _phi;
        //! Novelty evaluator that will be in charge of evaluating all nodes
        std::unique_ptr<NoveltyEvaluatorT>  _eval;
        //! Cache buffers for feature maps
        ValuationT                          _b1;
        ValuationT                          _b2;

    public:

        NoveltyFilter(FeatureMapT* fmap, NoveltyEvaluatorT* eval) :
            _phi(fmap),
            _eval(eval)
        {}

        ~NoveltyFilter() = default;

    public:

    	unsigned evaluate(const NodeT& node) {
    		unsigned nov;

    		if (node.parent) {
                _b1 = std::move(_phi->map(node.state));
                _b2 = std::move(_phi->map(node.parent->state));
    			// Important: the novel-based computation works only when the parent has the same novelty type and thus goes against the same novelty tables!!!
    			nov = _eval->evaluate(_b1, _b2);
    		} else {
                _b1 = std::move(_phi->map(node.state));
    			nov = _eval->evaluate(_b1);
    		}

    		return nov;
    	}

        //! Returns false iff we want to prune this node during the search
    	bool accept(NodeT& n) override {
    		return evaluate(n) < std::numeric_limits<unsigned>::max();
    	}

    };

} } // namespaces
