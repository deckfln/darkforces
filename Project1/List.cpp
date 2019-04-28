#include "List.h"
#include "glad/glad.h"
#include "Shader.h"

List::List()
{
	first = nullptr;
	last = nullptr;
	length = 0;
}

Element *List::Add(const std::string source, GLuint type)
{
	// is it already on the list
	Element *current = first;
	for (current = first; current != nullptr; current = current->get_next()) {
		if (type == ((Shader *)current)->get_type() && strcmp(source.c_str(), current->get_source()) == 0) {
			current->add_reference();
			return current;
		}
	}

	Element *shader = new Element();
	if (last) {
		// add at the end of the list
		last = shader->add_at_end(last, shader);
		length++;
	}
	else {
		// add at the head of the list
		last = first = shader->add_at_end(nullptr, shader);
		length = 1;
	}

	return shader;
}

void List::Remove(Shader *shader)
{
}

List::~List()
{
}
