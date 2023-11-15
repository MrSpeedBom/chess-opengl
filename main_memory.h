#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;
#include <GL/glew.h>
#define GLEW_STATIC
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "geometry.h"
#include "shader_s.h"
#include "window_setup.h"


enum peace { BlackSquare=-7,Pawn=1,Bishob,Knight,Rook,Queen,King,WhiteSquare,Light1,Light2,Light3,Light4,OptionsButton,PlayButton };

vector<unsigned int> VBOS,VAOS;
void (*UPDATE)()=nullptr,(*DRAW)()=nullptr;

struct unit
{
	unsigned int VAO,VBO;
	int x,y;
	bool alive=1;
	int type=-1;
	bool color;
	int moves=0;
	int unit_id=-1;
	void draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
	}
	void clear() {
		if(unit_id!=-1) {
			glDeleteBuffers(1,&VBO);
			glDeleteVertexArrays(1,&VAO);
		}
	}
};
struct board_cell
{
	int v=0;
	int color=0;
	int unit_id=-1;
	board_cell(int x) { v=x; }
	board_cell(int x,int c) { v=x; color=c; }
	bool operator==(int x) { return v==x; }
	bool operator<(int x) { return v<x; }
	bool operator<=(int x) { return v<=x; }
	bool operator>(int x) { return v>x; }
	bool operator>=(int x) { return v>=x; }
	bool operator!=(int x) { return v!=x; }

};

unsigned int maketex(string src,bool mask=0,vector<Color> maskc={})
{
	unsigned int res=0;
	cv::Mat im,im1;
	if(!cv::haveImageReader(src))
	{
		cout<<src+":file can't be opened"<<endl;
		return 0;
	}
	im=cv::imread(src);
	cout<<src+" loaded succesfuly!"<<endl;
	im1=im;
	cv::flip(im1,im,0);

	cv::cvtColor(im,im1,cv::COLOR_BGR2RGBA);
	im=im1;
	cv::Vec4b pix;
	if(mask)
		for(int i=0; i<im.size().width; i++)
			for(int j=0; j<im.size().height; j++)
			{
				pix=im.at<cv::Vec4b>(j,i);
				for(int k=0; k<maskc.size(); k++)
					if(pix[0]==maskc[k].r&&pix[1]==maskc[k].g&&
						pix[2]==maskc[k].b)
						im.at<cv::Vec4b>(j,i)[3]=0;
				;
			}
	glGenTextures(1,&res);
	glBindTexture(GL_TEXTURE_2D,res);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,im.size().width,im.size().height,0,
				 GL_RGBA,GL_UNSIGNED_BYTE,im.data);
	return res;
}

unsigned int BUTTONS_TEXTURE,GAME_TEXTURE;
Shader BUTTONS_SHADER,GAME_SHADER;
void setup_textures() {
	GAME_TEXTURE=maketex("chessboard.png",1,{{238, 238, 210}, {118, 150, 86}});
	BUTTONS_TEXTURE=maketex("buttons.png",1,{{127,127,127}});
}
void setup_shaders() {
	BUTTONS_SHADER=Shader("guiShader");
	GAME_SHADER=Shader("sampleShader");
}

unit insert_square(Point la,Point rd,int tex=0,Point dir={0,0},float start_t=0)
{
	vector<float> vertices={la.x, la.y, 0, 1, float(tex), dir.x, dir.y, start_t,
							  rd.x, la.y, 1, 1, float(tex), dir.x, dir.y, start_t,
							  la.x, rd.y, 0, 0, float(tex),dir.x, dir.y, start_t,
							  rd.x, rd.y, 1, 0, float(tex), dir.x, dir.y, start_t};
	vector<GLuint> elements={0, 1, 2, 1, 2, 3};
	GLuint VBO,VAO,EBO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vertices[0]),
				 &vertices[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,elements.size()*sizeof(elements[0]),
				 &elements[0],GL_STATIC_DRAW);
	glVertexAttribPointer(0,2,GL_FLOAT,false,8*sizeof(float),(void*)0);
	glVertexAttribPointer(1,2,GL_FLOAT,false,8*sizeof(float),
						  (void*)(2*sizeof(float)));
	glVertexAttribPointer(2,1,GL_FLOAT,false,8*sizeof(float),
						  (void*)(4*sizeof(float)));
	glVertexAttribPointer(3,2,GL_FLOAT,false,8*sizeof(float),
						  (void*)(5*sizeof(float)));
	glVertexAttribPointer(4,1,GL_FLOAT,false,8*sizeof(float),
						  (void*)(7*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glBindVertexArray(0);
	VBOS.push_back(VBO);
	VBOS.push_back(EBO);
	VAOS.push_back(VAO);
	return {VAO, VBO};
}

struct Button
{
	Point lu,rd;
	unsigned int VAO,VBO;
	int type=-1;
	void (*action)();
	Button() {};
	Button(Point left_up,Point right_down,int Type,void (*func)()=nullptr)
	{
		lu=left_up;
		rd=right_down;
		unit u=insert_square(lu,rd,Type,{0,0},-1);
		VAO=u.VAO;
		VBO=u.VBO;
		action=func;
		type=Type;
	}
	bool inside(Point x)
	{
		return(x.x>=lu.x&&x.x<=rd.x&&x.y<=lu.y&&x.y>=rd.y);
	}
	void draw() {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
	}
	void clear()
	{
		if(type!=-1)
		{
			glDeleteBuffers(1,&VBO);
			glDeleteVertexArrays(1,&VAO);
		}
	}
};
#endif 
