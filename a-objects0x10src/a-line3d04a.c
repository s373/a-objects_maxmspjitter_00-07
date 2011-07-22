/*����������������������������������������������������������������line3d - � andr� sier 2003				Max output of xy line3dular coords		responding to bangs, floats									20040402	- primeira versao			20040403	- segunda:adicionada outlet quando termina						  inserido mecanismo de lista fifo����������������������������������������������������������������*/#include "ext.h"#include "ext_common.h"//#include "math.h"/*#ifndef M_PI#define M_PI	3.14159265358979323846#endif#define pi2		2 * M_PI*/typedef struct	{	t_object 	        c_ob;				void 		        *c_out;		// outlet value		void				*c_out2;	void				*c_out3;	double			coords[3];		// as xy de saida	double			move[3];		// deslocamos o centro ?	double			steps;			// quantos passos por line3dulo	double			step;			// em q passo estamos	double			stride;			// a dist de passo a passo	t_atom			out[3];			// nao preciso de mais q 2 de saida	Boolean			end, om; // se chegou ao fim do stride, e o modo de saida	double			a[3], b[3]; // as coordenadas de cada ponto	short			numpoints, whichpoint; // quantos argumentos a lista tem, qual � o primeiro ponto...	double			inlist[256]; // armazenar os pontos a iterar sobre} line3d;void *line3d_new (Symbol *msg, short argc, Atom *argv);void line3d_bang (line3d *x);void line3d_calc (line3d *x);void line3d_move (line3d *x, double echo, double ech, double ec);void line3d_stride (line3d *x, double echo);void line3d_steps (line3d *x, double echo);void line3d_step (line3d *x, double echo);void line3d_st(line3d *x, int n);void line3d_echo (line3d *x, double echo);void line3d_int(line3d *x, int n);void line3d_float(line3d *x, double n);void line3d_a (line3d *x, double echo, double ech, double ec);void line3d_b (line3d *x, double echo, double ech, double ec);void line3d_ang_len (line3d *x, double azi, double ele, double dist);//@dded after hspline: stack methodvoid line3d_stack (line3d *x, double echo, double ech, double ec);void line3d_moveto (line3d *x, double echo, double ech, double ec);void line3d_mode (line3d *x, long echo);void line3d_om (line3d *x, long echo);void line3d_list (line3d *x, Symbol *msg, short argc, Atom *argv);void line3d_setpt (line3d *x, long echo);void line3d_post (line3d *x);void line3d_assist(line3d *x, void *b, long m, long a, char *s);void *line3d_class;void *line3d_new (Symbol *msg, short argc, Atom *argv) //input the args {	 line3d *x;	 int i;	 	 x=(line3d *)newobject(line3d_class);	 x->c_out3=bangout(x);// output bang at end points	 x->c_out2=bangout(x);// output bang at end	 x->c_out=listout(x);// xy line3d coords	 		x->coords[0] = 0.;		x->coords[1] = 0.;		x->coords[2] = 0.;		x->move[0] 	 = 0.; //so that the convolution is about		x->move[1] 	 = 0.; // the ooo origin		x->move[2] 	 = 0.; // the ooo origin		x->om = 0;			x->a[0] = 0.0;		x->a[1] = 0.0;		x->a[2] = 0.0;		x->b[0] = 0.0;		x->b[1] = 1.0;		x->b[2] = 1.0;		x->steps = 1.;		x->step = 0.;		x->stride = 0.01;		x->numpoints = 2;		x->whichpoint = 0;		x->end = 0;		for(i=0;i<256;i++)			x->inlist[i] = 0.0;	if (argc) { // 	for(i=0;i<argc/2;i++){		if(argv->a_w.w_sym == gensym("@move")){			argv++;			if(argv->a_type == A_FLOAT) {				x->move[0] = argv->a_w.w_float;	argv++;				x->move[1] = argv->a_w.w_float;	argv++;				x->move[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->move[0] = (double)argv->a_w.w_long;	argv++;				x->move[1] = (double)argv->a_w.w_long;	argv++;							x->move[2] = (double)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@a")){			argv++;			if(argv->a_type == A_FLOAT) {				x->a[0] = argv->a_w.w_float;	argv++;				x->a[1] = argv->a_w.w_float;	argv++;				x->a[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->a[0] = (double)argv->a_w.w_long;	argv++;				x->a[1] = (double)argv->a_w.w_long;	argv++;							x->a[2] = (double)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@b")){			argv++;			if(argv->a_type == A_FLOAT) {				x->b[0] = argv->a_w.w_float;	argv++;				x->b[1] = argv->a_w.w_float;	argv++;				x->b[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->b[0] = (double)argv->a_w.w_long;	argv++;				x->b[1] = (double)argv->a_w.w_long;	argv++;							x->b[2] = (double)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@steps")){			argv++;			if(argv->a_type == A_FLOAT) {				line3d_st(x, (long)argv->a_w.w_float);	argv++;					//			} else if(argv->a_type == A_LONG) {				line3d_st(x, argv->a_w.w_long);	argv++;							//			}		}		if(argv->a_w.w_sym == gensym("@stride")){			argv++;			if(argv->a_type == A_FLOAT) {				x->stride = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->stride = (double)argv->a_w.w_long;	argv++;			}		}		if(argv->a_w.w_sym == gensym("@step")){			argv++;			if(argv->a_type == A_FLOAT) {				x->step = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->step = (double)argv->a_w.w_long;	argv++;			}		}		if(argv->a_w.w_sym == gensym("@om")){			argv++;			if(argv->a_type == A_FLOAT) {				x->om = (long)argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->om = argv->a_w.w_long;	argv++;			}		}	}	}	 return(x);	}void line3d_a (line3d *x, double echo, double ech, double ec){	x->a[0]= echo;	x->a[1]= ech;	x->a[2]= ec;	x->numpoints = 2;	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_b (line3d *x, double echo, double ech, double ec){	x->b[0]= echo;	x->b[1]= ech;	x->b[2]= ec;	x->numpoints = 2;	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_stack (line3d *x, double echo, double ech, double ec){	if(x->numpoints!=1) x->numpoints=1; //special; avoids calling inlist and sets immediatly	x->a[0] = x->b[0];	x->a[1] = x->b[1];	x->a[2] = x->b[2];	x->b[0] = echo;	x->b[1] = ech;	x->b[2] = ec;}void line3d_moveto (line3d *x, double echo, double ech, double ec){	if(x->numpoints!=1) x->numpoints=1; //special; avoids calling inlist and sets immediatly	x->a[0] = echo;	x->a[1] = ech;	x->a[2] = ec;	x->b[0] = echo;	x->b[1] = ech;	x->b[2] = ec;}void line3d_ang_len (line3d *x, double azi, double ele, double dist){	}void line3d_list (line3d *x, Symbol *msg, short argc, Atom *argv){	short i;		x->numpoints = (short)argc/3;//	post("numpoint %ld",x->numpoints);	 	for (i=0; i<argc; i++) {				if (argv->a_type == A_FLOAT) {			x->inlist[i] = argv->a_w.w_float;			argv++;		} 		else if (argv->a_type == A_LONG) {			x->inlist[i] = (double)argv->a_w.w_long;			argv++;		} 			}}void  line3d_calc (line3d *x){	double 	value, step = x->step, a[3], b[3], move[3], stride = x->stride;	short	pt = x->whichpoint;	if (x->numpoints > 2) // se tivermos mais do que dois argumentos	{		x->a[0] = x->inlist[3*pt+0];		x->a[1] = x->inlist[3*pt+1];		x->a[2] = x->inlist[3*pt+2];		x->b[0] = x->inlist[3*pt+3];		x->b[1] = x->inlist[3*pt+4];		x->b[2] = x->inlist[3*pt+5];	}	a[0]= x->a[0];	a[1]= x->a[1];	a[2]= x->a[2];	b[0]= x->b[0];	b[1]= x->b[1];	b[2]= x->b[2];	move[0] = x->move[0];	move[1] = x->move[1];	move[2] = x->move[2];		x->coords[0] = step * (x->b[0] - x->a[0]) + x->a[0];	x->coords[1] = step * (x->b[1] - x->a[1]) + x->a[1];	x->coords[2] = step * (x->b[2] - x->a[2]) + x->a[2];		if (move[0] !=0. || move[1] != 0. || move[2] != 0.){ 	 x->coords[0] += move[0];	 	 x->coords[1] += move[1];	 	 x->coords[2] += move[2];		}	// advance stride and bang before last if if	x->step += stride;	value = x->step + stride;	if (value > x->steps) 	{		x->step = 0.0 + (value-x->steps);		x->end = 1;	}	if (value < 0.0f)	{		x->step = x->steps + value;		x->end = 1;	}/*	if (x->step > x->steps) 		x->step = 0.0;	if (x->step < 0.0f)		x->step = x->steps;	if (value > x->steps) // test to see if next crosses the line3d, then bangout		x->end = 1;	if (value < 0.0f)		x->end = 1;	if (x->end)	post ("point %ld\, %f %f %f : %f %f %f", pt, 											a[0], a[1], a[2],											b[0], b[1], b[2], 0);*/}void line3d_echo (line3d *x, double echo) //compute directly from number, no iter{	t_atom	*out;	double 	value,  a[3], b[3], move[3];	out	= x->out;	a[0]= x->a[0];	a[1]= x->a[1];	a[2]= x->a[2];	b[0]= x->b[0];	b[1]= x->b[1];	b[2]= x->b[2];	move[0] = x->move[0];	move[1] = x->move[1];	move[2] = x->move[2];		x->coords[0] = echo * (x->b[0] - x->a[0]) + x->a[0];	x->coords[1] = echo * (x->b[1] - x->a[1]) + x->a[1];	x->coords[2] = echo * (x->b[2] - x->a[2]) + x->a[2];		if (move[0] !=0. || move[1] != 0. || move[2] != 0.){ 	 x->coords[0] += move[0];	 	 x->coords[1] += move[1];	 	 x->coords[2] += move[2];		}			SETFLOAT(out+0, x->coords[0]);		SETFLOAT(out+1, x->coords[1]);		SETFLOAT(out+2, x->coords[2]);	outlet_list(x->c_out, 0L, 3, out); //xy line3dular coords}void line3d_int(line3d *x, int n)     { line3d_echo(x, (double)n);}void line3d_float(line3d *x, double n) { line3d_echo(x, n);}void line3d_move (line3d *x, double echo, double ech, double ec){	x->move[0] = echo;	x->move[1] = ech;	x->move[2] = ec;	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_setpt (line3d *x, long echo){	x->whichpoint = (short)echo; //	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_stride (line3d *x, double echo){	x->stride = echo; //	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_st(line3d *x, int n){	if (n>0) {		x->stride = x->steps/n;	}	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_steps (line3d *x, double echo){	x->steps = echo; ////	if (x->steps<0.1)//		x->steps = 100.;	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_step (line3d *x, double echo){	x->step = echo; ////	if (x->step<0.1)//		x->step = 100.;	if (x->om)	// output do novo line3dulo apenas se nao ha silencio		line3d_bang(x);}void line3d_om (line3d *x, long echo){	x->om = echo;}void line3d_bang (line3d *x){	t_atom	*out;	out	= x->out;		line3d_calc(x);		SETFLOAT(out+0, x->coords[0]);		SETFLOAT(out+1, x->coords[1]);		SETFLOAT(out+2, x->coords[2]);	outlet_list(x->c_out, 0L, 3, out); //xy line3dular coords	if (x->end) {		x->end = 0;		outlet_bang(x->c_out2);		if (x->numpoints>2) {//		 if (x->stride>0) {//		post("inc which point");			x->whichpoint +=1; // avan�amos o ponto que queremos ouvir			if (x->whichpoint> (x->numpoints-1)) {// se for maior que 3*o numero de coords , 3D				x->whichpoint = 0;				outlet_bang(x->c_out3);			}/*		 } else if (x->stride<0) {			x->whichpoint -=1; // avan�amos o ponto que queremos ouvir			if (x->whichpoint< 0) // se for maior que 3*o numero de coords , 3D				x->whichpoint = x->numpoints*3;		 		 }*/		x->end = 0;		}	}}void main(void){ long int tick = gettime(); setup((t_messlist**)&line3d_class,(method)line3d_new,0L,(short)sizeof(line3d),0L, A_GIMME,0); addbang((method)line3d_bang); addmess((method)line3d_stack,"stack",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0); addmess((method)line3d_moveto,"moveto",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0); addmess((method)line3d_a,"a",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0); addmess((method)line3d_b,"b",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0); addmess((method)line3d_move,"move",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0); addmess((method)line3d_stride,"stride",A_DEFFLOAT,0); addmess((method)line3d_st,"steps",A_DEFLONG,0);							// addfloat((method)line3d_float); addint((method)line3d_int); addmess((method)line3d_list,"list",A_GIMME,0); addmess((method)line3d_setpt,"set",A_DEFLONG,0); //addmess((method)line3d_post,"post",0); addmess((method)line3d_assist,"assist", A_CANT, 0);// post("	 \t \a \t  %08xd	  			a-line3d � A-Sier 2003"__TIME__,tick,0); post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-line3d3d   �   a n d r � s i e r   2 0 0 4   all rights reserved",tick, 0);}void line3d_assist(line3d *x, void *b, long m, long a, char *s){    if (m==1) { sprintf(s,"echo e   c  h   o"); }       else if (m==2&&a==0) { sprintf(s,"(list) line3d xyz    e         c            h              o"); }    else if (m==2&&a==1) { sprintf(s,"(ping) line3d end ramp    e         c            h              o"); }    else if (m==2&&a==2) { sprintf(s,"(ping) line3d end points ramp    e         c            h              o"); }}