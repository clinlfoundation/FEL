#pragma once
#include "FEL/memory.hpp"
#include "FEL/exception.hpp"

namespace fel{

	template <typename>
	class function;

	template <typename ret, typename ...args>
	class function<ret(args...)>{
		struct virtual_callable
		{
			virtual ~virtual_callable() = default;
			virtual ret operator() (args...) = 0;
		};

		template<typename fn>
		class callable : virtual_callable{
			fn internal_representation;
		public:
			callable(const fn& func)
			: internal_representation{func}
			{}

			virtual ~callable() override = default;

			ret operator() (args... arg_list)
			{
				return internal_representation(arg_list...);
			}
		};

		enum state_t : uint8_t{
			INACTIVE = 0,
			ACTIVE = 1,
			NO_SOO = 0,
			SOO = 2
		};

		state_t state = 0;
		union{
			virtual_callable* functor = nullptr;
			char inplace[12];
		} self;
		
	public:
		template <typename T>
		function& operator=(T& t) 
		{
			if(state & ACTIVE)
			{
				if(state & SOO)
				{
					((virtual_callable*)&self)->~virtual_callable();
				}
				else
				{
					delete self.functor;
				}
			}
			if constexpr (sizeof(callable<T>) <= sizeof(self))
			{
				new((void*)&self) callable(t);
				state = ACTIVE | SOO;
			}
			else
			{
				self = new callable(t);
				state = ACTIVE | NO_SOO;
			}
		}

		template <typename T>
		function(T t) 
		{
			if constexpr (sizeof(callable<T>) <= sizeof(self))
			{
				new((void*)&self) callable(t);
				state = ACTIVE | SOO;
			}
			else
			{
				self = new callable(t);
				state = ACTIVE | NO_SOO;
			}
		}

		template <typename T>
		function(T& t) 
		{
			if constexpr (sizeof(callable<T>) <= sizeof(self))
			{
				new((void*)&self) callable(t);
				state = ACTIVE | SOO;
			}
			else
			{
				self = new callable(t);
				state = ACTIVE | NO_SOO;
			}
		}

		ret operator()(args... arg_list) const {
			if constexpr (fel_config::has_exceptions)
			{
				if(!(state & ACTIVE))
				{
					throw bad_functor{};
				}
			}
			if(state & SOO)
			{
				return (*(virtual_callable*)&self)(arg_list...);
			}
			else
			{
				return (*(self.functor))(arg_list...);
			}
			
		}

		~function()
		{
			if(state & ACTIVE)
			{
				if(state & SOO)
				{
					((virtual_callable*)&self)->~virtual_callable();
				}
				else
				{
					delete self.functor;
				}
			}
			
		}
	};

}