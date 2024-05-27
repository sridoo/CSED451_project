#include "GL/glew.h"

#include "shaderParams.hpp"

void vertexAttribPointerForStaticObj(GLuint vao){
	glVertexAttribPointer(StaticObjVertParam::vPos, 3, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vPos));
	glVertexAttribPointer(StaticObjVertParam::vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vNormal));
	glVertexAttribPointer(StaticObjVertParam::vtexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vTexCoord));
	for (size_t i = 0; i < StaticObjVertParam::vEnd; ++i)
		glEnableVertexArrayAttrib(vao, i);
}