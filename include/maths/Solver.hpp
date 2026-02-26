#ifndef SOLVER_HPP
# define SOLVER_HPP

# include "Ray.hpp"
# include <vector>

using std::vector;

struct Solution
{
	double	real;
	bool	isComplex;
	Solution();
	Solution(double real, bool isComplex);
};

class Solver
{
	public:
		Solver();
		Solver(const Solver &other);
		~Solver();

		virtual Solution	getBestSolution() const = 0;
};

class LinearSolver : public Solver
{
	private:
		double	a;
		double	b;
	public:
		LinearSolver();
		LinearSolver(double a, double b);
		LinearSolver(const LinearSolver &other);
		~LinearSolver();

		LinearSolver		&operator=(const LinearSolver &other);

		Solution			getBestSolution() const override;
};

class BhaskaraSolver : public Solver
{
	private:
		double	a;
		double	b;
		double	c;
	public:
		BhaskaraSolver();
		BhaskaraSolver(double a, double b, double c);
		BhaskaraSolver(const BhaskaraSolver &other);
		~BhaskaraSolver();

		BhaskaraSolver		&operator=(const BhaskaraSolver &other);

		Solution			getBestSolution() const override;
};

class CardamoSolver : public Solver
{
	private:
		double	a;
		double	b;
		double	c;
		double	d;
	public:
		CardamoSolver();
		CardamoSolver(double a, double b, double c, double d);
		CardamoSolver(const CardamoSolver &other);
		~CardamoSolver();

		CardamoSolver		&operator=(const CardamoSolver &other);

		Solution			getBestSolution() const override;
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
		FerrariSolver();
		FerrariSolver(double a, double b, double c, double d, double e);
		FerrariSolver(const FerrariSolver &other);
		~FerrariSolver();

		FerrariSolver		&operator=(const FerrariSolver &other);

		Solution			getBestSolution() const override;
};

class RayMarchingSolver : public Solver
{
	private:
		Ray		ray;
		double	maxDistance;
		double	epsilon;
		double	maxSteps;
		double	(*sdf)(Point3 point);
		void	*data;
	public:
		RayMarchingSolver();
		RayMarchingSolver(const Ray &ray, double maxDistance, double epsilon, double maxSteps, double (*sdf)(Point3 point), void *data);
		RayMarchingSolver(const RayMarchingSolver &other);
		~RayMarchingSolver();

		RayMarchingSolver	&operator=(const RayMarchingSolver &other);

		Solution			getBestSolution() const override;
};

#endif /* SOLVER_HPP */
