#include "Solver.hpp"

Solution::Solution() : real(0.0), imaginary(0.0), isComplex(false) {}

Solution::Solution(double real, double imaginary, bool isComplex) : real(real), imaginary(imaginary), isComplex(isComplex) {}
