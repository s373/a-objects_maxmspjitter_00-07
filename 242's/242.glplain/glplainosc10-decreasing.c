/*deve receber uma lista, escrever essa posicao no espaco*/#include "Movies.h"#include "ext.h"#include "ext_common.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"#include "agl.h"#include "GL/glut.h"//#define MAXVECTOR 256#define MAXVECTOR 64typedef struct zttz {	Object    n_ob; 	void     *n2hdr;    // required nato - position = important _ must b 2nd	    void   	 *glout;    short     type, gridx, gridy, step;    GLfloat   vertex,vertex2,red,green,blue,              normalx,normaly,normalz,              posit[MAXVECTOR][MAXVECTOR];      GLboolean texture;                  Boolean        on, doubl, ignt;  		float		inputlist[MAXVECTOR];	float		st;//	short		listlenght, oplenght;	 long      shademodel;} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_vectinit(NNato *x);void nnato_grid(NNato *x, long gridx, long gridy);void nnato_list(NNato *x, Symbol *s, short argc, Atom *argv);				void nnato_st(NNato *x, float st);void nnato_doubl(NNato *x, long flag);void nnato_ignitv(NNato *x, int flaag, float flag);void nnato_glfun(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void    *nnato_class;main(){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_grid,"grid",A_DEFLONG,A_DEFLONG,0);	addmess((method)nnato_list,"list",A_GIMME, 0);    addmess((method)nnato_st,"stride",A_DEFFLOAT, 0);    addmess((method)nnato_shademodel,"model",A_DEFLONG,0);    addmess((method)nnato_type,"type",A_DEFLONG,0);    addmess((method)nnato_normalz,"normal",A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_doubl,"double",A_DEFLONG,0);    n2reklama();    post("	��� 		242.glplainoscillatoer   ��",0);    n2addfklass("242.glplainosc11");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   strcpy( s, m9ndfukcpropaganda00);       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, n2gltypestr);  // default `gl data type` outlet de[a]ss!zt--	   }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err, i,j;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glplain :  n2initobjekt err = %ld",err); return;}     nnato_revert(x);    post("revert bypassed",0);//    nnato_vectinit(x);	    for(i=0;i < x->gridx; i++)        for(j=0;j < x->gridy; j++){			x->posit[i][j] = 0; 	}			post ("zeroed vector i count %ld j count %ld", i, j,0);/*    for(i=0; i < MAXVECTOR; i++)    	x->inputlist[i]=0.0f;    post("inputlist zeroed  and i is %ld",i,0);*/    n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine    post("registered and set fun",0);			x->on = true;    post("x->on = true",0);        	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){   //	if(x->posit)		    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){//   if (!variant) n2objsetglfun(x,(method)nnato_glfun);}void nnato_st(NNato *x, float st){ 	x->st   = st; }void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_doubl(NNato *x, long flag){    x->doubl = flag;}void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_revert(NNato *x){	    x->normalx = x->normaly = 0.0;    x->normalz = 1.0;            x->gridx=64;    x->gridy=64;	x->ignt=false;    x->step=0;    x->st=0.2;    x->texture = true;    nnato_klr(x,50,50,50);}void nnato_grid(NNato *x, long gridx, long gridy){//	if((gridx<MAXVECTOR&&gridx>0)&&	\	(gridy<MAXVECTOR&&gridy>0)){    x->gridx = gridx;    x->gridy = gridy;    x->step = 0;     nnato_vectinit(x);//lets begin again//    } else {//    x->gridx = 64;//   x->gridy = 64;    //    }}void nnato_vectinit(NNato *x){	short i,j;	    for(i=0;i<x->gridx;i++)        for(j=0;j<x->gridy;j++)		{			x->posit[i][j] = 0.0f; 		}}void nnato_ignitv(NNato *x, int flaag, float flag){    register short i, j;        x->posit[0][flaag]=flag;     if (flaag==x->gridy-1)	{      for(i=1; i<x->gridx; i++)	   for(j=0; j<x->gridy; j++)	{		 x->posit[i][j] = x->posit[i-1][j];	//_copy	next				  }				x->ignt=true;    }    }void nnato_list(NNato *x, Symbol *s, short argc, Atom *argv){	register short 	i, j;	short	listlen, indi, gridy, cpstep, gridx=x->gridx;	float	poslisratio, incremento, a, b, indice, val;		for (i=0; i < argc; i++) {		if (argv[i].a_type==A_LONG) {			x->inputlist[i] = (float)argv[i].a_w.w_long;//divide por 100 se int		}		else if (argv[i].a_type==A_FLOAT) {			x->inputlist[i] = argv[i].a_w.w_float;		}	 }//	}	//	post ("args set in x->inputlist",0);	cpstep = x->step;	// o indice	gridy = x->gridy;	listlen = argc;	//o comprimento da lista de entrada	post ("listlen is %ld",listlen, 0);	poslisratio = (float)gridy / listlen;	post ("poslisratio is %f",poslisratio, 0);		for(j=0; j<gridy; j++){					indice = ((float)j/poslisratio);// valor inteiro			indi = (int)(indice);				// valor arredondado			a = x->inputlist[indi];			b = x->inputlist[indi+1];			incremento = (float) ((b-a)/poslisratio)*((indi+1)-indice);	//acho q vai exponencialment rebentar com a escala em cada unidade														//sempre 0.-1.			val = (float)x->inputlist[indi] + incremento;			if(!x->ignt) {nnato_ignitv(x, j, val);}			else {x->posit[0][j]=val;}	//entra elemento para a posicao 0			}// o elemento da posicao 1 passa para a pos 2, ie cpstep+1->c[step+2// depois o elemnto da pos 1 copia zero			for(i=gridx-1; i>=0; --i)	for(j=0; j<gridy; j++){// 	 for(j=gridy; j>=0; --j)		 x->posit[i+1][j] = x->posit[i][j];	//_copy	next				}	/*		post ("weirdloop comingup", 0);	if (cpstep => 2){	for(i=1; i<cpstep; i++) 	 for(j=0; j<gridy; j++){		 x->posit[cpstep+1][j] = x->posit[cpstep][j];	//_copy	next					}	}	post("weird for\:out",0);		//	post ("for cycle++",0);		 for(j=0; j<gridy; j++)		 x->posit[1][j] = x->posit[0][j];	//_copy	next					if (cpstep==1){ 		 for(j=0; j<gridy; j++){		 x->posit[2][j] = x->posit[1][j];		 }						 for(j=0; j<gridy; j++){		 x->posit[1][j] = x->posit[0][j];	//_copy	next					 }						}*/		if(x->step<=gridx)  x->step+=1;	post("x->step=%ld", x->step,0);	if(x->step==gridx) x->step=0;		post ("and current active step is %ld",cpstep,0);	post ("       next active step is %ld",x->step,0);}void nnato_glfun(n2glatom glatom){    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, gridy=x->gridy, gridx=x->gridx;	float   st=x->st;        if (!x->on) return;        glTranslatef(-st*gridx / 2.0 + .5, -st*gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it    for(i=0;i < gridx;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j <  gridy;j++)            glVertex3f( (float) st*i, (float) st*j, (float) x->posit[i][j]);        glEnd();    }    for(i=0;i < gridx;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j < gridy;j++)            glVertex3f( (float) st*j, (float) st*i, (float) x->posit[j][i]);        glEnd();    }  }     void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			