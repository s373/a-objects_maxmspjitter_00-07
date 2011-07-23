/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
lissa - © andrŽ sier 2003
		
		Max output of xy lissaular coords
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
	double			center[2];		// o centro do lissaulo
	double			raio;			// o raio
	double			raio2;			// o raio2
	double			move[2];		// deslocamos o centro ?
	double			steps;			// quantos passos por lissaulo
	double			step;			// em q passo estamos
	double			stride;			// a dist de passo a passo
	t_atom			out[2];			// nao preciso de mais q 2 de saida
	char			dir, 
					om;
					
	double			fx, fy;			//freq x e y
} lissa;


void *lissa_new (Symbol *msg, short argc, Atom *argv);
void lissa_bang (lissa *x);

void lissa_calc (lissa *x);
void lissa_center (lissa *x, double echo, double ech);
void lissa_move (lissa *x, double echo, double ech);
void lissa_raio (lissa *x, double echo, double ech);
void lissa_stride (lissa *x, double echo);
void lissa_steps (lissa *x, double echo);
void lissa_step (lissa *x, double echo);

void lissa_f (lissa *x, double echo, double ech);

void lissa_echo (lissa *x, double echo);
void lissa_int(lissa *x, int n);
void lissa_float(lissa *x, double n);


void lissa_mode (lissa *x, long echo);
void lissa_om (lissa *x, long echo);

void lissa_list (lissa *x, Symbol *msg, short argc, Atom *argv);


void lissa_post (lissa *x);

void lissa_assist(lissa *x, void *b, long m, long a, char *s);
void *lissa_class;


void *lissa_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 lissa *x;
	 int i;
	 
	 x=(lissa *)newobject(lissa_class);


	 x->c_out=listout(x);// xy lissa coords
	 

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

		x->fx = 0.5;
		x->fy = 5.;

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
		if(argv->a_w.w_sym == gensym("@freq")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->fx = argv->a_w.w_float;	argv++;
				x->fy = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->fx = (double)argv->a_w.w_long;	argv++;
				x->fy = (double)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@f")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->fx = argv->a_w.w_float;	argv++;
				x->fy = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->fx = (double)argv->a_w.w_long;	argv++;
				x->fy = (double)argv->a_w.w_long;	argv++;			
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
		x->fx = 0.5;
		x->fy = 5.;
	}	

	 return(x);	
}


void  lissa_calc (lissa *x)
{
	double 	valx, valy,
			fx = x->fx,
			fy = x->fy,
			step = x->step,
			steps = x->steps,
			stride = x->stride,
			raio = x->raio,
			raio2 = x->raio2,
			center[2];

	center[0] = x->center[0];
	center[1] = x->center[1];
	
	
/* eric singer lissajous func
 *
 * 		x = expr (f5 * cos(f2*2pi*(f1/f3)) + f4
 * f1 angulo que varre a fun‹o
 * f2 rate a que x se move
 * f3 numero de divisoes no circulo 
 * f4 offset
 * f5 scale
 
 	y = sin...
 */


	valx = fx * pi2 * (step/steps);
	valy = fy * pi2 * (step/steps);

	x->coords[0] = cos(valx)*raio + center[0];
	x->coords[1] = sin(valy)*raio2 + center[1];
	
	// advance step by stride
	step += stride;
		// see if

//	(stride > 0. ) ? ( if (step > steps) step = 0.0; ) :		\
//					 ( if (step < 0.0f) step = steps; )


/*	
	if (step > steps)
		step = 0.0;
	else if (step < 0.0f)
		step = steps;
*/
	x->step = step;
}

void lissa_center (lissa *x, double echo, double ech)
{
	x->center[0] = echo;
	x->center[1] = ech;
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}

void lissa_move (lissa *x, double echo, double ech)
{
	x->move[0] = echo;
	x->move[1] = ech;
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}

void lissa_raio (lissa *x, double echo, double ech)
{
	x->raio = echo; //
	x->raio2= ech;
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}

void lissa_stride (lissa *x, double echo)
{
	x->stride = echo; //
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}

void lissa_steps (lissa *x, double echo)
{
	x->steps = echo; //
	if (x->steps<0.1)
		x->steps = 100.;
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}

void lissa_step (lissa *x, double echo)
{
	x->step = echo; //
	if (x->step<0.1)
		x->step = 100.;
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}


void lissa_f (lissa *x, double echo, double ech)
{
	x->fx = echo;
	x->fy = ech;
	if (x->om)	// output do novo lissaulo apenas se nao ha silencio
		lissa_bang(x);
}


void lissa_echo (lissa *x, double echo) //compute directly from number, no iter
{
	t_atom	*out;
	double 	valx, valy,
			fx = x->fx,
			fy = x->fy,
			raio = x->raio,
			raio2 = x->raio2,
			center[2];

	center[0] = x->center[0];
	center[1] = x->center[1];

	out	= x->out;


	valx = fx * pi2 * (echo);
	valy = fy * pi2 * (echo);

	x->coords[0] = cos(valx)*raio + center[0];
	x->coords[1] = sin(valy)*raio2 + center[1];

		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);

	outlet_list(x->c_out, 0L, 2, out); //xy lissaular coords
}

void lissa_int(lissa *x, int n)     { lissa_echo(x, n*0.01);}
void lissa_float(lissa *x, double n) { lissa_echo(x, n);}


void lissa_om (lissa *x, long echo)
{
	x->om = echo;
}


void lissa_bang (lissa *x)
{
	t_atom	*out;
	out	= x->out;
	
	lissa_calc(x);


		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out, 0L, 2, out); //xy lissaular coords

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&lissa_class,(method)lissa_new,0L,(short)sizeof(lissa),0L,
 A_GIMME,0);

 addbang((method)lissa_bang);
 addmess((method)lissa_f,"freq",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)lissa_raio,"raio",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)lissa_center,"center",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)lissa_move,"move",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)lissa_stride,"stride",A_DEFFLOAT,0);
 addmess((method)lissa_steps,"steps",A_DEFFLOAT,0);
 addfloat((method)lissa_float);
 addint((method)lissa_int);

 addmess((method)lissa_assist,"assist", A_CANT, 0);
// post("	 \t \a \t  %08xd	  			a-lissa © A-Sier 2003"__TIME__,tick,0);

 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-lissa   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void lissa_assist(lissa *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) lissa xy    e         c            h              o"); }
}


