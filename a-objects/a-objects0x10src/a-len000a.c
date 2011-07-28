/*����������������������������������������������������������������a-len - � andr� sier 2004				receives a 2d, 3d vector and transforms its len		returning the right coordinates for the desired lenght����������������������������������������������������������������*/#include "ext.h"#include "ext_common.h"#include <math.h>typedef struct	_len{	t_object 	        c_ob;				void 		        *c_out[2];		// 2 outlets: outlet for vector (list), outlet_float for len		float	 i[6]; 					//storage for input vector = 2*3d	float	 len, real_len;  		//	t_atom o[6];		//the output vector	short	om;				//2d, 3d} len;void *len_new (Symbol *msg, short argc, Atom *argv);void len_bang (len *x);void len_calc (len *x); //exec rotationvoid len_len (len *x, float len); void len_list (len *x, Symbol* msg, short argc, Atom *argv);void len_assist(len *x, void *b, long m, long a, char *s);void *len_class;void *len_new (Symbol *msg, short argc, Atom *argv) //input the args {	 len *x;	 int i;	 	 x=(len *)newobject(len_class);	 x->c_out[1]=floatout(x);	 x->c_out[0]=listout(x);	 	if (argc) {		if (argv[0].a_type == A_LONG){ //if first arg == long			x->len = (float) argv[0].a_w.w_long;		} else if (argv[0].a_type == A_FLOAT){ //if first arg == float			x->len = (float) argv[0].a_w.w_float;		}	} else   {		x->len = 1.0f;	}		 return(x);	}void len_len (len *x, float len){ 	x->len = len;}void len_bang (len *x){ 	len_calc(x);}void  len_calc (len *x){	float len = x->len, real_len, ratio_len;	float recta[3], centro[3], dist;	short om = x->om, i;	t_atom *  o;			o = x->o;			if (!om) { // 2d		float tx = x->i[0];		float ty = x->i[1];		float txx = x->i[2];		float tyy = x->i[3];		recta[0] = txx - tx; // x'-x		recta[1] = tyy - ty; // y'-y		real_len = sqrt ((recta[0]*recta[0]) + (recta[1]*recta[1])); //len		if (real_len == 0) // prevent /0.			real_len = 0.01;		if (len == 0)			len = 0.01;		ratio_len = len / real_len;		if (ratio_len == 1.0) 		goto done;		// o centro ser� o ponto 0,0 da nova recta; 		// e a equa��o da recta j� est� estabelecida		// basta calcular o novo comprimento...		centro[0] = (tx + txx) / 2.; // (x+x')/2		centro[1] = (ty + tyy) / 2.; // (y+y')/2								done:		SETFLOAT(o+0, tx);		SETFLOAT(o+1, ty);		SETFLOAT(o+2, txx);		SETFLOAT(o+3, tyy);		outlet_float(x->c_out[1], real_len);		outlet_list(x->c_out[0], 0L, 4, o);		return;	} else if (om) { // 3d			outlet_float(x->c_out[1], real_len);		outlet_list(x->c_out[0], 0L, 6, o);	}}void len_list (len *x, Symbol* msg, short argc, Atom *argv){	short i;		if (argc) {		if (argc == 4) {			x->om = 0; // 2d		} else if (argc == 6) {			x->om = 1; // 3d		}		for(i=0; i<argc;i++) {			switch (argv[i].a_type) {				case A_FLOAT:					x->i[i] = argv[i].a_w.w_float;					break;				case A_LONG:					x->i[i] = (float)argv[i].a_w.w_long;					break;				case A_SYM:					error("now, that's a sym isn't it?");					break;			}				}	len_calc(x);	}}void main(void){ long int tick = gettime(); setup((t_messlist**)&len_class,(method)len_new,0L,(short)sizeof(len),0L, A_GIMME,0); addbang((method)len_bang); addfloat((method)len_len); addmess((method)len_list, "list", A_GIMME, 0); addmess((method)len_len, "len", A_FLOAT, 0); addmess((method)len_assist,"assist", A_CANT, 0); post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-len   �   a n d r � s i e r   2 0 0 4   all rights reserved",tick, 0);}void len_assist(len *x, void *b, long m, long a, char *s){    if (m==1) { sprintf(s,"echo e   c  h   o"); }       else if (m==2&&a==0) { sprintf(s,"(list) normalized vector     e         c            h              o"); }    else if (m==2&&a==1) { sprintf(s,"(float) lenght      e             c                h                   o"); }}