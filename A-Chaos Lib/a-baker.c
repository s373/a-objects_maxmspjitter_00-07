/*

————————————————————————————————————————————————————————————————
a-baker - © andré sier 2004
	
a port of Richard's chaos-baker from chaos collection
————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"

#include <math.h>


typedef struct	_baker
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet	
	double init, eval;
	Boolean  fold_cut, om;
} baker;



void *baker_new (Symbol *msg, short argc, Atom *argv);
void baker_calc (baker *x); 
void baker_bang (baker *x); 
void baker_set (baker *x, Symbol *msg, short argc, Atom *argv); 
void baker_reset (baker *x, Symbol *msg, short argc, Atom *argv); 

void baker_init (baker *x, double max);
void baker_eval (baker *x, double max);
void baker_fold (baker *x, double max);
void baker_cut  (baker *x, double max);
void baker_fold_cut  (baker *x, int max);

void baker_assist(baker *x, void *b, long m, long a, char *s);
void *baker_class;




void *baker_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
 baker *x;
 int i;
 
 x=(baker *)newobject(baker_class);

 x->c_out=floatout(x);

 //init
	x->init = 0.85f;
	x->eval = 0.85f;
	x->fold_cut = 0;
	x->om = 0;
		
	baker_set(x, msg, argc, argv); // & pass them

 return(x);	
}

void baker_set (baker *x, Symbol *msg, short argc, Atom *argv) //input the args 
{
	
	if (argc) {

		if (argc > 1) 
		{
			if (argv[1].a_type == A_FLOAT)
				x->init = fmod(argv[1].a_w.w_float, 1.);
			if (argv[1].a_type == A_LONG)
				x->fold_cut = argv[1].a_w.w_long;
				
			x->eval = x->init;
		}
		if (argc > 0) 
		{
			if (argv[0].a_type == A_FLOAT)
				x->init = fmod(argv[0].a_w.w_float, 1.);
			if (argv[0].a_type == A_LONG)
				x->fold_cut = argv[0].a_w.w_long;
				
			x->eval = x->init;
		}





/*  if init is _not_ init of eval
		if (argc==3) {
		
			if (argv[2].a_type == A_LONG)
				x->fold_cut = argv[2].a_w.w_long;
			else if (argv[2].a_type == A_FLOAT)
				x->fold_cut = (long) argv[2].a_w.w_float;


			if (argv[1].a_type == A_LONG)
				x->fold_cut = argv[1].a_w.w_long;
			else if (argv[1].a_type == A_FLOAT)
				x->eval = (double) fmod(argv[1].a_w.w_float, 1.);

			if (argv[0].a_type == A_LONG)
				x->fold_cut = argv[0].a_w.w_long;	//fold_cut
			else if (argv[0].a_type == A_FLOAT) 
				x->init = (double) fmod(argv[0].a_w.w_float, 1.); //init

		}

		if (argc==2) {

			if (argv[1].a_type == A_LONG)
				x->fold_cut = argv[1].a_w.w_long;
			else if (argv[1].a_type == A_FLOAT)
				x->eval = (double) fmod(argv[1].a_w.w_float, 1.);

			if (argv[0].a_type == A_LONG)
				x->fold_cut = argv[0].a_w.w_long;	//fold_cut
			else if (argv[0].a_type == A_FLOAT) 
				x->init = (double) fmod(argv[0].a_w.w_float, 1.); //init

		}

		if (argc==1) {
		
			if (argv[0].a_type == A_LONG)
				x->fold_cut = argv[0].a_w.w_long;	//fold_cut
			else if (argv[0].a_type == A_FLOAT) {
				x->init = (double) fmod(argv[0].a_w.w_float, 1.); //init
				x->eval = x->init; // only if 1 arg is float eval equals init
				}
		}

*/

	} //end if args
}

void baker_reset (baker *x, Symbol *msg, short argc, Atom *argv)
{
 //init
	x->eval = x->init;

}


void baker_bang (baker *x)
{
	outlet_float (x->c_out, x->eval);

	if 	(!x->fold_cut)    /* fold */
	{
		if (x->eval > 0.5)
			x->eval = 2. - (x->eval*2.);
		else
			x->eval *= 2.;
	}
	else					/* cut */
	{
		if (x->eval < 0.5)
			x->eval *= 2.;
		else
			x->eval = (x->eval*2.) - 1.;
	}
}

void baker_eval (baker *x, double max)
{
	x->eval = max;
	x->init = max;
	if(x->om) baker_bang(x);
}
void baker_fold (baker *x, double max)
{
	x->fold_cut = 0;
	x->init = fmod(max, 1.);
	x->eval = x->init;
	if(x->om) baker_bang(x);
}
void baker_cut (baker *x, double max)
{
	x->fold_cut = 1;
	x->init = fmod(max, 1.);
	x->eval = x->init;
	if(x->om) baker_bang(x);
}
void baker_fold_cut  (baker *x, int max)
{
	x->fold_cut = (max>0);
	if(x->om) baker_bang(x);
}
void baker_om  (baker *x, int max)
{
	x->om = (max>0);
}


void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&baker_class,(method)baker_new,0L,(short)sizeof(baker),0L,
 A_GIMME,0);
 
 addbang((method)baker_bang);
 addmess((method)baker_set, "set", A_GIMME, 0);
 addmess((method)baker_reset, "reset", A_GIMME, 0);
 addmess((method)baker_eval, "seed", A_FLOAT, 0);
 addmess((method)baker_fold, "fold", A_FLOAT, 0);
 addmess((method)baker_cut, "cut", A_FLOAT, 0);
 addmess((method)baker_fold_cut, "fold_cut", A_LONG, 0);
 addmess((method)baker_om, "om", A_LONG, 0);
 addmess((method)baker_assist,"assist", A_CANT, 0);
 post("A-Chaos Lib :: a-baker  " __DATE__" "__TIME__"                                   ©   a n d r é s i e r   2 0 0 4   all rights reserved",tick, 0);
}


void baker_assist(baker *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(float) baker     e         c            h              o"); }
 //   else if (m==2&&a==1) { sprintf(s,"(float) y baker     e             c                h                   o"); }
}


