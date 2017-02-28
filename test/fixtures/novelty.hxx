
#pragma once

#include <memory>
#include <vector>

#include <gtest/gtest.h>


using namespace lapkt::novelty;

//! Accepts feature indexes in the range 0..9, and feature values in the range 0..99
class TestValuationIndexer {
public:
	TestValuationIndexer() {}
	
	unsigned num_indexes() const { return 1000; }
	
	unsigned to_index(unsigned variable, int value) const {
		assert(variable < 10);
		assert(value >= 0 && value < 100);
		return variable * 100 + value;
	}
};

using AtomBinaryNoveltyEvaluator = AtomNoveltyEvaluator<bool, TestValuationIndexer, BoolVectorTuple2Marker>;


class NoveltyEvaluatorsFixture : public testing::Test {
public:
	AtomBinaryNoveltyEvaluator* buildAtomEvaluator(bool ignore_negative, unsigned k) {
		TestValuationIndexer indexer;
		AtomBinaryNoveltyEvaluator* evaluator = AtomBinaryNoveltyEvaluator::create(indexer, ignore_negative, k);
		return evaluator;
	}
};

