
#include <limits>

#include <gtest/gtest.h>
#include <lapkt/novelty/base.hxx>
#include <lapkt/novelty/atom_evaluator.hxx>


#include <fixtures/novelty.hxx>

using namespace lapkt::novelty;

class AtomEvaluatorTest : public NoveltyEvaluatorsFixture {
 public:


  virtual void SetUp() {
	  _evaluator = buildAtomEvaluator(true, 1);
  }


  // A helper tester
	void test_valuation(std::vector<bool> valuation, unsigned expected_novelty) {
		unsigned nov = _evaluator->evaluate(valuation, 1);
		EXPECT_EQ(nov, expected_novelty);
	}
	
	AtomBinaryNoveltyEvaluator* _evaluator;
};




static void test_derive_novel(const std::vector<int>& valuation1, const std::vector<int>& valuation2, const std::vector<unsigned>& expected) {
	EXPECT_EQ(expected, derive_novel(valuation1, valuation2));
}

TEST(derive_novel, empty) {
	test_derive_novel({}, {}, {});
}

TEST(derive_novel, all_repeated) {
	test_derive_novel({1,2,3}, {1,2,3}, {});
}

TEST(derive_novel, one_novel) {
	test_derive_novel({1,2,3}, {1,2,4}, {2});
}

TEST(derive_novel, all_novel) {
	test_derive_novel({1,2,3}, {4,5,6}, {0,1,2});
}




TEST_F(AtomEvaluatorTest, one_novel) {
	test_valuation({true, false, true}, 1);
	test_valuation({true, false, true}, std::numeric_limits<unsigned>::max());
}

