//////////////////   242.r  \\   resizer//		 //		//////								05/04/2001 / s////////#include "Movies.h"#include "ext_common.h"#include "ext.h"#include "ext_user.h"#include <math.h>#include <stdlib.h>#include "n2.basicstrings.c"#include "n2.0000.h"     // nato.inkludes//object data structuretypedef struct zttz {	Object   n_ob;     // required max 	void     *n2hdr;   // required nato - position = signifikant _ must b 2nd		void             *dataout;      // n2data outlet	n2imgptr         gvelt;         // img buffer	short		 widt, heigt, variant;	    Boolean          on,bypass,freeze,think,quarrel;  // nato.objstate flagz} NNato;// prototypesvoid *nnato_new(short widt, short heigt );void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_processimage(NNato *x, n2atom *data);		void nnato_on(NNato *x, long flag);void nnato_bypass(NNato *x, long flag);void nnato_freeze(NNato *x, long flag);void nnato_think(NNato *x, short val);void nnato_quarrel(NNato *x, short val);void nnato_bang(NNato *x);void nnato_int(NNato *x, long n);   void nnato_variant(NNato *x, short variant);void nnato_heigt(NNato *x, short heigt);void nnato_widt(NNato *x, short widt);void     *nnato_class;main(void){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 	      A_DEFLONG,A_DEFLONG, 0);    n2objaddimagecmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,(method)nnato_freeze,(method)nnato_bypass,                  (method)nnato_think,(method)nnato_quarrel,(method)nnato_variant,0); 	addmess((method)nnato_assist,	                   "assist",    	A_CANT,0); 	addmess((method)nnato_info,	                       "info",		    A_CANT,0);    addmess((method)nnato_widt,                     "w",           A_DEFLONG,0);    addmess((method)nnato_heigt,                   "h",         A_DEFLONG,0);    addmess((method)nnato_widt,                     "width",           A_DEFLONG,0);    addmess((method)nnato_heigt,                   "height",         A_DEFLONG,0);    n2addfklass("242.r");        post("242.\\ r    \\\\   2001.sier");}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // inlet			sprintf(s, "nato.imag   .var%ld", x->widt);    else       {	   if (msg == 2)   // outlet	      if (nr == 0)	      	if  (!x->quarrel)			      	sprintf(s, "no quarrel & high  %ld",x->widt);		        else  sprintf(s, " quarrelling & & high  %ld",x->heigt);       }}void nnato_info(NNato *x, void *p, void *b){      n2info();}void *nnato_new(short widt, short heigt ){	NNato      *x;    short      err;  		x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->dataout);        if (err = n2objinit(x))       { post("242.dev  :  n2objinit err = %ld",err); return;}            if ((err = n2inewimage(&x->gvelt,n2idefwidth,n2idefheight)))       { post("242.dev  :  n2inewimage err = %ld",err); goto ikk;}      n2objsetimagefun(x,(method)nnato_processimage);		// igniting...	if(widt) x->widt = widt; else  x->widt = 160;	if(heigt) x->heigt = heigt; else  x->heigt = 160;	x->on = true;    x->bypass = x->freeze = x->think = x->quarrel = false;           return (x);ikk:	nnato_free(x);}void nnato_free(NNato *x){    if (x->gvelt) { n2idisposeimage(x->gvelt); x->gvelt = 0;}    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}    void nnato_bypass(NNato *x, long flag){        x->bypass = flag;}      void nnato_freeze(NNato *x, long flag){        x->freeze = flag;} void nnato_int(NNato *x, long n)    {								       n2defintstatefun(x,n);}void nnato_bang(NNato *x){   n2registerimage(x,n2objtype,x->dataout,0,x->gvelt);			// &what!?          ,&data,}void nnato_widt(NNato *x, short widt){        x->widt = widt;} void nnato_heigt(NNato *x, short heigt){        x->heigt = heigt;} void nnato_variant(NNato *x, short variant){   if (!variant)       n2objsetimagefun(x,(method)nnato_processimage);    else    n2objsetimagefun(x,(method)nnato_processimage);    }void nnato_think(NNato *x, short val){   x->think = val;}void nnato_quarrel(NNato *x, short val){   x->quarrel = val;}void nnato_processimage(NNato *x, n2atom *data){                       long           i, j,  roubitez, pix                 ;                              short          width, height,w=x->widt, h= x->heigt , err;                            Ptr            src, dst, srcadresz, dstadresz;          n2pixmaphdl    srcpixmap, dstpixmap;    n2pixstate     srcpixztate, dstpixztate;    n2imgptr       dstgvelt, gveltptr = n2getatomimage(data);        if (!x->on || !gveltptr) return;        if (x->freeze)           {                 n2registerimage(x,n2objtype,x->dataout,data,x->gvelt);              return;           }                       if (x->bypass)           {              n2registerimage(x,n2objtype,x->dataout,data,gveltptr);              return;              }         if ((err = n2iensuredimeq(&gveltptr,&x->gvelt)))           {             post ("242.r  : n2iensureimagedimeq err = %ld", err);             return;           }                    if (!x->think)                      srcpixmap   = n2igetpixmap(gveltptr);        else            srcpixmap   = n2igetpixmap(x->gvelt);               if (!x->quarrel)             dstgvelt = x->gvelt;        else            dstgvelt = gveltptr;                         dstpixmap   = n2igetpixmap(dstgvelt);         srcpixztate = n2igetpixstate(srcpixmap);                dstpixztate = n2igetpixstate(dstpixmap);                if (!n2ilockpix(srcpixmap)) return;               if (!n2ilockpix(dstpixmap)) goto ikk;                      srcadresz = n2igetpixaddr(srcpixmap);        dstadresz = n2igetpixaddr(dstpixmap);		roubitez  = n2igetpixrowbytes(srcpixmap);    n2iresize (&gveltptr,w, h);           n2igetdim(gveltptr,&width,&height);   //n2iresize (*gveltptr,w, h);            for(i = 0; i < height; i++)   				             {                src = srcadresz + i * roubitez;                            dst = dstadresz + i * roubitez;                                   for(j = 0; j < width; j++)                 {                    pix = n2get32(src);					n2set32m(dst,					         n2get32alpha(pix),					         n2klamp255(n2get32red(pix)),					         n2klamp255(n2get32green(pix)),					         n2klamp255(n2get32blue(pix )));									    src+=4;				    dst+=4;					                 }		     }		  		  	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);        n2registerimage(x,n2objtype,x->dataout,data,dstgvelt);        return;        ikk: 	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);}				 