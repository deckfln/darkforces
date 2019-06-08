#include "glBufferAttribute.h"

class glDynamicBufferAttribute : public glBufferAttribute
{
public:
	glDynamicBufferAttribute(const std::string _name, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element, bool _delete_on_exit);
	~glDynamicBufferAttribute();
};
