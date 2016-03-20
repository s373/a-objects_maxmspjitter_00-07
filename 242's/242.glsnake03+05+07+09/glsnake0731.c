/*fifo 3d points in space lined with arrow on top*/#include "Movies.h"#include "agl.h"//#include "GL/glut.h"#include "glut.h"#include "math.h" //for sqrt, abs#include "ext.h"#include "ext_common.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"//#define MAXGRID 256#define MAXGRID 64 //gl max vector stuff#define MAXLEN 256 //snake max vector pointstypedef struct{	//floating	float x;	float y;	float z;	float w;	float h; //width & height} Snake;typedef struct zttz {	Object    n_ob;	void     *n2hdr;    // required nato - position = important _ must b 2nd		void     *glout;	// out gl fun	GLfloat   vertex,vertex2,red,green,blue,	          normalx,normaly,normalz, rate;	GLboolean texture,border;            	Boolean        on, doubl, ignt;  	//	short		listlenght, oplenght;	 long      shademodel;	 short		ponto, type; //Gpoint	 float		linewidth;	 Boolean	lsmooth;	 float		cdist;	Snake	mar[MAXLEN], mar2[MAXLEN], mar3[MAXLEN], delta[MAXLEN];	short		marCount, marSize;	double	interp, zfact, arrowsize;	short		sidez[6];//sides of tunnel snake to draw} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_xyzdump(NNato *x);void nnato_xyzstore(NNato *x);void nnato_xyzinterp(NNato *x, double f);void nnato_zfact(NNato *x, double f);void draw_arrow_xy(NNato *x,double fx, double fy);void draw_arrow_size(NNato *x, double f);void nnato_linew(NNato *x, float n, int l);void nnato_cdist(NNato *x, float m );void nnato_xyz(NNato *x, float m, float o, float p , float w, float h);void nnato_marSize (NNato *x, long flag);//void nnato_wire (NNato *x, long flag);void nnato_glfun(n2glatom glatom);void nnato_glfun2(n2glatom glatom);		//void nnato_glfun3(n2glatom glatom);void nnato_glfun4(n2glatom glatom);void nnato_glfun5(n2glatom glatom);void nnato_glfun6(n2glatom glatom);void nnato_glfun7(n2glatom glatom);void nnato_glfun8(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void    *nnato_class;void main(				void		){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_shademodel,"model",A_DEFLONG,0);    addmess((method)nnato_type,"type",A_DEFLONG,0);    addmess((method)nnato_normalz,"normal",A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_linew,"linew",A_DEFFLOAT,A_DEFLONG, 0);    addmess((method)nnato_cdist,"dist",A_DEFFLOAT, 0);    addmess((method)nnato_xyz,"xyz",A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);    addmess((method)nnato_marSize,"snake.size",A_DEFLONG,0);    addmess((method)nnato_xyzdump,"xyzdump",0);    addmess((method)nnato_xyzstore,"xyzstore",0);    addmess((method)nnato_xyzinterp,"xyzinterp",A_DEFFLOAT, 0);    addmess((method)nnato_zfact,"zfact",A_DEFFLOAT, 0);    addmess((method)draw_arrow_size,"arrow.size",A_DEFFLOAT, 0);post("�		Eastern Bunny.glsnake"__DATE__,0);post("88206D656D97726961206465",0)  ;post("20416C6D616461204E656772",0)  ;post("6569726F73202D204B342C20",0)  ;post("515541445241444F20415A55",0)  ;post("4C2C2D2D2034372078203536",0)  ;post("       20636D01         ",0)  ;post("    6D656D9772696120",0)  ;post("20416C6D616461204E656772",0)  ;post("6569726F73202D204B342C20",0)  ;post("515541445241444F20415A55",0)  ;post("4C2C2D2D2034372078203536",0)  ;    n2addfklass("242.glseth07");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      	   strcpy( s, "gldata, commands");       {	   if (msg == 2)   	      if (nr == 0)	         strcpy( s, "gldata");  		}}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err, i,j;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glplain :  n2initobjekt err = %ld",err); return;} 	x->ignt=0;    nnato_revert(x);//    post("revert bypassed",0);//    nnato_vectinit(x);	    for(i=0;i < MAXLEN; i++) {    	x->mar[i].x=0.0f ;    	x->mar[i].y=0.0f ;       	x->mar[i].z=0.0f ;    	x->mar[i].w=0.1f ;//important !=null div by zero    	x->mar[i].h=0.1f ;         	}    n2objsetglregister(x,(method)nnato_glregister);       n2objsetglfun(x,(method)nnato_glfun);          //    post("registered and set fun",0);			x->on = true;//    post("x->on = true",0);        	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){   //	if(posit)		    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   if (!variant) n2objsetglfun(x,(method)nnato_glfun);   else if (variant==1) n2objsetglfun(x,(method)nnato_glfun2);   else if (variant==2) n2objsetglfun(x,(method)nnato_glfun3);   else if (variant==3) n2objsetglfun(x,(method)nnato_glfun4);   else if (variant==4 ) n2objsetglfun(x,(method)nnato_glfun5);   else if (variant==5) n2objsetglfun(x,(method)nnato_glfun6);   else if (variant==6 ) n2objsetglfun(x,(method)nnato_glfun7);   else if (variant==7) n2objsetglfun(x,(method)nnato_glfun8);}void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_linew(NNato *x, float n, int l){	x->linewidth = n;	if (l) x->lsmooth = 1;	if (!l) x->lsmooth = 0;}void nnato_xyz(NNato *x, float m, float o, float p, float w, float h ){	short i, range = x->marSize;	x->mar[1].x 	= 	x->mar[0].x ;	x->mar[1].y 	=	 x->mar[0].y ;	x->mar[1].z 	=	 x->mar[0].z ;	x->mar[1].w 	=	 x->mar[0].w ;	x->mar[1].h 	=	 x->mar[0].h ;		x->mar[0].x 	= m;	x->mar[0].y 	= o;	x->mar[0].z 	= p;	if (w!=0.)   x->mar[0].w 	= w;	if (h!=0.)   x->mar[0].h 	= h;	//increment position to write on//	x->marCount+=1;//	if(x->marCount > (x->marSize-1)) x->marCount = 0;	for (i=(range-1); i>=1; i--) {	x->mar[i+1].x 	= 	x->mar[i+0].x ;	x->mar[i+1].y 	=	 x->mar[i+0].y ;	x->mar[i+1].z 	=	 x->mar[i+0].z ;		x->mar[i+1].w 	=	 x->mar[i+0].w ;		x->mar[i+1].h 	=	 x->mar[i+0].h ;		}}void nnato_xyzdump(NNato *x){	short i, range = x->marSize;	post("xyz � � ��������������������",0);	post("xyz  range (sanke.size = %ld", range, 0);		post("xyz � � ��������������������",0);	for (i=1; i<range; i++) {		post("    xyz \(%ld\)   x= %f  y=%f  z=%f  W=%f  H=%f", 	i,		x->mar[i].x ,	 			x->mar[i].y ,		 		x->mar[i].z , 		x->mar[i].w , 		x->mar[i].h , 				0);				}	for (i=1; i<range; i++) {		post("    xyz2 \(%ld\)   x= %f     y=%f     z=%f   W=%f      H=%f", 	i,		x->mar2[i].x ,	 			x->mar2[i].y ,		 		x->mar2[i].z , 		x->mar2[i].w , 		x->mar2[i].h , 		0);				}	for (i=1; i<range; i++) {		post("    xyz3 \(%ld\)   x= %f     y=%f     z=%f", 	i,		x->mar3[i].x ,	 			x->mar3[i].y ,		 		x->mar3[i].z , 0);				}	for (i=1; i<range; i++) {		post("    xyzdelta \(%ld\)   x= %f     y=%f     z=%f", 	i,		x->delta[i].x ,	 			x->delta[i].y ,		 		x->delta[i].z , 0);				}}void nnato_xyzstore(NNato *x){		short 	i;		for(i=0; i<x->marSize; i++){								x->mar3[i].x = x->mar2[i].x;		x->mar3[i].y = x->mar2[i].y;		x->mar3[i].z = x->mar2[i].z;		x->mar3[i].w = x->mar2[i].w;		x->mar3[i].h = x->mar2[i].h;		}							for(i=0; i<x->marSize; i++){								x->mar2[i].x = x->mar[i].x;		x->mar2[i].y = x->mar[i].y;		x->mar2[i].z = x->mar[i].z;		x->mar2[i].w = x->mar[i].w;		x->mar2[i].h = x->mar[i].h;		}							for(i=0; i<x->marSize; i++){							x->delta[i].x=	x->mar3[i].x - x->mar2[i].x;	x->delta[i].y=	x->mar3[i].y - x->mar2[i].y;	x->delta[i].z=	x->mar3[i].z - x->mar2[i].z;	x->delta[i].w=	x->mar3[i].w - x->mar2[i].w;	x->delta[i].h=	x->mar3[i].h - x->mar2[i].h;		}						}void nnato_xyzinterp(NNato *x, double f){	short		i;	//	for(i=0; i<x->marSize; i++){						//		x->mar[i].x = x->delta[i].x * f;//		x->mar[i].y = x->delta[i].y * f;//		x->mar[i].z = x->delta[i].z * f;//		}							for(i=0; i<x->marSize; i++){								x->mar[i].x = (x->mar3[i].x * f -  x->mar2[i].x*(1.0-f));		x->mar[i].y = (x->mar3[i].y * f -  x->mar2[i].y*(1.0-f));		x->mar[i].z = (i*x->zfact);		x->mar[i].w = (x->mar3[i].w * f -  x->mar2[i].w*(1.0-f));		x->mar[i].h = (x->mar3[i].h * f -  x->mar2[i].h*(1.0-f));		}						}void nnato_zfact(NNato *x, double f) { x->zfact = f; }void draw_arrow_xy(NNato *x,double fx, double fy){	double size = x->arrowsize, hsz = size/2.;	glBegin(x->type);		glVertex3f (fx, fy - hsz,0-x->mar[0].z);	glVertex3f (fx-size, fy - hsz,size -x->mar[0].z);	glVertex3f (fx+size, fy - hsz,size -x->mar[0].z);	glVertex3f (fx, fy- hsz,0 -x->mar[0].z);	glVertex3f (fx, fy+hsz,0 -x->mar[0].z);	glVertex3f (fx-size, fy+hsz,size -x->mar[0].z);	glVertex3f (fx+size, fy+hsz,size -x->mar[0].z);	glVertex3f (fx, fy+hsz,0-x->mar[0].z);			glEnd();}void draw_arrow_size(NNato *x, double f){ x->arrowsize = f; }void nnato_marSize (NNato *x, long flag) { x->marSize = flag ; }void nnato_cdist		(NNato *x, float m) { x->cdist = m ; }void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_revert(NNato *x){	    x->normalx = x->normaly = 0.0;    x->normalz = 1.0;             x->texture = true;    nnato_klr(x,50,50,50);    	x->linewidth = 1.0;	x->cdist = 0.0f;	x->marCount =0;	x->marSize =20;	x->zfact = 1.0;	x->arrowsize = 0.3;		x->sidez[0] = x->sidez[1] = x->sidez[2]= x->sidez[3] = x->sidez[4] = x->sidez[5] = 1;}void nnato_glfun(n2glatom glatom) // draw wireframe no normals{    NNato *x = (NNato *)n2getglatomobj(glatom);	short		i, j;       if (!x->on) return;    /*   if (x->On_Orbit) {      gluLookAt(x->camx,x->camy,x->camz,         0.0,0.0,0.0,         0.0,1.0,0.0);   } else {      gluLookAt(0. , 0. , 0. ,         0.0,0.0,0.0,         0.0,1.0,0.0);   }*///    glTranslatef(-st*grid / 2.0 + .5, -st*grid / 2.0 + .5, 0); //center it    glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light/* //dont calc normas    if (x->calcFaceNorms) {    getFaceNorms(x);    getVertNorms(x);	}*/		if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);    glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);    for(i=0;i < x->marSize;i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	                  glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  x->mar[i].z  ) ;          glEnd();        }}     void nnato_glfun2(n2glatom glatom) // draw wireframe master normals{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j;       if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);	draw_arrow_xy(x,  x->mar[0].x,  x->mar[0].y);	    for(i=0;i < (x->marSize -1);i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	                  glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  i*x->zfact  ) ;          glVertex3f( (float) x->mar[i+1].x,           		  (float)  x->mar[i+1].y,           		  (float)  (i+1)*x->zfact  ) ;          glEnd();        }}      void nnato_glfun3(n2glatom glatom) {    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j;	float   a, a2, b, b2, c, c2,  d, d2,  Ww2, wH2, Ww3, wH3;       if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);	draw_arrow_xy(x,  x->mar[0].x,  x->mar[0].y);	    for(i=0;i < (x->marSize -1);i++) {	 	 Ww2 = x->mar[i].w / 2.;	 wH2 = x->mar[i].h / 2.;	 	a =  x->mar[i].x - Ww2;	a2 =  x->mar[i].y + wH2;	b =  x->mar[i].x + Ww2;	b2 =  x->mar[i].y - wH2;	 Ww3 = x->mar[i+1].w / 2.;	 wH3 = x->mar[i+1].h / 2.;	c =  x->mar[i+1].x - Ww3;	c2 =  x->mar[i+1].y + wH3;	d =  x->mar[i+1].x + Ww3;	d2 =  x->mar[i+1].y - wH3;		glBegin(x->type);	 	        	//DHEA          glVertex3f( (float) a,           		  (float)  b2,           		  (float)  i*x->zfact  ) ;          glVertex3f( (float) c,           		  (float)  d2,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) c,           		  (float)  d,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) a,           		  (float)  a2,           		  (float)  i*x->zfact  ) ;	//BFEA          glVertex3f( (float) b,           		  (float)  a2,           		  (float)  i*x->zfact  ) ;          glVertex3f( (float) d,           		  (float)  c2,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) c,           		  (float)  d,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) a,           		  (float)  a2,           		  (float)  i*x->zfact  ) ;	//CGFB          glVertex3f( (float) b,           		  (float)  b2,           		  (float)  i*x->zfact  ) ;          glVertex3f( (float) d,           		  (float)  d2,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) d,           		  (float)  c2,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) b,           		  (float)  a2,           		  (float)  i*x->zfact  ) ;	//CGHD          glVertex3f( (float) b,           		  (float)  b2,           		  (float)  i*x->zfact  ) ;          glVertex3f( (float) d,           		  (float)  d2,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) c,           		  (float)  d2,           		  (float)  (i+1)*x->zfact  ) ;          glVertex3f( (float) a,           		  (float)  b2,           		  (float)  i*x->zfact  ) ;          glEnd();        }}void nnato_glfun4(n2glatom glatom)	//draew textured{    NNato *x = (NNato *)n2getglatomobj(glatom);       if (!x->on) return; }     void nnato_glfun5(n2glatom glatom) // { drawSmoothShaded(x); }{    NNato *x = (NNato *)n2getglatomobj(glatom);       if (!x->on) return;     }void nnato_glfun6(n2glatom glatom) // //    drawFlatShaded(x); }{    NNato *x = (NNato *)n2getglatomobj(glatom);       if (!x->on) return;    }  void nnato_glfun7(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);       if (!x->on) return;    }  void nnato_glfun8(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);       if (!x->on) return;      } void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			/*----------------------------------------------------------	EOF----------------------------------------------------------*/