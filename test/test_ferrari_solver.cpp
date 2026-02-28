#include "test_utils.hpp"
#include "Solver.hpp"
#include <algorithm>

static const double EPS = 1e-5;

// ax⁴ + bx³ + cx² + dx + e = 0

static std::vector<double> real_roots4(const std::vector<Solution> &sols)
{
	std::vector<double> reals;
	for (const auto &s : sols)
		if (!s.isComplex)
			reals.push_back(s.real);
	std::sort(reals.begin(), reals.end());
	return reals;
}

// x⁴ - 10x² + 9 = 0  =  (x²-1)(x²-9) = 0  →  x = ±1, ±3
static void test_ferrari_four_real_roots(int &passed, int &failed)
{
	print_case("x\u2074 - 10x\u00b2 + 9 = 0", "x = -3, x = -1, x = 1, x = 3");
	// a=1, b=0, c=-10, d=0, e=9
	FerrariSolver s(1.0, 0.0, -10.0, 0.0, 9.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 4, "x⁴-10x²+9=0 must yield 4 solutions");

	auto reals = real_roots4(sols);
	ASSERT_TRUE(reals.size() == 4, "x⁴-10x²+9=0 must have 4 real roots");
	ASSERT_NEAR(reals[0], -3.0, EPS, "root[0] should be -3");
	ASSERT_NEAR(reals[1], -1.0, EPS, "root[1] should be -1");
	ASSERT_NEAR(reals[2],  1.0, EPS, "root[2] should be  1");
	ASSERT_NEAR(reals[3],  3.0, EPS, "root[3] should be  3");

	print_result("FerrariSolver: x⁴-10x²+9=0", passed, failed);
}

// x⁴ - 5x² + 4 = 0  =  (x²-1)(x²-4) = 0  →  x = ±1, ±2
static void test_ferrari_four_real_roots_2(int &passed, int &failed)
{
	print_case("x\u2074 - 5x\u00b2 + 4 = 0", "x = -2, x = -1, x = 1, x = 2");
	FerrariSolver s(1.0, 0.0, -5.0, 0.0, 4.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 4, "x⁴-5x²+4=0 must yield 4 solutions");

	auto reals = real_roots4(sols);
	ASSERT_TRUE(reals.size() == 4, "x⁴-5x²+4=0 must have 4 real roots");
	ASSERT_NEAR(reals[0], -2.0, EPS, "root[0] should be -2");
	ASSERT_NEAR(reals[1], -1.0, EPS, "root[1] should be -1");
	ASSERT_NEAR(reals[2],  1.0, EPS, "root[2] should be  1");
	ASSERT_NEAR(reals[3],  2.0, EPS, "root[3] should be  2");

	print_result("FerrariSolver: x⁴-5x²+4=0", passed, failed);
}

// x⁴ - 1 = 0  →  x = ±1 (real), x = ±i (complex)
static void test_ferrari_two_real_two_complex(int &passed, int &failed)
{
	print_case("x\u2074 - 1 = 0", "x = -1, x = 1, x = +i, x = -i");
	FerrariSolver s(1.0, 0.0, 0.0, 0.0, -1.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 4, "x⁴-1=0 must yield 4 solutions");

	auto reals = real_roots4(sols);
	ASSERT_TRUE(reals.size() == 2, "x⁴-1=0 must have 2 real roots");
	ASSERT_NEAR(reals[0], -1.0, EPS, "x⁴-1=0 real root should be -1");
	ASSERT_NEAR(reals[1],  1.0, EPS, "x⁴-1=0 real root should be  1");

	int complex_count = 0;
	for (const auto &sol : sols)
		if (sol.isComplex) complex_count++;
	ASSERT_TRUE(complex_count == 2, "x⁴-1=0 must have 2 complex roots");

	print_result("FerrariSolver: x⁴-1=0", passed, failed);
}

// (x-1)(x-2)(x-3)(x-4) = x⁴ - 10x³ + 35x² - 50x + 24 = 0  →  x = 1,2,3,4
static void test_ferrari_consecutive_roots(int &passed, int &failed)
{
	print_case("x\u2074 - 10x\u00b3 + 35x\u00b2 - 50x + 24 = 0", "x = 1, x = 2, x = 3, x = 4");
	FerrariSolver s(1.0, -10.0, 35.0, -50.0, 24.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 4, "x⁴-10x³+35x²-50x+24=0 must yield 4 solutions");

	auto reals = real_roots4(sols);
	ASSERT_TRUE(reals.size() == 4, "x⁴-10x³+35x²-50x+24=0 must have 4 real roots");
	ASSERT_NEAR(reals[0], 1.0, EPS, "root[0] should be 1");
	ASSERT_NEAR(reals[1], 2.0, EPS, "root[1] should be 2");
	ASSERT_NEAR(reals[2], 3.0, EPS, "root[2] should be 3");
	ASSERT_NEAR(reals[3], 4.0, EPS, "root[3] should be 4");

	print_result("FerrariSolver: (x-1)(x-2)(x-3)(x-4)=0", passed, failed);
}

// a=0 must throw
static void test_ferrari_throws_on_zero_a(int &passed, int &failed)
{
	print_case("0x\u2074 + x\u00b3 + x\u00b2 + x + 1 = 0", "throws std::invalid_argument (a = 0)");
	ASSERT_THROWS(FerrariSolver(0.0, 1.0, 1.0, 1.0, 1.0), std::invalid_argument,
	              "FerrariSolver(a=0) must throw invalid_argument");

	print_result("FerrariSolver: throws on a=0", passed, failed);
}

void run_ferrari_solver_tests(int &total_passed, int &total_failed)
{
	print_suite("FerrariSolver");
	int p = 0, f = 0;

	test_ferrari_four_real_roots(p, f);
	test_ferrari_four_real_roots_2(p, f);
	test_ferrari_two_real_two_complex(p, f);
	test_ferrari_consecutive_roots(p, f);
	test_ferrari_throws_on_zero_a(p, f);

	total_passed += p;
	total_failed += f;
}
