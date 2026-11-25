inline ThreadPool& ThreadPool::instance() {
	static ThreadPool pool;
	return pool;
}
inline void ThreadPool::setThreadCount(unsigned total) {
	shutdown();
	total = std::max(1u, total);
	stop = false;
	for (unsigned i = 1; i < total; i++)
		workers.emplace_back([this] { workerLoop(); });
}
inline unsigned ThreadPool::threadCount() const { return static_cast<unsigned>(workers.size() + 1); }
inline void ThreadPool::run(uint32_t n, const std::function<void(uint32_t, uint32_t)>& rangeFn) {
	if (n == 0) return;
	if (workers.empty() || n < kChunk) {
		rangeFn(0, n);
		return;
	}
	{
		std::lock_guard<std::mutex> lock(mutex);
		job = &rangeFn;
		this->n = n;
		next.store(0);
		active = static_cast<unsigned>(workers.size());
		++generation;
	}
	wake.notify_all();
	work();
	std::unique_lock<std::mutex> lock(mutex);
	done.wait(lock, [this] { return active == 0; });
}

inline ThreadPool::~ThreadPool() { shutdown(); }
inline ThreadPool::ThreadPool() { setThreadCount(std::max(1u, std::thread::hardware_concurrency())); }
	
inline void ThreadPool::work() {
	while (true) {
		uint32_t begin = next.fetch_add(kChunk);
		if (begin >= n) break;
		(*job)(begin, std::min(n, begin + kChunk));
	}
}
inline void ThreadPool::workerLoop() {
	uint64_t seen = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(mutex);
		wake.wait(lock, [&] { return stop || generation != seen; });
		if (stop) return;
		seen = generation;
		lock.unlock();

		work();

		lock.lock();
		if (--active == 0) done.notify_one();
	}
}
inline void ThreadPool::shutdown() {
	{
		std::lock_guard<std::mutex> lock(mutex);
		stop = true;
	}
	wake.notify_all();
	for (auto& t : workers) t.join();
	workers.clear();
}

template <typename Fn>
inline void	parallelFor(uint32_t n, Fn&& fn) {
	ThreadPool::instance().run(n, [&fn](uint32_t begin, uint32_t end) {
		for (uint32_t i = begin; i < end; i++) fn(i);
	});
}

template <typename Fn>
inline void	parallelForChunks(uint32_t n, Fn&& fn) {
	ThreadPool::instance().run(n, [&fn](uint32_t begin, uint32_t end) { fn(begin, end); });
}
