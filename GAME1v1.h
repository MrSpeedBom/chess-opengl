#ifndef GAME1V1_H
#define GAME1V1_H
#include "main_memory.h"
namespace GAME1v1
{
	int turn=0;

	Button exit_b,restart_b;
	float current_time,move_time=0.5;
	bool black_side=0;
	int last_moved_peace_id=-1;
	int H=8,W=8;
	vector<board_cell> board;

	Point boardlu{-0.75, 0.8},boardrd={0.98, -0.8};

	vector<unit> boardunits,boardlight[4];
	vector<int> light;
	void buildBoard()
	{
		unit get;
		for(int i=0; i<W; i++)
			for(int j=0; j<H; j++)
			{
				if((i+j)%2==0)
					get=insert_square(getpoints(i,j,0,boardlu,boardrd,H,W),getpoints(i,j,1,boardlu,boardrd,H,W),1);
				else
					get=insert_square(getpoints(i,j,0,boardlu,boardrd,H,W),getpoints(i,j,1,boardlu,boardrd,H,W),0);
				boardunits.push_back(get);
			}
		for(int i=0; i<H*W; i++)
		{
			for(int k=0; k<4; k++)
			{
				boardlight[k].push_back(insert_square(getpoints(i%W,i/W,0,boardlu,boardrd,H,W),
					getpoints(i%W,i/W,1,boardlu,boardrd,H,W),2+k));
			}
			light.push_back(0);
		}
	}
	bool inside_board(int x,int y)
	{
		return x>=0&&x<W&&y>=0&&y<H;
	}
	vector<unit> chesspeaces;
	void distpeaces()
	{
		int x=0,y=0;
		unit get;
		int set_tex;

		for(int i=0; i<H*W; i++)
		{
			x=i%W;
			y=i/W;
			if(board[i]!=0)
			{
				set_tex=abs(board[i].v)-1+6*(board[i].color+1);
				get=insert_square(getpoints(x,y,0,boardlu,boardrd,H,W),getpoints(x,y,1,boardlu,boardrd,H,W),set_tex);
				get.x=x;
				get.y=y;
				get.color=board[i]<0;
				get.type=board[i].v;
				get.unit_id=board[i].unit_id=chesspeaces.size();
				chesspeaces.push_back(get);
			}
		}
	}
	void empty_lights()
	{
		for(int i=0; i<H*W; i++)
			if(light[i]>0) light[i]=0;
	}

	void flip_board()
	{
		return;
		black_side=!black_side;
		GAME_SHADER.use();
		glUniform1f(glGetUniformLocation(GAME_SHADER.ID,"black"),float(black_side));
	}
	void light_square_black(int x,int y,bool e=1,bool only_take=0);
	void light_square_white(int x,int y,bool e=1,bool only_take=0);
	bool chosen=0;
	int cx,cy;
	void move_peace(unit& u,int x,int y,int promotion=0,bool flip=1)
	{
		turn++;
		u.moves++;
		last_moved_peace_id=u.unit_id;
		float cub[4*8];
		glBindBuffer(GL_ARRAY_BUFFER,u.VBO);
		glGetBufferSubData(GL_ARRAY_BUFFER,0,(4*8)*sizeof(float),cub);
		for(int i=0; i<4*8; i+=8)
		{
			cub[i]+=cub[i+5];
			cub[i+1]+=cub[i+6];
			cub[i+7]=current_time;
			cub[i+6]=(y-cy)*(boardlu.y-boardrd.y)/H;
			cub[i+5]=(x-cx)*(boardrd.x-boardlu.x)/W;
			if(promotion!=0)
				cub[i+4]=promotion;
		}
		glBufferSubData(GL_ARRAY_BUFFER,0,(4*8)*sizeof(float),cub);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		board[x+W*y]=board[cx+cy*W];
		board[cx+cy*W].v=0;
		board[cx+cy*W].unit_id=0;
		for(int i=0; i<chesspeaces.size(); i++)
		{
			if(chesspeaces[i].x==x&&chesspeaces[i].y==y)
			{
				chesspeaces[i].alive=0;
			}
		}
		u.x=x;
		u.y=y;
		if(promotion!=0)
		{
			u.type=promotion;
			board[x+y*W].v=promotion%6+1;
			if((promotion/6)%2==0)board[x+y*W].v*=-1;
		}
		if(flip)
			flip_board();
	}

	bool promotion_dialog=0;
	unit promotions[4];
	Point promotion_pos;
	void show_promotion_dialog(int x,int y,int color=0)
	{////
		promotion_pos.x=x;
		promotion_pos.y=y;
		promotion_pos.r=color;
		promotion_dialog=1;
		for(int i=2; i<=5; i++)
		{
			if(promotions[i-2].type!=-1)
			{
				glDeleteBuffers(1,&promotions[i-2].VBO);
				glDeleteVertexArrays(1,&promotions[i-2].VAO);
			}
			promotions[i-2]=insert_square(getpoints(x,y,0,boardlu,boardrd,H,W),getpoints(x,y,1,boardlu,boardrd,H,W),i-1+6*(color+1),{((color%2==1 ? -1 : 1)*(i-2)-x+(color%2==1 ? W-1 : 0))*(boardrd.x-boardlu.x)/W,(!(color%2==1) ? 1 : -1)*(boardlu.y-boardrd.y)/H},current_time);
		}

	}


	void light_square(int x,int y,bool e=1,bool only_take=0)
	{
		if(light[x+W*y]==0&&board[x+W*y]!=0)
		{
			chosen=1;
			cx=x;
			cy=y;
		}
		else if(chosen&&light[x+W*y]>0)
		{
			int g=board[cx+cy*W].unit_id;
			if(g>=0&&!(x==cx&&y==cy))
			{
				if(chesspeaces[g].type==Pawn)
				{//white promotion
					if(y==7)
						show_promotion_dialog(x,y,board[cx+cy*W].color);
					else
						if(board[x+y*W].v==0&&abs(x-cx)==1)
						{
							move_peace(chesspeaces[g],x,y);
							chesspeaces[board[x+(y-1)*W].unit_id].alive=0;
							board[x+(y-1)*W].v=0;
						}
						else
							move_peace(chesspeaces[g],x,y);
				}
				else
					if(chesspeaces[g].type==-Pawn)
					{//black promotion
						if(y==0)
							show_promotion_dialog(x,y,board[cx+cy*W].color);
						else
							if(board[x+y*W].v==0&&abs(x-cx)==1)
							{
								move_peace(chesspeaces[g],x,y);
								chesspeaces[board[x+(y+1)*W].unit_id].alive=0;
								board[x+(y-1)*W].v=0;
							}
							else
								move_peace(chesspeaces[g],x,y);
					}
					else
						if(abs(chesspeaces[g].type)==King&&chesspeaces[g].moves==0)
						{//castling
							if(x-cx==2)
							{
								move_peace(chesspeaces[g],x,y,0,0);
								g=board[cx+3+cy*W].unit_id;
								cx+=3;
								x-=1;
								move_peace(chesspeaces[g],x,y);
							}
							else
								if(x-cx==-2)
								{
									move_peace(chesspeaces[g],x,y,0,0);
									g=board[cx-4+cy*W].unit_id;
									cx-=4;
									x+=1;
									move_peace(chesspeaces[g],x,y);
								}
								else
									move_peace(chesspeaces[g],x,y);
						}
						else
							move_peace(chesspeaces[g],x,y);
			}
			empty_lights();
			chosen=0;
			return;
		}
		if(board[x+y*W]>0)
			light_square_white(x,y,e,only_take);
		else
			light_square_black(x,y,e,only_take);
	}
	void light_square_white(int x,int y,bool e,bool only_take)
	{
		if(e) empty_lights();
		if(board[x+y*W]==0&&!only_take) light[x+y*W]=3;
		if(board[x+y*W]==1&&y!=H-1)
		{
			if(!only_take) light[x+y*W]=3;
			if(board[x+(y+1)*W]==0&&!only_take)
			{
				light[x+(y+1)*W]=2;
				if(y==1&&board[x+(y+2)*W]==0) light[x+(y+2)*W]=2;
			}
			if(x<W-1&&(board[x+1+(y+1)*W]<0||(board[x+1+(y)*W]==-Pawn&&last_moved_peace_id==board[x+1+(y)*W].unit_id&&chesspeaces[last_moved_peace_id].moves==1)||only_take))
				light[x+1+(y+1)*W]=1;
			if(x>0&&(board[x-1+(y+1)*W]<0||(board[x-1+(y)*W]==-Pawn&&last_moved_peace_id==board[x-1+(y)*W].unit_id&&chesspeaces[last_moved_peace_id].moves==1)||only_take))
				light[x-1+(y+1)*W]=1;
		}
		if(board[x+y*W]==2)
		{
			if(!only_take) light[x+y*W]=3;
			int xc[ ]={1, -1, -1, 1},yc[ ]={1, 1, -1, -1};
			bool b[4]={1, 1, 1, 1};
			for(int i=1; i<min(W,H); i++)
			{
				for(int j=0; j<4; j++)
				{
					if(inside_board(x+xc[j]*i,y+yc[j]*i)&&b[j])
					{
						if(board[x+xc[j]*i+(y+yc[j]*i)*W]==0)
							light[x+xc[j]*i+(y+yc[j]*i)*W]=2;
						else
						{
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]!=-6||!only_take)
								b[j]=0;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]<0)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]>0&&only_take)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
						}
					}
				}
			}
		}
		if(board[x+y*W]==3)
		{
			if(!only_take) light[x+y*W]=3;
			int xc[ ]={2, 1, -1, -2, -2, -1, 1, 2},
				yc[ ]={1, 2, 2, 1, -1, -2, -2, -1};
			for(int i=0; i<8; i++)
			{
				if(inside_board(x+xc[i],y+yc[i]))
				{
					if(board[x+xc[i]+(y+yc[i])*W]==0)
						light[x+xc[i]+(y+yc[i])*W]=2;
					else if(board[x+xc[i]+(y+yc[i])*W]<0)
						light[x+xc[i]+(y+yc[i])*W]=1;
					if(board[x+xc[i]+(y+yc[i])*W]>0&&only_take)
						light[x+xc[i]+(y+yc[i])*W]=1;
				}
			}
		}
		if(board[x+y*W]==4)
		{
			if(!only_take) light[x+y*W]=3;
			bool b[4]={1, 1, 1, 1};
			int xc[ ]={1, 0, -1, 0},yc[ ]={0, 1, 0, -1};
			for(int i=1; i<min(W,H); i++)
			{
				for(int j=0; j<4; j++)
				{
					if(inside_board(x+xc[j]*i,y+yc[j]*i)&&b[j])
					{
						if(board[x+xc[j]*i+(y+yc[j]*i)*W]==0)
							light[x+xc[j]*i+(y+yc[j]*i)*W]=2;
						else
						{
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]!=-6||!only_take)
								b[j]=0;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]<0)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]>0&&only_take)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
						}
					}
				}
			}
		}
		if(board[x+y*W]==5)
		{
			if(!only_take) light[x+y*W]=3;
			bool b[8]={1, 1, 1, 1, 1, 1, 1, 1};
			int xc[ ]={1, 1, 0, -1, -1, -1, 0, 1};
			int yc[ ]={0, 1, 1, 1, 0, -1, -1, -1};
			for(int i=1; i<min(W,H); i++)
			{
				for(int j=0; j<8; j++)
				{
					if(inside_board(x+xc[j]*i,y+yc[j]*i)&&b[j])
					{
						if(board[x+xc[j]*i+(y+yc[j]*i)*W]==0)
							light[x+xc[j]*i+(y+yc[j]*i)*W]=2;
						else
						{
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]!=-6||!only_take)
								b[j]=0;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]<0)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]>0&&only_take)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
						}
					}
				}
			}
		}
		if(board[x+y*W]==6)
		{
			int xc[ ]={1, 1, 0, -1, -1, -1, 0, 1,0},yc[ ]={0, 1, 1, 1, 0, -1, -1, -1,0};

			if(only_take)
			{
				for(int i=0; i<8; i++)
				{
					if(inside_board(x+xc[i],y+yc[i]))
						light[x+xc[i]+W*(y+yc[i])]=1;
				}
				return;
			}

			for(auto X:chesspeaces)
			{
				if(X.alive&&X.color==1)
				{
					light_square_black(X.x,X.y,0,1);
				}
			}

			for(int i=0; i<9; i++)
			{
				if(inside_board(x+xc[i],y+yc[i])&&
					light[x+xc[i]+W*(y+yc[i])]!=0)
					light[x+xc[i]+W*(y+yc[i])]*=-1;
			}
			if(inside_board(x+3,y))
				if(chesspeaces[board[x+y*W].unit_id].moves==0&&board[x+1+y*W].v==0&&board[x+2+y*W].v==0&&board[x+3+y*W].v==4&&chesspeaces[board[x+3+y*W].unit_id].moves==0
					&&light[x+y*W]!=1&&light[x+1+y*W]==0&&light[x+2+y*W]==0)light[x+2+y*W]=-2;
			if(inside_board(x-4,y))
				if(chesspeaces[board[x+y*W].unit_id].moves==0&&board[x-1+y*W].v==0&&board[x-2+y*W].v==0&&board[x-4+y*W].v==4&&chesspeaces[board[x-4+y*W].unit_id].moves==0
					&&light[x+y*W]!=1&&light[x-1+y*W]==0&&light[x-2+y*W]==0)light[x-2+y*W]=-2;

			empty_lights();
			if(!only_take&&(light[x+W*(y)]==0)) light[x+W*(y)]=3;
			else light[x+W*(y)]=1;
			if(inside_board(x+2,y))
				light[x+2+y*W]*=-1;
			if(inside_board(x-2,y))
				light[x-2+y*W]*=-1;

			for(int i=0; i<8; i++)
			{
				if(inside_board(x+xc[i],y+yc[i]))
				{
					if(light[x+xc[i]+W*(y+yc[i])]!=0&&(board[x+xc[i]+W*(y+yc[i])]<=0||i==8))
						light[x+xc[i]+W*(y+yc[i])]=1;
					else
						light[x+xc[i]+W*(y+yc[i])]=board[x+xc[i]+W*(y+yc[i])]<=0 ? 2 : 0;
				}
			}
		}
	}
	void light_square_black(int x,int y,bool e,bool only_take)
	{
		if(e) empty_lights();
		if(board[x+y*W]==0&&!only_take) light[x+y*W]=3;
		if(board[x+y*W]==-1&&y!=0)
		{
			if(!only_take) light[x+y*W]=3;
			if(board[x+(y-1)*W]==0&&!only_take)
			{
				light[x+(y-1)*W]=2;
				if(y==6&&board[x+(y-2)*W]==0) light[x+(y-2)*W]=2;
			}
			if(x<W-1&&(board[x+1+(y-1)*W]>0||(board[x+1+(y)*W]==Pawn&&last_moved_peace_id==board[x+1+(y)*W].unit_id&&chesspeaces[last_moved_peace_id].moves==1)||only_take))
				light[x+1+(y-1)*W]=1;
			if(x>0&&(board[x-1+(y-1)*W]>0||(board[x-1+(y)*W]==Pawn&&last_moved_peace_id==board[x-1+(y)*W].unit_id&&chesspeaces[last_moved_peace_id].moves==1)||only_take))
				light[x-1+(y-1)*W]=1;
		}
		if(board[x+y*W]==-2)
		{
			if(!only_take) light[x+y*W]=3;
			int xc[ ]={1, -1, -1, 1},yc[ ]={1, 1, -1, -1};
			bool b[4]={1, 1, 1, 1};
			for(int i=1; i<min(W,H); i++)
			{
				for(int j=0; j<4; j++)
				{
					if(inside_board(x+xc[j]*i,y+yc[j]*i)&&b[j])
					{
						if(board[x+xc[j]*i+(y+yc[j]*i)*W]==0)
							light[x+xc[j]*i+(y+yc[j]*i)*W]=2;
						else
						{
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]!=6||!only_take)
								b[j]=0;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]>0)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]<0&&only_take)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
						}
					}
				}
			}
		}
		if(board[x+y*W]==-3)
		{
			light[x+y*W]=3;
			int xc[ ]={2, 1, -1, -2, -2, -1, 1, 2},
				yc[ ]={1, 2, 2, 1, -1, -2, -2, -1};
			for(int i=0; i<8; i++)
			{
				if(inside_board(x+xc[i],y+yc[i]))
				{
					if(board[x+xc[i]+(y+yc[i])*W]==0)
						light[x+xc[i]+(y+yc[i])*W]=2;
					else if(board[x+xc[i]+(y+yc[i])*W]>0)
						light[x+xc[i]+(y+yc[i])*W]=1;
					if(board[x+xc[i]+(y+yc[i])*W]<0&&only_take)
						light[x+xc[i]+(y+yc[i])*W]=1;
				}
			}
		}
		if(board[x+y*W]==-4)
		{
			if(!only_take) light[x+y*W]=3;
			bool b[4]={1, 1, 1, 1};
			int xc[ ]={1, 0, -1, 0},yc[ ]={0, 1, 0, -1};
			for(int i=1; i<min(W,H); i++)
			{
				for(int j=0; j<4; j++)
				{
					if(inside_board(x+xc[j]*i,y+yc[j]*i)&&b[j])
					{
						if(board[x+xc[j]*i+(y+yc[j]*i)*W]==0)
							light[x+xc[j]*i+(y+yc[j]*i)*W]=2;
						else
						{
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]!=6||!only_take)
								b[j]=0;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]>0)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]<0&&only_take)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
						}
					}
				}
			}
		}
		if(board[x+y*W]==-5)
		{
			light[x+y*W]=2;
			bool b[8]={1, 1, 1, 1, 1, 1, 1, 1};
			int xc[ ]={1, 1, 0, -1, -1, -1, 0, 1};
			int yc[ ]={0, 1, 1, 1, 0, -1, -1, -1};
			for(int i=1; i<min(W,H); i++)
			{
				for(int j=0; j<8; j++)
				{
					if(inside_board(x+xc[j]*i,y+yc[j]*i)&&b[j])
					{
						if(board[x+xc[j]*i+(y+yc[j]*i)*W]==0)
							light[x+xc[j]*i+(y+yc[j]*i)*W]=2;
						else
						{
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]!=6||!only_take)
								b[j]=0;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]>0)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
							if(board[x+xc[j]*i+(y+yc[j]*i)*W]<0&&only_take)
								light[x+xc[j]*i+(y+yc[j]*i)*W]=1;
						}
					}
				}
			}
		}
		if(board[x+y*W]==-6)
		{
			int xc[ ]={1, 1, 0, -1, -1, -1, 0, 1,0},yc[ ]={0, 1, 1, 1, 0, -1, -1, -1,0};

			if(only_take)
			{
				for(int i=0; i<8; i++)
				{
					if(inside_board(x+xc[i],y+yc[i]))
						light[x+xc[i]+W*(y+yc[i])]=1;
				}
				return;
			}
			for(auto X:chesspeaces)
			{
				if(X.alive&&X.color==0)
				{
					light_square_white(X.x,X.y,0,1);
				}
			}

			for(int i=0; i<9; i++)
			{
				if(inside_board(x+xc[i],y+yc[i])&&
					light[x+xc[i]+W*(y+yc[i])]>0)
					light[x+xc[i]+W*(y+yc[i])]*=-1;
			};
			if(inside_board(x+3,y))
				if(chesspeaces[board[x+y*W].unit_id].moves==0&&board[x+1+y*W].v==0&&board[x+2+y*W].v==0&&board[x+3+y*W].v==-4&&chesspeaces[board[x+3+y*W].unit_id].moves==0
					&&light[x+y*W]!=1&&light[x+1+y*W]==0&&light[x+2+y*W]==0)light[x+2+y*W]=-2;
			if(inside_board(x-4,y))
				if(chesspeaces[board[x+y*W].unit_id].moves==0&&board[x-1+y*W].v==0&&board[x-2+y*W].v==0&&board[x-4+y*W].v==-4&&chesspeaces[board[x-4+y*W].unit_id].moves==0
					&&light[x+y*W]!=1&&light[x-1+y*W]==0&&light[x-2+y*W]==0)light[x-2+y*W]=-2;
			empty_lights();
			if(!only_take&&(light[x+W*(y)]==0)) light[x+W*(y)]=3;
			else light[x+W*(y)]=1;
			if(inside_board(x+2,y))
				light[x+2+y*W]*=-1;
			if(inside_board(x-2,y))
				light[x-2+y*W]*=-1;
			for(int i=0; i<8; i++)
			{
				if(inside_board(x+xc[i],y+yc[i]))
				{
					if(light[x+xc[i]+W*(y+yc[i])]!=0&&(board[x+xc[i]+W*(y+yc[i])]>=0||i==8))
						light[x+xc[i]+W*(y+yc[i])]=1;
					else
						light[x+xc[i]+W*(y+yc[i])]=board[x+xc[i]+W*(y+yc[i])]>=0 ? 2 : 0;
				}
			}
		}
	}
	void mouse_button_callback(GLFWwindow* window,int button,int action,int mods)
	{
		double xpos,ypos;
		glfwGetCursorPos(window,&xpos,&ypos);
		xpos=xpos/window_width*2-1;
		ypos=-ypos/window_height*2+1;

		if(button==GLFW_MOUSE_BUTTON_LEFT)
		{
			if(action==GLFW_PRESS)
			{


				if(promotion_dialog)
				{
					promotion_dialog=0;
					for(int i=0; i<4; i++)
					{
						Point lu,rd;
						lu=getpoints(i,8,0,boardlu,boardrd,H,W),rd=getpoints(i,8,1,boardlu,boardrd,H,W);
						if(int(promotion_pos.r)%2==1)
						{
							lu=getpoints(-i+W-1,-1,0,boardlu,boardrd,H,W),rd=getpoints(-i+W,-1,1,boardlu,boardrd,H,W);
						}
						if(xpos>=lu.x&&xpos<=rd.x&&ypos<=lu.y&&ypos>=rd.y)
						{
							int g=board[cx+cy*W].unit_id;
							if(g!=-1)
								move_peace(chesspeaces[g],promotion_pos.x,promotion_pos.y,i+1+6*(promotion_pos.r+1));
						}
					}
				}
				else
					if(xpos<=boardrd.x&&xpos>=boardlu.x&&ypos>=boardrd.y&&
						ypos<=boardlu.y)
					{
						if(promotion_dialog)promotion_dialog=0;
						int x=floor((xpos-boardlu.x)/((boardrd.x-boardlu.x)/W));
						int y=floor((ypos-boardrd.y)/((boardlu.y-boardrd.y)/H));
						if(black_side)
						{
							x=W-1-x;
							y=H-1-y;
						}
						if(chosen&&light[x+y*W]>0||board[x+y*W].color==turn%2)
							light_square(x,y);
					}
			}
			else if(action==GLFW_RELEASE)
			{
				for(Button X : {exit_b,restart_b})
				{
					if(X.inside(Point{float(xpos),float(ypos)}))
						X.action();
				}
			}
		}
	}
	void restart_b_action();
	void setup()
	{
		turn=0;
		board.clear();
		board_cell boardpush[ ]={
		{4,0},{3,0},{2,0},{5,0},{6,0},{2,0},{3,0},{4,0},
		{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},
		{0},{0},{0},{0},{0},{0},{0},{0},
		{0},{0},{0},{0},{0},{0},{0},{0},
		{0},{0},{0},{0},{0},{0},{0},{0},
		{0},{0},{0},{0},{0},{0},{0},{0},
		{-1,1},{-1,1},{-1,1},{-1,1},{-1,1},{-1,1},{-1,1},{-1,1},
		{-4,1},{-3,1},{-2,1},{-5,1},{-6,1},{-2,1},{-3,1},{-4,1}
		};
		for(board_cell X : boardpush)board.push_back(X);
		black_side=0;
		last_moved_peace_id=-1;
		chosen=0;
		promotion_dialog=0;

		GAME_SHADER.use();

		glUniform1f(glGetUniformLocation(GAME_SHADER.ID,"move_time"),move_time);
		glUniform1f(glGetUniformLocation(GAME_SHADER.ID,"black"),float(black_side));
		Point center=Point{getpoints(W/2, H/2, 0,boardlu,boardrd,H,W).x, getpoints(W/2, H/2, 1,boardlu,boardrd,H,W).y};
		glUniform2f(glGetUniformLocation(GAME_SHADER.ID,"center"),center.x,center.y);
		buildBoard();
		distpeaces();
		glfwSetMouseButtonCallback(window,mouse_button_callback);
		exit_b=Button({-1,0.8},{-0.8,0.7},3,exit_b.action);
		restart_b=Button({-1,0.65},{-0.8,0.55},4,&restart_b_action);

	}
	void update()
	{
		current_time=glfwGetTime();
		GAME_SHADER.use();
		glUniform1f(glGetUniformLocation(GAME_SHADER.ID,"current_time"),current_time);
	}
	void draw()
	{

		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D,GAME_TEXTURE);
		GAME_SHADER.use();
		for(unit X : boardunits)
		{
			X.draw();
		}
		for(int i=0; i<light.size(); i++)
		{
			if(light[i]>0)
			{
				boardlight[light[i]-1][i].draw();
			}
		}
		for(auto X:chesspeaces)
			if(X.alive)
			{
				X.draw();
			}
		if(promotion_dialog)
			for(int i=0; i<4; i++)
			{
				promotions[i].draw();
			}
		BUTTONS_SHADER.use();
		glBindTexture(GL_TEXTURE_2D,BUTTONS_TEXTURE);
		for(Button X : {exit_b,restart_b})
		{
			X.draw();
		}
	}
	void clear()
	{
		for(int i=0; i<4; i++)
		{
			promotions[i].clear();
		}
		for(unit X : chesspeaces)
		{
			X.clear();
		}
		chesspeaces.clear();
		for(int i=0; i<4; i++)
		{
			for(unit X : boardlight[i])
			{
				X.clear();
			}
			boardlight[i].clear();
		}
		for(unit X : boardunits)
		{
			X.clear();
		}
		boardunits.clear();

		for(Button X : {exit_b,restart_b})
		{
			X.clear();
		}
	}
	void restart_b_action()
	{
		clear();
		setup();
	}

}
#endif 