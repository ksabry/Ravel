#include "String.hpp"

namespace Ravel
{
	IndentOStream::IndentOStream(std::ostream & internal, uint32_t size)
		: internal(internal), size(size)
	{
	}

	IndentOStream & IndentOStream::Create(std::ostream & internal, IndentProxy & indent)
	{
		auto result = new IndentOStream(internal, indent.size);
		delete &indent;
		return *result;
	}

	IndentProxy::IndentProxy(uint32_t size)
		: size(size)
	{
	}

	IndentOStream & operator<<(std::ostream & os, IndentProxy & self)
	{
		return IndentOStream::Create(os, self);
	}

	IndentProxy & IndentProxy::Create(uint32_t size)
	{
		return *(new IndentProxy(size));
	}

	IndentProxy & Indent(uint32_t size)
	{
		return IndentProxy::Create(size);
	}
}