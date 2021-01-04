#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h> // (or others, depending on the system in use)
#include <iostream>
#include "linmath.h"
#include "load_mtl.hpp"
#include "load_obj.hpp"
#include "load_bmp.hpp"

bool fullscreen = false;
bool mouseDown = false;

/* view matrix parameters */
float pos[3] = {0, .8, 0};
float look[3] = {0, 0, -1};
float center[3] = {0, .8, -1};
float up[3] = {0, 1, 0};

/* camera position parameters */
float cam_pos[3] = {0, .8, 0};
float cam_look[3] = {0, 0, -1};
float cam_center[3] = {0, .8, -1};
float cam_up[3] = {0, 1, 0};

int camera_mode = 0; /* 0 for main, 1 for TV */

/* tv texture params */
const int cam_width = 1280;
const int cam_height = 720;
GLuint cam_tex;

/* TV buffer */
GLubyte TVbuffer[cam_width][cam_height][4];

/* latest position */
int cur_x;
int cur_y;

/* movement factors */
float scale_val = 1;
float move_factor = .04;

int win_width = 500;
int win_ht = 500;


/* light parameters */
GLfloat pos_l1[] = {-1.923018, 1.959999, -0.310372, 1.f};
GLfloat pos_l2[] = {-1.915507, 1.959999, -3.307540, 1.f};
GLfloat pos_l3[] = {3.227671, 1.919999, -4.110317, 1.f};
GLfloat pos_l4[] = {0.511854, -0.040000, -3.959631, 1.f};

float const_atten = 1.;
float lin_atten = 0.0;
float quad_atten = 0.1;

/* store all texture handels */
std::vector<GLuint> textures;

void update_center()
{
	center[0] = pos[0] + look[0];
	center[1] = pos[1] + look[1];
	center[2] = pos[2] + look[2];

	cam_center[0] = cam_pos[0] + cam_look[0];
	cam_center[1] = cam_pos[1] + cam_look[1];
	cam_center[2] = cam_pos[2] + cam_look[2];
}

bool init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glShadeModel(GL_SMOOTH);

	/*
	 * create the textures
	 */
	for (int i=0; i<object_list.size(); i++) {
		int internal_format;	
		if (images[i].bit_layout == GL_BGR_EXT) {
			internal_format = GL_RGB;	
		} else if (images[i].bit_layout == GL_BGRA_EXT) {
			internal_format = GL_RGBA;
		} else {
			printf("unknown internal format\n");
		}

		textures.push_back(0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, images[i].width, images[i].height,
				0, images[i].bit_layout, GL_UNSIGNED_BYTE, images[i].data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}


	/*
	 * setup the lights.
	 */
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuse[] = { 1.0, .77, .56, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);


	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, const_atten);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, lin_atten);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, quad_atten);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, const_atten);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, lin_atten);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, quad_atten);

	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, const_atten);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, lin_atten);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, quad_atten);

	glLightfv(GL_LIGHT4, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, specular);
	glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, lin_atten);
	glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, quad_atten);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);

	glEnable(GL_DEPTH_TEST);

	/* set initial cursor position */
	cur_x = win_width / 2;
	cur_y = win_ht/2;
	glutWarpPointer(win_width/2, win_ht/2);

	return true;
}


void render() 
{
	/*
	 * position the lights.
	 */
	glLightfv(GL_LIGHT1, GL_POSITION, pos_l1);
	glLightfv(GL_LIGHT2, GL_POSITION, pos_l2);
	glLightfv(GL_LIGHT3, GL_POSITION, pos_l3);
	glLightfv(GL_LIGHT4, GL_POSITION, pos_l4);

	for (int i=0; i<object_list.size(); i++) {
		if (object_list[i].object != "MIRROR_OBJ_MIRROR_PLANE") {
			glMaterialfv(GL_FRONT, GL_AMBIENT, mtl_map[object_list[i].material]->Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mtl_map[object_list[i].material]->Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, mtl_map[object_list[i].material]->Ks);
			glMaterialf(GL_FRONT, GL_SHININESS, mtl_map[object_list[i].material]->Ns);
//			glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, textures[i]);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, &(object_list[i].vertices[0]));

			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &(object_list[i].normals[0]));

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, &(object_list[i].uvs[0]));

			glDrawArrays(GL_TRIANGLES, 0, object_list[i].vertices.size()/3);
		}
	}
}


void equation_plane(float x1, float y1,  
                    float z1, float x2, 
                    float y2, float z2,  
                    float x3, float y3, float z3,
		    double *a, double *b, double *c, double *d) 
{ 
/*
 * SOURCE:  https://www.geeksforgeeks.org/program-to-find-equation-of-a-plane-passing-through-3-points/
 */
    float a1 = x2 - x1; 
    float b1 = y2 - y1; 
    float c1 = z2 - z1; 
    float a2 = x3 - x1; 
    float b2 = y3 - y1; 
    float c2 = z3 - z1; 
    *a = b1 * c2 - b2 * c1; 
    *b = a2 * c1 - a1 * c2; 
    *c = a1 * b2 - b1 * a2; 
    *d = (- *a * x1 - *b * y1 - *c * z1); 
    return; 
} 

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* 
	 * render "camera"
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)cam_width / (GLfloat)cam_height, .1, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2],
		cam_center[0], cam_center[1], cam_center[2],
		cam_up[0], cam_up[1], cam_up[2]
		);

	render();

	glReadPixels(0, 0, cam_width, cam_height, GL_RGBA, GL_UNSIGNED_BYTE, TVbuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glGenTextures(1, &cam_tex);
	glBindTexture(GL_TEXTURE_2D, cam_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cam_width, cam_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TVbuffer);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(2.926557, 0.400000, -3.787318);
	glTexCoord2f(0, 1); glVertex3f(2.926557, 1.20000, -3.787318);
	glTexCoord2f(1, 1); glVertex3f(3.484314, 1.200000, -2.666053);
	glTexCoord2f(1, 0); glVertex3f(3.484314, 0.400000, -2.666053);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	render();
	glReadPixels(0, 0, cam_width, cam_height, GL_RGBA, GL_UNSIGNED_BYTE, TVbuffer);



	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, win_width, win_ht);

	gluPerspective(45.0f, 1.0f * win_width / win_ht, .1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
		

	/* 
	 * render mirror effect
	 */
	double plane_eqn[4];
	int i = obj_map["MIRROR_OBJ_MIRROR_PLANE"];

	/* create orthonormal mirror basis using Gram-Schmidt process */
	vec3 v1, v2, v3;
	vec3 u1, u2, u3;
	vec3 e1, e2, e3;
	vec3 p1, p2, p3;

	vec3_set_arr(p1, &(object_list[i].vertices[0]));
	vec3_set_arr(p2, &(object_list[i].vertices[12]));
	vec3_set_arr(p3, &(object_list[i].vertices[6]));
	
	vec3_sub(v1, p2, p1);
	vec3_sub(v2, p3, p1);

	/* create e1 */
	vec3_set(u1, v1[0], v1[1], v1[2]);
	vec3_norm(e1, u1);

	/* create e2 */
	float proj_num = vec3_mul_inner(v2, u1);
	float proj_den = vec3_mul_inner(u1, u1);
	vec3 proj_u1_v2;
	vec3_scale(proj_u1_v2, u1, proj_num / proj_den);
	vec3_sub(u2, v2, proj_u1_v2);
	vec3_norm(e2, u2);

	/* create e3 */
	vec3_mul_cross(e3, e1, e2);

	/*
	 * create the basis matrix
	 */
	float R[4][4] = {
		e1[0], e2[0], e3[0], 0,
		e1[1], e2[1], e3[1], 0,
		e1[2], e2[2], e3[2], 0,
		0, 0, 0, 1
	};
/*
	float R[4][4] = {
		e1[0], e1[1], e1[2], 0,
		e2[0], e2[1], e2[2], 0,
		e3[0], e3[1], e3[2], 0,
		0, 0, 0, 1
	};
	*/


	/*
	 * create translation matrix
	 */
	mat4x4 T;
	mat4x4_translate(T, -p1[0], -p1[1], -p1[2]);

	/*
	 * combine the matricies
	 */
	mat4x4 RT;
	mat4x4_mul(RT, R, T);

	/*
	 * do the reflection
	 */
	mat4x4 RF;
	mat4x4_identity(RF);
	RF[2][2] = -1;

	mat4x4 Rinv;
	mat4x4_invert(Rinv, R);

	mat4x4 Tinv;
	mat4x4_invert(Tinv, T);

	mat4x4 first;
	mat4x4_mul(first, RF, RT);

	mat4x4 second;
	mat4x4_mul(second, Rinv, first);

	mat4x4 RT_RF_RTinv;
	mat4x4_mul(RT_RF_RTinv, Tinv, second);

	vec4 pos4; 
	vec4 cen4; 
	vec4_set(pos4, pos[0], pos[1], pos[2], 1);
	vec4_set(cen4, center[0], center[1], center[2], 1);
	
	vec4 temp;
	mat4x4_mul_vec4(temp, RT_RF_RTinv, pos4);
	pos4[0] = temp[0];
	pos4[1] = temp[1];
	pos4[2] = temp[2];

	mat4x4_mul_vec4(temp, RT_RF_RTinv, cen4);
	cen4[0] = temp[0];
	cen4[1] = temp[1];
	cen4[2] = temp[2];

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//printf("pos4: %f %f %f\n", pos4[0], pos4[1], pos4[2]);
	//printf("p1: %f %f %f\n", p1[0], p1[1], p1[2]);
	gluLookAt(
		pos4[0], pos4[1], pos4[2],
		cen4[0], cen4[1], cen4[2],
		up[0], up[1], up[2]);
/*
	gluLookAt(
		pos[0], pos[1], pos[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);
		*/

//	glMultMatrixf((float *) &RT_RF_RTinv[0]);

	glPointSize(30);
	glBegin(GL_POINTS);
		glColor3f(1, 0, 0);
		glVertex3f(p1[0], p1[1], p1[2]);
	glEnd();


	glPointSize(20);
	glBegin(GL_POINTS);
		glColor3f(0, 1, 0);
		glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();

	glPointSize(10);
	glBegin(GL_POINTS);
		glColor3f(0, 0, 1);
		glVertex3f(p3[0], p3[1], p3[2]);
	glEnd();

	glTranslatef(pos[0], pos[1], pos[2]);
	glutSolidSphere(.05, 10, 10);
	glTranslatef(-pos[0], -pos[1], -pos[2]);




	equation_plane(
			object_list[i].vertices[0],
			object_list[i].vertices[1],
			object_list[i].vertices[2],
			object_list[i].vertices[3],
			object_list[i].vertices[4],
			object_list[i].vertices[5],
			object_list[i].vertices[6],
			object_list[i].vertices[7],
			object_list[i].vertices[8],
			&plane_eqn[0],
			&plane_eqn[1],
			&plane_eqn[2],
			&plane_eqn[3]
		      );

	glClipPlane(GL_CLIP_PLANE0, plane_eqn);
	glEnable(GL_CLIP_PLANE0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		pos[0], pos[1], pos[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]);

	glDisable(GL_CLIP_PLANE0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	// draw the mirror to set the depth buffer.
	glBegin(GL_TRIANGLES);
		for (int j=0; j<object_list[i].vertices.size(); j+=3) {
			glVertex3fv(&(object_list[i].vertices[j]));
		}
	glEnd();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	render();


	glBindTexture(GL_TEXTURE_2D, cam_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cam_width, cam_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TVbuffer);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(2.926557, 0.400000, -3.787318);
		glTexCoord2f(0, 1); glVertex3f(2.926557, 1.20000, -3.787318);
		glTexCoord2f(1, 1); glVertex3f(3.484314, 1.200000, -2.666053);
		glTexCoord2f(1, 0); glVertex3f(3.484314, 0.400000, -2.666053);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	

		

	glFlush();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	/* update the state variables for window size */
	win_width = w;
	win_ht = h;

	printf("%i, %i\n", w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(45.0f, 1.0f * w / h, .1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(1);
		break;

	case 'c':
		if (camera_mode == 0) {
			camera_mode = 1;
		}
		else {
			camera_mode = 0;
		}
	}

	if (camera_mode == 0) {
		switch (key)
		{
		case 'w':
		case 'W':
			/* move in the xz-plane direction of 'look' */
			vec3 dir;
			vec3_set(dir, look[0], 0, look[2]);
			vec3_scale(dir, dir, move_factor);
			vec3_add(pos, pos, dir);
			update_center();
			glutPostRedisplay();
			break;

		case 's':
		case 'S':
			/* move in the xz-plane direction opposite of 'look' */
			vec3_set(dir, look[0], 0, look[2]);
			vec3_scale(dir, dir, -move_factor);
			vec3_add(pos, pos, dir);
			update_center();
			glutPostRedisplay();
			break;

		case 'a':
		case 'A':
			/*
			 * strafe in the direction opposite of the
			 * cross product of look and up
			 */
			vec3 strafe_dir;
			vec3_mul_cross(strafe_dir, look, up);
			vec3_scale(strafe_dir, strafe_dir, -move_factor);
			vec3_add(pos, pos, strafe_dir);
			update_center();
			glutPostRedisplay();
			break;

		case 'd':
		case 'D':
			/*
			 * strafe left in the direction of the
			 * cross product of look and up
			 */
			vec3_mul_cross(strafe_dir, look, up);
			vec3_scale(strafe_dir, strafe_dir, move_factor);
			vec3_add(pos, pos, strafe_dir);
			update_center();
			glutPostRedisplay();
			break;
		}
	}

	if (camera_mode == 1) {
		switch (key)
		{
		case 'w':
		case 'W':
			/* move in the xz-plane direction of 'look' */
			vec3 dir;
			vec3_set(dir, cam_look[0], 0, cam_look[2]);
			vec3_scale(dir, dir, move_factor);
			vec3_add(cam_pos, cam_pos, dir);
			update_center();
			glutPostRedisplay();
			break;

		case 's':
		case 'S':
			/* move in the xz-plane direction opposite of 'look' */
			vec3_set(dir, cam_look[0], 0, cam_look[2]);
			vec3_scale(dir, dir, -move_factor);
			vec3_add(cam_pos, cam_pos, dir);
			update_center();
			glutPostRedisplay();
			break;

		case 'a':
		case 'A':
			/*
			 * strafe in the direction opposite of the
			 * cross product of look and up
			 */
			vec3 strafe_dir;
			vec3_mul_cross(strafe_dir, cam_look, up);
			vec3_scale(strafe_dir, strafe_dir, -move_factor);
			vec3_add(cam_pos, cam_pos, strafe_dir);
			update_center();
			glutPostRedisplay();
			break;

		case 'd':
		case 'D':
			/*
			 * strafe left in the direction of the
			 * cross product of cam_look and up
			 */
			vec3_mul_cross(strafe_dir, cam_look, up);
			vec3_scale(strafe_dir, strafe_dir, move_factor);
			vec3_add(cam_pos, cam_pos, strafe_dir);
			update_center();
			glutPostRedisplay();
			break;
		}
	}



	switch(key) {
		case 'M':
			move_factor += 1;
			break;

		case 'm':
			move_factor -= 1;
			break;

		case 'L':
			lin_atten += .1;
			glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, lin_atten);
			glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, lin_atten);
			glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, lin_atten);
			glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, lin_atten);
			printf("lin_atten: %f\n", lin_atten);
			glutPostRedisplay();
			break;

		case 'l':
			lin_atten -= .1;
			glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, lin_atten);
			glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, lin_atten);
			glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, lin_atten);
			glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, lin_atten);
			printf("lin_atten: %f\n", lin_atten);
			glutPostRedisplay();
			break;
		case 'Q':
			quad_atten += .1;
			glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, quad_atten);
			glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, quad_atten);
			glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, quad_atten);
			glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, quad_atten);
			printf("quad_atten: %f\n", quad_atten);
			glutPostRedisplay();
			break;
		case 'q':
			quad_atten -= .1;
			glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, quad_atten);
			glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, quad_atten);
			glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, quad_atten);
			glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, quad_atten);
			printf("quad_atten: %f\n", quad_atten);
			glutPostRedisplay();
			break;
		}
	printf("POS = %f, %f, %f\n", pos[0], pos[1], pos[2]);
}

void specialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;

		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(500, 500);
			glutPositionWindow(50, 50);
		}
	}

	if (key == GLUT_KEY_UP) {
		pos[1] += move_factor;
		update_center();
		glutPostRedisplay();
	}

	if (key == GLUT_KEY_DOWN) {
		pos[1] -= move_factor;
		update_center();
		glutPostRedisplay();
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;

	}
	else
		mouseDown = false;
}

void mouseMotion(int x, int y)
{
	if (camera_mode == 0) {
		/* store the original cursor pos. state and update it. */
		float x0 = cur_x;
		float y0 = cur_y;
		cur_x = x;
		cur_y = y;

		/* calc. cursor pos. deltas and convert to gl coordinates */
		float gldx = 10 * (cur_x - x0) / win_width;
		float gldy = 10 * (y0 - cur_y) / win_ht;

		/*
		 * Use the mouse x disp. to rotate the camera look vector
		 * about the world y-axis.
		 */

		 /* create rotation transformation matrix. */
		static mat4x4 R;
		mat4x4_identity(R);
		mat4x4_rotate_Y(R, R, gldx);

		/* apply rotation transformation to look vector. */
		static vec3 temp, f;
		mat4x4_mul_vec3(temp, R, look);
		vec3_set(look, temp[0], temp[1], temp[2]);


		/*
		 * Use the mouse y disp. to rotate the camera vertically
		 * about an axis normal to the look vector and the world
		 * y-axis.
		 */

		 /* calculte the rotation axis. */
		vec3 axis;
		vec3_mul_cross(axis, look, e2);

		/* rotate the look vector. */
		mat4x4_identity(R);
		mat4x4_rotate(R, R, axis[0], axis[1], axis[2], gldy);
		mat4x4_mul_vec3(temp, R, look);
		vec3_set(look, temp[0], temp[1], temp[2]);
		update_center();

		/*
		 * warp the mouse back to the center when it reaches
		 * the window edge.
		 */
		if (x <= 10 || x >= win_width - 10 || y <= 10 || y >= win_ht - 10) {
			cur_x = win_width / 2;
			cur_y = win_ht / 2;
			glutWarpPointer(win_width / 2, win_ht / 2);
		}
	}
	if (camera_mode == 1) {
		/* store the original cursor pos. state and update it. */
		float x0 = cur_x;
		float y0 = cur_y;
		cur_x = x;
		cur_y = y;

		/* calc. cursor pos. deltas and convert to gl coordinates */
		float gldx = 10 * (cur_x - x0) / win_width;
		float gldy = 10 * (y0 - cur_y) / win_ht;

		/*
		 * Use the mouse x disp. to rotate the camera look vector
		 * about the world y-axis.
		 */

		 /* create rotation transformation matrix. */
		static mat4x4 R;
		mat4x4_identity(R);
		mat4x4_rotate_Y(R, R, gldx);

		/* apply rotation transformation to look vector. */
		static vec3 temp, f;
		mat4x4_mul_vec3(temp, R, cam_look);
		vec3_set(cam_look, temp[0], temp[1], temp[2]);


		/*
		 * Use the mouse y disp. to rotate the camera vertically
		 * about an axis normal to the look vector and the world
		 * y-axis.
		 */

		 /* calculte the rotation axis. */
		vec3 axis;
		vec3_mul_cross(axis, cam_look, e2);

		/* rotate the look vector. */
		mat4x4_identity(R);
		mat4x4_rotate(R, R, axis[0], axis[1], axis[2], gldy);
		mat4x4_mul_vec3(temp, R, cam_look);
		vec3_set(cam_look, temp[0], temp[1], temp[2]);
		update_center();

		/*
		 * warp the mouse back to the center when it reaches
		 * the window edge.
		 */
		if (x <= 10 || x >= win_width - 10 || y <= 10 || y >= win_ht - 10) {
			cur_x = win_width / 2;
			cur_y = win_ht / 2;
			glutWarpPointer(win_width / 2, win_ht / 2);
		}
	}


	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	/*
	 * load the model
	 */
	load_mtl();
	build_vtnf();
	build_arrays();

	for (int i=0; i<object_list.size(); i++) {
		std::cout << object_list[i].material << std::endl;
		std::cout << object_list[i].material << ": "
			<< mtl_map[object_list[i].material]->map_Kd << std::endl;
	}

	load_bmp();

	glutInit(&argc, argv);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("13 - Solid Shapes");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouseMotion);
	glutReshapeFunc(resize);

	if (!init())
		return 1;

	glutMainLoop();

	return 0;
}
