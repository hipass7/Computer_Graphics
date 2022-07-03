#pragma once
#pragma once
#pragma once

#include <math.h>
#include <iostream>
#include <fstream>
#include "gl\freeglut.h"		// OpenGL header files
#include <list>

#include <vector>
#include <algorithm>
#include <sstream>
#include <math.h>
#include <time.h>
#include "gl/glext.h"
#include <map>

#include "Library/loadpng.h"
#include "Library/process_image.h"
#include "Library/gl_texture.h"

//#define TIME_CHECK_
#define DEPTH_CALIB_
#pragma warning(disable:4996)
#define BLOCK 4
#define READ_SIZE 110404//5060//110404
#define scale 20
//#define TEST 14989
//#define STATIC_

using namespace std;

struct Vertex {
    float X;
    float Y;
    float Z;
    int index_1;
    int index_2;
    int index_3;
};
//
//struct MMesh {
//	int V1;
//	int V2;
//	int V3;
//	int V4;
//	int T1;
//	int T2;
//	int T3;
//	int T4;
//};

// variables for GUI
const float TRACKBALLSIZE = 0.8f;
const int RENORMCOUNT = 97;
//const int width = KinectBasic::nColorWidth;
//const int height = KinectBasic::nColorHeight;

GLint drag_state = 0;
GLint button_state = 0;

GLint rot_x = 0;
GLint rot_y = 0;
GLint trans_x = 0;
GLint trans_y = 0;
GLint trans_z = 0;

GLubyte mytexels[276][276][3];
GLubyte wall[512][512][3];
// ¸¸µç °Í
GLubyte door[512][512][3];
GLubyte floor1[512][512][3];


int add_depth_flag = 0;
int model_flag = 0;
int depth_display_flag = 0;
int geodesic_skel[23][5] = { 0 };
int trcon = 0;
float zmin = 100000, zmax = -100000;

int side_status[50] = { 0 };

float quat[4] = { 0 };
float t[3] = { 0 };

Vertex skt[23];
BOOLEAN bTracked = false;
bool checkt = false;
Vertex* vertexx;
Vertex* vertexx_color;


bool recheck;

// variables for display OpenGL based point viewer
int dispWindowIndex = 0;
GLuint dispBindIndex = 0;
const float dispPointSize = 2.0f;

// variables for display text
string dispString = "";
const string dispStringInit = "Depth Threshold: D\nInfrared Threshold: I\nNonlocal Means Filter: N\nPick BodyIndex: P\nAccumulate Mode: A\nSelect Mode: C,B(select)\nSave: S\nReset View: R\nQuit: ESC";
string frameRate;

HANDLE hMutex;
//KinectBasic kinect;

// functions for GUIs
void InitializeWindow(int argc, char* argv[]);

// high-level functions for GUI
void draw_center();
void idle();
void display();
void close();
void special(int, int, int) {}
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void reshape(int, int);
void motion(int, int);


// basic functions for computation/GUI
// trackball codes were imported from those of Gavin Bell
// which appeared in SIGGRAPH '88
void vzero(float*);
void vset(float*, float, float, float);
void vsub(const float*, const float*, float*);
void vcopy(const float*, float*);
void vcross(const float* v1, const float* v2, float* cross);
float vlength(const float* v);
void vscale(float* v, float div);
void vnormal(float* v);
float vdot(const float* v1, const float* v2);
void vadd(const float* src1, const float* src2, float* dst);

void trackball(float q[4], float, float, float, float);
//void add_quats(float*, float*, float*);
void axis_to_quat(float a[3], float phi, float q[4]);
void normalize_quat(float q[4]);
float tb_project_to_sphere(float, float, float);
void build_rotmatrix(float m[4][4], float q[4]);
void Reader();
void DrawObj();
void DrawMeshObj();
void Setskt();

//CameraSpacePoint m_SpacePoint[JointType::JointType_Count];
void Track();

#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include <time.h>

#include <fstream>
#include <map>
#include <vector>

#define WIDTH 600
#define HEIGHT 600

#define INTERVAL 15

class Model {
private:
    static int count_char(std::string& str, char ch) {
        int c = 0;
        int length = str.length() - 1;
        for (int i = 0; i < length; i++) {
            if (str[i] == ch)
                c++;
        }
        return c;
    }

    static bool has_double_slash(std::string& str) {
        int length = str.length() - 2;
        for (int i = 0; i < length; i++) {
            if (str[i] == '/' && str[i + 1] == '/')
                return true;
        }
        return false;
    }

    class Material {
    public:
        float* ambient;
        float* diffuse;
        float* specular;
        GLuint texture;

        Material(float* ambient, float* diffuse, float* specular) {
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->texture = 0;
        }
    };

    class Face {
    public:
        int edge;
        int* vertices;
        int* texcoords;
        int normal;

        Face(int edge, int* vertices, int* texcoords, int normal = -1) {
            this->edge = edge;
            this->vertices = vertices;
            this->texcoords = texcoords;
            this->normal = normal;
        }
    };

    std::string prefix;
    std::vector<Material> materials;
    std::map<std::string, int> map_material;

    std::vector<float*> vertices;
    std::vector<float*> texcoords;
    std::vector<float*> normals;
    std::vector<Face> faces;

    GLuint list;

    void load_material(const char* filename) {
        std::string line;
        std::vector<std::string> lines;
        std::ifstream in(filename);
        if (!in.is_open()) {
            printf("Cannot load material %s\n", filename);
            return;
        }

        while (!in.eof()) {
            std::getline(in, line);
            lines.push_back(line);
        }
        in.close();

        Material* m;
        int count_material = 0;
        char str[40];
        std::string material;
        float* a, * d, * s;

        for (std::string& line : lines) {
            if (line[0] == 'n' && line[1] == 'e') {
                sscanf(line.c_str(), "newmtl %s", str);
                material = str;
                map_material[material] = count_material;
                count_material++;
                a = new float[4]{ 0.2f, 0.2f, 0.2f, 1.0f };
                d = new float[4]{ 0.8f, 0.8f, 0.8f, 1.0f };
                s = new float[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
                materials.push_back(Material(a, d, s));
                m = &materials[materials.size() - 1];
            }
            else if (line[0] == 'K') {
                switch (line[1]) {
                case 'a':
                    sscanf(line.c_str(), "Ka %f %f %f", &a[0], &a[1], &a[2]);
                    break;
                case 'd':
                    sscanf(line.c_str(), "Kd %f %f %f", &d[0], &d[1], &d[2]);
                    break;
                case 's':
                    sscanf(line.c_str(), "Ks %f %f %f", &s[0], &s[1], &s[2]);
                    break;
                }
            }
            else if (line[0] == 'm' && line[1] == 'a') {
                sscanf(line.c_str(), "map_Kd %s", str);
                std::string file = prefix + str;
                Image img;
                Load_Texture_Swap(&img, file.c_str());
                glGenTextures(1, &(m->texture));
                glBindTexture(GL_TEXTURE_2D, m->texture);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.w, img.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.img);
                glBindTexture(GL_TEXTURE_2D, 0);
                Delete_Image(&img);
            }
        }
    }

    void add_face_3v(std::string& line) {
        int v0, v1, v2;
        sscanf(line.c_str(), "f %d %d %d", &v0, &v1, &v2);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        faces.push_back(Face(3, v, NULL));
    }

    void add_face_3vt(std::string& line) {
        int v0, v1, v2, t0, t1, t2;
        sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &v0, &t0, &v1, &t1, &v2, &t2);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        int* t = new int[3]{ t0 - 1, t1 - 1, t2 - 1 };
        faces.push_back(Face(3, v, t));
    }

    void add_face_3vn(std::string& line) {
        int v0, v1, v2, n;
        sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v0, &n, &v1, &n, &v2, &n);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        faces.push_back(Face(3, v, NULL, n - 1));
    }

    void add_face_3vtn(std::string& line) {
        int v0, v1, v2, t0, t1, t2, n;
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        int* t = new int[3]{ t0 - 1, t1 - 1, t2 - 1 };
        faces.push_back(Face(3, v, t, n - 1));
    }

    void add_face_4v(std::string& line) {
        int v0, v1, v2, v3;
        sscanf(line.c_str(), "f %d %d %d %d", &v0, &v1, &v2, &v3);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        faces.push_back(Face(4, v, NULL));
    }

    void add_face_4vt(std::string& line) {
        int v0, v1, v2, v3, t0, t1, t2, t3;
        sscanf(line.c_str(), "f %d/%d %d/%d %d/%d %d/%d", &v0, &t0, &v1, &t1, &v2, &t2, &v3, &t3);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        int* t = new int[4]{ t0 - 1, t1 - 1, t2 - 1, t3 - 1 };
        faces.push_back(Face(4, v, t));
    }

    void add_face_4vn(std::string& line) {
        int v0, v1, v2, v3, n;
        sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d", &v0, &n, &v1, &n, &v2, &n, &v3, &n);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        faces.push_back(Face(4, v, NULL, n - 1));
    }

    void add_face_4vtn(std::string& line) {
        int v0, v1, v2, v3, t0, t1, t2, t3, n;
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n, &v3,
            &t3, &n);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        int* t = new int[4]{ t0 - 1, t1 - 1, t2 - 1, t3 - 1 };
        faces.push_back(Face(4, v, t, n - 1));
    }

public:
    float pos_x, pos_y, pos_z;

    void load(const char* filename) {
        std::string tmp = filename;
        prefix = "";
        int n = tmp.find_last_of('/') + 1;
        if (n > 0)
            prefix = tmp.substr(0, n);

        std::string line;
        std::vector<std::string> lines;
        std::ifstream in(filename);
        if (!in.is_open()) {
            printf("Cannot load model %s\n", filename);
            return;
        }

        while (!in.eof()) {
            std::getline(in, line);
            lines.push_back(line);
        }
        in.close();

        float a, b, c;
        char str[40];

        pos_x = pos_y = 0.0f;

        float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;

        for (std::string& line : lines) {
            if (line[0] == 'v') {
                if (line[1] == ' ') {
                    sscanf(line.c_str(), "v %f %f %f", &a, &b, &c);
                    if (a > 0.0f)
                        sum_x += a;
                    else
                        sum_x -= a;
                    if (b > 0.0f)
                        sum_y += b;
                    else
                        sum_y -= b;
                    if (c > 0.0f)
                        sum_z += c;
                    else
                        sum_z -= c;
                    pos_x += a;
                    pos_y += b;
                    vertices.push_back(new float[3]{ a, b, c });
                }
                else if (line[1] == 't') {
                    sscanf(line.c_str(), "vt %f %f", &a, &b);
                    texcoords.push_back(new float[2]{ a, b });
                }
                else {
                    sscanf(line.c_str(), "vn %f %f %f", &a, &b, &c);
                    normals.push_back(new float[3]{ a, b, c });
                }
            }
            else if (line[0] == 'f') {
                int edge = count_char(line, ' ');
                int count_slash = count_char(line, '/');
                if (count_slash == 0) {
                    if (edge == 3)
                        add_face_3v(line);
                    else
                        add_face_4v(line);
                }
                else if (count_slash == edge) {
                    if (edge == 3)
                        add_face_3vt(line);
                    else
                        add_face_4vt(line);
                }
                else if (count_slash == edge * 2) {
                    if (has_double_slash(line)) {
                        if (edge == 3)
                            add_face_3vn(line);
                        else
                            add_face_4vn(line);
                    }
                    else {
                        if (edge == 3)
                            add_face_3vtn(line);
                        else
                            add_face_4vtn(line);
                    }
                }
            }
            else if (line[0] == 'm' && line[1] == 't') {
                sscanf(line.c_str(), "mtllib %s", &str);
                std::string file = prefix + str;
                load_material(file.c_str());
            }
            else if (line[0] == 'u' && line[1] == 's') {
                sscanf(line.c_str(), "usemtl %s", &str);
                std::string material = str;
                if (map_material.find(material) != map_material.end())
                    faces.push_back(Face(-1, NULL, NULL, map_material[material]));
            }
        }

        bool has_texcoord = false;

        list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        for (Face& face : faces) {
            if (face.edge == -1) {
                has_texcoord = false;
                glLightfv(GL_LIGHT0, GL_AMBIENT, materials[face.normal].ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, materials[face.normal].diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, materials[face.normal].specular);
                if (materials[face.normal].texture != 0) {
                    has_texcoord = true;
                    glBindTexture(GL_TEXTURE_2D, materials[face.normal].texture);
                }
                continue;
            }
            if (face.normal != -1)
                glNormal3fv(normals[face.normal]);
            else
                glDisable(GL_LIGHTING);
            if (has_texcoord) {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++) {
                    glTexCoord2fv(texcoords[face.texcoords[i]]);
                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();
            }
            else {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++)
                    glVertex3fv(vertices[face.vertices[i]]);
                glEnd();
            }
            if (face.normal == -1)
                glEnable(GL_LIGHTING);
        }
        glEndList();

        printf("Model: %s\n", filename);
        printf("Vertices: %d\n", vertices.size());
        printf("Texcoords: %d\n", texcoords.size());
        printf("Normals: %d\n", normals.size());
        printf("Faces: %d\n", faces.size());
        printf("Materials: %d\n", materials.size());

        sum_x /= vertices.size();
        sum_y /= vertices.size();
        sum_z /= vertices.size();
        pos_x /= vertices.size();
        pos_x = -pos_x;
        pos_y /= vertices.size();
        pos_y = -pos_y;
        pos_z = -sqrt(sum_x * sum_x + sum_y * sum_y + sum_z * sum_z) * 15;

        printf("Pos_X: %f\n", pos_x);
        printf("Pos_Y: %f\n", pos_y);
        printf("Pos_Z: %f\n", pos_z);

        for (Material& material : materials) {
            delete material.ambient;
            delete material.diffuse;
            delete material.specular;
        }

        materials.clear();
        map_material.clear();

        for (float* f : vertices)
            delete f;
        vertices.clear();
        for (float* f : texcoords)
            delete f;
        texcoords.clear();
        for (float* f : normals)
            delete f;
        normals.clear();
        faces.clear();
    }

    void draw() { glCallList(list); }
};

#endif