// Cubica

#include <stdlib.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <math.h> 
#include <string.h> 
#include <stdio.h>
#include <assert.h>
#include "glm.h"

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// the global Assimp scene object
const aiScene* scene01 = NULL;
const aiScene* scene02 = NULL;
const aiScene* scene03 = NULL;
int color = 1;
bool reflect = false;
GLuint scene_list = 0;
static GLuint textName[3];
static GLuint textNameCube[6];
GLubyte* textureID[3];
GLubyte* texturasCubo[6];
int sizes[3][2];
int sizesCubo[6][2];
aiVector3D scene_min, scene_max, scene_center;

//Spotlight value
GLfloat  lightPos[] = { 0.0f, 200.0f, 0.0f, 1.0f };
GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f};
GLfloat  spotDir[] = { 0.0f, -1.0f, 0.0f };

GLfloat qaAmbientLight[] = {1.0, 1.0, 1.0, 1.0};
GLfloat qaSpec[]    = {1.0, 0.1, 0.1, 1.0};
GLfloat qaYellow[] = {1.0, 1.0, 0.0, 1.0}; //Black Color
GLfloat qaGreen[] = {0.0, 1.0, 0.0, 1.0}; //Green Color
GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0}; //White Color
GLfloat qaRed[] = {1.0, 0.0, 0.0, 1.0}; //Red Color
GLfloat qaMagenta[] = {1.0, 0.0, 1.0, 1.0};
GLfloat qaBunny[] = {0.5, 0.5, 0.5, 1.0};
GLfloat cutoff_spot = 50.0f;
GLfloat exponent_spot = 25.0f;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

using namespace std;

#define DEF_floorGridScale	1.0
#define DEF_floorGridXSteps	10.0
#define DEF_floorGridZSteps	10.0

/*Cube Map*/
static GLenum faceTarget[6] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

/* Pre-generated cube map images. */
char *faceFile[6] = {
  "negx.ppm", 
  "posx.ppm", 
  "posy.ppm", 
  "negy.ppm", 
  "posz.ppm", 
  "negz.ppm", 
};


int hasTextureLodBias = 0;

int mipmaps = 1;


void makeCubeMap(void)
{
	int i;
	glEnable(GL_TEXTURE_CUBE_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
    glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
    glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);

	//INICIALIZANDO LAS TEXTURAS PARA EL CUBE

	glGenTextures(1, &textNameCube[0]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, textNameCube[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, (GLuint)sizesCubo[0][0], (GLuint)sizesCubo[0][1], 0, GL_RGB, GL_UNSIGNED_BYTE, texturasCubo[0]);

	glGenTextures(1, &textNameCube[1]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, textNameCube[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, (GLuint)sizesCubo[1][0], (GLuint)sizesCubo[1][1], 0, GL_RGB, GL_UNSIGNED_BYTE, texturasCubo[1]);

	glGenTextures(1, &textNameCube[2]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, textNameCube[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, (GLuint)sizesCubo[2][0], (GLuint)sizesCubo[2][1], 0, GL_RGB, GL_UNSIGNED_BYTE, texturasCubo[2]);

	glGenTextures(1, &textNameCube[3]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, textNameCube[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, (GLuint)sizesCubo[3][0], (GLuint)sizesCubo[3][1], 0, GL_RGB, GL_UNSIGNED_BYTE, texturasCubo[3]);

	glGenTextures(1, &textNameCube[4]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, textNameCube[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, (GLuint)sizesCubo[4][0], (GLuint)sizesCubo[4][1], 0, GL_RGB, GL_UNSIGNED_BYTE, texturasCubo[4]);

	glGenTextures(1, &textNameCube[5]);
	glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, textNameCube[5]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, (GLuint)sizesCubo[5][0], (GLuint)sizesCubo[5][1], 0, GL_RGB, GL_UNSIGNED_BYTE, texturasCubo[5]);

	// Sets the texture's behavior for wrapping (optional)
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_MODULATE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_MODULATE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_MODULATE);
    // Sets the texture's max/min filters
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}

void changeViewport(int w, int h) {
	
	float aspectratio;

	if (h==0)
		h=1;

   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 3000.0);
   glMatrixMode (GL_MODELVIEW);
}


void cargar_materiales(int idx) {
	
	GLfloat mShininess[] = {128};
	GLfloat whiteSpecularMaterial[] = {0.5, 0.5, 0.5};

	// Material Piso
	if (idx == 0){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLuint)sizes[0][0], (GLuint)sizes[0][1], 0, GL_RGB, GL_UNSIGNED_BYTE, textureID[0]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, qaWhite);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, qaWhite);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

	}

	// Material Columna
	if (idx == 1){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLuint)sizes[1][0], (GLuint)sizes[1][1], 0, GL_RGB, GL_UNSIGNED_BYTE, textureID[1]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, qaWhite);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, qaWhite);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

	}

	// Material Conejo
	if (idx == 2){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLuint)sizes[2][0], (GLuint)sizes[2][1], 0, GL_RGB, GL_UNSIGNED_BYTE, textureID[2]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, qaBunny);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, qaBunny);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
	}
}


void recursive_render (const aiScene *sc, const aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	aiMatrix4x4 m = nd->mTransformation;

	// update transform
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);
	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n) {
		
		const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				
				if(mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				
				if(mesh->mNormals != NULL) 
					glNormal3fv(&mesh->mNormals[index].x);
				
				if (mesh->HasTextureCoords(0)) {
					glTexCoord2f(mesh->mTextureCoords[0][index].x, 1-mesh->mTextureCoords[0][index].y);
				}
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n) {
		cargar_materiales(n);
		recursive_render(sc, nd->mChildren[n]);
	}
	glPopMatrix();
}


void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
	case 27:             
		exit (0);
		break;
	case 'q':
		cutoff_spot -= 2.0;
		break;
	case 'w':
		cutoff_spot += 2.0;
		break;
	case 'a':
		exponent_spot -= 2.0;
		break;
	case 's':
		exponent_spot += 2.0;
		break;
	case 'e':
		spotDir[0] -= 0.1;
		break;
	case 'd':
		spotDir[0] += 0.1;
		break;
	case 'r':
		spotDir[2] -= 0.1;
		break;
	case 'f':
		spotDir[2] += 0.1;
		break;
	case 'c':
		reflect = !reflect;
		break;
	case 't':
		qaBunny[0] -= 0.1;
		break;
	case 'g':
		qaBunny[0] += 0.1;

		break;
	case 'y':
		qaBunny[1] -= 0.1;
		break;
	case 'h':
		qaBunny[1] += 0.1;
		break;
	case 'u':
		qaBunny[2] -= 0.1;
		break;
	case 'j':
		qaBunny[2] += 0.1;
		break;
	case '1':
		color = 1;
		break;
	case '2':
		color = 2;
		break;
	case '3':
		color = 3;
		break;
	case '4':
		color = 4;
		break;
	case '5':
		color = 5;
		break;
  }

  scene_list = 0;
  glutPostRedisplay();
}


void render(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity ();                       
	gluLookAt (0, 80, 250, 0.0, 15.0, 0.0, 0.0, 1.0, 0.0);

	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );
	
	//Creando el spotligth
	switch (color){
		case 1:
			glLightfv(GL_LIGHT0, GL_AMBIENT, qaWhite);
			glLightfv(GL_LIGHT0,GL_DIFFUSE,qaWhite);
			break;
		case 2:
			glLightfv(GL_LIGHT0, GL_AMBIENT, qaRed);
			glLightfv(GL_LIGHT0,GL_DIFFUSE,qaRed);
			break;
		case 3:
			glLightfv(GL_LIGHT0, GL_AMBIENT, qaGreen);
			glLightfv(GL_LIGHT0,GL_DIFFUSE,qaGreen);
			break;
		case 4:
			glLightfv(GL_LIGHT0, GL_AMBIENT, qaYellow);
			glLightfv(GL_LIGHT0,GL_DIFFUSE,qaYellow);
			break;
		case 5:
			glLightfv(GL_LIGHT0, GL_AMBIENT, qaMagenta);
			glLightfv(GL_LIGHT0,GL_DIFFUSE,qaMagenta);
			break;
	}
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	//direccion de la luz
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
	// Cut off
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,cutoff_spot);
    //shiny spot
	glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,exponent_spot);
	//Termina spotlight
	
	if(reflect)
		makeCubeMap();

	glPushMatrix();
	glEnable(GL_NORMALIZE);
	if(scene_list == 0) {
	    scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);
            // now begin at the root node of the imported data and traverse
            // the scenegraph by multiplying subsequent local transforms
            // together on GL's matrix stack.		
	    recursive_render(scene01, scene01->mRootNode);
	    glEndList();
	}
	glCallList(scene_list);
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glutSwapBuffers();
}

void animacion(int value) {
	glutTimerFunc(10,animacion,1);
    glutPostRedisplay();
}

void get_bounding_box_for_node (const aiNode* nd, 
	aiVector3D* min, 
	aiVector3D* max, 
	aiMatrix4x4* trafo
){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

		prev = *trafo;
		aiMultiplyMatrix4(trafo,&nd->mTransformation);

		for (; n < nd->mNumMeshes; ++n) {
			const aiMesh* mesh = scene01->mMeshes[nd->mMeshes[n]];
				for (t = 0; t < mesh->mNumVertices; ++t) {

				aiVector3D tmp = mesh->mVertices[t];
				aiTransformVecByMatrix4(&tmp,trafo);

				min->x = aisgl_min(min->x,tmp.x);
				min->y = aisgl_min(min->y,tmp.y);
				min->z = aisgl_min(min->z,tmp.z);

				max->x = aisgl_max(max->x,tmp.x);
				max->y = aisgl_max(max->y,tmp.y);
				max->z = aisgl_max(max->z,tmp.z);
			}
		}

		for (n = 0; n < nd->mNumChildren; ++n) {
			get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
		}
		*trafo = prev;
	

}

void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	
	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene01->mRootNode,min,max,&trafo);

}

int loadasset (const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
	
		scene01 = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene01) {
			get_bounding_box(&scene_min,&scene_max);
			scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
			scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
			scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
			return 0;
		}

	return 1;
}


void init(){

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   //glEnable(GL_COLOR_MATERIAL);

	//Inicializa las texturas
	textureID[0] = glmReadPPM("texAO_plano.ppm", &sizes[0][0], &sizes[0][1]);
	textureID[1] = glmReadPPM("texAO_columna.ppm", &sizes[1][0], &sizes[1][1]);
	textureID[2] = glmReadPPM("texAO_bunny.ppm", &sizes[2][0], &sizes[2][1]);

	//Inicializa texturas del cubemap
	for(int i=0;i<6;i++) {
		texturasCubo[i] = glmReadPPM(faceFile[i], &sizesCubo[i][0], &sizesCubo[i][1]);
	}

	for(int i=0;i<3;i++){
		glGenTextures(1, &textName[i]);
		glBindTexture(GL_TEXTURE_2D, textName[i]);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//Termina de inicializar las texturas

	//Comenzando inicializacion de spotlight
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Bunny Project");

	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

	// the model name can be specified on the command line. If none
	// is specified, we try to locate one of the more expressive test 
	// models from the repository (/models-nonbsd may be missing in 
	// some distributions so we need a fallback from /models!).
	/*if( 0 != loadasset( argc >= 2 ? argv[1] : "dragon_vrip_res2.ply")) {
		if( argc != 1 || (0 != loadasset( "dragon_vrip_res2.ply") && 0 != loadasset( "dragon_vrip_res2.ply"))) { 
			return -1;
		}
	}*/


	if (loadasset( "escenario.obj") != 0) {
		return -1;
	}
	
	init ();
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutKeyboardFunc (Keyboard);

	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	*/
	//gluPerspective( /* field of view in degree */ 0.0, /* aspect ratio */ 0.0, /* Z near */ 5.0, /* Z far */ 10.0);
	/*glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
	//gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */ 0.0, 0.0, 0.0,      /* center is at (0,0,0) */0.0, 1.0, 0.);      /* up is in positive Y direction */
	/*
	glEnable(GL_DEPTH_TEST);*/

	glutMainLoop();
	return 0;

}
