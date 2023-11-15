
#include "main_memory.h"
#include "GAME1v1.h"
#include "GAME2v2.h"
#include "main_label.h"
int main()
{
	
	window=initWindow(800,600);
	setup_textures();
	setup_shaders();
	MAIN_LABEL::setup();
	UPDATE=&MAIN_LABEL::update;
	DRAW=&MAIN_LABEL::draw;
	glEnable(GL_BLEND);
	glClearColor(0.2f,0.3f,0.3f,1.0f);

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		UPDATE();
		DRAW();
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	glDeleteBuffers(VBOS.size(),&VBOS[0]);
	glDeleteVertexArrays(VAOS.size(),&VAOS[0]);
	VAOS.clear();
	VBOS.clear();
	return 0;
}


