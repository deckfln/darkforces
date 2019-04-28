#include "Element.h"



Element::Element()
{
	references = 1;
}

Element *Element::get_next(void)
{
	return next;
}

Element *Element::add_at_end(Element *last, Element *el)
{
	if (last) {
		last->next = el;
		el->next = nullptr;
		el->prev = last;
	}
	else {
		el->next = nullptr;
		el->prev = nullptr;
	}
	return el;
}

void Element::add_reference(void)
{
	references++;
}

Element::~Element()
{
}
