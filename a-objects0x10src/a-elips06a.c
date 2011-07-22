/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
elips - © andrŽ sier 2003
		
		Max output of xy elipsular coords
		responding to bangs, floats
			
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
	double			center[2];		// o centro do elipsulo
	double			raio;			// o raio
	double			raio2;			// o raio2
	double			move[2];		// deslocamos o centro ?
	double			steps;			// quantos passos por elipsulo
	double			step;			// em q passo estamos
	double			stride;			// a dist de passo a passo
	t_atom			out[2];			// nao preciso de mais q 2 de saida
	char			dir, 
					om;
} elips;


void *elips_new (Symbol *msg, short argc, Atom *argv);
void elips_bang (elips *x);

void elips_calc (elips *x);
void elips_center (elips *x, double echo, double ech);
void elips_move (elips *x, double echo, double ech);
void elips_raio (elips *x, double echo, double ech);
void elips_stride (elips *x, double echo);
void elips_steps (elips *x, double echo);
void elips_step (elips *x, double echo);

void elips_echo (elips *x, double echo);
void elips_int(elips *x, int n);
void elips_float(elips *x, double n);


void elips_mode (elips *x, long echo);
void elips_om (elips *x, long echo);

void elips_list (elips *x, Symbol *msg, short argc, Atom *argv);


void elips_post (elips *x);

void elips_assist(elips *x, void *b, long m, long a, char *s);
void *elips_class;


void *elips_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 elips *x;
	 int i;
	 
	 x=(elips *)newobject(elips_class);


	 x->c_out=listout(x);// xy elips coords
	 

		x->center[0] = 64.0;
		x->center[1] = 64.0;
		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->raio 	 = 32.0; 
		x->raio2 	 = 22.0; 
		x->dir 		 = 1;
		x->move[0] 	 = 0.; //so that the convolution is about
		x->move[1] 	 = 0.; // the ooo origin
		x->om = 0;

		x->steps = 100.;
		x->step = 0.;
		x->stride = 1.;


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
				x->raio2 = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->raio = (double)argv->a_w.w_long;	argv++;
				x->raio2 = (double)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@r")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->raio = argv->a_w.w_float;	argv++;
				x->raio2 = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->raio = (double)argv->a_w.w_long;	argv++;
				x->raio2 = (double)argv->a_w.w_long;	argv++;			
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
	} else if(!argc)  {
		x->center[0] = 64.0;
		x->center[1] = 64.0;
		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->raio 	 = 32.0; 
		x->raio2 	 = 22.0; 
		x->dir 		 = 1;
		x->move[0] 	 = 0.; //so that the convolution is about
		x->move[1] 	 = 0.; // the ooo origin
		x->om = 0;

		x->steps = 100.;
		x->step = 0.;
		x->stride = 1.;
	}	

	 return(x);	
}


void  elips_calc (elips *x)
{
	double 	value;

	// calc current elipsle coords
	value = pi2-x->step*(pi2/x->steps);
	x->coords[0] = sin (value) * x->raio + x->center[0];
	x->coords[1] = cos (value) * x->raio2 + x->center[1];//updated raio2 4 ellipsoi
	
	// advance stride
	x->step += x->stride;
	if (x->step > x->steps)
		x->step = 0.0;
	if (x->step < 0.0f)
		x->step = x->steps;
}

void elips_center (elips *x, double echo, double ech)
{
	x->center[0] = echo;
	x->center[1] = ech;
	if (x->om)	// output do novo elipsulo apenas se nao ha silencio
		elips_bang(x);
}

void elips_move (elips *x, double echo, double ech)
{
	x->move[0] = echo;
	x->move[1] = ech;
	if (x->om)	// output do novo elipsulo apenas se nao ha silencio
		elips_bang(x);
}

void elips_raio (elips *x, double echo, double ech)
{
	x->raio = echo; //
	x->raio2= ech;
	if (x->om)	// output do novo elipsulo apenas se nao ha silencio
		elips_bang(x);
}

void elips_stride (elips *x, double echo)
{
	x->stride = echo; //
	if (x->om)	// output do novo elipsulo apenas se nao ha silencio
		elips_bang(x);
}

void elips_steps (elips *x, double echo)
{
	x->steps = echo; //
	if (x->steps<0.1)
		x->steps = 100.;
	if (x->om)	// output do novo elipsulo apenas se nao ha silencio
		elips_bang(x);
}

void elips_step (elips *x, double echo)
{
	x->step = echo; //
	if (x->step<0.1)
		x->step = 100.;
	if (x->om)	// output do novo elipsulo apenas se nao ha silencio
		elips_bang(x);
}

/*
void elips_post (elips *x)
{
post(" 	");
post(" 	");
post("  a-elips 0.3 20031114");
post(" 	");
post(" x-center    = %f", x->center[0]);
post(" y-center    = %f", x->center[1]);
post(" raio-x    = %f", x->raio);
post(" raio-y    = %f", x->raio2);
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
*/

void elips_echo (elips *x, double echo) //compute directly from number, no iter
{
	t_atom	*out;
	double 	value;

	out	= x->out;

	// calc current elipsle coords
	value = pi2-echo*(pi2); // dividing by 1.
	x->coords[0] = sin (value) * x->raio + x->center[0];
	x->coords[1] = cos (value) * x->raio2 + x->center[1];

		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy elipsular coords
}

void elips_int(elips *x, int n)     { elips_echo(x, n*0.01);}
void elips_float(elips *x, double n) { elips_echo(x, n);}


void elips_om (elips *x, long echo)
{
	x->om = echo;
}


void elips_bang (elips *x)
{
	t_atom	*out;
	out	= x->out;
	
	elips_calc(x);


		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy elipsular coords

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&elips_class,(method)elips_new,0L,(short)sizeof(elips),0L,
 A_GIMME,0);

 addbang((method)elips_bang);
 addmess((method)elips_raio,"raio",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)elips_center,"center",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)elips_move,"move",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)elips_stride,"stride",A_DEFFLOAT,0);
 addmess((method)elips_steps,"steps",A_DEFFLOAT,0);
 addfloat((method)elips_float);
 addint((method)elips_int);

 //addmess((method)elips_post,"post",0);

 addmess((method)elips_assist,"assist", A_CANT, 0);
// post("	 \t \a \t  %08xd	  			a-elips © A-Sier 2003"__TIME__,tick,0);

 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-elips   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void elips_assist(elips *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) elips xy    e         c            h              o"); }
}


