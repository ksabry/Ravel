#include "Program.h"

namespace Ravel::SubML
{
	Program::Program()
	{
	}
	Program::~Program()
	{
	}

	void Program::PPrint(std::ostream & output)
	{
		output << "Program {\n";
		
		std::stringstream inner;
		if (substitutions.size() > 0) substitutions[0]->PPrint(inner);
		for (uint32_t i = 1; i < substitutions.size(); i++)
		{
			inner << ",\n";
			substitutions[i]->PPrint(inner);
		}
		output << Indent() << inner.str();

		output << "\n}";
	}
}
