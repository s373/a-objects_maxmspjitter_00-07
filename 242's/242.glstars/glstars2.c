#include "Movies.h"#include "ext_common.h"#include "ext.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"#include "agl.h"#include "GL/glut.h"#include <time.h>#include <stdlib.h>#include <math.h>#define PI n2mathpienum {  NORMAL = 0,  WEIRD = 1};enum {  STREAK = 0,  CIRCLE = 1};#define MAXSTARS 400#define MAXPOS 10000#define MAXWARP 10#define MAXANGLES 6000typedef struct _starRec {  GLint type;  float x[2], y[2], z[2];  float offsetX, offsetY, offsetR, rotation;} starRec;typedef struct zttz {	Object    		n_ob; 	void     		*n2hdr;    // required nato - position = important _ must b 2nd	    void   	 		*glout;    short     		type;    GLfloat   		vertex,vertex2,red,green,blue,              		normalx,normaly,normalz;      GLboolean 		texture;                  long      		shademodel;    Boolean        	on, ignt;  	GLenum 			doubleBuffer;	GLint 			windW, windH;	GLenum 			flag;	GLint 			starCount;	float 			speed;	GLint 			nitro;//	starRec 		stars[MAXSTARS];	starRec 		*stars;	float 			sinTable[MAXANGLES+1];	} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_vertex(NNato *x, long vertex, long vertex2);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_glfun(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void Idle(NNato *x);void Reshape(NNato *x,int width, int height);void ShowStars(NNato *x);void UpdateStars(NNato *x);void ShowStar(NNato *x,GLint n);GLenum StarPoint(NNato *x,GLint n);void MoveStars(NNato *x);void RotatePoint(NNato *x,float *xx, float *yy, float rotation);void NewStar(NNato *x,GLint n, GLint d);float Cos(NNato *x,float angle);float Sin(NNato *x,float angle);void    *nnato_class;main(){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_type,                       "drau.type",          A_DEFLONG,0);    addmess((method)nnato_shademodel,                 "normal.type",        A_DEFLONG,0);    addmess((method)nnato_normalz,                    "normal",             A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_vertex,                     "geo",                A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_revertgeo,                  "revert.geo",         0);    addmess((method)nnato_revert,                     "revert",             0);    n2reklama();    n2addfklass("242.glstars");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   strcpy( s, m9ndfukcpropaganda00);       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, n2gltypestr);  // default `gl data type` outlet de[a]ss!zt--	   }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glplain :  n2initobjekt err = %ld",err); return;}     nnato_revert(x);    post("revert bypassed",0);    n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine			x->on = false;	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){       if (x->stars) {n2disposeptr(x->stars);  x->stars = 0;}    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   if (!variant) n2objsetglfun(x,(method)nnato_glfun);}void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_vertex(NNato *x, long vertex, long vertex2){    x->vertex   = (float)vertex  * .01;    x->vertex2  = (float)vertex2 * .01;}			void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_revertgeo(NNato *x){	    x->vertex = x->vertex2 = .5;   }		void nnato_revert(NNato *x){	  float angle;  GLint n;//  srand((unsigned int) (__TIME__));  for (n = 0; n < MAXSTARS; n++) {    NewStar(x, n, 100);    post("star%ld", n);  }    angle = 0.0;  for (n = 0; n <= MAXANGLES; n++) {    x->sinTable[n] = sin(angle);    angle += PI / (MAXANGLES / 2.0);  }	x->type = 6;	    x->vertex = x->vertex2 = .5;    x->shademodel = GLU_SMOOTH;     x->normalx = x->normaly = 0.0;    x->normalz = 1.0;        x->texture = true;    nnato_klr(x,50,50,50);    	x->windW = 300, x->windH = 300;	x->flag = NORMAL;	x->starCount = MAXSTARS / 2;	x->speed = 1.0;	x->nitro = 0;}void nnato_glfun(n2glatom glatom){    NNato *x = (NNato *)n2getglatomobj(glatom);	int		i;	  GLint n;       if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);//    glTranslatef(-COLS / 2.0 + .5, 0., 0.);//	ShowStars(x);//  glClear(GL_COLOR_BUFFER_BIT);  for (n = 0; n < x->starCount; n++) {    if (x->stars[n].z[0] > x->speed || (x->stars[n].z[0] > 0.0 && x->speed < MAXWARP)) {      if (StarPoint(x,n) == GL_FALSE) {        NewStar(x, n, MAXPOS);      }    } else {      NewStar(x, n, MAXPOS);    }  }  for (n = 0; n < x->starCount; n++) {    if (x->stars[n].z[0] > x->speed || (x->stars[n].z[0] > 0.0 && x->speed < MAXWARP)) {      ShowStar(x, n);    }  }	}     void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			floatSin(NNato *x,float angle){  return (x->sinTable[(GLint) angle]);}floatCos(NNato *x,float angle){  return (x->sinTable[((GLint) angle + (MAXANGLES / 4)) % MAXANGLES]);}voidNewStar(NNato *x,GLint n, GLint d){  if (rand() % 4 == 0) {    x->stars[n].type = CIRCLE;  } else {    x->stars[n].type = STREAK;  }  x->stars[n].x[0] = (float) (rand() % MAXPOS - MAXPOS / 2);  x->stars[n].y[0] = (float) (rand() % MAXPOS - MAXPOS / 2);  x->stars[n].z[0] = (float) (rand() % MAXPOS + d);  x->stars[n].x[1] = x->stars[n].x[0];  x->stars[n].y[1] = x->stars[n].y[0];  x->stars[n].z[1] = x->stars[n].z[0];  if (rand() % 4 == 0 && x->flag == WEIRD) {    x->stars[n].offsetX = (float) (rand() % 100 - 100 / 2);    x->stars[n].offsetY = (float) (rand() % 100 - 100 / 2);    x->stars[n].offsetR = (float) (rand() % 25 - 25 / 2);  } else {    x->stars[n].offsetX = 0.0;    x->stars[n].offsetY = 0.0;    x->stars[n].offsetR = 0.0;  }}voidRotatePoint(NNato *x,float *xx, float *yy, float rotation){  float tmpX, tmpY;  tmpX = *xx * Cos(x,rotation) - *yy * Sin(x,rotation);  tmpY = *yy * Cos(x,rotation) + *xx * Sin(x,rotation);  *xx = tmpX;  *yy = tmpY;}voidMoveStars(NNato *x){  float offset;  GLint n;  offset = x->speed * 60.0;  for (n = 0; n < x->starCount; n++) {    x->stars[n].x[1] = x->stars[n].x[0];    x->stars[n].y[1] = x->stars[n].y[0];    x->stars[n].z[1] = x->stars[n].z[0];    x->stars[n].x[0] += x->stars[n].offsetX;    x->stars[n].y[0] += x->stars[n].offsetY;    x->stars[n].z[0] -= offset;    x->stars[n].rotation += x->stars[n].offsetR;    if (x->stars[n].rotation > MAXANGLES) {      x->stars[n].rotation = 0.0;    }  }}GLenumStarPoint(NNato *x,GLint n){  float x0, y0;  x0 = x->stars[n].x[0] * x->windW / x->stars[n].z[0];  y0 = x->stars[n].y[0] * x->windH / x->stars[n].z[0];  RotatePoint(x,&x0, &y0, x->stars[n].rotation);  x0 += x->windW / 2.0;  y0 += x->windH / 2.0;  if (x0 >= 0.0 && x0 < x->windW && y0 >= 0.0 && y0 < x->windH) {    return GL_TRUE;  } else {    return GL_FALSE;  }}voidShowStar(NNato *x,GLint n){  float x0, y0, x1, y1, width;  GLint i;  x0 = x->stars[n].x[0] * x->windW / x->stars[n].z[0];  y0 = x->stars[n].y[0] * x->windH / x->stars[n].z[0];  RotatePoint(x,&x0, &y0, x->stars[n].rotation);  x0 += x->windW / 2.0;  y0 += x->windH / 2.0;  if (x0 >= 0.0 && x0 < x->windW && y0 >= 0.0 && y0 < x->windH) {    if (x->stars[n].type == STREAK) {      x1 = x->stars[n].x[1] * x->windW / x->stars[n].z[1];      y1 = x->stars[n].y[1] * x->windH / x->stars[n].z[1];      RotatePoint(x,&x1, &y1, x->stars[n].rotation);      x1 += x->windW / 2.0;      y1 += x->windH / 2.0;      glLineWidth(MAXPOS / 100.0 / x->stars[n].z[0] + 1.0);      glColor3f(1.0, (MAXWARP - x->speed) / MAXWARP, (MAXWARP - x->speed) / MAXWARP);      if (fabs(x0 - x1) < 1.0 && fabs(y0 - y1) < 1.0) {        glBegin(GL_POINTS);        glVertex2f(x0, y0);        glEnd();      } else {        glBegin(GL_LINES);        glVertex2f(x0, y0);        glVertex2f(x1, y1);        glEnd();      }    } else {      width = MAXPOS / 10.0 / x->stars[n].z[0] + 1.0;      glColor3f(1.0, 0.0, 0.0);      glBegin(GL_POLYGON);      for (i = 0; i < 8; i++) {        float a = x0 + width * Cos(x, (float) i * MAXANGLES / 8.0);        float b = y0 + width * Sin(x, (float) i * MAXANGLES / 8.0);        glVertex2f(a, b);      };      glEnd();    }  }}voidUpdateStars(NNato *x){  GLint n;//  glClear(GL_COLOR_BUFFER_BIT);  for (n = 0; n < x->starCount; n++) {    if (x->stars[n].z[0] > x->speed || (x->stars[n].z[0] > 0.0 && x->speed < MAXWARP)) {      if (StarPoint(x,n) == GL_FALSE) {        NewStar(x, n, MAXPOS);      }    } else {      NewStar(x, n, MAXPOS);    }  }}voidShowStars(NNato *x){  GLint n;//  glClear(GL_COLOR_BUFFER_BIT);  for (n = 0; n < x->starCount; n++) {    if (x->stars[n].z[0] > x->speed || (x->stars[n].z[0] > 0.0 && x->speed < MAXWARP)) {      ShowStar(x, n);    }  }}voidReshape(NNato *x,int width, int height){  x->windW = width;  x->windH = height;post("glsatrs2     width = %ld 		height = %ld", width, height,0);/*  glViewport(0, 0, windW, windH);  glMatrixMode(GL_PROJECTION);  glLoadIdentity();  gluOrtho2D(-0.5, windW + 0.5, -0.5, windH + 0.5);  glMatrixMode(GL_MODELVIEW);*/}voidIdle(NNato *x){  MoveStars(x);  UpdateStars(x);  if (x->nitro > 0) {    x->speed = (float) (x->nitro / 10) + 1.0;    if (x->speed > MAXWARP) {      x->speed = MAXWARP;    }    if (++x->nitro > MAXWARP * 10) {      x->nitro = -x->nitro;    }  } else if (x->nitro < 0) {    x->nitro++;    x->speed = (float) (-x->nitro / 10) + 1.0;    if (x->speed > MAXWARP) {      x->speed = MAXWARP;    }  }//  glutPostRedisplay();}