#pragma once
#include <FEL/function.hpp>

namespace fel{
	template<typename T>
	class generator
	{
		fel::function<T()> func;
	};
}