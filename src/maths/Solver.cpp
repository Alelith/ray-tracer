#include "Solver.hpp"

Solution::Solution() : real(0.0), isComplex(false) {}

Solution::Solution(double real, bool isComplex) : real(real), isComplex(isComplex) {}

Solver::Solver() {}

Solver::~Solver() {}
