#ifndef PROFILER_HPP
# define PROFILER_HPP

# include <chrono>
# include <array>

enum class StageID : int {
	Generate,
	Intersect,
	Miss,
	Emissive,
	Lambertian,
	Metal,
	Dielectric,
	Count
};

class Profiler {
	public:
		bool	enabled = false;

		inline static Profiler&	instance();
		inline void				add(StageID stage, double time);
		inline void				print() const;

	private:
		std::array<double, static_cast<int>(StageID::Count)>	seconds;
};

class ScopedStageTimer {
	public:
		explicit ScopedStageTimer(StageID stage);
		inline ~ScopedStageTimer();
	
	private:
		StageID		id;
		bool		active;
		std::chrono::high_resolution_clock::time_point	start;
};

# include "detail/Profiler.ipp"

#endif // PROFILER_HPP
