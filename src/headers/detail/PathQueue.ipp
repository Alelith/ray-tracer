inline void PathQueue::allocate(uint32_t cap_) {
	cap = cap_;
	zip(*this, [cap_](auto& mine, auto&) { mine.resize(cap_); });
	hitT.resize(cap_);
	hitPrim.resize(cap_);
	count.store(0);
}
inline uint32_t PathQueue::capacity() const { return cap; }
inline uint32_t PathQueue::size() const { return count.load(); }
inline void PathQueue::clear() { count.store(0); }
inline uint32_t PathQueue::append() { return count.fetch_add(1); }
inline void PathQueue::setSize(uint32_t n) { count.store(n); }
inline void PathQueue::appendBlock(PathQueue& block) {
	uint32_t n = block.size();
	if (n == 0) return;
	uint32_t base = count.fetch_add(n);
	zip(block, [&](auto& mine, auto& theirs) {
		std::copy(theirs.begin(), theirs.begin() + n, mine.begin() + base);
	});
}
inline Ray PathQueue::ray(uint32_t i) const { return {{ox[i], oy[i], oz[i]}, {dx[i], dy[i], dz[i]}}; }
inline void PathQueue::setRay(uint32_t i, const Ray& r) {
	ox[i] = r.origin.x;
	oy[i] = r.origin.y;
	oz[i] = r.origin.z;
	dx[i] = r.dir.x;
	dy[i] = r.dir.y;
	dz[i] = r.dir.z;
}
inline Color PathQueue::throughput(uint32_t i) const { return {tr[i], tg[i], tb[i]}; }
inline void PathQueue::setThroughput(uint32_t i, const Color& c) {
	tr[i] = c.x;
	tg[i] = c.y;
	tb[i] = c.z;
}
template <typename F>
inline void PathQueue::zip(PathQueue& o, F&& f) {
	f(ox, o.ox); f(oy, o.oy); f(oz, o.oz);
	f(dx, o.dx); f(dy, o.dy); f(dz, o.dz);
	f(pixel, o.pixel); f(rng, o.rng);
	f(tr, o.tr); f(tg, o.tg); f(tb, o.tb);
	f(countEmission, o.countEmission);
}

inline void ShadowQueue::allocate(uint32_t cap_) {
	ox.resize(cap_);
	oy.resize(cap_);
	oz.resize(cap_);
	dx.resize(cap_);
	dy.resize(cap_);
	dz.resize(cap_);
	tMax.resize(cap_);
	cr.resize(cap_);
	cg.resize(cap_);
	cb.resize(cap_);
	pixel.resize(cap_);
	count.store(0);
}
inline uint32_t ShadowQueue::size() const { return count.load(); }
inline void ShadowQueue::clear() { count.store(0); }
inline uint32_t ShadowQueue::append() { return count.fetch_add(1); }
inline void ShadowQueue::appendBlock(const ShadowQueue& block) {
	uint32_t n = block.size();
	if (n == 0) return;
	uint32_t base = count.fetch_add(n);
	auto copy = [&](const auto& src, auto& dst) { std::copy(src.begin(), src.begin() + n, dst.begin() + base); };
	copy(block.ox, ox); copy(block.oy, oy); copy(block.oz, oz);
	copy(block.dx, dx); copy(block.dy, dy); copy(block.dz, dz);
	copy(block.tMax, tMax);
	copy(block.cr, cr); copy(block.cg, cg); copy(block.cb, cb);
	copy(block.pixel, pixel);
}

inline void IndexQueue::allocate(uint32_t capacity) {
	index.resize(capacity);
	count.store(0);
}
inline uint32_t IndexQueue::size() const { return count.load(); }
inline void IndexQueue::clear() { count.store(0); }
inline void IndexQueue::push(uint32_t pathIndex) { index[count.fetch_add(1)] = pathIndex; }
inline void IndexQueue::appendBlock(const IndexQueue& block) {
	uint32_t n = block.size();
	if (n == 0) return;
	uint32_t base = count.fetch_add(n);
	std::copy(block.index.begin(), block.index.begin() + n, index.begin() + base);
}

template <typename Queue>
Batch<Queue>::Batch(Queue& target, uint32_t maxEntries) : target(target) { local.allocate(maxEntries); }
template <typename Queue>
Batch<Queue>::~Batch() { target.appendBlock(local); }
