#pragma once

#include "Matcher.hpp"

namespace Ravel::SubML
{
	template<typename... TArgs>
	class PassMatcher : public Matcher<TArgs...>
	{
	public:
		PassMatcher()
		{
		}
		~PassMatcher()
		{
		}

		virtual PassMatcher<TArgs...> * DeepCopy() override
		{
			return new PassMatcher<TArgs...>();
		}

	protected:
		virtual void BeginInternal() override
		{
		}

		virtual uint64_t * NextInternal() override
		{
			this->Finish();
			return this->match_captures;
		}
	
	public:
		virtual void PPrint(std::ostream & output) override
		{
			output << "PassMatcher()";
		}
	};
}