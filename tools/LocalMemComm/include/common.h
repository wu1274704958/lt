#pragma once
#include <numeric>

namespace eqd {
	enum class LMC_state : uint8_t
	{
		Success = 0			,
		Failed				,
		Busy				,
		Idle				,
		Uninit				
	};
}