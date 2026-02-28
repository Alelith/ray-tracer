#include "test_utils.hpp"
#include <iostream>

// Declared in each test file
void run_linear_solver_tests(int &passed, int &failed);
void run_bhaskara_solver_tests(int &passed, int &failed);
void run_cardano_solver_tests(int &passed, int &failed);
void run_ferrari_solver_tests(int &passed, int &failed);

int main()
{
	int passed = 0;
	int failed = 0;

	run_linear_solver_tests(passed, failed);
	run_bhaskara_solver_tests(passed, failed);
	run_cardano_solver_tests(passed, failed);
	run_ferrari_solver_tests(passed, failed);

	std::cout << "\n────────────────────────────────────────\n";
	std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
	std::cout << "────────────────────────────────────────\n";

	return failed == 0 ? 0 : 1;
}
