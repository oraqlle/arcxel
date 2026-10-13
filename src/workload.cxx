#include "workload.h"

namespace arcxel {

[[nodiscard]] auto synthetic_work(u32 iterations, f32 seed) -> f32 {
	f32 acc = seed;

	for ([[maybe_unused]] const auto _ : std::views::iota(u32{ 0 }, iterations)) {
		acc = fmaf(acc, 1.0000001f, 0.0000001f);
	}

	return acc;
}

} // namespace arcxel
