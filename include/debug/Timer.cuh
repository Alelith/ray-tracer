#ifndef TIMER_CUH
# define TIMER_CUH

# include <chrono>

class Timer
{
	public:
		Timer();

		void start();
		void stop();

		double elapsedMilliseconds() const;
		double elapsedSeconds() const;

		void printElapsed() const;
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;

		bool m_bRunning;
};

#endif /* TIMER_CUH */
