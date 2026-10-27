inline uint32_t rngNextU32(uint32_t& state) {
	state = state * 747796405u + 2891336453u;
	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}
inline float rngNext(uint32_t& state) {
	return static_cast<float>(rngNextU32(state) >> 8) * (1.0f / 16777216.0f);
}
inline uint32_t hash32(uint32_t v) {
	v ^= v >> 16;
	v *= 0x7feb352du;
	v ^= v >> 15;
	v *= 0x846ca68bu;
	v ^= v >> 16;
	return v;
}
inline uint32_t makeSeed(uint32_t pixel, uint32_t sample) {
	return hash32(pixel ^ hash32(sample + 0x9e3779b9u));
}
