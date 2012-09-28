#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <Windows.h>

#define W 6                        /* 地面の幅の２分の１　 */
#define D 8                        /* 地面の長さの２分の１ */
double pos_x=0.0,pos_y=1.0,pos_z=8.00,move=0.04,tea=0.0;
/*
 * 地面を描く
 */
static void myGround(double height)
{
  const static GLfloat ground[][4] = {
    { 0.6, 0.6, 0.6, 1.0 },
    { 0.3, 0.3, 0.3, 1.0 }
  };

  int i, j;

  glBegin(GL_QUADS);
  glNormal3d(0.0, 1.0, 0.0);
  for (j = -D; j < D; ++j) {
    for (i = -W; i < W; ++i) {
      glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
      glVertex3d((GLdouble)i, height, (GLdouble)j);
      glVertex3d((GLdouble)i, height, (GLdouble)(j + 1));
      glVertex3d((GLdouble)(i + 1), height, (GLdouble)(j + 1));
      glVertex3d((GLdouble)(i + 1), height, (GLdouble)j);
    }
  }
  glEnd();
}

/*
 * 画面表示
 */
static void display(void)
{
  const static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /* 光源の位置 */
  const static GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };   /* 車の色　　 */
  const static GLfloat blue[] = { 0.2, 0.8, 0.8, 1.0 };
  static GLdouble px = 1.0, pz = -3.0;                       /* 車の位置　 */
  static GLdouble r = 0.0;                                  /* 車の方向　 */
  /* 画面クリア */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* モデルビュー変換行列の初期化 */
  glLoadIdentity();

  /* 光源の位置を設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

  /* 視点の移動（物体の方を奥に移す）*/
  glTranslated(0.0, 0.0, -25.0);
  glRotated(30.0, 1.0, 0.0, 0.0);

  /* シーンの描画 */
  myGround(0.0);
  glPushMatrix();
  glTranslated(px, 1.0, pz);
  glScalef(1.5,1.0,1.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
  glutSolidCube(2.0);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-3.2 ,1.0, 2.0);
  glScalef(1.0,1.0,1.5);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
  glutSolidCube(2.0);
  glPopMatrix();

  glPushMatrix();
  glTranslated(3.8 ,1.0, 3.0);
  glScalef(1.3,1.0,1.5);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
  glutSolidCube(2.0);
  glPopMatrix();

  glPushMatrix();
  glTranslated(pos_x, pos_y, pos_z);
  glRotated(90.0,0.0,1.0,0.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
  glutSolidTeapot(0.5);
  glPopMatrix();

  glFlush();
}

static void resize(int w, int h)
{
  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);

  /* 透視変換行列の指定 */
  glMatrixMode(GL_PROJECTION);

  /* 透視変換行列の初期化 */
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

  /* モデルビュー変換行列の指定 */
  glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char key, int x, int y)
{
  /* ESC か q をタイプしたら終了 */
  if (key == '\033' || key == 'q') {
    exit(0);
  }
  if (key == 'd') {
	if(pos_x > 5.25) {
		puts("GAMEOVER");
		return ;
	}
	pos_x += move;
	glutPostRedisplay();
  }
  if (key == 'a') {
	if(pos_x < -5.25) {
		return ;
	}
	pos_x -= move;
	glutPostRedisplay();
  }
  if (key == 's') {
	if(pos_z > 8.00) {
		return ;
	}
	pos_z += move;
	glutPostRedisplay();
  }
  if (key == 'w') {
	if(pos_z < -6.25) {
		puts("GAME CLEAR!");
		Sleep(2000);
		exit(1);
	}
	pos_z -= move;
	glutPostRedisplay();
  }
  if (key == 'j') {
	pos_y += move;
	glutPostRedisplay();
  }
  if (key == 'k') {
	  if(pos_y <= 1.00) {
		  return ;
	  }
	  pos_y -= move;
	  glutPostRedisplay();
  }
  if (key == 'r') {
	  puts("set movement");
	  scanf("%lf",&move);
  }
}

static void init(void)
{
  /* 初期設定 */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize( 1000, 800);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();
  return 0;
}