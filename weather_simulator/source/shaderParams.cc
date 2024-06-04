#include "GL/glew.h"

#include "shaderParams.hpp"

void vertexAttribPointerForStaticObj(GLuint vao){
	glVertexAttribPointer(StaticObjProgramParam::vPos, 3, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vPos));
	glVertexAttribPointer(StaticObjProgramParam::vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vNormal));
	glVertexAttribPointer(StaticObjProgramParam::vtexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vTexCoord));
	for (size_t i = 0; i < StaticObjProgramParam::vEnd; ++i)
		glEnableVertexArrayAttrib(vao, i);
}

GLuint staticObjProgram;
GLuint sunProgram;
GLuint terminalProgram;
GLuint skyBoxProgram;