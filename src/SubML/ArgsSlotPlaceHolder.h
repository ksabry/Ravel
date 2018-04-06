#pragma once

#include "ArgsSlotMatcher.h"

namespace Ravel
{
	namespace SubML
	{
		class ArgsSlotPlaceHolder : public ArgsSlotMatcher
		{
		public:
			ArgsSlotPlaceHolder(size_t size);

			inline virtual Quantifier GetQuantifier() override
			{
				return Quantifier{ size, size };
			}
			
			inline virtual bool IsPlaceHolder() override
			{
				return true;
			}

		protected:
			virtual void BeginInternal() override;
			virtual void ** NextInternal() override;

		private:
			size_t size;
		};
	}
}
