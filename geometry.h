#ifndef GEOMETRY_STRUCT_H
#define GEOMETRY_STRUCT_H
struct Point {
    float x, y, z;
    float r = 0, g = 0, b = 0, a = 0;
    Point operator+(Point p) {
        return Point{ x + p.x,y + p.y,z + p.z,r,g,b,a };
    }
    Point operator-(Point p) {
        return Point{ x - p.x,y - p.y,z - p.z,r,g,b,a };
    }
    Point operator*(Point p) {
        return Point{ x * p.x,y * p.y,z * p.z,r,g,b,a };
    }
    void out() {
        cout << x << " " << y << " " << z << " " << r << " " << g << " " << b << " " << endl;
    }
};
struct Color {
    float r=0, g=0, b=0, a=0;
};
struct drawable{
    GLuint id;
    bool uses_elements = 0;
    GLuint size=0;
    bool textured=0;
    GLuint texture;
    GLuint shader = 0;
    GLuint draw_time = 0;//implement to sort drawing(not implemented yet
};

struct character {
    float x, y,speed=1;
    bool moving = 0;
    int direction = -1;
    GLuint id;
    float start_time = 0;
};

struct Texture {
    int heright=0, width=0;
    unsigned int id;
};


Point getpoints(int x,int y,bool b,const Point &lu,const Point &rd,const unsigned int H,const unsigned int W)
{  // 0 lu, 1 rd    ///  x>   y^
    if(!b)
    {
        return {lu.x+float(x)*(rd.x-lu.x)/W,
                rd.y+float(y+1)*(lu.y-rd.y)/H};
    }
    else
        return {lu.x+float(x+1)*(rd.x-lu.x)/W,
                rd.y+float(y)*(lu.y-rd.y)/H};
}
#endif