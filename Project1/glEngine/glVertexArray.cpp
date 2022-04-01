#include "glVertexArray.h"
#include "glVertexAttribute.h"


glVertexArray::glVertexArray()
{
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glVertexArray::unbind();

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	//glBufferObject::unbind();
}

void glVertexArray::bind(void)
{
	glBindVertexArray(m_id);
}

void glVertexArray::unbind(void)
{
	//glBufferObject::unbind();
	glBindVertexArray(0);
}

/**
 * Add a label for debugging
 */
void glVertexArray::label(const char* s)
{
	glObjectLabel(GL_VERTEX_ARRAY, m_id, -1, s);
}

void glVertexArray::draw(GLenum mode, bool indexed, int count)
{
	bind(); 

	if (indexed) {
		glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(mode, 0, count);
	}
}

glVertexArray::~glVertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}
