/*****************\242.glk4_4 __ 6.12.2001a++					 .					adapted from(c) Mark J. Kilgard, 1994. ****************/#include "Movies.h"#include "ext_common.h"#include "ext.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"#include "agl.h"#include "GL/glut.h" //glu glu++#include <stdlib.h> // for rand() RANDMAX... no we donttypedef struct zttz {	Object    n_ob; 	void     *n2hdr;    	    void   	 *glout;    short     type;    GLfloat   vertex,vertex2,red,green,blue,              normalx,normaly,normalz,               anglx, angly, anglz,              dev, offset,              nr, nr1, nr2, nr3,              rate;      GLint	  r, c;									// row&columns of quad-vertex field    GLint	  current;									// current solide    GLboolean texture;                  long      shademodel, variant, step, result;    Boolean        on;  } NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_vertex(NNato *x, long vertex, long vertex2);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_angl(NNato *x, float val, float val1, float val2);void nnato_O(NNato *x, float val, float val1);void nnato_row(NNato *x, long n);   void nnato_column(NNato *x, long n);   void nnato_stgo(NNato *x, long n, long n1);void nnato_rate(NNato *x, float n);void nnato_bing(NNato *x);										//method to change values																//currently, variant 6++void nnato_glfun(n2glatom glatom);		// only alphavoid nnato_glfun2(n2glatom glatom);		//void nnato_glfun3(n2glatom glatom);void nnato_glfun4(n2glatom glatom);void nnato_glfun5(n2glatom glatom);void nnato_glfun6(n2glatom glatom);void nnato_glfun7(n2glatom glatom);void nnato_glfun8(n2glatom glatom);void nnato_glfun9(n2glatom glatom);void nnato_glfun10(n2glatom glatom);void nnato_glfun11(n2glatom glatom);void nnato_glfun12(n2glatom glatom);void nnato_glfun13(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);double ran(void);double alea(double min, double max);void    *nnato_class;main(){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_type,                       "drau.type",          A_DEFLONG,0);    addmess((method)nnato_shademodel,                 "normal.type",        A_DEFLONG,0);    addmess((method)nnato_normalz,                    "normal",             A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_vertex,                     "geo",                A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_revertgeo,                  "revert.geo",         0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_angl,                        "alpha",              A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT,0);    addmess((method)nnato_row,                    "r",            A_DEFLONG,0);    addmess((method)nnato_column,                    "c",            A_DEFLONG,0);    addmess((method)nnato_row,                    "row",            A_DEFLONG,0);    addmess((method)nnato_column,                    "col",            A_DEFLONG,0);    addmess((method)nnato_O,                        "do",              A_DEFFLOAT, A_DEFFLOAT,0);    addmess((method)nnato_stgo,                     "step",            A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_stgo,                     "%",               A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_bing,                    "bing",             0);    addmess((method)nnato_rate,                    "rate",            A_DEFFLOAT,0);    post ("242.glk4 ��� sier,2001", 0);    n2addfklass("242.glk4");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   strcpy( s, "� mem�ria de Almada Negreiros - K4\, QUADRADO AZUL\,-- 47 x 56 cm");       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, n2gltypestr);  // default `gl data type` outlet de[a]ss!zt--	   }}void nnato_info(NNato *x, void *p, void *b){ }void *nnato_new(){    NNato      *x;	short      err;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glk4 :  n2initobjekt err = %ld",err); return;}     n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine    nnato_revert(x);			x->on = true;	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){       n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   if (!variant) n2objsetglfun(x,(method)nnato_glfun);   else if (variant==1) n2objsetglfun(x,(method)nnato_glfun2);   else if (variant==2) n2objsetglfun(x,(method)nnato_glfun3);   else if (variant==3) n2objsetglfun(x,(method)nnato_glfun4);   else if (variant==4) n2objsetglfun(x,(method)nnato_glfun5);   else if (variant==5) n2objsetglfun(x,(method)nnato_glfun6);   else if (variant==6) n2objsetglfun(x,(method)nnato_glfun7);   else if (variant==7) n2objsetglfun(x,(method)nnato_glfun8);   else if (variant==8) n2objsetglfun(x,(method)nnato_glfun9);   else if (variant==9) n2objsetglfun(x,(method)nnato_glfun10);   else if (variant==10) n2objsetglfun(x,(method)nnato_glfun11);   else if (variant==11) n2objsetglfun(x,(method)nnato_glfun12);   else if (variant==12) n2objsetglfun(x,(method)nnato_glfun13);}void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_vertex(NNato *x, long vertex, long vertex2){    x->vertex   = (float)vertex  * .01;    x->vertex2  = (float)vertex2 * .01;}			void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_revertgeo(NNato *x){	    x->vertex = x->vertex2 = .5;   }		void nnato_revert(NNato *x){		x->type = 6;	    x->vertex = x->vertex2 = .5;    x->shademodel = GLU_SMOOTH;     x->normalx = x->normaly = 0.0;    x->normalz = 1.0;        x->texture = true;    x->r = x->c = 6;    x->anglx =     x->angly =     x->anglz = 0.;    x->step =    x->result = 0 ;    x->dev = 2.;    x->offset = -1.;    x->nr = x->nr1 = x->nr2 = 0.;    nnato_klr(x,50,50,50);    x->current = 0;}void nnato_angl(NNato *x, float val, float val1, float val2){    x->anglx = val;    x->angly = val1;    x->anglz = val2;}void nnato_O(NNato *x, float val, float val1){    x->dev = val;    x->offset = val1;//    nnato_bing(x);}void nnato_row(NNato *x, long n){        x->r = n;}  void nnato_column(NNato *x, long n){        x->c = n;	//was x->r... that's why...}  void nnato_rate(NNato *x, float n){        x->rate = n * 0.01;}  void nnato_stgo(NNato *x, long n, long n1){        x->step = n;    x->result = n1;}  void nnato_bing(NNato *x)	//method to change values{								// change this really soon    x->nr = ran() * x->dev + x->offset;    x->nr1 = ran() * x->dev + x->offset;    x->nr2 = ran() * x->dev + x->offset;    x->nr3 = ran() * x->dev + x->offset;        }void nnato_glfun(n2glatom glatom)				//only segment{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, 0);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, 0);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, 0);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, 0);        glEnd();        glPopMatrix();      }    }}void nnato_glfun2(n2glatom glatom)		//random at gl rate & mod{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);		if(j%x->step==x->result)        nr = ran() * x->dev + x->offset;        glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr);        glEnd();        glPopMatrix();      }    }}void nnato_glfun3(n2glatom glatom)		//igual a 2 + angulo rnd{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);		if(j%x->step==x->result)        nr = ran() * x->dev + x->offset;        glRotatef(anglx + nr, 1, 0, 0);        glRotatef(angly + nr, 0, 1, 0);        glRotatef(anglz + nr, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr);        glEnd();        glPopMatrix();      }    }}void nnato_glfun4(n2glatom glatom)				//3 e geom com 3 pontos dif{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr, nr1, nr2 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);		if(j%x->step==x->result){        nr = ran() * x->dev + x->offset;        nr1 = ran() * x->dev + x->offset;        nr2 = ran() * x->dev + x->offset;        }        glRotatef(anglx + nr, 1, 0, 0);        glRotatef(angly + nr1, 0, 1, 0);        glRotatef(anglz + nr2, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr1);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr2);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr1);        glEnd();        glPopMatrix();      }    }}void nnato_glfun5(n2glatom glatom){    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr, nr1, nr2 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);		if(j%x->step==x->result){        nr = ran() * x->dev + x->offset;        nr1 = ran() * x->dev + x->offset;        nr2 = ran() * x->dev + x->offset;        }        glRotatef(anglx , 1, 0, 0);        glRotatef(angly , 0, 1, 0);        glRotatef(anglz , 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr1);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr2);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr1);        glEnd();        glPopMatrix();      }    }}void nnato_glfun6(n2glatom glatom){    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r,    		cmax = COLS + ROWS,    		curr;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr = x->nr,            nr1 = x->nr1,             nr2 = x->nr2 ;        if (!x->on) return;    if (!curr) curr = 0;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {				curr = i+j;	//current solid         glPushMatrix();        glTranslatef(i, j, 0);        glRotatef(anglx , 1, 0, 0);        glRotatef(angly , 0, 1, 0);        glRotatef(anglz , 0, 0, 1);        		if (curr == x->current) nnato_bing(x);		        glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr);        glEnd();        glPopMatrix();		x->current = (x->current + 1) % cmax;	//cycle from 0 to lastsolid      }    }}void nnato_glfun7(n2glatom glatom){    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r,    		cmax = COLS + ROWS,    		curr;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr = x->nr,            nr1 = x->nr1,             nr2 = x->nr2,              nr3 = x->nr3 ;        if (!x->on) return;    if (!curr) curr = 0;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {				curr = i+j;	//current solid         glPushMatrix();        glTranslatef(i, j, 0);        glRotatef(anglx , 1, 0, 0);        glRotatef(angly , 0, 1, 0);        glRotatef(anglz , 0, 0, 1);        		if (curr == x->current) nnato_bing(x);		        glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr1);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr2);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr3);        glEnd();        glPopMatrix();		x->current = (x->current + 1) % cmax;	//cycle from 0 to lastsolid      }    }}void nnato_glfun8(n2glatom glatom)							// with rate++{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr = x->nr,            nr1 = x->nr1,             nr2 = x->nr2 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);        glRotatef(anglx , 1, 0, 0);        glRotatef(angly , 0, 1, 0);        glRotatef(anglz , 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr2 += x->rate);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr1 -= x->rate);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr2 += x->rate);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr1 -= x->rate);        glEnd();        glPopMatrix();      }    }}void nnato_glfun9(n2glatom glatom)							// with rate++{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr = x->nr,            nr1 = x->nr1,             nr2 = x->nr2 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);        glRotatef(anglx , 1, 0, 0);        glRotatef(angly , 0, 1, 0);        glRotatef(anglz , 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(i * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(-a, -b, nr2 += x->rate);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr1 -= x->rate);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr2 += x->rate);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr1 -= x->rate);        glEnd();        glPopMatrix();      }    }}void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			void nnato_glfun10(n2glatom glatom)				//segment & whole image there{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        	glTexCoord2f(0.0f ,0.0f);        glVertex3f(-a, -b, 0);        	glTexCoord2f(1.0f , 0.0f);        glVertex3f(a, -b, 0);        	glTexCoord2f(1.0f , 1.0f);        glVertex3f(a, b, 0);        	glTexCoord2f(0.0f , 1.0f);        glVertex3f(-a, b, 0);        glEnd();        glPopMatrix();      }    }}void nnato_glfun11(n2glatom glatom)							// with rate++ & whole image there{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,           nr = x->nr   ,            nr1 = x->nr1 ,             nr2 = x->nr2 ,              nr3 = x->nr3 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);        glRotatef(anglx , 1, 0, 0);        glRotatef(angly , 0, 1, 0);        glRotatef(anglz , 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        glTexCoord2f(0.0f , 1.0f);        glVertex3f(-a, -b, nr += x->rate);        glTexCoord2f((i + 1) * TILE_TEX_W, j * TILE_TEX_H);        glVertex3f(a, -b, nr1 -= x->rate);        glTexCoord2f((i + 1) * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(a, b, nr2 += x->rate);        glTexCoord2f(i * TILE_TEX_W, (j + 1) * TILE_TEX_H);        glVertex3f(-a, b, nr3 -= x->rate);        glEnd();        glPopMatrix();      }    }}void nnato_glfun12(n2glatom glatom)				//segment & whole image there &rand1 bing{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,            nr = x->nr   ,            nr1 = x->nr1 ,             nr2 = x->nr2 ,              nr3 = x->nr3 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        	glTexCoord2f(0.0f ,0.0f);        glVertex3f(-a, -b, nr);        	glTexCoord2f(1.0f , 0.0f);        glVertex3f(a, -b, nr);        	glTexCoord2f(1.0f , 1.0f);        glVertex3f(a, b, nr);        	glTexCoord2f(0.0f , 1.0f);        glVertex3f(-a, b, nr);        glEnd();        glPopMatrix();      }    }}void nnato_glfun13(n2glatom glatom)				//segment & whole image there &rand4 bing{    NNato *x = (NNato *)n2getglatomobj(glatom);    int	   i, j,    		COLS = x->c,    		ROWS = x->r;                           float  a  = x->vertex,           b  = x->vertex2,           anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           TILE_TEX_W = 1.0/COLS,           TILE_TEX_H = 1.0/ROWS,            nr = x->nr   ,            nr1 = x->nr1 ,             nr2 = x->nr2 ,              nr3 = x->nr3 ;        if (!x->on) return;        glShadeModel(x->shademodel);    glColor3f(x->red, x->green, x->blue);    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);    for (i = 0; i < COLS; i++) {      for (j = 0; j < ROWS; j++) {        glPushMatrix();        glTranslatef(i, j, 0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);        	glTexCoord2f(0.0f ,0.0f);        glVertex3f(-a, -b, nr);        	glTexCoord2f(1.0f , 0.0f);        glVertex3f(a, -b, nr1);        	glTexCoord2f(1.0f , 1.0f);        glVertex3f(a, b, nr2);        	glTexCoord2f(0.0f , 1.0f);        glVertex3f(-a, b, nr3);        glEnd();        glPopMatrix();      }    }}// // returns a random double between 0 and 1 //double ran(void){		return(alea(0.0, 1.0));}//// returns a random value as double between min and max value // ANSI says that RAND_MAX = 32767, pretty piss poor I say!//double alea(double min, double max){	if (max >= min)		return ((max-min)*((double)rand()/RAND_MAX) + min);	else		return ((min-max)*((double)rand()/RAND_MAX) + max);	}