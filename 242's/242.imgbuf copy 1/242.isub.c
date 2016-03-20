#include "Movies.h"#include "ext_common.h"#include "ext.h"#include "ext_user.h"#include "n2.basicstrings.c"#include "n2.0000.h"     // n2.inkludes#include <math.h>#include <stdlib.h>//#include "rand.c"#define MAX_IMAGES 50#define VERSION 20011018//#define RAND_MAX  32767typedef struct zttz {	Object   n_ob;     // required max 	void     *n2hdr;   // required nato - position = signifikant _ must b 2nd		void             *dataout;      // n2data outlet	 	n2imgptr         gvelt;         // img buffer    double			matrix[9];    double			imbuf[MAX_IMAGES][2];    Boolean          on,bypass,freeze,think,quarrel,reset;  // nato.objstate flagz} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_processimage(NNato *x, n2atom *data);void nnato_on(NNato *x, long flag);void nnato_bypass(NNato *x, long flag);void nnato_freeze(NNato *x, long flag);void nnato_int(NNato *x, long n);void nnato_bang(NNato *x);void nnato_think(NNato *x, short val);void nnato_quarrel(NNato *x, short val);void nnato_matrix(NNato *x, Symbol *s, short ac, Atom *av);void nnato_reset(NNato *x);void tristimulus_product(NNato *x,double *matrix,double *input,double *output);double non_zero_random(void);double ran(void);double alea(double min, double max);//int rand(void);void     *nnato_class;main(void){	register short i;	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 	      A_DEFLONG, 0);    n2objaddimagecmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,(method)nnato_freeze,(method)nnato_bypass,                  (method)nnato_think,(method)nnato_quarrel,0L,0); 	addmess((method)nnato_assist,	                   "assist",    	A_CANT,0); 	addmess((method)nnato_info,	                       "info",		    A_CANT,0); 	addmess((method)nnato_matrix, "matrix",A_GIMME,0); 	addmess((method)nnato_reset,	                       "reset",		    0);//    n2reklama();   n2addfklass("242.imbuf");	post("242.imgbuf%freeram!",VERSION);  }void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // inlet	   strcpy( s, "(Nato) Input Image");    else       {	   if (msg == 2)   // outlet	      if (nr == 0)	         strcpy( s, "(Nato) Output Image");       }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void nnato_reset(NNato *x){	register long i1;	for(i1=0;i1<MAX_IMAGES;i1++) {		x->imbuf[i1][0]=0;		x->imbuf[i1][1]=0;		x->imbuf[i1][2]=0;		//post("buffer[%ld][0]=0",i1)	}}void *nnato_new(void){	NNato      *x;    short      err, i,i1;  		x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->dataout);        if (err = n2objinit(x))       { post("242.traffic  :  n2objinit err = %ld",err); return;}            if ((err = n2inewimage(&x->gvelt,n2idefwidth,n2idefheight)))       { post("242.traffic  :  n2inewimage err = %ld",err); goto ikk;}      n2objsetimagefun(x,(method)nnato_processimage);	for(i=0;i<9;i++) {		x->matrix[i] = 1.;	}		for(i1=0;i1<MAX_IMAGES;i1++) {		x->imbuf[i1][0]=0;		x->imbuf[i1][1]=0;		x->imbuf[i1][2]=0;		//post("buffer[%ld][0]=0",i1)	}   	x->on = true;    x->bypass = x->freeze = x->think = x->quarrel = false;         return (x);ikk:	nnato_free(x);}void nnato_free(NNato *x){    if (x->gvelt) { n2idisposeimage(x->gvelt); x->gvelt = 0;}    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}    void nnato_bypass(NNato *x, long flag){        x->bypass = flag;}      void nnato_freeze(NNato *x, long flag){        x->freeze = flag;} void nnato_int(NNato *x, long n){   n2defintstatefun(x,n);}void nnato_bang(NNato *x){   n2registerimage(x,n2objtype,x->dataout,0,x->gvelt);}void nnato_think(NNato *x, short val){   x->think = val;}void nnato_quarrel(NNato *x, short val){   x->quarrel = val;}void nnato_matrix(NNato *x, Symbol *s, short argc, Atom *argv){	register short i;if (s==gensym("matrix")) {	for (i=0; i < argc; i++) {		if (argv[i].a_type==A_LONG) {			x->matrix[i] = (float)argv[i].a_w.w_long;		}		else if (argv[i].a_type==A_FLOAT) {			x->matrix[i] = argv[i].a_w.w_float;		}	}}}// tristimulus product used for matrix conversionsvoid free_architecture_copy(NNato *x,double *matrix,double *input,double *output, short width, short height){  int f,g,r,r1;  for(f=0;f<3;f++)    {    r=non_zero_random()*MAX_IMAGES;    ((r < 0) ? 0 : (r > MAX_IMAGES) ? MAX_IMAGES : r)//    if (r<0){ r= 0;}if (r>MAX_IMAGES){ r=MAX_IMAGES;}    x->imbuf[r][f]=input[f];    r1+=1;    if (r1<0){ r1= 0;}if (r1>MAX_IMAGES){ r1=MAX_IMAGES;}        output[f] = x->imbuf[r1][0]*matrix[f*3+0]+                x->imbuf[r1][1]*matrix[f*3+1]+                x->imbuf[r1][2]*matrix[f*3+2];    }}// perform routines do tristimulus in the main loopdouble non_zero_random(void){	double value;	do 	{		value = ran();	}	   	while (value == 0.0);		return(value);}// // returns a random double between 0 and 1 //double ran(void){		return(alea(0.0, 1.0));}//// returns a random value as double between min and max value // ANSI says that RAND_MAX = 32767, pretty piss poor I say!//double alea(double min, double max){	if (max >= min)		return ((max-min)*((double)rand()/RAND_MAX) + min);	else		return ((min-max)*((double)rand()/RAND_MAX) + max);	}void nnato_processimage(NNato *x, n2atom *data){                       long           i, j, roubitez, pix, xc, yc, zc;    short          width, height, err;                        Ptr            src, dst, srcadresz, dstadresz;          n2pixmaphdl    srcpixmap, dstpixmap;    n2pixstate     srcpixztate, dstpixztate;    n2imgptr       dstgvelt, gveltptr = n2getatomimage(data);	double input[3],output[3];        if (!x->on || !gveltptr) return;        if (x->freeze)           {                 n2registerimage(x,n2objtype,x->dataout,data,x->gvelt);              return;           }                       if (x->bypass)           {              n2registerimage(x,n2objtype,x->dataout,data,gveltptr);              return;              }         if ((err = n2iensuredimeq(&gveltptr,&x->gvelt)))           {             post ("242.traffic  : n2iensureimagedimeq err = %ld", err);             return;           }           if (!x->think)                      srcpixmap   = n2igetpixmap(gveltptr);        else            srcpixmap   = n2igetpixmap(x->gvelt);               if (!x->quarrel)             dstgvelt = x->gvelt;        else            dstgvelt = gveltptr;                         dstpixmap   = n2igetpixmap(dstgvelt);         srcpixztate = n2igetpixstate(srcpixmap);                dstpixztate = n2igetpixstate(dstpixmap);                if (!n2ilockpix(srcpixmap)) return;               if (!n2ilockpix(dstpixmap)) goto ikk;                      srcadresz = n2igetpixaddr(srcpixmap);        dstadresz = n2igetpixaddr(dstpixmap);		roubitez  = n2igetpixrowbytes(srcpixmap);         n2igetdim(gveltptr,&width,&height);                    for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;                        dst = dstadresz + i * roubitez;                                  for(j = 0; j < width; j++)            {                pix = n2get32(src);                input[0] = n2get32red(pix);                input[1] = n2get32green(pix);                input[2] = n2get32blue(pix);				tristimulus_product(x,x->matrix,input,output);				xc=output[0]; 				yc=output[1]; 				zc=output[2]; 				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(xc),				         n2klamp255(yc),				         n2klamp255(zc));				src+=4;				dst+=4;					             }		 }  	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);        n2registerimage(x,n2objtype,x->dataout,data,dstgvelt);        return;        ikk: 	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);}		