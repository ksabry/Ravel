#include "Substitution.h"

namespace Ravel::SubML
{
	Substitution::Substitution(QuantifiedExpressionMatcher * matcher, Populator<Expression ** &, uint32_t &> * populator)
		: matcher(matcher), populator(populator), capture_count(1)
	{
		capture_names.push_back("<EMPTY>");
	}
	Substitution::~Substitution()
	{
		for (char const * name : capture_names)
		{
			delete[] name;
		}
	}

	uint32_t Substitution::CaptureFromName(char const * name)
	{
		for (uint32_t i = 0; i < capture_names.size(); i++)
		{
			if (strcmp(name, capture_names[i]) == 0) return i;
		}
		capture_count++;

		uint32_t length = strlen(name) + 1;
		char * name_copy = new char[length];
		ArrCpy(name_copy, name, length);

		capture_names.push_back(name_copy);
		return capture_count - 1;
	}

	void Substitution::PPrint(std::ostream & output)
	{
		output << "Substitution {\n";
		
		std::stringstream inner;
		matcher->PPrint(inner);
		inner << ",\n";
		populator->PPrint(inner);
		output << Indent() << inner.str();

		output << "\n}";
	}
}
