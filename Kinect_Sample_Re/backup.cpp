#include "backup.h"

int POS_X, POS_Y;

string model_name1 = "Library/cyborg.obj";
Model model1, model2;
string model_name2 = "Library/cyborg.obj";
string model_name3 = "Library/Lowpoly_Fox.obj";
string model_name4 = "Library/Lowpoly_Fox.obj";
string model_name5 = "Library/scrubPine.obj";
string model_name6 = "Library/scrubPine.obj";
string model_name7 = "Library/BMW_M3_GTR.obj";
string model_name8 = "Library/BMW_M3_GTR.obj";
string model_name9 = "Library/estrellica.obj";
string model_name10 = "Library/estrellica.obj";

GLfloat light_pos[] = { -10.0f, 10.0f, 100.00f, 1.0f };

float pos_x, pos_y, pos_z;
float angle_x = 30.0f, angle_y = 0.0f;

int x_old = 0, y_old = 0;
int current_scroll = 5;
float zoom_per_scroll;

bool is_holding_mouse = false;
bool is_updated = false;

Model model3, model4;
Model model5, model6;
Model model7, model8;
Model model9, model10;

void draw_center(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); /* R */
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); /* G */
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); /* B */
	glVertex3f(0.0f, 0.0f, -0.2f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.0f, -0.2f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'z');
}

void idle() {
	static GLuint previousClock = glutGet(GLUT_ELAPSED_TIME);
	static GLuint currentClock = glutGet(GLUT_ELAPSED_TIME);
	static GLfloat deltaT;

	currentClock = glutGet(GLUT_ELAPSED_TIME);
	deltaT = currentClock - previousClock;
	if (deltaT < 1000.0 / 20.0) { return; }
	else { previousClock = currentClock; }

	//char buff[256];
	//sprintf_s(buff, "Frame Rate = %f", 1000.0 / deltaT);
	//frameRate = buff;

	glutPostRedisplay();
}

void close()
{
	glDeleteTextures(1, &dispBindIndex);
	glutLeaveMainLoop();
	CloseHandle(hMutex);
}

void add_quats(float q1[4], float q2[4], float dest[4])
{
	static int count = 0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy(q1, t1);
	vscale(t1, q2[3]);

	vcopy(q2, t2);
	vscale(t2, q1[3]);

	vcross(q2, q1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = q1[3] * q2[3] - vdot(q1, q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat(dest);
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(58, (double)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	GLfloat spin_quat[4];
	float gain;
	gain = 2.0; /* trackball gain */

	if (drag_state == GLUT_DOWN)
	{
		if (button_state == GLUT_LEFT_BUTTON)
		{
			trackball(spin_quat,
				(gain * rot_x - 500) / 500,
				(500 - gain * rot_y) / 500,
				(gain * x - 500) / 500,
				(500 - gain * y) / 500);
			add_quats(spin_quat, quat, quat);
		}
		else if (button_state == GLUT_RIGHT_BUTTON)
		{
			t[0] -= (((float)trans_x - x) / 500);
			t[1] += (((float)trans_y - y) / 500);
		}
		else if (button_state == GLUT_MIDDLE_BUTTON)
			t[2] -= (((float)trans_z - y) / 500 * 4);
		else if (button_state == 3 || button_state == 4) // scroll
		{

		}
		//glutPostRedisplay();
	}

	rot_x = x;
	rot_y = y;

	trans_x = x;
	trans_y = y;
	trans_z = y;
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			rot_x = x;
			rot_y = y;

			//t[0] = t[0] + 1;


		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			trans_x = x;
			trans_y = y;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			//trcon = trcon + 1;
			trans_z = y;
		}
		else if (button == 3 || button == 4)
		{
			const float sign = (static_cast<float>(button) - 3.5f) * 2.0f;
			t[2] -= sign * 500 * 0.00015f;
		}
	}

	drag_state = state;
	button_state = button;
}

void vzero(float* v)
{
	v[0] = 0.0f;
	v[1] = 0.0f;
	v[2] = 0.0f;
}

void vset(float* v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void vsub(const float* src1, const float* src2, float* dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}

void vcopy(const float* v1, float* v2)
{
	register int i;
	for (i = 0; i < 3; i++)
		v2[i] = v1[i];
}

void vcross(const float* v1, const float* v2, float* cross)
{
	float temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy(temp, cross);
}

float vlength(const float* v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void vscale(float* v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}

void vnormal(float* v)
{
	vscale(v, 1.0f / vlength(v));
}

float vdot(const float* v1, const float* v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vadd(const float* src1, const float* src2, float* dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}

void trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
	float a[3]; /* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	float p1[3], p2[3], d[3];
	float t;

	if (p1x == p2x && p1y == p2y) {
		/* Zero rotation */
		vzero(q);
		q[3] = 1.0;
		return;
	}

	/*
	 * First, figure out z-coordinates for projection of P1 and P2 to
	 * deformed sphere
	 */
	vset(p1, p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y));
	vset(p2, p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y));

	/*
	 *  Now, we want the cross product of P1 and P2
	 */
	vcross(p2, p1, a);

	/*
	 *  Figure out how much to rotate around that axis.
	 */
	vsub(p1, p2, d);
	t = vlength(d) / (2.0f * TRACKBALLSIZE);

	/*
	 * Avoid problems with out-of-control values...
	 */
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	phi = 2.0f * asin(t);

	axis_to_quat(a, phi, q);
}

void axis_to_quat(float a[3], float phi, float q[4])
{
	vnormal(a);
	vcopy(a, q);
	vscale(q, sin(phi / 2.0f));
	q[3] = cos(phi / 2.0f);
}

float tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x * x + y * y);
	if (d < r * 0.70710678118654752440f) {    /* Inside sphere */
		z = sqrt(r * r - d * d);
	}
	else {           /* On hyperbola */
		t = r / 1.41421356237309504880f;
		z = t * t / d;
	}
	return z;
}

void normalize_quat(float q[4])
{
	int i;
	float mag;

	mag = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	for (i = 0; i < 4; i++) q[i] /= mag;
}

void build_rotmatrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0f;

	m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0f;

	m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void InitializeWindow(int argc, char* argv[])
{
	// initialize glut settings
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1000 / 2, 1000 / 2);

	glutInitWindowPosition(0, 0);

	dispWindowIndex = glutCreateWindow("3D Model");

	trackball(quat, 90.0, 0.0, 0.0, 0.0);

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutCloseFunc(close);
	//GLuint image = load   ("./my_texture.bmp");

	//glBindTexture(1,)

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// bind textures
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	reshape(1000, 1000);

	/*glGenTextures(1, &dispBindIndex);
	glBindTexture(GL_TEXTURE_2D, dispBindIndex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}


void init() {
	model1.load(model_name1.c_str());
	model2.load(model_name2.c_str());
	model3.load(model_name3.c_str());
	model4.load(model_name4.c_str());
	model5.load(model_name5.c_str());
	model6.load(model_name6.c_str());
	model7.load(model_name7.c_str());
	model8.load(model_name8.c_str());
	model9.load(model_name9.c_str());
	model10.load(model_name10.c_str());
}

float loc1 = 0.0;
float loc2 = -0.2;
float cnt = 0.0001;
float cnt1 = 0.0001;
float cnt2 = -0.0001;
bool direction;
float gap1 = 0.16;
float cnt_fox = 0.1;
int tree = 0;
float gap2 = 0.0;
float cnt_car = 0.1;

vector<float> a, bb, c;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 0.1, 200);
	glTranslatef(t[0], t[1], t[2] - 1.0f);
	glScalef(1, 1, 1);
	GLfloat m[4][4], m1[4][4];
	build_rotmatrix(m, quat);
	gluLookAt(0, 0.2, 2.0, 0, 0, 0, 0, 1.0, 0);

	GLfloat r, g, b;
	glMultMatrixf(&m[0][0]);

	// 문 하나
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, door);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	for (float fl = -1.0; fl < 1.0; fl = fl + 0.2)
	{
		for (float fr = -1.0; fr < 1.0; fr = fr + 0.2)
		{
			if (loc1 > 0.4)
				direction = false;
			else if (loc1 < 0.0)
				direction = true;

			if (direction) {
				cnt += 0.0001;
				loc1 = cnt;
				loc2 = -0.2 - (sqrt(pow(0.4, 2) - pow((0.4 - loc1),2)));
			}

			else if (!direction) {
				cnt -= 0.0001;
				loc1 = cnt;
				loc2 = -0.2 - (sqrt(pow(0.4, 2) - pow((0.4 - loc1), 2)));
			}

			glTexCoord2d(0.0, 0.0);
			glVertex3f(-0.2, loc1, loc2); // loc1은 0.0 <-> 0.4, loc2는 -0.2 <-> -0.6
			glTexCoord2d(1.0, 0.0);
			glVertex3f(-0.2, 0.4, -0.2);
			glTexCoord2d(1.0, 1.0);
			glVertex3f(0.2, 0.4, -0.2);
			glTexCoord2d(0.0, 1.0);
			glVertex3f(0.2, loc1, loc2);
		}

	}
	glEnd();

	//floor
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 276, 276, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for (float fl = -1.0; fl < 1.0; fl = fl + 0.2)
	{
		for (float fr = -1.0; fr < 1.0; fr = fr + 0.2)
		{
			glTexCoord2d(0.0, 0.0);
			glVertex3f(-0.2 + fl, 0.0, -0.2 + fr);
			glTexCoord2d(1.0, 0.0);
			glVertex3f(-0.2 + fl, 0.0, 0.2 + fr);
			glTexCoord2d(1.0, 1.0);
			glVertex3f(0.2 + fl, 0.0, 0.2 + fr);
			glTexCoord2d(0.0, 1.0);
			glVertex3f(0.2 + fl, 0.0, -0.2 + fr);
		}

	}
	glEnd();


	//wall
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, wall);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for (float fl = -1.0; fl < 1.0; fl = fl + 0.2)
	{
		for (float fr = -1.0; fr < 1.0; fr = fr + 0.2)
		{
			// 가운데꺼
			glTexCoord2d(0.0, 0.0);
			glVertex3f(-0.2, 0.0, 0.2);
			glTexCoord2d(1.0, 0.0);
			glVertex3f(-0.2, 0.4, 0.2);
			glTexCoord2d(1.0, 1.0);
			glVertex3f(0.2, 0.4, 0.2);
			glTexCoord2d(0.0, 1.0);
			glVertex3f(0.2, 0.0, 0.2);

			// 오른쪽꺼
			glTexCoord2d(1.0, 1.0);
			glVertex3f(-0.2, 0.4, -0.2);
			glTexCoord2d(1.0, 0.0);
			glVertex3f(-0.2, 0.4, 0.2);
			glTexCoord2d(0.0, 0.0);
			glVertex3f(-0.2, 0.0, 0.2);
			glTexCoord2d(0.0, 1.0);
			glVertex3f(-0.2, 0.0, -0.2);

			// 왼쪽꺼
			glTexCoord2d(0.0, 1.0);
			glVertex3f(0.2, 0.0, -0.2);
			glTexCoord2d(0.0, 0.0);
			glVertex3f(0.2, 0.0, 0.2);
			glTexCoord2d(1.0, 0.0);
			glVertex3f(0.2, 0.4, 0.2);
			glTexCoord2d(1.0, 1.0);
			glVertex3f(0.2, 0.4, -0.2);

		}

	}
	glEnd();


	// 바닥 하나
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, floor1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for (float fl = -1.0; fl < 1.0; fl = fl + 0.2)
	{
		for (float fr = -1.0; fr < 1.0; fr = fr + 0.2)
		{
			glTexCoord2d(0.0, 0.0);
			glVertex3f(0.2, 0.001, -0.2);
			glTexCoord2d(1.0, 0.0);
			glVertex3f(-0.2, 0.001, -0.2);
			glTexCoord2d(1.0, 1.0);
			glVertex3f(-0.2, 0.001, 0.2);
			glTexCoord2d(0.0, 1.0);
			glVertex3f(0.2, 0.001, 0.2);

		}

	}
	glEnd();


	// 테스트
	glTranslatef(-0.15, 0, 0);
	glRotatef(90, 0, 1, 0);
	glScalef(0.1, 0.1, 0.1);
	model1.draw();
	model2.draw();
	glScalef(10, 10, 10);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0.15, 0, 0);

	if (gap1 > 0.1)
		cnt_fox = 100*cnt2;
	else if (gap1 < -0.1)
		cnt_fox = 100*cnt1;
	gap1 += cnt_fox;

	glTranslatef(0.18, 0, 0);
	glRotatef(180, 0, 1, 0);
	glTranslatef(0, 0, gap1);
	glScalef(0.001, 0.001, 0.001);
	model3.draw();
	model4.draw();
	glScalef(1000, 1000, 1000);
	glTranslatef(0, 0, -gap1);
	glRotatef(-180, 0, 1, 0);
	glTranslatef(-0.18, 0, 0);

	tree += 10000 * cnt1;
	glTranslatef(-0.15, 0, -0.15);
	glRotatef(tree, 0, 1, 0);
	glScalef(0.0001, 0.0001, 0.0001);
	model5.draw();
	model6.draw();
	glScalef(10000, 10000, 10000);
	glRotatef(-tree, 0, 1, 0);
	glTranslatef(0.15, 0, 0.15);

	if (gap2 > 0.4 && !direction)
		cnt_car = 200 * cnt2;
	else if (gap2 <= 0.0 && direction)
		cnt_car = 200 * cnt1;
	gap2 += cnt_car;
	glRotatef(90, 0, 1, 0);
	glTranslatef(gap2, 0, 0);
	glScalef(0.000025, 0.000025, 0.000025);
	model7.draw();
	model8.draw();
	glScalef(40000, 40000, 40000);
	glTranslatef(-gap2, 0, 0);
	glRotatef(-90, 0, 1, 0);

	glRotatef(-tree * 3, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -0.3);
	glScalef(0.008, 0.008, 0.008);
	model9.draw();
	model10.draw();
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	vertexx = new Vertex[100000];
	vertexx_color = new Vertex[100000];
	// mymesh = new MMesh[100000];

	int i, j, k = 0;
	FILE* f = fopen("carpet.bmp", "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];
	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
		{
			mytexels[j][i][0] = data[k * 3 + 2];
			mytexels[j][i][1] = data[k * 3 + 1];
			mytexels[j][i][2] = data[k * 3];
			k++;
		}

	k = 0;
	FILE* f2 = fopen("wall.bmp", "rb");
	unsigned char info2[54];
	fread(info2, sizeof(unsigned char), 54, f2); // read the 54-byte header
	int width2 = *(int*)&info2[18];
	int height2 = *(int*)&info2[22];
	int size2 = 3 * width2 * height2;
	unsigned char* data2 = new unsigned char[size2]; // allocate 3 bytes per pixel
	fread(data2, sizeof(unsigned char), size2, f2); // read the rest of the data at once
	fclose(f2);
	for (i = 0; i < width2; i++)
		for (j = 0; j < height2; j++)
		{
			wall[j][i][0] = data2[k * 3 + 2];
			wall[j][i][1] = data2[k * 3 + 1];
			wall[j][i][2] = data2[k * 3];
			k++;
		}

	// 만든 것 
	k = 0;
	FILE* f3 = fopen("floor.bmp", "rb");
	unsigned char info3[54];
	fread(info3, sizeof(unsigned char), 54, f3); // read the 54-byte header
	int width3 = *(int*)&info3[18];
	int height3 = *(int*)&info3[22];
	int size3 = 3 * width3 * height3;
	unsigned char* data3 = new unsigned char[size3]; // allocate 3 bytes per pixel
	fread(data3, sizeof(unsigned char), size3, f3); // read the rest of the data at once
	fclose(f3);
	for (i = 0; i < width3; i++)
		for (j = 0; j < height3; j++)
		{
			floor1[j][i][0] = data3[k * 3 + 2];
			floor1[j][i][1] = data3[k * 3 + 1];
			floor1[j][i][2] = data3[k * 3];
			k++;
		}
	////

	// 만든 것 
	k = 0;
	FILE* f4 = fopen("door.bmp", "rb");
	unsigned char info4[54];
	fread(info4, sizeof(unsigned char), 54, f4); // read the 54-byte header
	int width4 = *(int*)&info4[18];
	int height4 = *(int*)&info4[22];
	int size4 = 3 * width4 * height4;
	unsigned char* data4 = new unsigned char[size4]; // allocate 3 bytes per pixel
	fread(data4, sizeof(unsigned char), size4, f4); // read the rest of the data at once
	fclose(f4);
	for (i = 0; i < width4; i++)
		for (j = 0; j < height4; j++)
		{
			door[j][i][0] = data4[k * 3 + 2];
			door[j][i][1] = data4[k * 3 + 1];
			door[j][i][2] = data4[k * 3];
			k++;
		}
	////

	InitializeWindow(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
	POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
	init();
	display();

	glutMainLoop();
	delete[] vertexx;
	delete[] vertexx_color;
	return 0;
}