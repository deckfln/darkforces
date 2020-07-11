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
	if (m_isStatic) {
		return false;	// never derefernce
	}
	return (--references) == 0;
}

Reference::~Reference()
{
}
