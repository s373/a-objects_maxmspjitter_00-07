/*fifo 3d points in space lined with arrow on top*/#include "Movies.h"#include "agl.h"//#include "GL/glut.h"#include "glut.h"#include "math.h" //for sqrt, abs#include "ext.h"#include "ext_common.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"//#define MAXGRID 256#define MAXGRID 64 //gl max vector stuff#define MAXLEN 256 //snake max vector pointstypedef struct{	//floating	float x;	float y;	float z;} Snake;typedef struct zttz {	Object    n_ob;	void     *n2hdr;    // required nato - position = important _ must b 2nd		void     *glout;	// out gl fun	short     type, grid, gridy, step;//removed gridy and gridx->grid for vfun compatibility	GLfloat   vertex,vertex2,red,green,blue,	          normalx,normaly,normalz, rate;	GLboolean texture,border, calcFaceNorms, drawFaceNorms, antialias;              	Boolean        on, doubl, ignt;  	float		inputlist[MAXGRID];	float		st;	//	short		listlenght, oplenght;	 long      shademodel;	 short		ponto; //Gpoint	 Boolean   	On_Orbit;	 short		camx, camy; //look@coords	 float		camz;	 short		wirex, wirey;	 float		linewidth;	 Boolean	lsmooth;	 float		cdist;	Snake	mar[MAXLEN], mar2[MAXLEN], mar3[MAXLEN], delta[MAXLEN];	short		marCount, marSize;	double	interp, zfact, arrowsize;} NNato;//global carve stuff//float	    posit[MAXGRID][MAXGRID];			        //positionfloat	    posit[MAXGRID][MAXGRID];			        //positionfloat	    vertNorms[MAXGRID][MAXGRID][3];				//normalsfloat	    faceNorms[2][MAXGRID][MAXGRID][3];  		//facenormalsfloat	    faceNormSegs[2][2][MAXGRID][MAXGRID][3];	//facenormal segmentsfloat 		texCoords[MAXGRID][MAXGRID][2];				//texture coordinates//vector void copy(float vec0[3], float vec1[3]);void sub(float vec0[3], float vec1[3], float vec2[3]);void add(float vec0[3], float vec1[3], float vec2[3]);void scalDiv(float vec[3], float c);void cross(float vec0[3], float vec1[3], float vec2[3]);void norm(float vec[3]);void set(float vec[3], float x, float y, float z);void getFaceNorms(NNato *x);void getVertNorms(NNato *x);void getFaceNormSegs(NNato *x);void getTexCoords(NNato *x);void drawFaceNormals(NNato *x);void nnato_drawFaceNorms(NNato *x, long flag);void nnato_calcFaceNorms(NNato *x, long flag);void nnato_antialias(NNato *x, long flag);void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_vectinit(NNato *x);void nnato_grid(NNato *x, long grid, long gridy);void nnato_list(NNato *x, Symbol *s, short argc, Atom *argv);void nnato_float(NNato *x,double flag);				void nnato_st(NNato *x, float st);void nnato_doubl(NNato *x, long flag);void nnato_ignitv(NNato *x, int flaag, float flag);void nnato_shifth(NNato *x, int amount);void nnato_shifthh(NNato *x, int amount);void nnato_rate(NNato *x, float n);void nnato_border(NNato *x, long flag);void nnato_xyzdump(NNato *x);void nnato_xyzstore(NNato *x);void nnato_xyzinterp(NNato *x, double f);void nnato_zfact(NNato *x, double f);void draw_arrow_xy(NNato *x,double fx, double fy);void draw_arrow_size(NNato *x, double f);void nnato_point(NNato *x, long flag);void nnato_pass(NNato *x);void 	nnato_On_Orbit 		(NNato *x, int val, int vala);void 	nnato_camz 		(NNato *x, float val);void nnato_wire (NNato *x, long flag);void nnato_linew(NNato *x, float n, int l);void nnato_cdist(NNato *x, float m );void nnato_xyz(NNato *x, float m, float o, float p );void nnato_marSize (NNato *x, long flag);//void nnato_wire (NNato *x, long flag);void nnato_glfun(n2glatom glatom);void nnato_glfun2(n2glatom glatom);		//void nnato_glfun3(n2glatom glatom);void nnato_glfun4(n2glatom glatom);void nnato_glfun5(n2glatom glatom);void nnato_glfun6(n2glatom glatom);void nnato_glfun7(n2glatom glatom);void nnato_glfun8(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void    *nnato_class;void main(				void		){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_grid,"grid",A_DEFLONG,A_DEFLONG,0);	addmess((method)nnato_list,"list",A_GIMME, 0);	addfloat((method)nnato_float);    addmess((method)nnato_st,"stride",A_DEFFLOAT, 0);    addmess((method)nnato_shademodel,"model",A_DEFLONG,0);    addmess((method)nnato_type,"type",A_DEFLONG,0);    addmess((method)nnato_normalz,"normal",A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_shifth,"shift.left",0);    addmess((method)nnato_shifthh,"shift.right",0);    addmess((method)nnato_shifthh,"rate",A_DEFFLOAT,0);    addmess((method)nnato_border,"border",A_DEFLONG,0);    addmess((method)nnato_drawFaceNorms,"face_norms",A_DEFLONG,0);    addmess((method)nnato_calcFaceNorms,"calc_face_norms",A_DEFLONG,0);    addmess((method)nnato_antialias,"antialias",A_DEFLONG,0);    addmess((method)nnato_point,"point",A_DEFLONG,0);    addmess((method)nnato_pass, "pass",0);    addmess((method)nnato_On_Orbit,"orbit",A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_camz,"cam.z",A_DEFFLOAT, 0);    addmess((method)nnato_wire,"wier",A_DEFLONG,0);    addmess((method)nnato_linew,"linew",A_DEFFLOAT,A_DEFLONG, 0);    addmess((method)nnato_cdist,"dist",A_DEFFLOAT, 0);    addmess((method)nnato_xyz,"xyz",A_DEFFLOAT,A_DEFFLOAT, A_DEFFLOAT, 0);    addmess((method)nnato_marSize,"snake.size",A_DEFLONG,0);    addmess((method)nnato_xyzdump,"xyzdump",0);    addmess((method)nnato_xyzstore,"xyzstore",0);    addmess((method)nnato_xyzinterp,"xyzinterp",A_DEFFLOAT, 0);    addmess((method)nnato_zfact,"zfact",A_DEFFLOAT, 0);    addmess((method)draw_arrow_size,"arrow.size",A_DEFFLOAT, 0);    post("�		Eastern Bunny.glsnake"__DATE__,0);    n2addfklass("242.glterrain04");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      	   strcpy( s, "gldata, commands");       {	   if (msg == 2)   	      if (nr == 0)	         strcpy( s, "gldata");  		}}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err, i,j;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glplain :  n2initobjekt err = %ld",err); return;} 	x->ignt=0;    nnato_revert(x);    post("revert bypassed",0);//    nnato_vectinit(x);	    for(i=0;i < x->grid; i++)        for(j=0;j < x->grid; j++){			posit[i][j] = 0.0f; 	}			post ("zeroed vector i count %ld j count %ld", i, j,0);    for(i=0;i < MAXLEN; i++) {    	x->mar[i].x=0.0f ;    	x->mar[i].y=0.0f ;       	x->mar[i].z=0.0f ;    	}	post ("zeroed snake i count %ld ", i,0);/*    for(i=0; i < MAXGRID; i++)    	x->inputlist[i]=0.0f;    post("inputlist zeroed  and i is %ld",i,0);*/    n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine    post("registered and set fun",0);			x->on = true;    post("x->on = true",0);        	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){   //	if(posit)		    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   if (!variant) n2objsetglfun(x,(method)nnato_glfun);   else if (variant==1) n2objsetglfun(x,(method)nnato_glfun2);   else if (variant==2) n2objsetglfun(x,(method)nnato_glfun3);   else if (variant==3) n2objsetglfun(x,(method)nnato_glfun4);   else if (variant==4 ) n2objsetglfun(x,(method)nnato_glfun5);   else if (variant==5) n2objsetglfun(x,(method)nnato_glfun6);   else if (variant==6 ) n2objsetglfun(x,(method)nnato_glfun7);   else if (variant==7) n2objsetglfun(x,(method)nnato_glfun8);}void nnato_st(NNato *x, float st){ 	x->st   = st; }void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_point(NNato *x, long flag){    x->ponto = flag;}void nnato_pass(NNato *x){ 	short p = x->ponto, i, j, grid=x->grid;   	for(i=grid-1; i>=p; --i)//	for(i=p; i>=0; --i)	for(j=grid; j>=0; --j){		 posit[i+1][j] = posit[i][j];	//_copy	next		}	}void 	nnato_On_Orbit 		(NNato *x, int val, int vala){	if (val) x->On_Orbit = 1 ;	else if (!val) { x->On_Orbit = 0; return; }		x->camx = val; x->camy = vala;}void 	nnato_camz 		(NNato *x, float val){	x->camz = val;}void nnato_wire (NNato *x, long flag){	if           (flag < 1)  { x->wirex = 1; x->wirey = 0; }	else if (flag == 1) { x->wirex = 0; x->wirey = 1; }	else if (flag  > 1)   { x->wirex = 1; x->wirey = 1; }}void nnato_linew(NNato *x, float n, int l){	x->linewidth = n;	if (l) x->lsmooth = 1;	if (!l) x->lsmooth = 0;}void nnato_xyz(NNato *x, float m, float o, float p ){	short i, range = x->marSize;	x->mar[1].x 	= 	x->mar[0].x ;	x->mar[1].y 	=	 x->mar[0].y ;	x->mar[1].z 	=	 x->mar[0].z ;		x->mar[0].x 	= m;	x->mar[0].y 	= o;	x->mar[0].z 	= p;	//increment position to write on//	x->marCount+=1;//	if(x->marCount > (x->marSize-1)) x->marCount = 0;	for (i=(range-1); i>=1; i--) {	x->mar[i+1].x 	= 	x->mar[i+0].x ;	x->mar[i+1].y 	=	 x->mar[i+0].y ;	x->mar[i+1].z 	=	 x->mar[i+0].z ;		}}void nnato_xyzdump(NNato *x){	short i, range = x->marSize;	post("xyz � � ��������������������",0);	post("xyz  range (sanke.size = %ld", range, 0);		post("xyz � � ��������������������",0);	for (i=1; i<range; i++) {		post("    xyz \(%ld\)   x= %f     y=%f     z=%f", 	i,		x->mar[i].x ,	 			x->mar[i].y ,		 		x->mar[i].z , 0);				}	for (i=1; i<range; i++) {		post("    xyz2 \(%ld\)   x= %f     y=%f     z=%f", 	i,		x->mar2[i].x ,	 			x->mar2[i].y ,		 		x->mar2[i].z , 0);				}	for (i=1; i<range; i++) {		post("    xyz3 \(%ld\)   x= %f     y=%f     z=%f", 	i,		x->mar3[i].x ,	 			x->mar3[i].y ,		 		x->mar3[i].z , 0);				}	for (i=1; i<range; i++) {		post("    xyzdelta \(%ld\)   x= %f     y=%f     z=%f", 	i,		x->delta[i].x ,	 			x->delta[i].y ,		 		x->delta[i].z , 0);				}}void nnato_xyzstore(NNato *x){		short 	i;		for(i=0; i<x->marSize; i++){								x->mar3[i].x = x->mar2[i].x;		x->mar3[i].y = x->mar2[i].y;		x->mar3[i].z = x->mar2[i].z;		}							for(i=0; i<x->marSize; i++){								x->mar2[i].x = x->mar[i].x;		x->mar2[i].y = x->mar[i].y;		x->mar2[i].z = x->mar[i].z;		}							for(i=0; i<x->marSize; i++){							x->delta[i].x=	x->mar3[i].x - x->mar2[i].x;	x->delta[i].y=	x->mar3[i].y - x->mar2[i].y;	x->delta[i].z=	x->mar3[i].z - x->mar2[i].z;		}						}void nnato_xyzinterp(NNato *x, double f){	short		i;	//	for(i=0; i<x->marSize; i++){						//		x->mar[i].x = x->delta[i].x * f;//		x->mar[i].y = x->delta[i].y * f;//		x->mar[i].z = x->delta[i].z * f;//		}							for(i=0; i<x->marSize; i++){								x->mar[i].x = (x->mar3[i].x * f -  x->mar2[i].x*(1.0-f));		x->mar[i].y = (x->mar3[i].y * f -  x->mar2[i].y*(1.0-f));		x->mar[i].z = (i*x->zfact);		}						}void nnato_zfact(NNato *x, double f) { x->zfact = f; }void draw_arrow_xy(NNato *x,double fx, double fy){	double size = x->arrowsize, hsz = size/2.;	glBegin(x->type);		glVertex3f (fx, fy - hsz,0-x->mar[0].z);	glVertex3f (fx-size, fy - hsz,size -x->mar[0].z);	glVertex3f (fx+size, fy - hsz,size -x->mar[0].z);	glVertex3f (fx, fy- hsz,0 -x->mar[0].z);	glVertex3f (fx, fy+hsz,0 -x->mar[0].z);	glVertex3f (fx-size, fy+hsz,size -x->mar[0].z);	glVertex3f (fx+size, fy+hsz,size -x->mar[0].z);	glVertex3f (fx, fy+hsz,0-x->mar[0].z);			glEnd();}void draw_arrow_size(NNato *x, double f){ x->arrowsize = f; }void nnato_marSize (NNato *x, long flag) { x->marSize = flag ; }void nnato_cdist		(NNato *x, float m) { x->cdist = m ; }void nnato_doubl(NNato *x, long flag){    x->doubl = flag;}void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_border(NNato *x, long flag){        x->border = flag;}  void nnato_revert(NNato *x){	    x->normalx = x->normaly = 0.0;    x->normalz = 1.0;            nnato_grid(x,64/4, 64/4);//    x->grid=64;//    x->grid=128;  //  x->grid=128;	x->ignt=0;    x->step=0;    x->st=0.2;    x->texture = true;    x->rate=0.;        x->border=0;    nnato_klr(x,50,50,50);        x->calcFaceNorms = 1;    x->drawFaceNorms = 1;    x->antialias = 0;    x->ponto = 64/6;	x->On_Orbit = 0;	x->camx = x->camy = 0; //look@coords	x->camz = 0.6;	nnato_wire(x,2); //default wireframe both x,y	x->linewidth = 1.0;	x->cdist = 0.0f;	x->marCount =0;	x->marSize =20;	x->zfact = 1.0;	x->arrowsize = 0.3;}void nnato_grid(NNato *x, long grid, long gridy){	if((grid<MAXGRID&&grid>0))		x->grid = grid;	if((gridy<MAXGRID&&gridy>0))		x->gridy = gridy;	getTexCoords(x);	//refresh space}void nnato_vectinit(NNato *x){	short i,j;	    for(i=0;i<x->grid;i++)        for(j=0;j<x->grid;j++)		{			posit[i][j] = 0.0f; 		}}void nnato_rate(NNato *x, float n){        int cand=0, i, j;        x->rate = n ;//* 0.01;          for(i=1; i<x->grid; i++){	   for(j=0; j<x->grid; j++)	{	   	cand = (i+j)%2;	   	if (!cand) posit[i][j] += (posit[i][j]*x->rate);		//even numbrs	   	else if (cand==1)		 posit[i][j] -= (posit[i][j]*x->rate);				//odd numbrs		}  	}  }  void nnato_ignitv(NNato *x, int flaag, float flag){    register short i, j;        posit[0][flaag]=flag;     if (flaag==x->grid-1)	{      for(i=1; i<x->grid; i++){	   for(j=0; j<x->grid; j++)	{		 posit[i][j] = posit[i-1][j];	//_copy	next			//		 posit[i][j] = 0.01f;	//just zero the vector out				  		}		   	   }			x->ignt=true;		post("x->ignt=%ld", x->ignt,0);  	 	}}void nnato_list(NNato *x, Symbol *s, short argc, Atom *argv){	//parse incoming triplets into fifo spacial points		register short 	i, j;	short	indi, grid=x->grid, ponto = x->ponto;	double	poslisratio, incremento, a, b, indice, val; //typecasted double	//	if (argc > 64) return; //we can fix this now	for (i=0; i < argc; i++) {		if (argv[i].a_type==A_LONG) {			x->inputlist[i] = (float)argv[i].a_w.w_long * 0.01;//divide por 100 se int		}		else if (argv[i].a_type==A_FLOAT) {			x->inputlist[i] = argv[i].a_w.w_float;		}	 }//	if (argc == 64) goto stemall;	poslisratio = (double)grid / argc;		for(j=0; j<grid; j++){			indice = (double)(j/poslisratio);// valor inteiro			indi = (int)(indice);		// valor arredondado			a = x->inputlist[indi];			b = x->inputlist[indi+1];			incremento = (double) (((-b+a)/poslisratio)*((indi)-indice));				val = (float)x->inputlist[indi] + incremento;			posit[0][j]=val;	//entra elemento para a posicao 0		}						// o elemento da posicao 1 passa para a pos 2, ie cpstep+1->c[step+2// depois o elemnto da pos 1 copia zero//	dist = abs(ponto-(grid-1));//	for(i=grid-1; i>=0; --i)	for(i=ponto; i>=0; --i)	for(j=grid; j>=0; --j){		 posit[i+1][j] = posit[i][j];	//_copy	next		}		   	return;}void nnato_float(NNato *x,double flag){	register short 	i, j;	short grid=x->grid, ponto=x->ponto;	for(j=0; j<grid; j++){				posit[0][j]=(float)flag;	//entra elemento para a posicao 0		}//	for(i=grid-1; i>=0; --i)	for(i=ponto; i>=0; --i)	for(j=grid; j>=0; --j){		 posit[i+1][j] = posit[i][j];	//_copy	next	}}void nnato_shifth(NNato *x, int amount){	register short i, j;//	short	a1,b1;	//	if (amount){	for(i=x->grid-1; i>=0; --i)	for(j=x->grid-1; j>=0; --j){//		a1=amount+j;//		b1=a1%j;//		 posit[i][b1] = posit[i+1][j];	//not 					//		 posit[i][j+1] = posit[i+1][j];	//shift 1 left					 posit[i+1][j+1] = posit[i][j];	//shift 1 left				  }	  // }}void nnato_shifthh(NNato *x, int amount){	register short i, j;	short	a1,b1;	//	if (amount){	for(i=x->grid-1; i>=0; --i)	for(j=0; j<x->grid-1; j++){//		a1=amount+j;//		b1=a1%j;//		 posit[i][b1] = posit[i+1][j];	//not 							 posit[i+1][j] = posit[i][j+1];	//shift 1 left				  }	//   }}void nnato_glfun(n2glatom glatom) // draw wireframe no normals{    NNato *x = (NNato *)n2getglatomobj(glatom);	short		i, j, grid=x->grid, gridy=x->gridy;	float   	st = x->st;        if (!x->on) return;        if (x->antialias)    {        glEnable(GL_BLEND);        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);        glEnable(GL_LINE_SMOOTH);        glLineWidth(1.5);    }    else    {        glDisable(GL_BLEND);        glDisable(GL_LINE_SMOOTH);        glLineWidth(1.0);    }/*   if (x->On_Orbit) {      gluLookAt(x->camx,x->camy,x->camz,         0.0,0.0,0.0,         0.0,1.0,0.0);   } else {      gluLookAt(0. , 0. , 0. ,         0.0,0.0,0.0,         0.0,1.0,0.0);   }*///    glTranslatef(-st*grid / 2.0 + .5, -st*grid / 2.0 + .5, 0); //center it    glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light/* //dont calc normas    if (x->calcFaceNorms) {    getFaceNorms(x);    getVertNorms(x);	}*/		if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);    glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);    for(i=0;i < x->marSize;i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	                  glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  x->mar[i].z  ) ;          glEnd();        }}     void nnato_glfun2(n2glatom glatom) // draw wireframe master normals{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid, gridy=x->gridy;	float   st=x->st;        if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);	draw_arrow_xy(x,  x->mar[0].x,  x->mar[0].y);	    for(i=0;i < (x->marSize -1);i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	                  glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  i*x->zfact  ) ;          glVertex3f( (float) x->mar[i+1].x,           		  (float)  x->mar[i+1].y,           		  (float)  (i+1)*x->zfact  ) ;          glEnd();        }}      void nnato_glfun3(n2glatom glatom) // draw wireframe vector normals{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid, gridy=x->gridy;	float   st=x->st, tx, ty, tz;        if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);    for(i=0;i < x->marSize ;i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	///radiosity control        	tx = x->mar[i].x - x->cdist;	ty = x->mar[i].y - x->cdist;	tz = x->mar[i].z - x->cdist;	          glVertex3f( (float) 	0.0f + tx,           		  (float)  	0.0f + ty,           		  (float)  	0.0f  + tz) ;          glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  x->mar[i].z  ) ;          glEnd();        }}  void nnato_glfun4(n2glatom glatom)	//draew textured{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid, gridy=x->gridy;	float   st=x->st;        if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);    for(i=0;i < x->marSize -1;i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	///radiosity control        	 	glNormal3f(x->normalx,x->normaly,x->normalz); 	if (x->texture) 	   {	        glTexCoord2f(0.0f, 0.0f);                glVertex3f( - x->mar[i+1].x, -x->mar[i+1].y, x->mar[i+1].z);	        glTexCoord2f(1.0f, 0.0f);                glVertex3f( x->mar[i+1].x, -x->mar[i+1].y, x->mar[i+1].z);	        glTexCoord2f(1.0f, 1.0f);                glVertex3f( x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z);	        glTexCoord2f(0.0f, 1.0f);                glVertex3f(-x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z); 	   } 	else 	   {                glVertex3f( - x->mar[i+1].x, -x->mar[i+1].y, x->mar[i+1].z);                glVertex3f( x->mar[i+1].x, -x->mar[i+1].y, x->mar[i+1].z);                glVertex3f( x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z);                glVertex3f(-x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z);                     }	glEnd();}  }     void nnato_glfun5(n2glatom glatom) // { drawSmoothShaded(x); }{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid, gridy=x->gridy;	float   st=x->st;        if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);    for(i=0;i < x->marSize -1;i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	///radiosity control        	 	glNormal3f(x->normalx,x->normaly,x->normalz); 	if (x->texture) 	   {	        glTexCoord2f(0.0f, 0.0f);                glVertex3f( 0.0f, 0.0f, 0.0f);	        glTexCoord2f(1.0f, 0.0f);                glVertex3f( x->mar[i+1].x, -x->mar[i+1].y, x->mar[i+1].z);	        glTexCoord2f(1.0f, 1.0f);                glVertex3f( x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z);	        glTexCoord2f(0.0f, 1.0f);                glVertex3f(-x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z); 	   } 	else 	   {                glVertex3f( 0.0f, 0.0f, 0.0f);                glVertex3f( x->mar[i+1].x, -x->mar[i+1].y, x->mar[i+1].z);                glVertex3f( x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z);                glVertex3f(-x->mar[i+1].x, x->mar[i+1].y, x->mar[i+1].z);                     }	glEnd();}  }void nnato_glfun6(n2glatom glatom) // //    drawFlatShaded(x); }{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid, gridy=x->gridy;	float   st=x->st;        if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);    for(i=0;i < (x->marSize);i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	                  glVertex3f( (float) 0.,           		  (float)  0.,           		  (float)  0.  ) ;          glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  x->mar[i].z  ) ;          glEnd();        }}  void nnato_glfun7(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid, gridy=x->gridy;	float   st=x->st;        if (!x->on) return;        glShadeModel(x->shademodel); //added shademodel//    glTranslatef(-grid / 2.0 + .5, -gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it, ha, material response to light	if(x->lsmooth) glEnable(GL_LINE_SMOOTH);	if(!x->lsmooth) glDisable(GL_LINE_SMOOTH);	glLineWidth(x->linewidth);	if(x->lsmooth) glEnable(GL_POINT_SMOOTH);	if(!x->lsmooth) glDisable(GL_POINT_SMOOTH);    glPointSize(x->cdist);    for(i=0;i < (x->marSize);i++) { //   	 glBegin(GL_LINE_STRIP);	 	glBegin(x->type);	 	                  glVertex3f( (float) 0.,           		  (float)  0.,           		  (float)  0.  ) ;          glVertex3f( (float) x->mar[i].x,           		  (float)  x->mar[i].y,           		  (float)  x->mar[i].z  ) ;          glVertex3f( (float) 0.,           		  (float)  0.,           		  (float)  0.  ) ;          glVertex3f( (float) x->mar[i+1].x,           		  (float)  x->mar[i+1].y,           		  (float)  x->mar[i+1].z  ) ;          glEnd();        }}  void nnato_glfun8(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, grid=x->grid;	float   st=x->st;        if (!x->on) return;        if (x->antialias)    {        glEnable(GL_BLEND);        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);        glEnable(GL_LINE_SMOOTH);        glLineWidth(1.5);    }    else    {        glDisable(GL_BLEND);        glDisable(GL_LINE_SMOOTH);        glLineWidth(1.0);    }   if (x->On_Orbit) {      gluLookAt(x->camx,x->camy,x->camz,         0.0,0.0,0.0,         0.0,1.0,0.0);   } else {      gluLookAt(0. , 0. , 0. ,         0.0,0.0,0.0,         0.0,1.0,0.0);   }//    glTranslatef(-st*grid / 2.0 + .5, -st*grid / 2.0 + .5, 0); //center it    glTranslatef(-grid / 2.0 + .5, -grid / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it    if (x->calcFaceNorms) {    getFaceNorms(x);    getVertNorms(x);	}/*	    for(i=0;i < grid;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j <  grid;j++)//            glVertex3f( (float) st*i, (float) st*j, (float) posit[i][j]);          glVertex3f( (float) i, (float) j, (float) posit[i][j]);          glEnd();    }*/    for(i=0;i < grid;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j < grid;j++)            glVertex3f( (float) j, (float) i, (float) posit[j][i]);        glEnd();    }    if (x->drawFaceNorms)        {        getFaceNormSegs(x);        drawFaceNormals(x);    }/*    if (x->antialias)    {        glDisable(GL_BLEND);        glDisable(GL_LINE_SMOOTH);        glLineWidth(1.0);    }*/  } void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			/* face normals - for flat shading */void getFaceNorms(NNato *x){	int i,j;    float vec0[3], vec1[3], vec2[3], norm0[3], norm1[3];    float geom0[3], geom1[3], geom2[3], geom3[3];    for (i = 0; i < x->grid-1; ++i)    {        for (j = 0; j < x->gridy-1; ++j)        {            /* get vectors from geometry points */            geom0[0] = i; geom0[1] = j; geom0[2] = posit[i][j];            geom1[0] = i; geom1[1] = j+1; geom1[2] = posit[i][j+1];            geom2[0] = i+1; geom2[1] = j; geom2[2] = posit[i+1][j];            geom3[0] = i+1; geom3[1] = j+1; geom3[2] = posit[i+1][j+1];            sub( vec0, geom1, geom0 );            sub( vec1, geom1, geom2 );            sub( vec2, geom1, geom3 );            /* get triangle face normals from vectors & normalize them */            cross( norm0, vec0, vec1 );            norm( norm0 );            cross( norm1, vec1, vec2 );             norm( norm1 );            copy( faceNorms[0][i][j], norm0 );            copy( faceNorms[1][i][j], norm1 );        }    }}/* vertex normals - average of face normals for smooth shading */void getVertNorms(NNato *x){	int i,j;    float avg[3];    for (i = 0; i < x->grid; ++i)    {        for (j = 0; j < x->gridy; ++j)        {            /* For each vertex, average normals from all faces sharing */            /* vertex.  Check each quadrant in turn */            set(avg, 0.0, 0.0, 0.0);            /* Right & above */            if (j < x->grid-1 && i < x->grid-1)            {                add( avg, avg, faceNorms[0][i][j] );            }            /* Right & below */            if (j < x->grid-1 && i > 0)            {                add( avg, avg, faceNorms[0][i-1][j] );                add( avg, avg, faceNorms[1][i-1][j] );            }            /* Left & above */            if (j > 0 && i < x->grid-1)            {                add( avg, avg, faceNorms[0][i][j-1] );                add( avg, avg, faceNorms[1][i][j-1] );            }            /* Left & below */            if (j > 0 && i > 0)            {                add( avg, avg, faceNorms[1][i-1][j-1] );            }            /* Normalize */            norm( avg );            copy( vertNorms[i][j], avg );        }    }}void getFaceNormSegs(NNato *x){	int i,j;    float center0[3], center1[3], normSeg0[3], normSeg1[3];    float geom0[3], geom1[3], geom2[3], geom3[3];     for (i = 0; i < x->grid - 1; ++i)    {        for (j = 0; j < x->gridy - 1; ++j)        {            geom0[0] = i; geom0[1] = j; geom0[2] = posit[i][j];            geom1[0] = i; geom1[1] = j+1; geom1[2] = posit[i][j+1];            geom2[0] = i+1; geom2[1] = j; geom2[2] = posit[i+1][j];            geom3[0] = i+1; geom3[1] = j+1; geom3[2] = posit[i+1][j+1];            /* find center of triangle face by averaging three vertices */            add( center0, geom2, geom0 );            add( center0, center0, geom1 );            scalDiv( center0, 3.0 );            add( center1, geom2, geom1 );            add( center1, center1, geom3 );            scalDiv( center1, 3.0 );            /* translate normal to center of triangle face to get normal segment */            add( normSeg0, center0, faceNorms[0][i][j] );            add( normSeg1, center1, faceNorms[1][i][j] );            copy( faceNormSegs[0][0][i][j], center0 );            copy( faceNormSegs[1][0][i][j], center1 );            copy( faceNormSegs[0][1][i][j], normSeg0 );            copy( faceNormSegs[1][1][i][j], normSeg1 );        }    }}void getTexCoords(NNato *x){	int i,j;    for (i = 0; i < x->grid; ++i)    {        for (j = 0; j < x->gridy; ++j)        {            texCoords[i][j][0] = (float)j/(float)(x->gridy-1);            texCoords[i][j][1] = (float)i/(float)(x->grid-1);        }    }}void drawFaceNormals(NNato *x){    int i,j;    glColor3f(1.0,1.0,1.0);    for (i = 0; i < x->grid - 1; ++i)    {        for (j = 0; j < x->gridy - 1; ++j)        {            glBegin(GL_LINES);            glVertex3fv(faceNormSegs[0][0][i][j]);            glVertex3fv(faceNormSegs[0][1][i][j]);            glEnd();            glBegin(GL_LINES);            glVertex3fv(faceNormSegs[1][0][i][j]);            glVertex3fv(faceNormSegs[1][1][i][j]);            glEnd();        }    }}void nnato_drawFaceNorms(NNato *x, long flag){        x->drawFaceNorms = flag;}  void nnato_calcFaceNorms(NNato *x, long flag){        x->calcFaceNorms = flag;}  void nnato_antialias(NNato *x, long flag){        x->antialias = flag;}  // mathvoid copy(float vec0[3], float vec1[3]){    vec0[0] = vec1[0];    vec0[1] = vec1[1];    vec0[2] = vec1[2];}void sub(float vec0[3], float vec1[3], float vec2[3]){    vec0[0] = vec1[0] - vec2[0];    vec0[1] = vec1[1] - vec2[1];    vec0[2] = vec1[2] - vec2[2];}void add(float vec0[3], float vec1[3], float vec2[3]){    vec0[0] = vec1[0] + vec2[0];    vec0[1] = vec1[1] + vec2[1];    vec0[2] = vec1[2] + vec2[2];}void scalDiv(float vec[3], float c){    vec[0] /= c; vec[1] /= c; vec[2] /= c;}void cross(float vec0[3], float vec1[3], float vec2[3]){    vec0[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];    vec0[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];    vec0[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];}void norm(float vec[3]){    float c = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);    scalDiv(vec, c); }void set(float vec[3], float x, float y, float z){    vec[0] = x;    vec[1] = y;    vec[2] = z;}/*----------------------------------------------------------	EOF----------------------------------------------------------*/