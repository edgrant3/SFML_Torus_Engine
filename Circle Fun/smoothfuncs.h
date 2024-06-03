#pragma once

#include <vector>

float smoothStep(float x) {
	x = std::max(std::min(x, 1.0f), 0.0f);
	return x * x * (3.0f - 2.0f * x);
}

std::vector<float> smoothstepArray(uint32_t size) {
	std::vector<float> res;
	res.reserve(size);

	for (float i = 0; i < size; ++i) {
		res.push_back(smoothStep(i / static_cast<float>(size - 1)));
	}

	return res;
}

