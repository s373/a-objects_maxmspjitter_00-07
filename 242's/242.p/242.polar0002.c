																								/*					242.p _ sier														    pixel polar.conv*/#include "Movies.h"#include "ext_common.h"#include "ext.h"#include "ext_user.h"//#include "stdlib.h"#include "math.h"		#include "n2.basicstrings.c"#include "n2.0000.h"     // n2.inkludestypedef struct zttz {	Object   n_ob;     // required max 	void     *n2hdr;   // required nato - position = signifikant _ must b 2nd	void             *dataout;      // n2data outlet	n2imgptr         gvelt, pix2;         // img buffer	short 			mode, change;    //variaveis	float			magnitude, phase, zoom;    Boolean          on,bypass,freeze,think,quarrel;  // nato.objstate flagz} NNato;void *nnato_new(void);void nnato_assist(NNato *x, void *b, long msg, long nr, char *s);void nnato_info(NNato *x, void *p, void *b);void nnato_free(NNato *x);void nnato_processimage(NNato *x, n2atom *data);void nnato_processimage2(NNato *x, n2atom *data);void nnato_processimage3(NNato *x, n2atom *data);void nnato_on(NNato *x, long flag);void nnato_bypass(NNato *x, long flag);void nnato_freeze(NNato *x, long flag);void nnato_int(NNato *x, long n);void nnato_bang(NNato *x);void nnato_think(NNato *x, short val);void nnato_quarrel(NNato *x, short val);void nnato_variant(NNato *x, short variant);void nnato_mode(NNato *x, short mode);void nnato_change(NNato *x, short change);//float frand (NNato *x);void nnato_magnitude		(NNato *x, float magnitude);void nnato_phase		(NNato *x, float phase);void nnato_zoom		(NNato *x, float zoom);void     *nnato_class;main(void){	setup(&nnato_class, nnato_new, (method)nnato_free, (short)sizeof(NNato), 0L, 	      A_DEFLONG,A_DEFLONG, 0);    n2objaddimagecmd();    n2addstatecmd((method)nnato_int,(method)nnato_bang,                  (method)nnato_on,(method)nnato_freeze,(method)nnato_bypass,                  (method)nnato_think,(method)nnato_quarrel,(method)nnato_variant,0); 	addmess((method)nnato_assist,	                   "assist",    	A_CANT,0); 	addmess((method)nnato_info,	                       "info",		    A_CANT,0);	addmess((method)nnato_mode,					"mode",			A_DEFLONG,0);	addmess((method)nnato_change,					"exchange",			A_DEFLONG,0);	addmess((method)nnato_change,					"change",			A_DEFLONG,0);	addmess((method)nnato_magnitude,					"magnitude",			A_DEFFLOAT,0);	addmess((method)nnato_magnitude,					"mag",			A_DEFFLOAT,0);	addmess((method)nnato_magnitude,					"m",			A_DEFFLOAT,0);	addmess((method)nnato_phase,							"phase",			A_DEFFLOAT,0);	addmess((method)nnato_phase,							"phs",			A_DEFFLOAT,0);	addmess((method)nnato_phase,							"p",			A_DEFFLOAT,0);	addmess((method)nnato_zoom,							"zoom",			A_DEFFLOAT,0);	addmess((method)nnato_zoom,							"z",			A_DEFFLOAT,0);	  //   n2reklama();    n2addfklass("242.polar0002");    post("");    post("242.polar0002");    post("											*");//    post("");  //  post("");  //  post(""); //   post("");}void nnato_assist(NNato *x, void *b, long msg, long nr, char *s){	if (msg == 1)      // !nlet	   sprintf( s, "]]] \\in.%ld",x->zoom);     // s!ng.oun.pra!zez - comment +? hap!l+e +? da.     else       {	   if (msg == 2)   // outlet	      if (nr == 0)	   sprintf( s, "]]] \\out.%ld",(x->zoom-999));  // default `image data type` outlet de[a]ss!zt--       }}void nnato_info(NNato *x, void *p, void *b){   //   n2info();}void *nnato_new(void){	NNato      *x;    short      err;  		x = (NNato *)newobject(nnato_class);    n2addoutlet(x,&x->dataout);        if (err = n2objinit(x))       { post("242.1/f  :  n2objinit err = %ld",err); return;}            if ((err = n2inewimage(&x->gvelt,n2idefwidth,n2idefheight)))       { post("242.1/f  :  n2inewimage err = %ld",err); goto ikk;}      if ((err = n2inewimage(&x->pix2,n2idefwidth,n2idefheight)))       { post("242.1/f  :  n2inewimage err = %ld",err); goto ikk;}      if (err = n2objnewimagebuffer(x,2,0,0)) goto ikk;    n2objsetimagefun(x,(method)nnato_processimage);            n2objaddinlets(x,1);    x->mode=0;    x->magnitude=0.000001;    x->phase=0.000001;    x->zoom=1.000001;       	x->on = true;    x->bypass = x->freeze = x->think = x->quarrel = false;           return (x);ikk:	nnato_free(x);}void nnato_free(NNato *x){    if (x->gvelt) { n2idisposeimage(x->gvelt); x->gvelt = 0;}    if (x->pix2) { n2idisposeimage(x->pix2); x->pix2 = 0;}    n2objfree(x);}void nnato_on(NNato *x, long flag){        x->on = flag;}    void nnato_bypass(NNato *x, long flag){        x->bypass = flag;}      void nnato_freeze(NNato *x, long flag){        x->freeze = flag;} void nnato_int(NNato *x, long n){   n2defintstatefun(x,n);}void nnato_bang(NNato *x){   n2registerimage(x,n2objtype,x->dataout,0,x->gvelt);}void nnato_think(NNato *x, short val){   x->think = val;}void nnato_quarrel(NNato *x, short val){   x->quarrel = val;}void nnato_variant(NNato *x, short variant){   if (!variant)       n2objsetimagefun(x,(method)nnato_processimage);    else if (variant == 1)       n2objsetimagefun(x,(method)nnato_processimage2);   else if (variant == 2)       n2objsetimagefun(x,(method)nnato_processimage3);}void nnato_magnitude(NNato *x, float magnitude){   x->magnitude = magnitude;}void nnato_phase(NNato *x, float phase){   x->phase = phase;}void nnato_mode(NNato *x, short mode){   x->mode = mode;}void nnato_change(NNato *x, short change){   x->change = change;}void nnato_zoom(NNato *x, float zoom){   x->zoom = zoom;}/*float frand(NNato *x){	unsigned long n;		x->seed = x->seed * 1103515245 + 12345;	n = x->seed & 2147483647;	return(((float)n)/2147483648.0);}*/void nnato_processimage(NNato *x, n2atom *data){                       long           i, j, roubitez, roubitez2, pix, pix2, rix, gix, bix,     			   rix2,gix2,bix2,rx,gx,bx,inletnr = n2objgetinletnr(x);                              short          width, height, err, change=x->change;          float 		   magnitude, phase, real, imaginary;                                                                 Ptr            src, dst, srcadresz, dstadresz, src2, scradresz2;          n2pixmaphdl    srcpixmap,srcpixmap2, dstpixmap;    n2pixstate     srcpixztate, dstpixztate, srcpixztate2;    n2imgptr       dstgvelt, gveltptr = n2getatomimage(data);        if (!x->on || !gveltptr) return;        if (inletnr == 1)        {        	if ((err = n2iensuredimeq(&gveltptr,&x->pix2)))            {             post ("242.polar0002: n2iensureimagedimeq (2) err = %ld", err);             return;            }            n2icopypixraw( gveltptr, x->pix2 );			return;		}        if (x->freeze)           {                 n2registerimage(x,n2objtype,x->dataout,data,x->gvelt);              return;           }                       if (x->bypass)           {              n2registerimage(x,n2objtype,x->dataout,data,gveltptr);              return;              }         if ((err = n2iensuredimeq(&gveltptr,&x->gvelt)))           {             post ("242.polar0002: n2iensureimagedimeq err = %ld", err);             return;           }           if ((err = n2iensuredimeq(&gveltptr,&x->pix2)))           {             post ("242.polar0002: n2iensureimagedimeq err = %ld", err);             return;           }            if (!x->think)                      srcpixmap  = srcpixmap2 = n2igetpixmap(gveltptr);        else            srcpixmap   = n2igetpixmap(x->gvelt);               if (!x->quarrel)             dstgvelt = x->gvelt;        else            dstgvelt = gveltptr;                         dstpixmap   = n2igetpixmap(dstgvelt);         srcpixztate = n2igetpixstate(srcpixmap);                dstpixztate = n2igetpixstate(dstpixmap);        		srcpixmap2 = n2igetpixmap(x->pix2);		srcpixztate2 = n2igetpixstate(srcpixmap2);		        if (!n2ilockpix(srcpixmap)) return;               if (!n2ilockpix(dstpixmap)) goto ikk;              if(!change) {         srcadresz = n2igetpixaddr(srcpixmap);		roubitez  = n2igetpixrowbytes(srcpixmap);         srcadresz2 = n2igetpixaddr(srcpixmap2);		roubitez2  = n2igetpixrowbytes(srcpixmap2); }else {        srcadresz = n2igetpixaddr(srcpixmap2);		roubitez  = n2igetpixrowbytes(srcpixmap2);         srcadresz2 = n2igetpixaddr(srcpixmap);		roubitez2  = n2igetpixrowbytes(srcpixmap); }        n2igetdim(gveltptr,&width,&height); if (!x->mode) // Polar to Rectangular 	{                   for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;             src2 = src2adresz + i * roubitez2                       dst = dstadresz + i * roubitez;                  for(j = 0; j < width; j++)            {                pix2 = n2get32(src2);                pix = n2get32(src);                                rix2 = n2get32red(pix2);                gix2 = n2get32green(pix2);                bix2 = n2get32blue(pix2);                rix = n2get32red(pix);                gix = n2get32green(pix);                bix = n2get32blue(pix);                                rx = (long)rix * cos (rix2);	//watching real - no zoom                gx = (long)gix * cos (gix2);                bx = (long)bix * cos (bix2);                              				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(rx),				         n2klamp255(gx),				         n2klamp255(bx));				src+=4;				dst+=4;					             }		 }  		}  else       // Rectangular to Polar 	{                   for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;                        dst = dstadresz + i * roubitez;                  for(j = 0; j < width; j++)            {                pix = n2get32(src);                rix2 = n2get32red(pix2);                gix2 = n2get32green(pix2);                bix2 = n2get32blue(pix2);                rix = n2get32red(pix);                gix = n2get32green(pix);                bix = n2get32blue(pix);                                rx = (long)sqrt((rix*rix)+ (rix2*rix2);		//watching magnitude without zoom                gx = (long)sqrt((gix*gix)+ (gix2*gix2);                bx = (long)sqrt((bix*bix)+ (bix2*bix2);                                              				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(rx),				         n2klamp255(gx),				         n2klamp255(bx));                 				src+=4;				dst+=4;					             }		 }  		}    	  		  		  		  			    n2isetpixstate(srcpixmap,srcpixztate);	    n2isetpixstate(srcpixmap2,srcpixztate2);        n2isetpixstate(dstpixmap,dstpixztate);        n2registerimage(x,n2objtype,x->dataout,data,dstgvelt);        return;        ikk: 	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);}		void nnato_processimage2(NNato *x, n2atom *data){                       long           i, j, roubitez, roubitez2, pix, pix2, rix, gix, bix,     			   rix2,gix2,bix2,rx,gx,bx,inletnr = n2objgetinletnr(x);                              short          width, height, err, change=x->change;          float 		   magnitude, phase, real, imaginary;                                                                 Ptr            src, dst, srcadresz, dstadresz, src2, scradresz2;          n2pixmaphdl    srcpixmap,srcpixmap2, dstpixmap;    n2pixstate     srcpixztate, dstpixztate, srcpixztate2;    n2imgptr       dstgvelt, gveltptr = n2getatomimage(data);        if (!x->on || !gveltptr) return;        if (inletnr == 1)        {        	if ((err = n2iensuredimeq(&gveltptr,&x->pix2)))            {             post ("242.polar0002: n2iensureimagedimeq (2) err = %ld", err);             return;            }            n2icopypixraw( gveltptr, x->pix2 );			return;		}        if (x->freeze)           {                 n2registerimage(x,n2objtype,x->dataout,data,x->gvelt);              return;           }                       if (x->bypass)           {              n2registerimage(x,n2objtype,x->dataout,data,gveltptr);              return;              }         if ((err = n2iensuredimeq(&gveltptr,&x->gvelt)))           {             post ("242.polar0002: n2iensureimagedimeq err = %ld", err);             return;           }           if ((err = n2iensuredimeq(&gveltptr,&x->pix2)))           {             post ("242.polar0002: n2iensureimagedimeq err = %ld", err);             return;           }            if (!x->think)                      srcpixmap  = srcpixmap2 = n2igetpixmap(gveltptr);        else            srcpixmap   = n2igetpixmap(x->gvelt);               if (!x->quarrel)             dstgvelt = x->gvelt;        else            dstgvelt = gveltptr;                         dstpixmap   = n2igetpixmap(dstgvelt);         srcpixztate = n2igetpixstate(srcpixmap);                dstpixztate = n2igetpixstate(dstpixmap);        		srcpixmap2 = n2igetpixmap(x->pix2);		srcpixztate2 = n2igetpixstate(srcpixmap2);		        if (!n2ilockpix(srcpixmap)) return;               if (!n2ilockpix(dstpixmap)) goto ikk;              if(!change) {         srcadresz = n2igetpixaddr(srcpixmap);		roubitez  = n2igetpixrowbytes(srcpixmap);         srcadresz2 = n2igetpixaddr(srcpixmap2);		roubitez2  = n2igetpixrowbytes(srcpixmap2); }else {        srcadresz = n2igetpixaddr(srcpixmap2);		roubitez  = n2igetpixrowbytes(srcpixmap2);         srcadresz2 = n2igetpixaddr(srcpixmap);		roubitez2  = n2igetpixrowbytes(srcpixmap); }        n2igetdim(gveltptr,&width,&height); if (!x->mode) // Polar to Rectangular 	{                   for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;             src2 = src2adresz + i * roubitez2                       dst = dstadresz + i * roubitez;                  for(j = 0; j < width; j++)            {                pix2 = n2get32(src2);                pix = n2get32(src);                                rix2 = n2get32red(pix2);                gix2 = n2get32green(pix2);                bix2 = n2get32blue(pix2);                rix = n2get32red(pix);                gix = n2get32green(pix);                bix = n2get32blue(pix);                                rx = (long)rix * sin (rix2);	//watching imaginary - no zoom                gx = (long)gix * sin (gix2);                bx = (long)bix * sin (bix2);                              				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(rx),				         n2klamp255(gx),				         n2klamp255(bx));				src+=4;				dst+=4;					             }		 }  		}  else       // Rectangular to Polar 	{                   for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;                        dst = dstadresz + i * roubitez;                  for(j = 0; j < width; j++)            {                pix = n2get32(src);                rix2 = n2get32red(pix2);                gix2 = n2get32green(pix2);                bix2 = n2get32blue(pix2);                rix = n2get32red(pix);                gix = n2get32green(pix);                bix = n2get32blue(pix);                                rx = (long)atan(rix2 / rix);		//watching phase without zoom                gx = (long)atan(gix2 / gix);                bx = (long)atan(bix2 / bix);                                              				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(rx),				         n2klamp255(gx),				         n2klamp255(bx));                 				src+=4;				dst+=4;					             }		 }  		}    	  		  		  		  			    n2isetpixstate(srcpixmap,srcpixztate);	    n2isetpixstate(srcpixmap2,srcpixztate2);        n2isetpixstate(dstpixmap,dstpixztate);        n2registerimage(x,n2objtype,x->dataout,data,dstgvelt);        return;        ikk: 	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);}void nnato_processimage3(NNato *x, n2atom *data)   {                       long           i, j, roubitez, roubitez2, pix, pix2, rix, gix, bix,     			   rix2,gix2,bix2,rx,gx,bx,inletnr = n2objgetinletnr(x);                              short          width, height, err, change=x->change;          float 		   magnitude, phase, real, imaginary, zoom = x->zoom;              //zoom 2                                                   Ptr            src, dst, srcadresz, dstadresz, src2, scradresz2;          n2pixmaphdl    srcpixmap,srcpixmap2, dstpixmap;    n2pixstate     srcpixztate, dstpixztate, srcpixztate2;    n2imgptr       dstgvelt, gveltptr = n2getatomimage(data);        if (!x->on || !gveltptr) return;        if (inletnr == 1)        {        	if ((err = n2iensuredimeq(&gveltptr,&x->pix2)))            {             post ("242.polar0002: n2iensureimagedimeq (2) err = %ld", err);             return;            }            n2icopypixraw( gveltptr, x->pix2 );			return;		}        if (x->freeze)           {                 n2registerimage(x,n2objtype,x->dataout,data,x->gvelt);              return;           }                       if (x->bypass)           {              n2registerimage(x,n2objtype,x->dataout,data,gveltptr);              return;              }         if ((err = n2iensuredimeq(&gveltptr,&x->gvelt)))           {             post ("242.polar0002: n2iensureimagedimeq err = %ld", err);             return;           }           if ((err = n2iensuredimeq(&gveltptr,&x->pix2)))           {             post ("242.polar0002: n2iensureimagedimeq err = %ld", err);             return;           }            if (!x->think)                      srcpixmap  = srcpixmap2 = n2igetpixmap(gveltptr);        else            srcpixmap   = n2igetpixmap(x->gvelt);               if (!x->quarrel)             dstgvelt = x->gvelt;        else            dstgvelt = gveltptr;                         dstpixmap   = n2igetpixmap(dstgvelt);         srcpixztate = n2igetpixstate(srcpixmap);                dstpixztate = n2igetpixstate(dstpixmap);        		srcpixmap2 = n2igetpixmap(x->pix2);		srcpixztate2 = n2igetpixstate(srcpixmap2);		        if (!n2ilockpix(srcpixmap)) return;               if (!n2ilockpix(dstpixmap)) goto ikk;              if(!change) {         srcadresz = n2igetpixaddr(srcpixmap);		roubitez  = n2igetpixrowbytes(srcpixmap);         srcadresz2 = n2igetpixaddr(srcpixmap2);		roubitez2  = n2igetpixrowbytes(srcpixmap2); }else {        srcadresz = n2igetpixaddr(srcpixmap2);		roubitez  = n2igetpixrowbytes(srcpixmap2);         srcadresz2 = n2igetpixaddr(srcpixmap);		roubitez2  = n2igetpixrowbytes(srcpixmap); }        n2igetdim(gveltptr,&width,&height); if (!x->mode) // Polar to Rectangular 	{                   for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;             src2 = src2adresz + i * roubitez2                       dst = dstadresz + i * roubitez;                  for(j = 0; j < width; j++)            {                pix2 = n2get32(src2);                pix = n2get32(src);                                rix2 = n2get32red(pix2);                gix2 = n2get32green(pix2);                bix2 = n2get32blue(pix2);                rix = n2get32red(pix);                gix = n2get32green(pix);                bix = n2get32blue(pix);                                rx = (long)rix * sin (rix2)*zoom;	//watching imaginary                 gx = (long)gix * sin (gix2) *zoom;                bx = (long)bix * sin (bix2)*zoom;                              				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(rx),				         n2klamp255(gx),				         n2klamp255(bx));				src+=4;				dst+=4;					             }		 }  		}  else       // Rectangular to Polar 	{                   for(i = 0; i < height; i++)          {            src = srcadresz + i * roubitez;                        dst = dstadresz + i * roubitez;                  for(j = 0; j < width; j++)            {                pix = n2get32(src);                rix2 = n2get32red(pix2);                gix2 = n2get32green(pix2);                bix2 = n2get32blue(pix2);                rix = n2get32red(pix);                gix = n2get32green(pix);                bix = n2get32blue(pix);                                rx = (long)atan(rix2 / rix)*zoom;		//watching phase without zoom                gx = (long)atan(gix2 / gix)*zoom;                bx = (long)atan(bix2 / bix)*zoom;                                              				n2set32m(dst,				         n2get32alpha(pix),				         n2klamp255(rx),				         n2klamp255(gx),				         n2klamp255(bx));                 				src+=4;				dst+=4;					             }		 }  		}    	  		  		  		  			    n2isetpixstate(srcpixmap,srcpixztate);	    n2isetpixstate(srcpixmap2,srcpixztate2);        n2isetpixstate(dstpixmap,dstpixztate);        n2registerimage(x,n2objtype,x->dataout,data,dstgvelt);        return;        ikk: 	    n2isetpixstate(srcpixmap,srcpixztate);        n2isetpixstate(dstpixmap,dstpixztate);}