#ifndef MAIN_LABEL_H
#define MAIN_LABEL_H
#include "main_memory.h"
#include "GAME1v1.h"
namespace MAIN_LABEL
{
	
	float move_time=1;
	Button game_1v1_b,game_2v2_b,options_b;
	float current_time;
	void mouse_button_callback(GLFWwindow* window,int button,int action,int mods)
	{
		if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_RELEASE)
		{
			double xpos,ypos;
			glfwGetCursorPos(window,&xpos,&ypos);
			xpos=xpos/window_width*2-1;
			ypos=-ypos/window_height*2+1;
			for(Button X : {game_1v1_b,game_2v2_b,options_b})
			{
				if(X.inside(Point{float(xpos),float(ypos)}))X.action();
			}
		}

	}
	void clear();
	void setup();
	void update();
	void draw();
	void come_from_1v1()
	{
		GAME1v1::clear();
		MAIN_LABEL::setup();
		DRAW=&MAIN_LABEL::draw;
		UPDATE=&MAIN_LABEL::update;
	}
	void game_1v1_b_action() {
		MAIN_LABEL::clear();
		GAME1v1::setup();
		GAME1v1::exit_b.action=&come_from_1v1;
		DRAW=&GAME1v1::draw;
		UPDATE=&GAME1v1::update;
	}
	void come_from_2v2() {
		GAME2v2::clear();
		MAIN_LABEL::setup();
		DRAW=&MAIN_LABEL::draw;
		UPDATE=&MAIN_LABEL::update;
	}
	void game_2v2_b_action()
	{
		
		MAIN_LABEL::clear();
		GAME2v2::setup();
		GAME2v2::exit_b.action=&come_from_2v2;
		DRAW=&GAME2v2::draw;
		UPDATE=&GAME2v2::update;
	}
	void option_b_action()
	{
		cout<<"clicked 3"<<endl;
	}
	void setup_buttons()
	{
		game_1v1_b=Button({-0.9,0.9},{0.1,0.4},0,&game_1v1_b_action);
		game_2v2_b=Button({-0.9,0.3},{0.1,-0.2},1,&game_2v2_b_action);
		options_b=Button({-0.9,-0.3},{0.1,-0.8},2,&option_b_action);
	}
	void setup() {
		BUTTONS_SHADER.use();
		glUniform1f(glGetUniformLocation(BUTTONS_SHADER.ID,"move_time"),move_time);
		setup_buttons();
		glfwSetMouseButtonCallback(window,mouse_button_callback);
	}
	void update() {
		current_time=glfwGetTime();
		BUTTONS_SHADER.use();
		
		glUniform1f(glGetUniformLocation(BUTTONS_SHADER.ID,"current_time"),current_time);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D,BUTTONS_TEXTURE);
	}
	void draw() {
		for(Button X : {game_1v1_b,game_2v2_b,options_b})
		{
			glBindVertexArray(X.VAO);
			glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
		}
		
	}
	void clear()
	{
		for(Button X : {game_1v1_b,game_2v2_b,options_b})
		{
			X.clear();
		}
	}
	

}
#endif 
