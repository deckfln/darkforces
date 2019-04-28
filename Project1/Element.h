#pragma once
class Element
{
	Element *next;
	Element *prev;
	int references;
public:
	Element();
	void add_reference(void);
	char *get_source(void) { return nullptr; };
	Element *get_next(void);
	Element *add_at_end(Element *last, Element *el);
	~Element();
};
