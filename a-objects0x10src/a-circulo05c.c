/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
circulo - © andrŽ sier 2003
		
		Max output of xy circular coords
		responding to bangs, doubles
		
		fixed to doubles 20040418 working on ppc, not on interl
			
			and fixed rounding errors
			and also float->circulo doesnt listen to x->steps
			so that 0.->1. always same proportion in 2¹
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"
#include "math.h"

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define pi2		2 * M_PI

typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet value	

	double			coords[2];		// as xy de saida
	double			center[2];		// o centro do circulo
	double			raio;			// o raio
	double			move[2];		// deslocamos o centro ?
	double			steps;			// quantos passos por circulo
	double			step;			// em q passo estamos
	double			stride;			// a dist de passo a passo
	t_atom			out[2];			// nao preciso de mais q 2 de saida
	char			dir, 
					om;
} circ;


void *circ_new (Symbol *msg, short argc, Atom *argv);
void circ_bang (circ *x);

void circ_calc (circ *x);
void circ_center (circ *x, double echo, double ech);
void circ_move (circ *x, double echo, double ech);
void circ_raio (circ *x, double echo);
void circ_stride (circ *x, double echo);
void circ_steps (circ *x, double echo);
void circ_step (circ *x, double echo);

void circ_echo (circ *x, double echo);
void circ_int(circ *x, int n);
void circ_double(circ *x, double n);


void circ_mode (circ *x, long echo);
void circ_om (circ *x, long echo);

void circ_list (circ *x, Symbol *msg, short argc, Atom *argv);


void circ_post (circ *x);

void circ_assist(circ *x, void *b, long m, long a, char *s);
void *circ_class;


void *circ_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 circ *x;
	 int i;
	 
	 x=(circ *)newobject(circ_class);


	 x->c_out=listout(x);// xy circ coords
	 

		x->center[0] = 64.0;
		x->center[1] = 64.0;
		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->raio 	 = 32.0; 
		x->dir 		 = 1;
		x->move[0] 	 = 0.; //so that the convolution is about
		x->move[1] 	 = 0.; // the ooo origin
		x->om = 0;

		x->steps = 1.;
		x->step = 0.;
		x->stride = 0.01*M_PI;


	if (argc) { // 
	for (i=0; i<argc/2;i++) 
	{

		if(argv->a_w.w_sym == gensym("@center")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->center[0] = argv->a_w.w_float;	argv++;
				x->center[1] = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->center[0] = (double)argv->a_w.w_long;	argv++;
				x->center[1] = (double)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@c")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->center[0] = argv->a_w.w_float;	argv++;
				x->center[1] = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->center[0] = (double)argv->a_w.w_long;	argv++;
				x->center[1] = (double)argv->a_w.w_long;	argv++;			
			}
		}
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
		if(argv->a_w.w_sym == gensym("@raio")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->raio = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->raio = (double)argv->a_w.w_long;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@r")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->raio = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->raio = (double)argv->a_w.w_long;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@steps")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->steps = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->steps = argv->a_w.w_long;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@stride")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->stride = argv->a_w.w_float * pi2;	argv++;
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
	} else if(!argc)  {
		x->center[0] = 64.0;
		x->center[1] = 64.0;
		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->raio 	 = 32.0; 
		x->dir 		 = 1;
		x->move[0] 	 = 0.; //so that the convolution is about
		x->move[1] 	 = 0.; // the ooo origin
		x->om = 0;

		x->steps = 100.;
		x->step = 0.;
		x->stride = 0.01 * M_PI;
	}	

	 return(x);	
}


void  circ_calc (circ *x)
{
	double 	value;

	// advance stride, rounding errs
	x->step += x->stride;
	if (x->step > x->steps)
		x->step = 0.0 + (x->step-x->steps);
	if (x->step < 0.0f)
		x->step = x->steps + (x->step);
		
		

		

	// calc current circle coords
	value =  -x->step*(pi2/x->steps);// pi2-x->step*(pi2/x->steps);
//		post("calc method step is %f val is %f", x->step, value);

	x->coords[0] = sin (value) * x->raio + x->center[0];
	x->coords[1] = cos (value) * x->raio + x->center[1];
	
}

void circ_center (circ *x, double echo, double ech)
{
	x->center[0] = echo;
	x->center[1] = ech;
	if (x->om)	// output do novo circulo apenas se nao ha silencio
		circ_bang(x);
}

/*
void circ_move (circ *x, double echo, double ech)
{
	x->move[0] = echo;
	x->move[1] = ech;
	if (x->om)	// output do novo circulo apenas se nao ha silencio
		circ_bang(x);
}
*/

void circ_raio (circ *x, double echo)
{
	x->raio = echo; //
	if (x->om)	// output do novo circulo apenas se nao ha silencio
		circ_bang(x);
}

void circ_stride (circ *x, double echo)
{
	x->stride = echo ;//* pi2; //
	if (x->om)	// output do novo circulo apenas se nao ha silencio
		circ_bang(x);
}

void circ_steps (circ *x, double echo)
{
	x->steps = echo; //
	if (x->steps<0.1)
		x->steps = 100.;
	if (x->om)	// output do novo circulo apenas se nao ha silencio
		circ_bang(x);
}

void circ_step (circ *x, double echo)
{
	x->step = echo; //
	if (x->step<0.1)
		x->step = 100.;
	if (x->om)	// output do novo circulo apenas se nao ha silencio
		circ_bang(x);
}


void circ_post (circ *x)
{
post(" 	");
post(" 	");
post("  a-circ 0.3 20031114");
post(" 	");
post(" 2*pi        = %f", pi2);
post(" M_PI        = %f", M_PI);
post(" x-center    = %f", x->center[0]);
post(" y-center    = %f", x->center[1]);
post(" coords[0]   = %f", x->coords[0]);
post(" coords[1]   = %f", x->coords[1]);
post(" move[0]     = %f", x->move[0]);
post(" move[1]     = %f", x->move[1]);
post(" steps       = %f", x->steps);
post(" step        = %f", x->step);
post(" stride      = %f", x->stride);
post(" om          = %ld", x->om);
post(" 		");
post(" 	");
}


void circ_echo (circ *x, double echo) //compute directly from number, no iter
{
	t_atom	*out;
	double 	value;

	out	= x->out;

	// important, init positions by double
//	x->step = echo; x->steps = 1.;

	// calc current circle coords
//	value = pi2-echo*(pi2); // dividing by 1. // first stance of this

		//fixed 070703
	value = -echo*(pi2);///x->steps); // this is used in calc x->step*(pi2/x->steps);
	x->step = echo * x->steps;// and set value accordingly in x->steps space
	
	
	//store step
//	x->step = value*x->steps;
//	post("seting step to %f",x->step);
	
	x->coords[0] = sin (value) * x->raio + x->center[0];
	x->coords[1] = cos (value) * x->raio + x->center[1];

		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy circular coords
}

void circ_int(circ *x, int n)     { circ_echo(x, n*0.01);}
void circ_double(circ *x, double n) { circ_echo(x, n);}


void circ_om (circ *x, long echo)
{
	x->om = echo;
}


void circ_bang (circ *x)
{
	t_atom	*out;
	out	= x->out;
	
	circ_calc(x);


		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy circular coords

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&circ_class,(method)circ_new,0L,(short)sizeof(circ),0L,
 A_GIMME,0);

 addbang((method)circ_bang);
 
 addmess((method)circ_raio,"raio",A_DEFFLOAT,0);
 addmess((method)circ_center,"center",A_DEFFLOAT,A_DEFFLOAT,0);
// addmess((method)circ_move,"move",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)circ_stride,"stride",A_DEFFLOAT,0);
 addmess((method)circ_steps,"steps",A_DEFFLOAT,0);
 addfloat((method)circ_double);
 addint((method)circ_int);

 addmess((method)circ_post,"post",0);

 addmess((method)circ_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-circulo   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);
}


void circ_assist(circ *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) circular xy    e         c            h              o"); }
}


