/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
line2d - © andrŽ sier 2003
		
		Max output of xy line2dular coords
		responding to bangs, floats
		
fixed line2d intel bug
fixed rounding errors from loops
		
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"
/*#include "math.h"

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define pi2		2 * M_PI
*/

typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet value	
	void 		        *c_out2;		// outlet value	
	void 		        *c_out3;		// outlet value	

	double			coords[2];		// as xy de saida
	double			move[2];		// deslocamos o centro ?
	double			steps;			// quantos passos por line2dulo
	double			step;			// em q passo estamos
	double			stride;			// a dist de passo a passo
	t_atom			out[2];			// nao preciso de mais q 2 de saida
	Boolean			end, om; // se chegou ao fim do stride, e o modo de saida

	double			a[2], b[2];
	short			numpoints, whichpoint; // quantos argumentos a lista tem, qual Ž o primeiro ponto...
	double			inlist[256]; // armazenar os pontos a iterar sobre


} line2d;


void *line2d_new (Symbol *msg, short argc, Atom *argv);
void line2d_bang (line2d *x);

void line2d_calc (line2d *x);
void line2d_center (line2d *x, double echo, double ech);
void line2d_move (line2d *x, double echo, double ech);
void line2d_raio (line2d *x, double echo, double ech);
void line2d_stride (line2d *x, double echo);
void line2d_steps (line2d *x, double echo);
void line2d_step (line2d *x, double echo);
void line2d_st(line2d *x, int n);

void line2d_echo (line2d *x, double echo);
void line2d_int(line2d *x, int n);
void line2d_float(line2d *x, double n);


void line2d_a (line2d *x, double echo, double ech);
void line2d_b (line2d *x, double echo, double ech);
void line2d_ang_len (line2d *x, double echo, double ech);


void line2d_mode (line2d *x, long echo);
void line2d_om (line2d *x, long echo);

void line2d_list (line2d *x, Symbol *msg, short argc, Atom *argv);
void line2d_setpt (line2d *x, long echo);


void line2d_post (line2d *x);

void line2d_assist(line2d *x, void *b, long m, long a, char *s);
void *line2d_class;


void *line2d_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 line2d *x;
	 int i;
	 
	 x=(line2d *)newobject(line2d_class);


	 x->c_out3=bangout(x);// xy line2d coords
	 x->c_out2=bangout(x);// xy line2d coords
	 x->c_out=listout(x);// xy line2d coords
	 

		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->move[0] 	 = 0.; //so that the convolution is about
		x->move[1] 	 = 0.; // the ooo origin
		x->om = 0;
	
		x->a[0] = 1.0;
		x->a[1] = 0.0;
		x->b[0] = 0.0;
		x->b[1] = 1.0;


		x->steps = 1.;
		x->step = 0.;
		x->stride = 0.01;

		x->numpoints = 2;
		x->whichpoint = 0;
		x->end = 0;

		for(i=0;i<256;i++)
			x->inlist[i] = 0.0;


	if (argc) { // 
	for(i=0;i<argc/2;i++){

		if(argv->a_w.w_sym == gensym("@move")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->move[0] = argv->a_w.w_float;	argv++;
				x->move[1] = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->move[0] = (double)argv->a_w.w_long;	argv++;
				x->move[1] = (double)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@a")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->a[0] = argv->a_w.w_float;	argv++;
				x->a[1] = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->a[0] = (double)argv->a_w.w_long;	argv++;
				x->a[1] = (double)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@b")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->b[0] = argv->a_w.w_float;	argv++;
				x->b[1] = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->b[0] = (double)argv->a_w.w_long;	argv++;
				x->b[1] = (double)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@steps")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				line2d_st(x, (long)argv->a_w.w_float);	argv++;					//
			} else if(argv->a_type == A_LONG) {
				line2d_st(x, argv->a_w.w_long);	argv++;							//
			}
		}
		if(argv->a_w.w_sym == gensym("@stride")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->stride = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->stride = (double)argv->a_w.w_long;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@step")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->step = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->step = (double)argv->a_w.w_long;	argv++;
			}
		}

		if(argv->a_w.w_sym == gensym("@om")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->om = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->om = argv->a_w.w_long;	argv++;
			}
		}

	}
	}

	 return(x);	
}



void line2d_a (line2d *x, double echo, double ech)
{
	x->a[0]= echo;
	x->a[1]= ech;
	x->numpoints = 2;
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}
void line2d_b (line2d *x, double echo, double ech)
{
	x->b[0]= echo;
	x->b[1]= ech;
	x->numpoints = 2;
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}
void line2d_ang_len (line2d *x, double echo, double ech)
{
	
}
void line2d_list (line2d *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	x->numpoints = (short)argc/2;
//	post("numpoint %ld",x->numpoints);
	 
	for (i=0; i<argc; i++) {
		
		if (argv->a_type == A_FLOAT) {
			x->inlist[i] = argv->a_w.w_float;
			argv++;
		} 
		else if (argv->a_type == A_LONG) {
			x->inlist[i] = (double)argv->a_w.w_long;
			argv++;
		} 
	
	
	}

}


void  line2d_calc (line2d *x)
{
	double 	value, step = x->step, a[2], b[2], move[2], stride = x->stride;
	short pt = x->whichpoint;

	if (x->numpoints > 2) // se tivermos mais do que dois argumentos
	{
		x->a[0] = x->inlist[2*pt+0];
		x->a[1] = x->inlist[2*pt+1];
		x->b[0] = x->inlist[2*pt+2];
		x->b[1] = x->inlist[2*pt+3];
	}

	a[0]= x->a[0];
	a[1]= x->a[1];
	b[0]= x->b[0];
	b[1]= x->b[1];
	move[0] = x->move[0];
	move[1] = x->move[1];
	
	x->coords[0] = step * (x->b[0] - x->a[0]) + x->a[0];
	x->coords[1] = step * (x->b[1] - x->a[1]) + x->a[1];
	
	if (move[0] !=0. || move[1] != 0.){
 	 x->coords[0] += move[0];	
 	 x->coords[1] += move[1];	
	}

	// advance stride
	// advance stride and bang before last if if
	x->step += stride;
	value = x->step + stride;

	if (value > x->steps) 
	{
		x->step = 0.0 + (value-x->steps);
		x->end = 1;
	}
	if (value < 0.0f)
	{
		x->step = x->steps + value;
		x->end = 1;
	}
}

void line2d_echo (line2d *x, double echo) //compute directly from number, no iter
{
	t_atom	*out;
	double 	value, eq[2], a[2], b[2], move[2];

	out	= x->out;

	a[0]= x->a[0];
	a[1]= x->a[1];
	b[0]= x->b[0];
	b[1]= x->b[1];
	move[0] = x->move[0];
	move[1] = x->move[1];
	
	x->coords[0] = echo * (x->b[0] - x->a[0]) + x->a[0];
	x->coords[1] = echo * (x->b[1] - x->a[1]) + x->a[1];

	if (move[0] !=0. || move[1] != 0.){
 	 x->coords[0] += move[0];	
 	 x->coords[1] += move[1];	
	}
	
		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy line2dular coords
}


void line2d_setpt (line2d *x, long echo)
{
	x->whichpoint = (short)echo; //
	if (x->om)	// output do novo lineulo apenas se nao ha silencio
		line2d_bang(x);
}

void line2d_move (line2d *x, double echo, double ech)
{
	x->move[0] = echo;
	x->move[1] = ech;
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}


void line2d_stride (line2d *x, double echo)
{
	x->stride = echo; //
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}


void line2d_st(line2d *x, int n)
{
	if (n>0) {
		x->stride = x->steps/n;
	}
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}

void line2d_steps (line2d *x, double echo)
{
	x->steps = echo; //
	if (x->steps<0.1)
		x->steps = 100.;
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}

void line2d_step (line2d *x, double echo)
{
	x->step = echo; //
	if (x->step<0.1)
		x->step = 100.;
	if (x->om)	// output do novo line2dulo apenas se nao ha silencio
		line2d_bang(x);
}



void line2d_int(line2d *x, int n)     { line2d_echo(x, (double)n);}
void line2d_float(line2d *x, double n) { line2d_echo(x, n);}


void line2d_om (line2d *x, long echo)
{
	x->om = echo;
}


void line2d_bang (line2d *x)
{
	t_atom	*out;
	out	= x->out;
	
	line2d_calc(x);


		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy line2dular coords

	if (x->end) {

		x->end = 0;

		outlet_bang(x->c_out2);

		if (x->numpoints>2) {
			x->whichpoint +=1; // avanamos o ponto que queremos ouvir
			if (x->whichpoint> (x->numpoints-1)) {// se for maior que 3*o numero de coords , 3D
				x->whichpoint = 0;
				outlet_bang(x->c_out3);
				
			}

		}

	}

}




void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&line2d_class,(method)line2d_new,0L,(short)sizeof(line2d),0L,
 A_GIMME,0);

 addbang((method)line2d_bang);
 addmess((method)line2d_a,"a",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)line2d_b,"b",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)line2d_move,"move",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)line2d_stride,"stride",A_DEFFLOAT,0);
// addmess((method)line2d_steps,"steps",A_DEFFLOAT,0);
  addmess((method)line2d_st,"steps",A_DEFLONG,0); // important!
addfloat((method)line2d_float);
 addint((method)line2d_int);
 addmess((method)line2d_list,"list",A_GIMME,0);
 addmess((method)line2d_setpt,"set",A_DEFLONG,0);

 //addmess((method)line2d_post,"post",0);

 addmess((method)line2d_assist,"assist", A_CANT, 0);
// post("	 \t \a \t  %08xd	  			a-line2d © A-Sier 2003"__TIME__,tick,0);

 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-line2d   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void line2d_assist(line2d *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) line2d xy    e         c            h              o"); }
    else if (m==2&&a==1) { sprintf(s,"(bang) line2d line end    e         c            h              o"); }
    else if (m==2&&a==1) { sprintf(s,"(bang) line2d poly end    e         c            h              o"); }
}


