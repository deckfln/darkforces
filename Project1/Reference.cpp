#include "Reference.h"



Reference::Reference() :
	references(0)
{
}

void Reference::reference(void)
{
	references++;
}

bool Reference::dereference(void)
{
	return (--references) == 0;
}

Reference::~Reference()
{
}
