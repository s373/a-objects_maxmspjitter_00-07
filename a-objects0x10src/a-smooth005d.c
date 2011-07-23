/*

————————————————————————————————————————————————————————————————
a-smooth - © andré sier 2004
		
	one pole smoothing lowpass filter
	
	added smooth_up, smooth_down
	
	
	broken on intel??
————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"

//#include <math.h>

#define LSIZE	256


typedef struct	_smooth
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet	
	double in[LSIZE];
	double out[LSIZE];
	double smooth[2];
	t_atom	outatom[LSIZE];
	long	count;

} smooth;



void *smooth_new (Symbol *msg, short argc, Atom *argv);
void smooth_calc (smooth *x); //exec rotation
void smooth_bang (smooth *x); //exec rotation
void smooth_setsmooth (smooth *x,  Symbol *s, short ac, Atom *av); //input the args 
void smooth_float (smooth *x,double f32in); //input the args 
void smooth_int (smooth *x, int t); //input the args 
void smooth_list(smooth *x, Symbol *s, short ac, Atom *av);
void smooth_post(smooth *x, Symbol *s, short ac, Atom *av);

void smooth_slide_up (smooth *x,float t); //input the args 
void smooth_slide_down (smooth *x,float t); //input the args 

void smooth_assist(smooth *x, void *b, long m, long a, char *s);
void *smooth_class;




void *smooth_new (Symbol *msg, short argc, Atom *argv)//input the args 
{

 smooth *x;
	long i;
 
 x=(smooth *)newobject(smooth_class);

	for (i=0; i<LSIZE; i++) {
		x->in[i] =0.;
		x->out[i] =0.;
		x->outatom[i].a_w.w_float =0.;
	}

	if (argc) { 
		smooth_setsmooth(x, msg, argc, argv);
	} else if (!argc)  {
		 x->smooth[0] = 0.5;
		 x->smooth[1] = 0.9;
	}	

 x->c_out = listout(x);


 return(x);	
}

void smooth_setsmooth (smooth *x, Symbol *s, short ac, Atom *av)
{
	short i;
	double temp[2];


	for (i=0; i<ac; i++) {
		 switch (av[i].a_type) {
			case A_FLOAT:
				temp[i] = (double)av[i].a_w.w_float; //float, silly
				break;
			case A_LONG:
				temp[i] = (double)av[i].a_w.w_long;
				break;
			case A_SYM:
				error("no syms in this smooth method");
				break;
			}

		}

	if(ac==1) {
		x->smooth[0]=x->smooth[1]=temp[0];
	} else {
		x->smooth[0]=temp[0];
		x->smooth[1]=temp[1];
		
	}

							

//	post("smoothes are %f and %f",x->smooth[0], x->smooth[1]);

	

}

void smooth_float (smooth *x,double f32in)
{
	double in = x->in[0];
	double out = x->out[0], temp;	
	double lp1, lp2 ;
	
	if(in<out)
		lp2=x->smooth[1];
	else if (in>=out)
		lp2=x->smooth[0];
	
	lp1 = 1.-lp2;
	
	x->in[0] = (double)f32in;

	temp = in*lp1;
	temp+= out*lp2;
	x->out[0] = temp;

	outlet_float (x->c_out, x->out[0]);
	x->count = 1;
}

void smooth_int (smooth *x, int t)
{
	double in = x->in[0];
	double out = x->out[0], temp;	
	double lp1, lp2 ;

	if(in<out)
		lp2=x->smooth[1];
	else if (in>=out)
		lp2=x->smooth[0];

	lp1 = 1.-lp2;
	
	x->in[0] = (double)t;

	temp = in*lp1;
	temp+= out*lp2;
	x->out[0] = temp;

	outlet_float (x->c_out, x->out[0]);
	x->count = 1;
}

void smooth_post(smooth *x, Symbol *s, short ac, Atom *av)
{
	long i, count = x->count;


	post("smoothes are %f and %f",x->smooth[0], x->smooth[1]);


	for (i=0; i<count; i++) {
		post ("in_list[%ld]: %f", i, x->in[i]);
	}	
	for (i=0; i<count; i++) {
		post ("out_list[%ld]: %f", i, x->out[i]);
	}	

	for (i=0; i<count; i++) {
		post ("atom_list[%ld]: %f", i, x->outatom[i].a_w.w_float);
	}	
}


void smooth_slide_up (smooth *x,float t)
{
	x->smooth[0]=t;
}

void smooth_slide_down (smooth *x,float t)
{
	x->smooth[1]=t;
}


void smooth_list(smooth *x, Symbol *s, short ac, Atom *av)
{
	long i;
	double temp;
	double lp1, lp2;
	t_atom *ot;
	
	ot = x->outatom;


	for (i=0; i<ac; i++) {

	 switch (av[i].a_type) {
		case A_FLOAT:
			x->in[i] = (double)av[i].a_w.w_float; //float, silly
			break;
		case A_LONG:
			x->in[i] = (double)av[i].a_w.w_long;
			break;
		case A_SYM:
			error("no syms in this list");
			break;
		}


		if( x->in[i] < x->out[i] )
			lp2=x->smooth[1];
		else if  (x->in[i] >= x->out[i] )
			lp2=x->smooth[0];

		lp1 = 1.-lp2;
		
		temp = x->in[i]*lp1;
		temp+= x->out[i]*lp2;
		x->out[i] = temp;

		SETFLOAT (ot+i, x->out[i]);

	}

//	for (i=0; i<ac; i++) {
//		SETFLOAT (ot+i, x->out[i]);
//	}

	x->count = ac;
	outlet_list(x->c_out, 0L, ac, ot);
}




void smooth_bang (smooth *x)
{

	t_atom *ot;
	long i;
	long count = x->count;
	double temp;	
	double lp1, lp2 ;

	ot = x->outatom;

	
	for(i=0; i<count; i++) {

		if( x->in[i] < x->out[i] )
			lp2=x->smooth[1];
		else if  (x->in[i] >= x->out[i] )
			lp2=x->smooth[0];

		lp1 = 1.-lp2;

		temp = x->in[i]*lp1;
		temp+= x->out[i]*lp2;
		x->out[i] = temp;	
		SETFLOAT (ot+i, x->out[i]);
	}

	outlet_list(x->c_out, 0L, count, ot);

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&smooth_class,(method)smooth_new,0L,(short)sizeof(smooth),0L,
 A_GIMME,0);
 
 addbang((method)smooth_bang);
 addfloat((method)smooth_float);
 addint((method)smooth_int);
 addmess((method)smooth_list, "list", A_GIMME, 0);
// addmess((method)smooth_post, "post", A_GIMME, 0);
 addmess((method)smooth_setsmooth, "smooth", A_GIMME, 0);
 addmess((method)smooth_slide_up, "slide_up", A_FLOAT, 0);
 addmess((method)smooth_slide_down, "slide_down", A_FLOAT, 0);
 addmess((method)smooth_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-smooth   ©   a n d r é s i e r   2 0 0 4   all rights reserved",tick, 0);
//new post method 20040124
}


void smooth_assist(smooth *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) smooth     e         c            h              o"); }
 //   else if (m==2&&a==1) { sprintf(s,"(float) y       e             c                h                   o"); }
}


