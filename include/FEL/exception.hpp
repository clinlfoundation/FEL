#pragma once
#include "fel_config.hpp"
#include <new>

namespace fel{
	class runtime_error{
	protected:
		const char* what_str;
	public:
		runtime_error(const char* what)
		: what_str{what}
		{}

		runtime_error()
		{
			what_str = "an unknown error has occured";
		}

		const char* what()
		{
			return what_str;
		}
	};

	class out_of_range : public runtime_error{
		public:
		out_of_range(const char* what)
		: runtime_error{what}
		{}

		out_of_range()
		: runtime_error{}
		{
			what_str = "out_of_range error has occured";
		}
	};

	class bad_optional : public runtime_error{
		public:
		bad_optional(const char* what)
		: runtime_error{what}
		{}

		bad_optional()
		: runtime_error{}
		{
			what_str = "bad_optional error has occured";
		}
	};

	template<typename Expected>
	class bad_variant_access : public runtime_error{
		public:
		bad_variant_access(const char* what)
		: runtime_error{what}
		{}

		bad_variant_access()
		: runtime_error{}
		{
			what_str = "bad_variant_access error has occured";
		}
	};

	class bad_hashmap_access : public runtime_error{
		public:
		bad_hashmap_access(const char* what)
		: runtime_error{what}
		{}

		bad_hashmap_access()
		: runtime_error{}
		{
			what_str = "bad_hashmap_access error has occured";
		}
	};

	class bad_alloc : public runtime_error{
		public:
		bad_alloc(const char* what)
		: runtime_error{what}
		{}

		bad_alloc()
		: runtime_error{}
		{
			what_str = "bad_alloc error has occured";
		}
	};

	class bad_functor : public runtime_error{
		public:
		bad_functor(const char* what)
		: runtime_error{what}
		{}

		bad_functor()
		: runtime_error{}
		{
			what_str = "bad_functor error has occured";
		}
	};
}