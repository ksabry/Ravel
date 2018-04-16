#pragma once

#include "Matcher.hpp"
#include "Semantic/Expression.hpp"

namespace Ravel::SubML
{
	using namespace Semantic;

	class ImmediateMatcher : public Matcher<Expression *>
	{
	public:
		ImmediateMatcher(DataType data_type, uint64_t data);
		~ImmediateMatcher();
	
		virtual ImmediateMatcher * DeepCopy() override;

	protected:
		virtual void BeginInternal() override;
		virtual bool NextInternal() override;

	private:
		DataType data_type;
		uint64_t data;

	public:
		virtual void PPrint(std::ostream & output) override;
	};
}