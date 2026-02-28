#ifndef SOLVER_HPP
# define SOLVER_HPP

# include "Ray.hpp"
# include <vector>
# include <memory>

using std::vector;
using std::shared_ptr;

struct Solution
{
	double	real;
	double	imaginary;
	bool	isComplex;
	Solution();
	Solution(double real, double imaginary, bool isComplex);
};

class Solver
{
	public:
		virtual ~Solver() = default;

		virtual vector<Solution>	getSolutions() const = 0;
};

class LinearSolver : public Solver
{
	private:
		double	a;
		double	b;
	public:
		LinearSolver(double a, double b);

		vector<Solution>	getSolutions() const override;
};

class BhaskaraSolver : public Solver
{
	private:
		double	a;
		double	b;
		double	c;
	public:
		BhaskaraSolver(double a, double b, double c);

		vector<Solution>	getSolutions() const override;
};

class CardamoSolver : public Solver
{
	private:
		double	a;
		double	b;
		double	c;
		double	d;
	public:
		CardamoSolver(double a, double b, double c, double d);

		vector<Solution>	getSolutions() const override;
};

class FerrariSolver : public Solver
{
	private:
		double	a;
		double	b;
		double	c;
		double	d;
		double	e;
	public:
		FerrariSolver(double a, double b, double c, double d, double e);

		vector<Solution>	getSolutions() const override;
};

class RayMarchingSolver : public Solver
{
	private:
		Ray					ray;
		double				maxDistance;
		double				epsilon;
		double				maxSteps;
		double				(*sdf)(Point3 point);
		shared_ptr<void>	data;
	public:
		RayMarchingSolver(const Ray &ray, double maxDistance, double epsilon, double maxSteps, double (*sdf)(Point3 point), shared_ptr<void> data);

		vector<Solution>	getSolutions() const override;
};

#endif /* SOLVER_HPP */
