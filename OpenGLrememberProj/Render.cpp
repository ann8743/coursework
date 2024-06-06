//B
#include "Render.h"
#include <chrono>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "MyOGL.h"
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include "GUItextRectangle.h"
#include <math.h>
#include "ObjLoader.h"
#define PI 3.14159265358979323846 
using std::vector;
double Search_delta_time();
void Texture();

class Point {
public:
	double x;
	double y;
	double z;
	void DrawPoint() {
		glVertex3d(this->x, this->y, this->z);
	}
	void Translated() {
		glTranslated(this->x, this->y, this->z);
	}
	Point(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

static Point SearchVector(Point A, Point B) {
	return Point(B.x - A.x, B.y - A.y, B.z - A.z);
}
double sharpness_t = 0.1;

//����� ��� ������ �����
extern vector<Point> PointHB;
//������� ����� ����� ��� ������ �����
vector<Point> PointHB = {
	// ������ ������ ����� 3-�� �������
	Point(2.5, 0, 4), Point(4, 0, 4), Point(6, 0, 4), Point(14, 0, 3)
};


bool textureMode = true;
bool lightMode = true;
//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����



//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
		delete POINT;
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


	if (!OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON)) {
		LPPOINT POINT = new tagPOINT();
		//��������� ��������� ������� ���� � ����������� ������.
		GetCursorPos(POINT);
		//������� ScreenToClient ������������ �������� ���������� ��������� ����� �� ������ � ���������� ������� �������
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		// ��������� viewport-a
		GLint viewport[4];
		// ������� ��������
		GLdouble projection[16];
		// ������� �������
		GLdouble modelview[16];

		// ����� ��������� viewport-a
		glGetIntegerv(GL_VIEWPORT, viewport);
		//����� ������� ��������
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		// ����� ������� �������
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		double delta = 10;
		for (auto& elem : PointHB) {
			double tempPoint[3];
			// �������� �������� ���������� �������
			gluProject(elem.x, elem.y, elem.z, modelview, projection, viewport, &tempPoint[0], &tempPoint[1], &tempPoint[2]);
			if (tempPoint[0] > POINT->x - delta && tempPoint[0] < POINT->x + delta &&
				tempPoint[1] > POINT->y - delta && tempPoint[1] < POINT->y + delta)
			{
				tempPoint[0] -= dx;
				tempPoint[1] += dy;
				// ��������� �������� ���������� ������� � ���������
				gluUnProject(tempPoint[0], tempPoint[1], tempPoint[2], modelview, projection, viewport, &elem.x, &elem.y, &elem.z);
			}
		}
		delete POINT;
	}
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}


int anim = 0;
void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == 'V') {
		anim += 1;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

GLuint skin, mish, raduga;


void Texture() {
	static int k = 0;
	if (k % 2 == 0) {
		RGBTRIPLE* texarray2;
		char* texCharArray2;
		int texW2, texH2;
		OpenGL::LoadBMP("textures\\zel.bmp", &texW2, &texH2, &texarray2);
		OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);



		//���������� �� ��� ��������
		glGenTextures(1, &mish);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, mish);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);

		//�������� ������
		free(texCharArray2);
		free(texarray2);
		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		k++;
	}
	else {
		RGBTRIPLE* texarray2;
		char* texCharArray2;
		int texW2, texH2;
		OpenGL::LoadBMP("textures\\kras.bmp", &texW2, &texH2, &texarray2);
		OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);



		//���������� �� ��� ��������
		glGenTextures(1, &mish);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, mish);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);

		//�������� ������
		free(texCharArray2);
		free(texarray2);
		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		k++;
	}
}




ObjFile lyk, strela, banki;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray, *texarray2, *texarray3;

	//1
	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("textures\\tree2.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//���������� �� ��� ��������
	glGenTextures(1, &skin);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, skin);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//2
	char* texCharArray2;
	int texW2, texH2;
	OpenGL::LoadBMP("textures\\zel.bmp", &texW2, &texH2, &texarray2);
	OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);

	//���������� �� ��� ��������
	glGenTextures(1, &mish);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, mish);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);
	//�������� ������
	free(texCharArray2);
	free(texarray2);
	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	//3 ���� ������
	char* texCharArray3;
	int texW3, texH3;
	OpenGL::LoadBMP("textures\\radug.bmp", &texW3, &texH3, &texarray3);
	OpenGL::RGBtoChar(texarray3, texW3, texH3, &texCharArray3);

	//���������� �� ��� ��������
	glGenTextures(1, &raduga);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, raduga);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW3, texH3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray3);
	//�������� ������
	free(texCharArray3);
	free(texarray3);
	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	loadModel("models\\arrow.obj", &strela);
	loadModel("models\\bow.obj", &lyk);
	loadModel("models\\cans.obj", &banki);

	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


bool isPointOnCircle(double pointX, double pointY, double pointZ, double centerX, double centerY, double centerZ, double radius) {
	double distanceSquared = pow(pointX - centerX, 2) + pow(pointY - centerY, 2) + pow(pointZ - centerZ, 2);
	return distanceSquared <= pow(radius, 2);
}

Point BezierCurve3(Point P0, Point P1, Point P2, Point P3, double t) {
	double t_2 = t * t;
	double t_3 = t_2 * t;
	double var = 1 - t;
	double var_2 = var * var;
	double var_3 = var_2 * var;

	double x = var_3 * P0.x + 3 * t * var_2 * P1.x + 3 * t_2 * var * P2.x + t_3 * P3.x;
	double y = var_3 * P0.y + 3 * t * var_2 * P1.y + 3 * t_2 * var * P2.y + t_3 * P3.y;
	double z = var_3 * P0.z + 3 * t * var_2 * P1.z + 3 * t_2 * var * P2.z + t_3 * P3.z;

	return Point(x, y, z);
}

vector<Point> PointBezierCurve3(Point P0, Point P1, Point P2, Point P3, double t_max = 1) {
	vector<Point> points;
	t_max = 1 + sharpness_t * 0.001;

	for (double i = 0; i <= t_max; i += sharpness_t) {
		points.push_back(BezierCurve3(P0, P1, P2, P3, i));
	}
	return points;
}


// ������ ������ ����� �������� �������
void DrawBezierCurve3(Point A, Point B, Point C, Point D) {
	glPointSize(10);

	glColor3d(1, 0, 1);
	glBegin(GL_POINTS);
	A.DrawPoint();
	B.DrawPoint();
	C.DrawPoint();
	D.DrawPoint();
	glEnd();

	glPointSize(1);

	glColor3d(0.7, 0.2, 0.6);
	glBegin(GL_LINE_STRIP);
	A.DrawPoint();
	B.DrawPoint();
	C.DrawPoint();
	D.DrawPoint();
	glEnd();

	glLineWidth(3);

	glColor3d(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	vector<Point> points = PointBezierCurve3(A, B, C, D);
	for (int i = 0; i < (int)points.size(); i++) {
		points[i].DrawPoint();
	}
	glEnd();

	glLineWidth(1);
}



void AnimationObjectCurve(vector<Point> points, vector<double> t) {
	vector<Point> PointsCurve;
	Point point(0, 0, 0);
	Point next_point(0, 0, 0);

	float s1 = 0.4, s2 = 0.5;

	double t_max = t[0], t_max_next = t[1];

	double centerX = 20;
	double centerY = 0;
	double centerZ = 4;
	double radius = 10;
	Point item = BezierCurve3(points[0], points[1], points[2], points[3], t_max);
	if (anim % 2 == 1) {
		DrawBezierCurve3(points[0], points[1], points[2], points[3]);
		if (isPointOnCircle(item.x, item.y, item.z, centerX, centerY, centerZ, radius)) {
			Texture();
		}
		glPushMatrix();

		point = BezierCurve3(points[0], points[1], points[2], points[3], t_max);
		next_point = BezierCurve3(points[0], points[1], points[2], points[3], t_max_next);
		point.Translated();
		glBindTexture(GL_TEXTURE_2D, raduga);
		glScalef(s1, s1, s1);
		glScalef(10.0f, 10.0f, 10.0f);
		glRotatef(90, 0.0f, 0.0f, -1.0f);
		glRotatef(180, 0.0f, 0.0f, 1.0f);
		strela.DrawObj();
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, mish);
	glTranslatef(30.0f, -3.0f, -20.0f);
	glScalef(75.0f, 75.0f, 75.0f);
	glRotatef(90, 0.0f, -1.0f, 0.0f);
	banki.DrawObj();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();;

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, skin);
	glTranslatef(1.0f, 0.0f, 0.0f);
	glScalef(3.0f, 3.0f, 3.0f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glRotatef(90, 0.0f, 0.0f, 1.0f);
	lyk.DrawObj();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}


//�������� ���������� ������� ����� ����������
double Search_delta_time() {
	static auto end_render = std::chrono::steady_clock::now();
	auto cur_time = std::chrono::steady_clock::now();
	auto deltatime = cur_time - end_render;
	double delta = 1.0 * std::chrono::duration_cast<std::chrono::microseconds>(deltatime).count() / 1000000;
	end_render = cur_time;
	return delta;
}


void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);



	//////////////////////////////////////
 
	static double t_max = 0;
	static double t_max_next = 0;
	static bool flag_tmax = true;

	//��������� �������
	double delta_time1 = Search_delta_time();
	double go = delta_time1 / 3; 
	t_max += go;
	t_max_next = t_max + go;
	if (t_max > 1) {
		t_max = 0;
		flag_tmax = false;
	}
		

	vector<double> t = { t_max, t_max_next };
	double s2 = 0.5;

	// ������ ������ �����
	AnimationObjectCurve(vector<Point> {PointHB[0], PointHB[1], PointHB[2], PointHB[3]}, t);




	// ��������� ������ ������


	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	//ss << "� - ����������� ��������" << std::endl;
	ss << "V - �������� ��������" << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}