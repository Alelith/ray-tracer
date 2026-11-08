inline void PathQueue::allocate(uint32_t cap_) {
	cap = cap_;
	ox.resize(cap_);
	oy.resize(cap_);
	oz.resize(cap_);
	dx.resize(cap_);
	dy.resize(cap_);
	dz.resize(cap_);
	pixel.resize(cap_);
	count.store(0);
}
inline uint32_t PathQueue::capacity() const { return cap; }
inline uint32_t PathQueue::size() const { return count.load(); }
inline void PathQueue::clear() { count.store(0); }
inline uint32_t PathQueue::append() { return count.fetch_add(1); }
inline Ray PathQueue::ray(uint32_t i) const { return {{ox[i], oy[i], oz[i]}, {dx[i], dy[i], dz[i]}}; }
inline void PathQueue::setRay(uint32_t i, const Ray& r) {
	ox[i] = r.origin.x;
	oy[i] = r.origin.y;
	oz[i] = r.origin.z;
	dx[i] = r.dir.x;
	dy[i] = r.dir.y;
	dz[i] = r.dir.z;
}
