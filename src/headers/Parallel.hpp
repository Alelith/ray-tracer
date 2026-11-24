#ifndef PARALLEL_HPP
# define PARALLEL_HPP

# include <cstdint>
# include <functional>
# include <vector>
# include <thread>
# include <mutex>
# include <condition_variable>
# include <atomic>

class ThreadPool {
	public:
		inline static ThreadPool&	instance();
		inline void					setThreadCount(unsigned total);
		inline unsigned				threadCount() const;
		inline void					run(uint32_t n, const std::function<void(uint32_t, uint32_t)>& rangeFn);

		inline ~ThreadPool();
	
	private:
	
		inline ThreadPool();
	
		inline void		work();
		inline void		workerLoop();
		inline void		shutdown();
	
		static constexpr uint32_t	kChunk = 512;
		std::vector<std::thread>	workers;
		std::mutex					mutex;
		std::condition_variable		wake, done;
		uint32_t					n = 0;
		std::atomic<uint32_t>		next{0};
		unsigned					active = 0;
		uint64_t					generation = 0;
		bool						stop = false;

		const std::function<void(uint32_t, uint32_t)>*	job = nullptr;
};

template <typename Fn>
inline void	parallelFor(uint32_t n, Fn&& fn);

template <typename Fn>
inline void	parallelForChunks(uint32_t n, Fn&& fn);

# include "detail/Parallel.ipp"

#endif // PARALLEL_HPP
