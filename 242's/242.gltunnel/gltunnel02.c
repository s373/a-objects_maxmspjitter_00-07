/*242.gltunnel.cproduzir sequencias de rects ao longo do eixo dos z's.a struct deve ter distancia da origem para o lado e para cimamais 3 valores para a rotacao/inclinacao do rectangulo.*/#include "Movies.h"#include "ext.h"#include "ext_common.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"#include "agl.h"#include "glut.h"#include "math.h" //for sqrt#define MAXLEN 256 //snake max vector points#define MAXGRID 64 //gl max vector stufftypedef struct{	//floating	GLfloat x;	//h dist	GLfloat y;	//v dist	GLfloat rotx,	//rot mags		  roty,		  rotz;} Tunnel_atom;typedef struct zttz {	Object    n_ob; 	void     *n2hdr;    // required nato - position = important _ must b 2nd	    void   	 *glout;	// out gl fun    short     type, grid, step;//removed gridy and gridx->grid for vfun compatibility    GLfloat   vertex,vertex2,red,green,blue,              normalx,normaly,normalz, rate;    GLboolean texture,border, calcFaceNorms, drawFaceNorms, antialias;                  Boolean        on, doubl, ignt;  		float		inputlist[MAXGRID];	float		st;//	short		listlenght, oplenght;	 long      shademodel;	 float		linewidth;	 Boolean	lsmooth;	float		irotx, iroty, irotz, ix, iy; //internal duplicates	Tunnel_atom		tunnel[MAXLEN];	short		tunCount, tunSize;} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_st(NNato *x, float st);void nnato_linew(NNato *x, float n, int l);//void nnato_hv(NNato *x, float m, float o);void nnato_tunSize (NNato *x, long flag);void nnato_rott(NNato *x, float m, float o, float p );void nnato_hv_set(NNato *x, float m, float o); // only set xyvoid nnato_rot_set(NNato *x, float m, float o, float p);//only set r.xyzvoid nnato_hv_only(NNato *x, float m, float o);void nnato_hv_do(NNato *x, float m, float o); //do with xy coordsvoid nnato_hv_rot_do(NNato *x, float m, float o, float rx, float ry, float rz);//do with rot onlyvoid nnato_hv_fi(NNato *x); //only fifovoid nnato_hv_rot(NNato *x, float m, float o, float rx, float ry, float rz); //synked all inputsvoid nnato_tunReport (NNato *x) ;void nnato_glfun(n2glatom glatom);void nnato_glfun2(n2glatom glatom);		//void nnato_glfun3(n2glatom glatom);void nnato_glfun4(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void    *nnato_class;main(){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_st,"stride",A_DEFFLOAT, 0);    addmess((method)nnato_shademodel,"model",A_DEFLONG,0);    addmess((method)nnato_type,"type",A_DEFLONG,0);    addmess((method)nnato_normalz,"normal",A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_linew,"linew",A_DEFFLOAT,A_DEFLONG, 0);    addmess((method)nnato_hv_do,"hv",A_DEFFLOAT,A_DEFFLOAT, 0);    addmess((method)nnato_tunSize,"t.size",A_DEFLONG,0);    addmess((method)nnato_rot_set,"rot",A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT, 0);    addmess((method)nnato_hv_fi,"fi",0);    addmess((method)nnato_hv_rot_do,"hvr",A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT, 0);//    addmess((method)nnato_hv_rot,"hvr",A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT, 0);    addmess((method)nnato_hv_set,"hv.set",A_DEFFLOAT,A_DEFFLOAT, 0);    addmess((method)nnato_rot_set,"rot.set",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);    addmess((method)nnato_tunReport,"report",0);    n2reklama();    post("�		Eastern Bunny",0);    n2addfklass("242.gltunnel");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   strcpy( s, m9ndfukcpropaganda00);       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, n2gltypestr);  // default `gl data type` outlet de[a]ss!zt--	   }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err, i,j;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glplain :  n2initobjekt err = %ld",err); return;} 	x->ignt=0;    nnato_revert(x);    post("revert bypassed",0);//    nnato_vectinit(x);	    for(i=0;i < MAXLEN; i++) {    	x->tunnel[i].x=0.0f ;    	x->tunnel[i].y=0.0f ;       	x->tunnel[i].rotx=0.0f ;    	x->tunnel[i].roty=0.0f ;    	x->tunnel[i].rotz=0.0f ;    	}	post ("zeroed tunnel i count %ld ", i,0);/*    for(i=0; i < MAXGRID; i++)    	x->inputlist[i]=0.0f;    post("inputlist zeroed  and i is %ld",i,0);*/    n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine    post("registered and set fun",0);			x->on = true;    post("x->on = true",0);        	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x) { n2objfree(x); }void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   if (!variant) n2objsetglfun(x,(method)nnato_glfun);//   else if (variant==1) n2objsetglfun(x,(method)nnato_glfun2);//   else if (variant==2) n2objsetglfun(x,(method)nnato_glfun3);//   else if (variant==3) n2objsetglfun(x,(method)nnato_glfun4);}void nnato_st(NNato *x, float st){ 	x->st   = st; }void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_revert(NNato *x){	    x->normalx = x->normaly = 0.0;    x->normalz = 1.0;     x->tunSize = 10;   	x->ignt=0;    x->step=0;    x->st= 1.0;    x->texture = true;     nnato_klr(x,50,50,50);    }void nnato_linew(NNato *x, float n, int l){	x->linewidth = n;	if (l) x->lsmooth = 1;	if (!l) x->lsmooth = 0;}void nnato_hv_set(NNato *x, float m, float o){	x->ix = m;	x->iy = o;}void nnato_rot_set(NNato *x, float m, float o, float p){	x->irotx = m;	x->iroty = o;	x->irotz = p;}void nnato_hv_only(NNato *x, float m, float o){	//set @ origin	x->tunnel[0].x = m;	x->tunnel[0].y = o;	x->tunnel[0].rotx = x->irotx;	x->tunnel[0].roty = x->iroty;	x->tunnel[0].rotz = x->irotz;}void nnato_hv_do(NNato *x, float m, float o) //set and cycle{	register short i;	//set @ origin	x->tunnel[0].x = m;	x->tunnel[0].y = o;	x->tunnel[0].rotx = x->irotx;	x->tunnel[0].roty = x->iroty;	x->tunnel[0].rotz = x->irotz;	//copy @ fifo tunnel struct	for(i = x->tunSize-1; i>=0; --i){		x->tunnel[i].x = x->tunnel[i+1].x;		x->tunnel[i].y = x->tunnel[i+1].y;		x->tunnel[i].rotx = x->tunnel[i+1].rotx;		x->tunnel[i].roty = x->tunnel[i+1].roty;		x->tunnel[i].rotz = x->tunnel[i+1].rotz;	}}void nnato_hv_rot_do(NNato *x, float m, float o, float rx, float ry, float rz){	//set @ origin	x->tunnel[0].x = m;	x->tunnel[0].y = o;	x->tunnel[0].rotx = rx;	x->tunnel[0].roty = ry;	x->tunnel[0].rotz = rz;}void nnato_hv_fi(NNato *x){	register short i;/*	//set @ origin	x->tunnel[0].x = x->ix;	x->tunnel[0].y = x->iy;	x->tunnel[0].rotx = x->irotx;	x->tunnel[0].roty = x->iroty;	x->tunnel[0].rotz = x->irotz;*/	//copy @ fifo tunnel struct	for(i = x->tunSize-1; i>=0; --i){		x->tunnel[i].x = x->tunnel[i+1].x;		x->tunnel[i].y = x->tunnel[i+1].y;		x->tunnel[i].rotx = x->tunnel[i+1].rotx;		x->tunnel[i].roty = x->tunnel[i+1].roty;		x->tunnel[i].rotz = x->tunnel[i+1].rotz;	}}void nnato_hv_rot(NNato *x, float m, float o, float rx, float ry, float rz){	register short i;	//set @ origin	x->tunnel[0].x = m;	x->tunnel[0].y = o;	x->tunnel[0].rotx = rx;	x->tunnel[0].roty = ry;	x->tunnel[0].rotz = rz;	//copy @ fifo tunnel struct	for(i = x->tunSize-1; i>=0; --i){		x->tunnel[i].x = x->tunnel[i+1].x;		x->tunnel[i].y = x->tunnel[i+1].y;		x->tunnel[i].rotx = x->tunnel[i+1].rotx;		x->tunnel[i].roty = x->tunnel[i+1].roty;		x->tunnel[i].rotz = x->tunnel[i+1].rotz;	}}void nnato_tunSize (NNato *x, long flag) { x->tunSize = flag ; }void nnato_tunReport (NNato *x) {	short i;	post("tunnel report:",0);		post("			tunnel size: %ld",x->tunSize, 0);		for(i =0; i<= x->tunSize-1; i++){	post("			tunnel bit: %ld",i, 0);		post("			tunnel bit.x   : %f",x->tunnel[i].x,0) ;	post("			tunnel bit.y   : %f",x->tunnel[i].y,0) ;	post("			tunnel bit.rotx: %f",x->tunnel[i].rotx,0) ;	post("			tunnel bit.roty: %f",x->tunnel[i].roty,0) ;	post("			tunnel bit.rotz: %f",x->tunnel[i].rotz,0) ;	} }void nnato_glfun(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, tsize = x->tunSize;	float   st = x->st;        if (!x->on) return;    //    glTranslatef(0., 0., -tsize / 2.0 + .5); //center it    glColor3f(x->red, x->green, x->blue);	//color it 	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth); 	    for(i=0;i < tsize; i++) {	 	glBegin(x->type);	 	         		glNormal3f(x->normalx,x->normaly,x->normalz);         glRotatef(x->tunnel[i].rotx, 1, 0, 0); 	//pitch        glRotatef(x->tunnel[i].rotz, 0, 0, 1);        //yaw        glRotatef(x->tunnel[i].roty, 0, 1, 0);			//roll 		 	if (x->texture)	 	   {		        glTexCoord2f(0.0f, 0.0f);	                glVertex3f(-x->tunnel[i].x, -x->tunnel[i].y, st*i);		        glTexCoord2f(1.0f, 0.0f);	               glVertex3f( x->tunnel[i].x, -x->tunnel[i].y, st*i);		        glTexCoord2f(1.0f, 1.0f);	                glVertex3f( x->tunnel[i].x,  x->tunnel[i].y, st*i);		        glTexCoord2f(0.0f, 1.0f);	                glVertex3f(-x->tunnel[i].x,  x->tunnel[i].y, st*i);	 	   }	 	else	 	   {                glVertex3f(-x->tunnel[i].x, -x->tunnel[i].y, st*i);                glVertex3f( x->tunnel[i].x, -x->tunnel[i].y, st*i);                glVertex3f( x->tunnel[i].x,  x->tunnel[i].y, st*i);                glVertex3f(-x->tunnel[i].x,  x->tunnel[i].y, st*i);	       }		glEnd();    }  }     void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			/*----------------------------------------------------------	EOF----------------------------------------------------------*/