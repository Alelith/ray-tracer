#include "test_utils.hpp"
#include "Solver.hpp"

static const double EPS = 1e-9;

// ax² + bx + c = 0

// x² - 5x + 6 = 0  →  x = 2, x = 3  (two distinct real roots)
static void test_bhaskara_two_real_roots(int &passed, int &failed)
{
	print_case("x\u00b2 - 5x + 6 = 0", "x = 2, x = 3");
	BhaskaraSolver s(1.0, -5.0, 6.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 2, "x²-5x+6=0 must yield 2 solutions");
	ASSERT_FALSE(sols[0].isComplex, "x²-5x+6=0 first root must be real");
	ASSERT_FALSE(sols[1].isComplex, "x²-5x+6=0 second root must be real");

	double r0 = sols[0].real;
	double r1 = sols[1].real;
	// Order may vary; sort for comparison
	double big = r0 > r1 ? r0 : r1;
	double sml = r0 < r1 ? r0 : r1;
	ASSERT_NEAR(big, 3.0, EPS, "x²-5x+6=0 larger root should be 3");
	ASSERT_NEAR(sml, 2.0, EPS, "x²-5x+6=0 smaller root should be 2");

	print_result("BhaskaraSolver: x²-5x+6=0", passed, failed);
}

// x² - 2x + 1 = 0  →  x = 1  (double root, discriminant = 0)
static void test_bhaskara_double_root(int &passed, int &failed)
{
	print_case("x\u00b2 - 2x + 1 = 0", "x = 1 (double root, \u0394 = 0)");
	BhaskaraSolver s(1.0, -2.0, 1.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 1, "x²-2x+1=0 must yield 1 solution (double root)");
	ASSERT_FALSE(sols[0].isComplex, "x²-2x+1=0 root must be real");
	ASSERT_NEAR(sols[0].real, 1.0, EPS, "x²-2x+1=0 root should be 1");

	print_result("BhaskaraSolver: x²-2x+1=0 (double root)", passed, failed);
}

// x² + 1 = 0  →  x = ±i  (no real roots)
static void test_bhaskara_complex_roots(int &passed, int &failed)
{
	print_case("x\u00b2 + 1 = 0", "x = +i, x = -i");
	BhaskaraSolver s(1.0, 0.0, 1.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 2, "x²+1=0 must yield 2 complex solutions");
	ASSERT_TRUE(sols[0].isComplex, "x²+1=0 first root must be complex");
	ASSERT_TRUE(sols[1].isComplex, "x²+1=0 second root must be complex");
	ASSERT_NEAR(sols[0].real, 0.0, EPS, "x²+1=0 real part should be 0");
	ASSERT_NEAR(std::abs(sols[0].imaginary), 1.0, EPS, "x²+1=0 imaginary part should be ±1");
	ASSERT_NEAR(std::abs(sols[1].imaginary), 1.0, EPS, "x²+1=0 imaginary part should be ±1");

	print_result("BhaskaraSolver: x²+1=0 (complex)", passed, failed);
}

// 2x² - 8 = 0  →  x = ±2
static void test_bhaskara_no_b_term(int &passed, int &failed)
{
	print_case("2x\u00b2 - 8 = 0", "x = +2, x = -2");
	BhaskaraSolver s(2.0, 0.0, -8.0);
	auto sols = s.getSolutions();
	print_solutions(sols);

	ASSERT_TRUE(sols.size() == 2, "2x²-8=0 must yield 2 solutions");
	ASSERT_FALSE(sols[0].isComplex, "2x²-8=0 roots must be real");
	ASSERT_FALSE(sols[1].isComplex, "2x²-8=0 roots must be real");
	double r0 = sols[0].real, r1 = sols[1].real;
	double big = r0 > r1 ? r0 : r1;
	double sml = r0 < r1 ? r0 : r1;
	ASSERT_NEAR(big,  2.0, EPS, "2x²-8=0 larger root should be 2");
	ASSERT_NEAR(sml, -2.0, EPS, "2x²-8=0 smaller root should be -2");

	print_result("BhaskaraSolver: 2x²-8=0", passed, failed);
}

// a=0 must throw
static void test_bhaskara_throws_on_zero_a(int &passed, int &failed)
{
	print_case("0x\u00b2 + x + 1 = 0", "throws std::invalid_argument (a = 0)");
	ASSERT_THROWS(BhaskaraSolver(0.0, 1.0, 1.0), std::invalid_argument,
	              "BhaskaraSolver(a=0) must throw invalid_argument");

	print_result("BhaskaraSolver: throws on a=0", passed, failed);
}

void run_bhaskara_solver_tests(int &total_passed, int &total_failed)
{
	print_suite("BhaskaraSolver");
	int p = 0, f = 0;

	test_bhaskara_two_real_roots(p, f);
	test_bhaskara_double_root(p, f);
	test_bhaskara_complex_roots(p, f);
	test_bhaskara_no_b_term(p, f);
	test_bhaskara_throws_on_zero_a(p, f);

	total_passed += p;
	total_failed += f;
}
