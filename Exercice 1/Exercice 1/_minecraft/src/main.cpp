//Includes application
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>
#include <ctime>

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

#include "world.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"


NYRenderer * g_renderer = NULL;
GLuint g_program;
NYTimer * g_timer = NULL;
int g_nb_frames = 0;
float g_elapsed_fps = 0;
int g_main_window_id;
int g_mouse_btn_gui_state = 0;
bool g_fullscreen = false;

//Mouse position
int xMouse = 0;
int yMouse = 0;

//Keyboard
bool ctrl = false;

//Time
time_t timer;
struct tm * now;

//World
NYWorld * g_world;

//GUI 
GUIScreenManager * g_screen_manager = NULL;
GUIBouton * BtnParams = NULL;
GUIBouton * BtnClose = NULL;
GUILabel * LabelFps = NULL;
GUILabel * LabelCam = NULL;
GUIScreen * g_screen_params = NULL;
GUIScreen * g_screen_jeu = NULL;
GUISlider * g_slider;


//////////////////////////////////////////////////////////////////////////
// GESTION APPLICATION
//////////////////////////////////////////////////////////////////////////
void update(void)
{
	float elapsed = g_timer->getElapsedSeconds(true);

	static float g_eval_elapsed = 0;

	//Calcul des fps
	g_elapsed_fps += elapsed;
	g_nb_frames++;
	if(g_elapsed_fps > 1.0)
	{
		LabelFps->Text = std::string("FPS : ") + toString(g_nb_frames);
		g_elapsed_fps -= 1.0f;
		g_nb_frames = 0;
	}

	//Rendu
	g_renderer->render(elapsed);
}


void render2d(void)
{
	g_screen_manager->render();
}

void renderObjects(void)
{	
	glEnable(GL_LIGHTING);

	//Materials 
	//Diffuse
	GLfloat materialDiffuse[] = { 0, 0.7, 0,1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//Speculaire
	GLfloat whiteSpecularMaterial[] = { 0.3, 0.3, 0.3,1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
	GLfloat mShininess = 100;
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

	//Emissive
	GLfloat emissive[] = { 0.0, 0.0, 0.0,1.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, emissive);

	//Ambient
	GLfloat materialAmbient[] = { 0, 0.2, 0,1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	//Utilisation des shaders
	glUseProgram(g_program);

	GLuint elap = glGetUniformLocation(g_program, "elapsed");
	glUniform1f(elap, NYRenderer::_DeltaTimeCumul);

	GLuint amb = glGetUniformLocation(g_program, "ambientLevel");
	glUniform1f(amb, 0.4);

	// Commenté car pas de inverviewmatrix
	GLuint invView = glGetUniformLocation(g_program, "invertView");
	glUniformMatrix4fv(invView, 1, true, g_renderer->_Camera->_InvertViewMatrix.Mat.t);

	//Rendu des axes
	glBegin(GL_LINES);
	//Primitives axe des X
	materialAmbient[0] = 0.1;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 1;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glColor3d(1,0,0);
	glVertex3d(0,0,0);
	glVertex3d(10000,0,0);
	//Primitives axe des y
	materialAmbient[0] = 0;
	materialAmbient[1] = 0.1;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 1;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glColor3d(0,1,0);
	glVertex3d(0,0,0);
	glVertex3d(0,10000,0);
	//Primitives axe des z
	materialAmbient[0] = 0;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0.1;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glColor3d(0,0,1);
	glVertex3d(0,0,0);
	glVertex3d(0,0,10000);
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	/*
	//Les deux codes font la même chose. attention à l'application des rotate/translate dans le sens inverse
	glRotatef(45, 0, 0, 1);
	glTranslatef(sqrt(8.0f), 0, 0);

	//glTranslatef(2, 2, 0);
	//glRotatef(45, 0, 0, 1);

	glBegin(GL_QUADS);
	glEnable(GL_CULL_FACE);
	glColor3d(1, 1, 1);

	//Face1 (gauche)
	materialAmbient[0] = 0;
	materialAmbient[1] = 0.1;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 1;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	glNormal3f(0, -1, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);
	
	//Face2 (droite)
	materialAmbient[0] = 0;
	materialAmbient[1] = 0.1;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 1;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	glNormal3f(0, 1, 0);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, -1);
	
	//Face3 (face)
	materialAmbient[0] = 0.1;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 1;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	glNormal3f(1, 0, 0);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	
	//Face4 (arrière)
	materialAmbient[0] = 0.1;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 1;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	glNormal3f(-1, 0, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, 1, -1);
	
	//Face5 (haut)
	materialAmbient[0] = 0;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0.1;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	glNormal3f(0, 0, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);
	
	//Face6 (bas)
	materialAmbient[0] = 0;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0.1;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	glNormal3f(0, 0, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, -1, -1);
	
	glEnd();
	*/
	glPopMatrix();

	glPushMatrix();

	materialAmbient[0] = 0.3;
	materialAmbient[1] = 0.15;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = 0.3;
	materialDiffuse[1] = 0.15;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	float hourRatio = now->tm_hour / 24.f;
	glRotatef((hourRatio - 0.5f) * 360, 1, 0, 0);

	glTranslatef(0, 0, 40.f);
	glutSolidCube(5);

	glPopMatrix();

	glPushMatrix();
	g_world->render_world_old_school();
	glPopMatrix();

	//Changement de la couleur de fond
	NYColor skyColor(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 0.5f);
	g_renderer->setBackgroundColor(skyColor);

}

void setLights(void)
{
	//On active la light 0
	glEnable(GL_LIGHT0);

	//On définit une lumière directionelle (un soleil)

	float hourRatio = now->tm_hour / 24.f;
	NYVert3Df lightPos = NYVert3Df(0, 0, 1);
	lightPos.rotate(NYVert3Df(1, 0, 0), (hourRatio-0.5f) * 6.28);
	float direction[4] = {lightPos.X, lightPos.Y, lightPos.Z, 0}; ///w = 0 donc elle est a l'infini

	glLightfv(GL_LIGHT0, GL_POSITION, direction );
	float color[4] = {0.5f,0.5f,0.5f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color );
	float color2[4] = {0.3f,0.3f,0.3f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, color2 );
	float color3[4] = {0.3f,0.3f,0.3f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, color3 );
}

void resizeFunction(int width, int height)
{
	glViewport(0, 0, width, height);
	g_renderer->resize(width,height);
}

//////////////////////////////////////////////////////////////////////////
// GESTION CLAVIER SOURIS
//////////////////////////////////////////////////////////////////////////

void specialDownFunction(int key, int p1, int p2)
{
	//On change de mode de camera
	if(key == GLUT_WINDOW_ACCUM_ALPHA_SIZE)
	{
		ctrl = true;
	}
}

void specialUpFunction(int key, int p1, int p2)
{
	if (key == GLUT_WINDOW_ACCUM_ALPHA_SIZE)
	{
		ctrl = false;
	}
}

void keyboardDownFunction(unsigned char key, int p1, int p2)
{
	cout << key << endl;

	if(key == VK_ESCAPE)
	{
		glutDestroyWindow(g_main_window_id);	
		exit(0);
	}

	if(key == 'f')
	{
		if(!g_fullscreen){
			glutFullScreen();
			g_fullscreen = true;
		} else if(g_fullscreen){
			glutLeaveGameMode();
			glutLeaveFullScreen();
			glutReshapeWindow(g_renderer->_ScreenWidth, g_renderer->_ScreenWidth);
			glutPositionWindow(0,0);
			g_fullscreen = false;
		}
	}	
	if (key == '+')
	{
		now->tm_hour = now->tm_hour + 1;
	}
	if (key == '-')
	{
		now->tm_hour = now->tm_hour - 1;
	}
}

void keyboardUpFunction(unsigned char key, int p1, int p2)
{
}

void mouseWheelFunction(int wheel, int dir, int x, int y)
{
	g_renderer->_Camera->move(NYVert3Df(0, 0, 1*dir));
}

void mouseFunction(int button, int state, int x, int y)
{
	//Gestion de la roulette de la souris
	if((button & 0x07) == 3 && state)
		mouseWheelFunction(button,1,x,y);
	if((button & 0x07) == 4 && state)
		mouseWheelFunction(button,-1,x,y);

	//GUI
	g_mouse_btn_gui_state = 0;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_mouse_btn_gui_state |= GUI_MLBUTTON;
	
	bool mouseTraite = false;
	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
}

void mouseMoveFunction(int x, int y, bool pressed)
{
	bool mouseTraite = false;

	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
	if (pressed && xMouse != 0 && yMouse != 0 && !ctrl) 
	{
	
		float angle = (x - xMouse);
		angle = -angle / 80;
		g_renderer->_Camera->rotate(angle);

		angle = (y - yMouse);
		angle = -angle / 80;
		g_renderer->_Camera->rotateUp(angle);
	}
	else if (pressed && ctrl)
	{
		NYVert3Df newWay = g_renderer->_Camera->_Direction.normalize() * (yMouse-y);
		newWay = newWay + g_renderer->_Camera->_NormVec.normalize() * (x-xMouse);
		g_renderer->_Camera->move(newWay);
	}
	xMouse = x;
	yMouse = y;

}

void mouseMoveActiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,true);
}
void mouseMovePassiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,false);
}


void clickBtnParams (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_params);
}

void clickBtnCloseParam (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_jeu);
}

/**
  * POINT D'ENTREE PRINCIPAL
  **/
int main(int argc, char* argv[])
{ 
	LogConsole::createInstance();

	int screen_width = 800;
	int screen_height = 600;

	//Getting the time
	timer = time(0);
	now = localtime(&timer);
	

	glutInit(&argc, argv); 
	glutInitContextVersion(3,0);
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(screen_width,screen_height);
	glutInitWindowPosition (0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );

	glEnable(GL_MULTISAMPLE);

	Log::log(Log::ENGINE_INFO, (toString(argc) + " arguments en ligne de commande.").c_str());	
	bool gameMode = true;
	for(int i=0;i<argc;i++)
	{
		if(argv[i][0] == 'w')
		{
			Log::log(Log::ENGINE_INFO,"Arg w mode fenetre.\n");
			gameMode = false;
		}
	}

	if(gameMode)
	{
		int width = glutGet(GLUT_SCREEN_WIDTH);
		int height = glutGet(GLUT_SCREEN_HEIGHT);
		
		char gameModeStr[200];
		sprintf(gameModeStr,"%dx%d:32@60",width,height);
		glutGameModeString(gameModeStr);
		g_main_window_id = glutEnterGameMode();
	}
	else
	{
		g_main_window_id = glutCreateWindow("MyNecraft");
		glutReshapeWindow(screen_width,screen_height);
	}

	if(g_main_window_id < 1) 
	{
		Log::log(Log::ENGINE_ERROR,"Erreur creation de la fenetre.");
		exit(EXIT_FAILURE);
	}
	
	GLenum glewInitResult = glewInit();

	if (glewInitResult != GLEW_OK)
	{
		Log::log(Log::ENGINE_ERROR,("Erreur init glew " + std::string((char*)glewGetErrorString(glewInitResult))).c_str());
		_cprintf("ERROR : %s",glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	//Affichage des capacités du système
	Log::log(Log::ENGINE_INFO,("OpenGL Version : " + std::string((char*)glGetString(GL_VERSION))).c_str());

	glutDisplayFunc(update);
	glutReshapeFunc(resizeFunction);
	glutKeyboardFunc(keyboardDownFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(specialDownFunction);
	glutSpecialUpFunc(specialUpFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(mouseMoveActiveFunction);
	glutPassiveMotionFunc(mouseMovePassiveFunction);
	glutIgnoreKeyRepeat(1);

	//Initialisation du renderer
	g_renderer = NYRenderer::getInstance();
	g_renderer->setRenderObjectFun(renderObjects);
	g_renderer->setRender2DFun(render2d);
	g_renderer->setLightsFun(setLights);
	g_renderer->setBackgroundColor(NYColor());
	g_renderer->initialise(true);


	//Creation d'un programme de shader, avec vertex et fragment shaders
	g_program = g_renderer->createProgram("shaders/psbase.glsl", "shaders/vsbase.glsl");


	//On applique la config du renderer
	glViewport(0, 0, g_renderer->_ScreenWidth, g_renderer->_ScreenHeight);
	g_renderer->resize(g_renderer->_ScreenWidth,g_renderer->_ScreenHeight);
	
	//Ecran de jeu
	uint16 x = 10;
	uint16 y = 10;
	g_screen_jeu = new GUIScreen(); 

	g_screen_manager = new GUIScreenManager();
		
	//Bouton pour afficher les params
	BtnParams = new GUIBouton();
	BtnParams->Titre = std::string("Params");
	BtnParams->X = x;
	BtnParams->setOnClick(clickBtnParams);
	g_screen_jeu->addElement(BtnParams);

	y += BtnParams->Height + 1;

	LabelFps = new GUILabel();
	LabelFps->Text = "FPS";
	LabelFps->X = x;
	LabelFps->Y = y;
	LabelFps->Visible = true;
	g_screen_jeu->addElement(LabelFps);

	//Ecran de parametrage
	x = 10;
	y = 10;
	g_screen_params = new GUIScreen();

	GUIBouton * btnClose = new GUIBouton();
	btnClose->Titre = std::string("Close");
	btnClose->X = x;
	btnClose->setOnClick(clickBtnCloseParam);
	g_screen_params->addElement(btnClose);

	y += btnClose->Height + 1;
	y+=10;
	x+=10;

	GUILabel * label = new GUILabel();
	label->X = x;
	label->Y = y;
	label->Text = "Param :";
	g_screen_params->addElement(label);

	y += label->Height + 1;

	g_slider = new GUISlider();
	g_slider->setPos(x,y);
	g_slider->setMaxMin(1,0);
	g_slider->Visible = true;
	g_screen_params->addElement(g_slider);

	y += g_slider->Height + 1;
	y+=10;

	//Ecran a rendre
	g_screen_manager->setActiveScreen(g_screen_jeu);
	
	//Init Camera
	g_renderer->_Camera->setPosition(NYVert3Df(400,400,400));
	g_renderer->_Camera->setLookAt(NYVert3Df(0,0,0));
	

	//Fin init moteur

	//Init application



	//Init Timer
	g_timer = new NYTimer();
	
	//On start
	g_timer->start();

	//A la fin du main, on genere un monde
	g_world = new NYWorld();
	g_world->_FacteurGeneration = 5;
	g_world->init_world();

	glutMainLoop(); 

	return 0;
}

