/*deve receber uma lista, escrever essa posicao no espacooscilar o resto da estrutura*/#include "Movies.h"#include "ext.h"#include "ext_common.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"#include "agl.h"#include "GL/glut.h"//#define MAXVECTOR 256#define MAXVECTOR 64typedef struct zttz {	Object    n_ob; 	void     *n2hdr;    // required nato - position = important _ must b 2nd	    void   	 *glout;    short     type, gridx, gridy, step;    GLfloat   vertex,vertex2,red,green,blue,              normalx,normaly,normalz, rate;    float     posit[MAXVECTOR][MAXVECTOR];      GLboolean texture,border;                  Boolean        on, doubl, ignt;  		float		inputlist[MAXVECTOR];	float		st;//	short		listlenght, oplenght;	 long      shademodel;} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_on(NNato *x, long flag);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_type(NNato *x, short type);void nnato_shademodel(NNato *x, long type);void nnato_klr(NNato *x, long red, long green, long blue);void nnato_normalz(NNato *x, long koordx, long koordy, long koordz);void nnato_texture(NNato *x, long flag);void nnato_revertgeo(NNato *x);void nnato_revert(NNato *x);void nnato_vectinit(NNato *x);void nnato_grid(NNato *x, long gridx, long gridy);void nnato_list(NNato *x, Symbol *s, short argc, Atom *argv);void nnato_float(NNato *x,double flag);				void nnato_st(NNato *x, float st);void nnato_doubl(NNato *x, long flag);void nnato_ignitv(NNato *x, int flaag, float flag);void nnato_shifth(NNato *x, int amount);void nnato_shifthh(NNato *x, int amount);void nnato_rate(NNato *x, float n);void nnato_border(NNato *x, long flag);void nnato_glfun(n2glatom glatom);void nnato_glfun2(n2glatom glatom);		//void nnato_glfun3(n2glatom glatom);void nnato_glfun4(n2glatom glatom);void nnato_glregister(NNato *x, void *klientz);void    *nnato_class;main(){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 0L, 0); 	addmess((method)nnato_assist,	                  "assist",          	A_CANT,0); 	addmess((method)nnato_info,	                      "info",		        A_CANT,0);    n2objaddglcmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,-1,-1,-1,-1,(method)nnato_variant,0);    addmess((method)nnato_klr,                        "color",              A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_texture,                    "texture",            A_DEFLONG,0);    addmess((method)nnato_revert,                     "revert",             0);    addmess((method)nnato_grid,"grid",A_DEFLONG,A_DEFLONG,0);	addmess((method)nnato_list,"list",A_GIMME, 0);	addfloat((method)nnato_float);    addmess((method)nnato_st,"stride",A_DEFFLOAT, 0);    addmess((method)nnato_shademodel,"model",A_DEFLONG,0);    addmess((method)nnato_type,"type",A_DEFLONG,0);    addmess((method)nnato_normalz,"normal",A_DEFLONG,A_DEFLONG,A_DEFLONG,0);    addmess((method)nnato_shifth,"shift.left",0);    addmess((method)nnato_shifthh,"shift.right",0);    addmess((method)nnato_shifthh,"rate",A_DEFFLOAT,0);    addmess((method)nnato_border,"border",A_DEFLONG,0);    n2reklama();    post("�		Eastern Bunny",0);    n2addfklass("242.glterrain02");	}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   strcpy( s, m9ndfukcpropaganda00);       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, n2gltypestr);  // default `gl data type` outlet de[a]ss!zt--	   }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(){    NNato      *x;	short      err, i,j;	x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->glout);        if (err = n2objinit(x))       { post("242.glplain :  n2initobjekt err = %ld",err); return;} 	x->ignt=0;    nnato_revert(x);    post("revert bypassed",0);//    nnato_vectinit(x);	    for(i=0;i < x->gridx; i++)        for(j=0;j < x->gridy; j++){			x->posit[i][j] = 0.0f; 	}			post ("zeroed vector i count %ld j count %ld", i, j,0);/*    for(i=0; i < MAXVECTOR; i++)    	x->inputlist[i]=0.0f;    post("inputlist zeroed  and i is %ld",i,0);*/    n2objsetglregister(x,(method)nnato_glregister);   // zpecify gl regizter routine    n2objsetglfun(x,(method)nnato_glfun);             // zpecify gl drau routine    post("registered and set fun",0);			x->on = true;    post("x->on = true",0);        	return (x);   ikk:	nnato_free(x);}void nnato_free(NNato *x){   //	if(x->posit)		    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}  void nnato_int(NNato *x, long n)    {								       if (!n) x->on = false;   else x->on = true;}void nnato_bang(NNato *x){   n2gldraw(x, n2objtype);}void nnato_variant(NNato *x, short variant){   if (!variant) n2objsetglfun(x,(method)nnato_glfun);   else if (variant==1) n2objsetglfun(x,(method)nnato_glfun2);   else if (variant==2) n2objsetglfun(x,(method)nnato_glfun3);   else if (variant==3) n2objsetglfun(x,(method)nnato_glfun4);}void nnato_st(NNato *x, float st){ 	x->st   = st; }void nnato_type(NNato *x, short type){   if (type >= 0 && type <= 9) x->type = type;}void nnato_klr(NNato *x, long red, long green, long blue){   x->red   = (float)red   * 0.01;   x->green = (float)green * 0.01;   x->blue  = (float)blue  * 0.01;      }  void nnato_normalz(NNato *x, long koordx, long koordy, long koordz){   x->normalx  = (float)koordx * 0.01;   x->normaly  = (float)koordy * 0.01;   x->normalz  = (float)koordz * 0.01;      }  void nnato_shademodel(NNato *x, long type){   if (type == 0) x->shademodel = GLU_SMOOTH;   else if (type == 1) x->shademodel = GLU_FLAT;   else if (type == 2) x->shademodel = GLU_NONE;}void nnato_doubl(NNato *x, long flag){    x->doubl = flag;}void nnato_texture(NNato *x, long flag){    x->texture = flag;}void nnato_border(NNato *x, long flag){        x->border = flag;}  void nnato_revert(NNato *x){	    x->normalx = x->normaly = 0.0;    x->normalz = 1.0;            x->gridx=64;    x->gridy=64;//    x->gridx=128;  //  x->gridy=128;	x->ignt=0;    x->step=0;    x->st=0.2;    x->texture = true;    x->rate=0.;        x->border=0;    nnato_klr(x,50,50,50);}void nnato_grid(NNato *x, long gridx, long gridy){	if((gridx<MAXVECTOR&&gridx>0)&&	\	(gridy<MAXVECTOR&&gridy>0)){    x->gridx = gridx;    x->gridy = gridy;    x->step = 0;     nnato_vectinit(x);//lets begin again    } else {    x->gridx = 64;   x->gridy = 64;        }}void nnato_vectinit(NNato *x){	short i,j;	    for(i=0;i<x->gridx;i++)        for(j=0;j<x->gridy;j++)		{			x->posit[i][j] = 0.0f; 		}}void nnato_rate(NNato *x, float n){        int cand=0, i, j;        x->rate = n ;//* 0.01;          for(i=1; i<x->gridx; i++){	   for(j=0; j<x->gridy; j++)	{	   	cand = (i+j)%2;	   	if (!cand) x->posit[i][j] += (x->posit[i][j]*x->rate);		//even numbrs	   	else if (cand==1)		 x->posit[i][j] -= (x->posit[i][j]*x->rate);				//odd numbrs		}  	}  }  void nnato_ignitv(NNato *x, int flaag, float flag){    register short i, j;        x->posit[0][flaag]=flag;     if (flaag==x->gridy-1)	{      for(i=1; i<x->gridx; i++){	   for(j=0; j<x->gridy; j++)	{		 x->posit[i][j] = x->posit[i-1][j];	//_copy	next			//		 x->posit[i][j] = 0.01f;	//just zero the vector out				  		}		   	   }			x->ignt=true;		post("x->ignt=%ld", x->ignt,0);  	 	}}void nnato_list(NNato *x, Symbol *s, short argc, Atom *argv){	register short 	i, j;	short	indi, gridy=x->gridy, gridx=x->gridx;	float	poslisratio, incremento, a, b, indice, val;		if (argc > 64) return;	for (i=0; i < argc; i++) {		if (argv[i].a_type==A_LONG) {			x->inputlist[i] = (float)argv[i].a_w.w_long * 0.01;//divide por 100 se int		}		else if (argv[i].a_type==A_FLOAT) {			x->inputlist[i] = argv[i].a_w.w_float;		}	 }	if (argc == 64) goto stemall;	poslisratio = (float)gridy / argc;		for(j=0; j<gridy; j++){			indice = ((float)j/poslisratio);// valor inteiro			indi = (int)(indice);		// valor arredondado			a = x->inputlist[indi];			b = x->inputlist[indi+1];			incremento = (float) ((b-a)/poslisratio)*((indi+1)-indice);			val = (float)x->inputlist[indi] + incremento;//			if(x->ignt==true) {				x->posit[0][j]=val;	//entra elemento para a posicao 0//					}//			else nnato_ignitv(x, j, val);//			if(x->ignt==0) {//				nnato_ignitv(x, j, val);//				}//			else if (x->ignt==1) {//				x->posit[0][j]=val;	//entra elemento para a posicao 0//			}		}						// o elemento da posicao 1 passa para a pos 2, ie cpstep+1->c[step+2// depois o elemnto da pos 1 copia zerojustdoit:			for(i=gridx-1; i>=0; --i)	for(j=gridy; j>=0; --j){//		if(!x->border)		 x->posit[i+1][j] = x->posit[i][j];	//_copy	next//		else//		if(x->border)//			if( j==0 ||  i == 0	|| j == gridy-1 || i == gridx-2)//				x->posit[i][j]=0.0f;			//		if (i==0||j==0||i==gridx-1||j==gridy-1) x->posit[i][j]=0.0;		}		   	return;stemall:	for(j=0; j<gridy; j++){	x->posit[0][j]=x->inputlist[j];	} 	goto justdoit;}void nnato_float(NNato *x,double flag){	register short 	i, j;	short gridy=x->gridy, gridx=x->gridx;	for(j=0; j<gridy; j++){				x->posit[0][j]=(float)flag;	//entra elemento para a posicao 0		}	for(i=gridx-1; i>=0; --i)	for(j=gridy; j>=0; --j){//		if(!x->border)		 x->posit[i+1][j] = x->posit[i][j];	//_copy	next//		else if (x->border) {//			if( j==0 ||  i == 0	|| j == gridy-1 || i == gridx-2) {//				x->posit[i+1][j]=0.0f;	//			  	} //			  else x->posit[i+1][j] = x->posit[i][j];	//		}	//		if (i==0||j==0||i==gridx-1||j==gridy-1) x->posit[i][j]=0.0;	}}void nnato_shifth(NNato *x, int amount){	register short i, j;//	short	a1,b1;	//	if (amount){	for(i=x->gridx-1; i>=0; --i)	for(j=x->gridy-1; j>=0; --j){//		a1=amount+j;//		b1=a1%j;//		 x->posit[i][b1] = x->posit[i+1][j];	//not 					//		 x->posit[i][j+1] = x->posit[i+1][j];	//shift 1 left					 x->posit[i+1][j+1] = x->posit[i][j];	//shift 1 left				  }	  // }}void nnato_shifthh(NNato *x, int amount){	register short i, j;	short	a1,b1;	//	if (amount){	for(i=x->gridx-1; i>=0; --i)	for(j=0; j<x->gridy-1; j++){//		a1=amount+j;//		b1=a1%j;//		 x->posit[i][b1] = x->posit[i+1][j];	//not 							 x->posit[i+1][j] = x->posit[i][j+1];	//shift 1 left				  }	//   }}void nnato_glfun(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, gridy=x->gridy, gridx=x->gridx;	float   st=x->st;        if (!x->on) return;        glTranslatef(-st*gridx / 2.0 + .5, -st*gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it    for(i=0;i < gridx;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j <  gridy;j++)            glVertex3f( (float) st*i, (float) st*j, (float) x->posit[i][j]);        glEnd();    }    for(i=0;i < gridx;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j < gridy;j++)            glVertex3f( (float) st*j, (float) st*i, (float) x->posit[j][i]);        glEnd();    }  }     void nnato_glfun2(n2glatom glatom)	//draew textured{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, gridy=x->gridy, gridx=x->gridx;	float   st=x->st;        if (!x->on) return;        glTranslatef(-st*gridx / 2.0 + .5, -st*gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it  //  glEnable(GL_TEXTURE_2D);    for (i = 0; i < gridx - 1; ++i)    {        glBegin(GL_TRIANGLE_STRIP);        for (j = 0; j < gridy; ++j)        {            glNormal3f(x->normalx,x->normaly,x->normalz);//            glTexCoord2f( x->texCoords[i][j] );            glVertex3f( i, j, x->posit[i][j] );            glNormal3f(x->normalx,x->normaly,x->normalz);  //          glTexCoord2fv( x->texCoords[i+1][j] );            glVertex3f( i+1, j, x->posit[i+1][j] );        }        glEnd();    } //   glDisable(GL_TEXTURE_2D);}      void nnato_glfun3(n2glatom glatom) // draw wireframe{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, gridy=x->gridy, gridx=x->gridx;	float   st=x->st;        if (!x->on) return;        glTranslatef(-st*gridx / 2.0 + .5, -st*gridy / 2.0 + .5, 0); //center it//    glColor3f(x->red, x->green, x->blue);	//color it    glEnable(GL_POLYGON_OFFSET_FILL);    glColor3f(x->red, x->green, x->blue);	//color it    for (i = 0; i < gridx - 1; ++i)    {        glBegin(GL_TRIANGLE_STRIP);        glVertex3f( (float) i, (float) 0, (float) x->posit[i][0]);        glVertex3f( (float) i+1, (float) 0, (float) x->posit[i+1][0]);        for (j = 1; j < gridy; ++j)        {            glNormal3f(x->normalx,x->normaly,x->normalz);            glVertex3f( (float) i, (float) j, (float) x->posit[i][j]);            glNormal3f(x->normalx,x->normaly,x->normalz);            glVertex3f( (float) i+1, (float) j, (float) x->posit[i+1][j]);        }        glEnd();    }    glDisable(GL_POLYGON_OFFSET_FILL);  }     void nnato_glfun4(n2glatom glatom) // draw wireframe deform{    NNato *x = (NNato *)n2getglatomobj(glatom);	short	i,j, gridy=x->gridy, gridx=x->gridx;	float   st=x->st, rate=x->rate;        if (!x->on) return;        glTranslatef(-st*gridx / 2.0 + .5, -st*gridy / 2.0 + .5, 0); //center it    glColor3f(x->red, x->green, x->blue);	//color it    for(i=0;i < gridx;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j <  gridy;j++)            glVertex3f( (float) st*i, (float) st*j, (float) x->posit[i][j]);        glEnd();    }    for(i=0;i < gridx;i++)    {        glBegin(GL_LINE_STRIP);        for(j=0;j < gridy;j++)            glVertex3f( (float) st*j, (float) st*i, (float) x->posit[j][i]);        glEnd();    }  }void nnato_glregister(NNato *x, void *klientz){    n2glregisterx(x,n2objtype,x->glout,klientz);  // bkom 01 klient ov gl alimentari data}			