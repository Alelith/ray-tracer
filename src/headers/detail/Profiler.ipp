#include <cstdio>

inline Profiler& Profiler::instance() {
	static Profiler instance;
	return instance;
}
inline void Profiler::add(StageID stage, double time) { seconds[static_cast<int>(stage)] += time; }
inline void Profiler::print() const {
	static const char* stageNames[] = {
		"Generate",
		"Intersect",
		"Miss",
		"Emissive",
		"Lambertian",
		"Metal",
		"Dielectric"
	};
	double total = 0.0;
	for (double s : seconds) total += s;
	std::printf("Profiling results:\n");
	for (int i = 0; i < static_cast<int>(StageID::Count); ++i)
		std::printf("  %-16s: %7.3f seconds (%5.1f%%)\n", stageNames[i], seconds[i], (total > 0.0 ? (seconds[i] / total * 100.0) : 0.0));
}

inline ScopedStageTimer::ScopedStageTimer(StageID stage) : id(stage), active(Profiler::instance().enabled) {
	if (active) start = std::chrono::high_resolution_clock::now();
}
inline ScopedStageTimer::~ScopedStageTimer() {
	if (active) {
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		Profiler::instance().add(id, elapsed.count());
	}
}
