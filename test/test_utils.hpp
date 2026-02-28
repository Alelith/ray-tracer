#pragma once

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Solver.hpp"

// ── Simple test helpers ────────────────────────────────────────────────────────
//
//  Each test function receives (int& passed, int& failed) by reference and uses
//  the macros below to record results.
// ──────────────────────────────────────────────────────────────────────────────

#define ASSERT_TRUE(cond, msg) \
	do { \
		if (cond) { \
			passed++; \
		} else { \
			failed++; \
			std::cerr << "  FAIL [" << __FILE__ << ":" << __LINE__ << "] " \
			          << (msg) << "\n"; \
		} \
	} while (0)

#define ASSERT_FALSE(cond, msg) ASSERT_TRUE(!(cond), msg)

#define ASSERT_NEAR(a, b, tol, msg) \
	ASSERT_TRUE(std::abs((a) - (b)) <= (tol), \
		std::string(msg) + " | expected ~" + std::to_string(b) + " got " + std::to_string(a))

#define ASSERT_THROWS(expr, exc_type, msg) \
	do { \
		try { \
			expr; \
			failed++; \
			std::cerr << "  FAIL [" << __FILE__ << ":" << __LINE__ << "] " \
			          << (msg) << " | no exception thrown\n"; \
		} catch (const exc_type &) { \
			passed++; \
		} catch (...) { \
			failed++; \
			std::cerr << "  FAIL [" << __FILE__ << ":" << __LINE__ << "] " \
			          << (msg) << " | wrong exception type\n"; \
		} \
	} while (0)

// Formats a single Solution as a human-readable string
inline std::string format_solution(const Solution &sol)
{
	std::ostringstream oss;
	if (!sol.isComplex)
	{
		oss << "x = " << sol.real;
	}
	else
	{
		oss << "x = " << sol.real;
		if (sol.imaginary >= 0)
			oss << " + " << sol.imaginary << "i";
		else
			oss << " - " << -sol.imaginary << "i";
	}
	return oss.str();
}

// Prints the solutions obtained from a solver call
inline void print_solutions(const std::vector<Solution> &sols)
{
	std::cout << "  obtained: ";
	for (size_t i = 0; i < sols.size(); ++i)
	{
		if (i > 0) std::cout << ", ";
		std::cout << format_solution(sols[i]);
	}
	std::cout << "\n";
}

// Prints a section header for a group of tests
inline void print_suite(const std::string &name)
{
	std::cout << "\n=== " << name << " ===\n";
}

// Prints the equation being tested and the expected solutions before assertions run
inline void print_case(const std::string &equation, const std::string &expected)
{
	std::cout << "  eq:       " << equation << "\n";
	std::cout << "  expected: " << expected << "\n";
}

// Prints the result of a single test run and returns true on full pass
inline bool print_result(const std::string &name, int passed, int failed)
{
	if (failed == 0)
		std::cout << "  [PASS] " << name << " (" << passed << " assertions)\n";
	else
		std::cout << "  [FAIL] " << name << " (" << passed << " passed, "
		          << failed << " failed)\n";
	return failed == 0;
}
