#include "test_utils.hpp"
#include "Solver.hpp"

static const float EPS = 1e-9;

// ax + b = 0

// 2x + 4 = 0  →  x = -2
static void test_linear_basic_negative(int &passed, int &failed)
{
	print_case("2x + 4 = 0", "x = -2");
	auto sols = solveLinear(2.0, 4.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 1, "2x+4=0 must yield 1 solution");
	ASSERT_FALSE(sols.data[0].isComplex, "2x+4=0 solution must be real");
	ASSERT_NEAR(sols.data[0].real, -2.0, EPS, "2x+4=0 root should be -2");

	print_result("LinearSolver: 2x+4=0", passed, failed);
}

// -3x + 9 = 0  →  x = 3
static void test_linear_basic_positive(int &passed, int &failed)
{
	print_case("-3x + 9 = 0", "x = 3");
	auto sols = solveLinear(-3.0, 9.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 1, "-3x+9=0 must yield 1 solution");
	ASSERT_FALSE(sols.data[0].isComplex, "-3x+9=0 solution must be real");
	ASSERT_NEAR(sols.data[0].real, 3.0, EPS, "-3x+9=0 root should be 3");

	print_result("LinearSolver: -3x+9=0", passed, failed);
}

// 0x + 5 = 0  →  no solution (degenerate case)
static void test_linear_degenerate(int &passed, int &failed)
{
	print_case("0x + 5 = 0", "no solution (degenerate, a=0)");
	auto sols = solveLinear(0.0, 5.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 0, "0x+5=0 must yield 0 solutions (degenerate, a=0)");

	print_result("LinearSolver: 0x+5=0 (degenerate)", passed, failed);
}

// x = 0  →  x = 0   (b=0)
static void test_linear_zero_root(int &passed, int &failed)
{
	print_case("7x = 0", "x = 0");
	auto sols = solveLinear(7.0, 0.0);
	print_solutions(sols);

	ASSERT_TRUE(sols.count == 1, "7x=0 must yield 1 solution");
	ASSERT_FALSE(sols.data[0].isComplex, "7x=0 solution must be real");
	ASSERT_NEAR(sols.data[0].real, 0.0, EPS, "7x=0 root should be 0");

	print_result("LinearSolver: 7x=0", passed, failed);
}

void run_linear_solver_tests(int &total_passed, int &total_failed)
{
	print_suite("LinearSolver");
	int p = 0, f = 0;

	test_linear_basic_negative(p, f);
	test_linear_basic_positive(p, f);
	test_linear_degenerate(p, f);
	test_linear_zero_root(p, f);

	total_passed += p;
	total_failed += f;
}
