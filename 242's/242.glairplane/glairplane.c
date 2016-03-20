#include "Movies.h"#include "ext_common.h"#include "ext.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"#include "agl.h"#include "GL/glut.h"typedef struct zttz {	Object    n_ob; 	void     *n2hdr;    // required nato - position = important _ must b 2nd	    void   	 *glout;    short     type;    GLfloat   vertex,vertex2,red,green,blue,alpha,              normalx,normaly,normalz,              anglx, angly, anglz,              sred, sgreen, sblue,salpha,              srad;      GLint	  ssidea, ssideb;    GLboolean texture;                  long      shademodel;    Boolean        on;          		// We'll request a sphere from the GLU library at run-time.	struct GLUquadric *plane_center;} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_vertex(NNato *x, long vertex, long vertex2);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue, long alpha);//rgbavoid nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_angl(NNato *x, float val, float val1, float val2);void nnato_sklr(NNato *x, long red, long green, long blue, long alpha);//rgbavoid nnato_sgeo(NNato *x, float val, int val1, int val2);void nnato_glfun(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void    *nnato_class;main(){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_type,                       "drau.type",          A_DEFLONG,0);    addmess((method)nnato_shademodel,                 "normal.type",        A_DEFLONG,0);    addmess((method)nnato_normalz,                    "normal",             A_DEFLONG,A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_sklr,                  "s.color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_sgeo,                  	"s.geo",              A_DEFFLOAT,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_vertex,                     "geo",                A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_revertgeo,                  "revert.geo",         0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_angl,                        "alpha",              A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT,0);    addmess((method)nnato_angl,                        "rotate",              A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT,0);    n2reklama();    n2addfklass("242.glairplane");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   strcpy( s, m9ndfukcpropaganda00);       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, n2gltypestr);  // default `gl data type` outlet de[a]ss!zt--	   }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glrekt :  n2initobjekt err = %ld",err); return;}     n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine    nnato_revert(x);			x->on = true;	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){       n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   // e.g. if (!variant) n2objsetglfun(x,(method)nnato_glfun); etc}void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue, long alpha) //rgba{   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;         x->alpha  = (float)alpha  * 0.01;      }  void nnato_sklr(NNato *x, long red, long green, long blue, long alpha) //rgba{   x->sred   = (float)red   * 0.01;   x->sgreen = (float)green * 0.01;   x->sblue  = (float)blue  * 0.01;         x->salpha  = (float)alpha  * 0.01;      }  void nnato_sgeo(NNato *x, float val, int val1, int val2){   x->srad   = val;   x->ssidea = val1;   x->ssideb  = val2;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_vertex(NNato *x, long vertex, long vertex2){    x->vertex   = (float)vertex  * .01;    x->vertex2  = (float)vertex2 * .01;}			void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_revertgeo(NNato *x){	    x->vertex = x->vertex2 = .5;   }		void nnato_revert(NNato *x){	//   glEnable(GL_BLEND);//   glDisable(GL_ALPHA_TEST);   // Blending mode used for fire, lit gas, etc.//   glBlendFunc(GL_SRC_ALPHA,GL_T); //  if (!(x->plane_center = gluNewQuadric()))   //   return;	x->type = 6;	    x->vertex = x->vertex2 = .5;    x->shademodel = GLU_SMOOTH;     x->normalx = x->normaly = 0.0;    x->normalz = 1.0;        x->texture = true;    nnato_klr(x,50,50,50,100);    nnato_sklr(x,0,100,0,70);    nnato_sgeo(x,0.02,8,8);        x->anglx = x->angly = x->anglz = 0.0f;    }void nnato_angl(NNato *x, float val, float val1, float val2){    x->anglx = val;    x->angly = val1;    x->anglz = val2;}void nnato_glfun(n2glatom glatom){    NNato *x = (NNato *)n2getglatomobj(glatom);    float  a  = x->vertex,           b  = x->vertex2,                                   anglx = x->anglx,           angly = x->angly,           anglz = x->anglz,           sred = x->sred,           sgreen = x->sgreen,           sblue = x->sblue,           salpha = x->salpha           ;       if (!x->on) return;    //   if (!(x->plane_center = gluNewQuadric()))//      return;//	glLoadIdentity();	glPushMatrix();	   // No texturing.//   glDisable(GL_TEXTURE_2D);   // Black holes are BLACK!   glColor4f(sred, sgreen, sblue, salpha);   gluSphere(x->plane_center,x->srad,x->ssidea,x->ssideb);	glPopMatrix();   // No texturing.//   glEnable(GL_TEXTURE_2D);    glShadeModel(x->shademodel);    glColor4f(x->red, x->green, x->blue, x->alpha);//    glTranslatef(-COLS / 2.0 + .5, -ROWS / 2.0 + .5, 0);     glTranslatef(-1.5, 0, -1.5);        glPushMatrix();	//first left wing//        glTranslatef(0, 0, 0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);      	 	if (x->texture) {        glTexCoord2f(0.0f,0.0f); 	glVertex3f(-a, -b, 0);        glTexCoord2f(1.0f,0.0f);	glVertex3f(a, -b, 0);        glTexCoord2f(1.0f,1.0f);	glVertex3f(a, b, 0);        glTexCoord2f(0.0f,1.0f);	glVertex3f(-a, b, 0);        }	else			{        glVertex3f(-a, -b, 0);        glVertex3f(a, -b, 0);        glVertex3f(a, b, 0);        glVertex3f(-a, b, 0);        }        glEnd();        glPopMatrix();         glPushMatrix();	//second left wing        glTranslatef(0, 0,1.0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);      	 	if (x->texture) {        glTexCoord2f(0.0f,0.0f); 	glVertex3f(-a, -b, 0);        glTexCoord2f(1.0f,0.0f);	glVertex3f(a, -b, 0);        glTexCoord2f(1.0f,1.0f);	glVertex3f(a, b, 0);        glTexCoord2f(0.0f,1.0f);	glVertex3f(-a, b, 0);        }	else			{        glVertex3f(-a, -b, 0);        glVertex3f(a, -b, 0);        glVertex3f(a, b, 0);        glVertex3f(-a, b, 0);        }        glEnd();        glPopMatrix();                glPushMatrix();	//third left wing        glTranslatef(0,0, 2.0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);      	 	if (x->texture) {        glTexCoord2f(0.0f,0.0f); 	glVertex3f(-a, -b, 0);        glTexCoord2f(1.0f,0.0f);	glVertex3f(a, -b, 0);        glTexCoord2f(1.0f,1.0f);	glVertex3f(a, b, 0);        glTexCoord2f(0.0f,1.0f);	glVertex3f(-a, b, 0);        }	else			{        glVertex3f(-a, -b, 0);        glVertex3f(a, -b, 0);        glVertex3f(a, b, 0);        glVertex3f(-a, b, 0);        }        glEnd();        glPopMatrix();          glPushMatrix();	//first right wing        glTranslatef(2.0, 0, 0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);      	 	if (x->texture) {        glTexCoord2f(0.0f,0.0f); 	glVertex3f(-a, -b, 0);        glTexCoord2f(1.0f,0.0f);	glVertex3f(a, -b, 0);        glTexCoord2f(1.0f,1.0f);	glVertex3f(a, b, 0);        glTexCoord2f(0.0f,1.0f);	glVertex3f(-a, b, 0);        }	else			{        glVertex3f(-a, -b, 0);        glVertex3f(a, -b, 0);        glVertex3f(a, b, 0);        glVertex3f(-a, b, 0);        }        glEnd();        glPopMatrix();          glPushMatrix();	//second right wing        glTranslatef(2.0, 0, 1.0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);      	 	if (x->texture) {        glTexCoord2f(0.0f,0.0f); 	glVertex3f(-a, -b, 0);        glTexCoord2f(1.0f,0.0f);	glVertex3f(a, -b, 0);        glTexCoord2f(1.0f,1.0f);	glVertex3f(a, b, 0);        glTexCoord2f(0.0f,1.0f);	glVertex3f(-a, b, 0);        }	else			{        glVertex3f(-a, -b, 0);        glVertex3f(a, -b, 0);        glVertex3f(a, b, 0);        glVertex3f(-a, b, 0);        }        glEnd();        glPopMatrix();          glPushMatrix();	//third right wing        glTranslatef(2.0, 0, 2.0);                glRotatef(anglx, 1, 0, 0);        glRotatef(angly, 0, 1, 0);        glRotatef(anglz, 0, 0, 1);                glBegin(x->type);      	glNormal3f(x->normalx,x->normaly,x->normalz);      	 	if (x->texture) {        glTexCoord2f(0.0f,0.0f); 	glVertex3f(-a, -b, 0);        glTexCoord2f(1.0f,0.0f);	glVertex3f(a, -b, 0);        glTexCoord2f(1.0f,1.0f);	glVertex3f(a, b, 0);        glTexCoord2f(0.0f,1.0f);	glVertex3f(-a, b, 0);        }	else			{        glVertex3f(-a, -b, 0);        glVertex3f(a, -b, 0);        glVertex3f(a, b, 0);        glVertex3f(-a, b, 0);        }        glEnd();        glPopMatrix();} void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			