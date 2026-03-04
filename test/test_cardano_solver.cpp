#include "test_utils.hpp"
#include "Solver.hpp"
#include <algorithm>
#include <vector>

static const double EPS = 1e-6;

// ax³ + bx² + cx + d = 0

// Collect only real solutions from a Solutions struct
static std::vector<double> real_roots(const Solutions &sols)
{
	std::vector<double> reals;
	for (int i = 0; i < sols.count; ++i)
		if (!sols.data[i].isComplex)
			reals.push_back(sols.data[i].real);
	std::sort(reals.begin(), reals.end());
	return reals;
}

// x³ - 6x² + 11x - 6 = 0  →  x = 1, 2, 3  (three distinct real roots)
static void test_cardano_three_real_roots(int &passed, int &failed)
{
	print_case("x\u00b3 - 6x\u00b2 + 11x - 6 = 0", "x = 1, x = 2, x = 3");
	// Passes unnormalized: a=1, b=-6, c=11, d=-6
	auto sols = solveCardano(1.0, -6.0, 11.0, -6.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 3, "x³-6x²+11x-6=0 must yield 3 solutions");

	auto reals = real_roots(sols);
	ASSERT_TRUE(reals.size() == 3, "x³-6x²+11x-6=0 must have 3 real roots");
	ASSERT_NEAR(reals[0], 1.0, EPS, "smallest root should be 1");
	ASSERT_NEAR(reals[1], 2.0, EPS, "middle root should be 2");
	ASSERT_NEAR(reals[2], 3.0, EPS, "largest root should be 3");

	print_result("CardamoSolver: x³-6x²+11x-6=0", passed, failed);
}

// x³ - 1 = 0  →  x = 1 (real), two complex cube-roots of unity
static void test_cardano_one_real_two_complex(int &passed, int &failed)
{
	print_case("x\u00b3 - 1 = 0", "x = 1, x = -1/2 \u00b1 (\u221a3/2)i");
	// a=1, b=0, c=0, d=-1
	auto sols = solveCardano(1.0, 0.0, 0.0, -1.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 3, "x³-1=0 must yield 3 solutions");

	auto reals = real_roots(sols);
	ASSERT_TRUE(reals.size() == 1, "x³-1=0 must have exactly 1 real root");
	ASSERT_NEAR(reals[0], 1.0, EPS, "x³-1=0 real root should be 1");

	int complex_count = 0;
	for (int i = 0; i < sols.count; ++i)
		if (sols.data[i].isComplex) complex_count++;
	ASSERT_TRUE(complex_count == 2, "x³-1=0 must have 2 complex roots");

	print_result("CardamoSolver: x³-1=0", passed, failed);
}

// x³ + 1 = 0  →  x = -1 (real), two complex roots
static void test_cardano_negative_real_root(int &passed, int &failed)
{
	print_case("x\u00b3 + 1 = 0", "x = -1, x = 1/2 \u00b1 (\u221a3/2)i");
	auto sols = solveCardano(1.0, 0.0, 0.0, 1.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 3, "x³+1=0 must yield 3 solutions");

	auto reals = real_roots(sols);
	ASSERT_TRUE(reals.size() == 1, "x³+1=0 must have exactly 1 real root");
	ASSERT_NEAR(reals[0], -1.0, EPS, "x³+1=0 real root should be -1");

	print_result("CardamoSolver: x³+1=0", passed, failed);
}

// x³ - 3x + 2 = 0  →  x = 1 (double), x = -2  (discriminant = 0)
static void test_cardano_repeated_root(int &passed, int &failed)
{
	print_case("x\u00b3 - 3x + 2 = 0", "x = 1 (double root), x = -2");
	auto sols = solveCardano(1.0, 0.0, -3.0, 2.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 3, "x³-3x+2=0 must yield 3 solutions");

	auto reals = real_roots(sols);
	// All three should be real (1, 1, -2)
	for (int i = 0; i < sols.count; ++i)
		ASSERT_FALSE(sols.data[i].isComplex, "x³-3x+2=0 all roots must be real");
	ASSERT_NEAR(reals[0], -2.0, EPS, "x³-3x+2=0 smallest root should be -2");
	ASSERT_NEAR(reals[1],  1.0, EPS, "x³-3x+2=0 middle root should be 1");
	ASSERT_NEAR(reals[2],  1.0, EPS, "x³-3x+2=0 largest root should be 1");

	print_result("CardamoSolver: x³-3x+2=0 (repeated)", passed, failed);
}

// a=0  →  returns empty Solutions (no throw)
static void test_cardano_zero_a(int &passed, int &failed)
{
	print_case("0x\u00b3 + x\u00b2 + x + 1 = 0", "returns empty Solutions (a = 0, not cubic)");
	auto s0 = solveCardano(0.0, 1.0, 1.0, 1.0);
	ASSERT_TRUE(s0.count == 0, "solveCardano(a=0) must return empty Solutions");

	print_result("CardamoSolver: returns empty on a=0", passed, failed);
}

void run_cardano_solver_tests(int &total_passed, int &total_failed)
{
	print_suite("CardamoSolver (Cardano's method)");
	int p = 0, f = 0;

	test_cardano_three_real_roots(p, f);
	test_cardano_one_real_two_complex(p, f);
	test_cardano_negative_real_root(p, f);
	test_cardano_repeated_root(p, f);
	test_cardano_zero_a(p, f);

	total_passed += p;
	total_failed += f;
}
