/*

————————————————————————————————————————————————————————————————

a-delta - © andré sier 2004
		
	output the difference from last input
	x[n] - x[n-1]
	
	om 0 _ delta normal
	om 1 _ delta onde um argumento é fixo


————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"

//#include <math.h>

#define LSIZE	256

typedef struct	_delta
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet	
	double in[LSIZE];
	double out[LSIZE];
	double in2[LSIZE];
	t_atom	outatom[LSIZE];
	long	count;

	short	om, t;
} delta;



void *delta_new (Symbol *s, short ac, Atom *av);
void delta_om (delta *x, short t); //input the args 
void delta_calc (delta *x); //exec rotation
void delta_bang (delta *x); //exec rotation
void delta_delta (delta *x, double t); //input the args 
void delta_float (delta *x, double t); //input the args 
void delta_int (delta *x, double t); //input the args 
void delta_list(delta *x, Symbol *s, short ac, Atom *av);
void delta_reset(delta *x, Symbol *s, short ac, Atom *av);
void delta_post(delta *x, Symbol *s, short ac, Atom *av);

void delta_assist(delta *x, void *b, long m, long a, char *s);
void *delta_class;



void delta_om (delta *x, short t) { x->om = (t>0); }

void *delta_new (Symbol *s, short ac, Atom *av) //input the args 
{

 delta *x;
	long i;
 
 x=(delta *)newobject(delta_class);

	for (i=0; i<LSIZE; i++) {
		x->in[i] =0.;
		x->in2[i] =0.;
		x->out[i] =0.;
		x->outatom[i].a_w.w_float =0.;
	}

	x->om = 0;

 x->c_out = listout(x);

	if (ac) {
		if (ac>1) {
		 delta_list(x, s, ac, av);
//			post("ac>1");
		}
		else if(ac==1){
//		post("ac==1");
		 switch (av[0].a_type) { // has to be 0!
			case A_FLOAT: delta_om (x, (short)av[0].a_w.w_float);break;
			case A_LONG: delta_om (x, (short)av[0].a_w.w_long); break;
			}
		}
	
	}
		


 return(x);	
}


void delta_float (delta *x, double t)
{
	double out = x->out[0], temp, in, in2;
	short tog = x->t;	
	
	if(tog) {
		x->in[0] = t;
	
	} else if (!tog) {
		x->in2[0] = t;
	}

	in = x->in[0];
	in2= x->in2[0];

	if(tog) {
		temp = in-in2;
	
	} else if (!tog) {
		temp = in2-in;
	}

	x->out[0] = temp;

	outlet_float (x->c_out, temp);
	x->count = 1;
	x->t = !tog;
}

void delta_int (delta *x, double t)
{
	double out = x->out[0], temp, in, in2;
	short tog = x->t;	
	
	if(tog) {
		x->in[0] = t;
	
	} else if (!tog) {
		x->in2[0] = t;
	}

	in = x->in[0];
	in2= x->in2[0];

	if(tog) {
		temp = in-in2;
	
	} else if (!tog) {
		temp = in2-in;
	}

	x->out[0] = temp;
	
	outlet_float (x->c_out, temp);
	x->count = 1;

	if (!x->om)
		x->t = !tog;
	else if (x->om) {
		x-> t = 1;
	}
}

void delta_list(delta *x, Symbol *s, short ac, Atom *av)
{
	long i;
	double temp;
	t_atom *ot;
	short t = x->t;
	
	ot = x->outatom;



	if (t) {

		for (i=0; i<ac; i++) {
		 switch (av[i].a_type) {
			case A_FLOAT:
				x->in[i] = (double)av[i].a_w.w_float; 
				break;
			case A_LONG:
				x->in[i] = (double)av[i].a_w.w_long;
				break;
			case A_SYM:
				error("no syms in this list");
				break;
			}
		}

	 } else if (!t) {

		for (i=0; i<ac; i++) {
		 switch (av[i].a_type) {
			case A_FLOAT:
				x->in2[i] = (double)av[i].a_w.w_float; 
				break;
			case A_LONG:
				x->in2[i] = (double)av[i].a_w.w_long;
				break;
			case A_SYM:
				error("no syms in this list");
				break;
			}

		}
	}



	if (t) {
		for (i=0; i<ac; i++) {
			temp = x->in[i] - x->in2[i]; // get xn - xn-1
			SETFLOAT (ot+i, temp); // set it in the output vector
		}
	} else 	if (!t) {
		for (i=0; i<ac; i++) {
			temp = x->in2[i] - x->in[i]; // get xn - xn-1
			SETFLOAT (ot+i, temp); // set it in the output vector
		}
	}




	x->count = ac;



	if (!x->om)
		x->t = !t;
	else if (x->om) {
		x-> t = 1;
	}


	outlet_list(x->c_out, 0L, ac, ot);
}

void delta_reset(delta *x, Symbol *s, short ac, Atom *av)
{
	long i;
	double temp;
	t_atom *ot;
	short t ;
	
	
	x->t = 0;
	
	t = x->t;
	
	
	
	ot = x->outatom;



	if (ac) {

		if (t) {

			for (i=0; i<ac; i++) {
			 switch (av[i].a_type) {
				case A_FLOAT:
					x->in[i] = (double)av[i].a_w.w_float; 
					break;
				case A_LONG:
					x->in[i] = (double)av[i].a_w.w_long;
					break;
				case A_SYM:
					error("no syms in this list");
					break;
				}
			}

		 } else if (!t) {

			for (i=0; i<ac; i++) {
			 switch (av[i].a_type) {
				case A_FLOAT:
					x->in2[i] = (double)av[i].a_w.w_float; 
					break;
				case A_LONG:
					x->in2[i] = (double)av[i].a_w.w_long;
					break;
				case A_SYM:
					error("no syms in this list");
					break;
				}

			}
		}



		if (t) {
			for (i=0; i<ac; i++) {
				temp = x->in[i] - x->in2[i]; // get xn - xn-1
				SETFLOAT (ot+i, temp); // set it in the output vector
			}
		} else 	if (!t) {
			for (i=0; i<ac; i++) {
				temp = x->in2[i] - x->in[i]; // get xn - xn-1
				SETFLOAT (ot+i, temp); // set it in the output vector
			}
		}




	x->count = ac;

	x->t = 1; // if arg c > 1 , we've set our init point as we'd like to
	outlet_list(x->c_out, 0L, ac, ot);

	}



}




void delta_bang (delta *x)
{

	t_atom *ot;
	long i;
	long count = x->count;

	ot = x->outatom;

	
	for(i=0; i<count; i++) {
		SETFLOAT (ot+i, x->out[i]);
	}

	outlet_list(x->c_out, 0L, count, ot);

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&delta_class,(method)delta_new,0L,(short)sizeof(delta),0L,
 A_GIMME,0);
 
 addbang((method)delta_bang);
 addfloat((method)delta_float);
 addint((method)delta_int);
 addmess((method)delta_list, "list", A_GIMME, 0);
 addmess((method)delta_om, "om", A_LONG, 0);
 addmess((method)delta_reset, "reset", A_GIMME, 0);
 addmess((method)delta_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-delta   ©   a n d r é s i e r   2 0 0 4   all rights reserved",tick, 0);
//new post method 20040124
}


void delta_assist(delta *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) delta     e         c            h              o"); }
 //   else if (m==2&&a==1) { sprintf(s,"(double) y       e             c                h                   o"); }
}


