/*

random walk in msp~
*/

#include "ext.h"
#include "ext_common.h"
#include "z_dsp.h"
#include <math.h>
//#include
#include <stdlib.h> // for rand calling
//#define RAND_MAX 	32767
#ifndef RAND_MAX
#define RAND_MAX 	2147483647
#endif
#define ONE_OVER_RAND_MAX 1.0/RAND_MAX
//#define	rndmax	2147483647

void *random_walk_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _random_walk	// Data Structure for this object
{
	t_pxobject x_obj;
//	double random_walk_b0, random_walk_b1, random_walk_b2, random_walk_b3, random_walk_b4, random_walk_b5, random_walk_b6; // Coefficients for the "pinking" filter

//	unsigned long randprev;
	double rndwalkmax;	
	double rndwalkmin;
	double val; //actual sample val
	double min, max;
	double friction;
} t_random_walk;

// Prototypes for methods: need a method for each incoming message type
void *random_walk_new(double red);							// New Object Creation Method
t_int *random_walk_perform(t_int *w);						// An MSP Perform (signal) Method
void random_walk_dsp(t_random_walk *x, t_signal **sp, short *count);		// DSP Method
void random_walk_assist(t_random_walk *x, void *b, long m, long a, char *s);	// Assistance Method

void random_walk_min(t_random_walk *x,double m);					
void random_walk_max(t_random_walk *x,double m);

void random_walk_rndwalkmin(t_random_walk *x,double m);					
void random_walk_rndwalkmax(t_random_walk *x,double m);	
void random_walk_rndwalk(t_random_walk *x,double m);	
void random_walk_friction(t_random_walk *x,double m);

//void random_walk_free(t_random_walk *x);
double Randomfloat11(t_random_walk *x);//double Randomfloat11() ;
double Randomwalk(t_random_walk *x);
/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&random_walk_class, (method)random_walk_new, (method)dsp_free, (short)sizeof(t_random_walk), 0L, A_DEFFLOAT, 0);

	addmess((method)random_walk_dsp, "dsp", A_CANT, 0);					
	addmess((method)random_walk_assist, "assist", A_CANT, 0);				
//	addint((method)random_walk_param);									
//	addfloat((method)random_walk_setphaseinc);									

	addmess((method)random_walk_min, "min",A_DEFFLOAT, 0);				
	addmess((method)random_walk_max, "max",A_DEFFLOAT, 0);				
	addmess((method)random_walk_rndwalk, "rndwalk",A_DEFFLOAT, 0);				
	addmess((method)random_walk_rndwalkmin, "rndwalkmin",A_DEFFLOAT, 0);				
	addmess((method)random_walk_rndwalkmax, "rndwalkmax",A_DEFFLOAT, 0);				
	addmess((method)random_walk_friction, "friction",A_DEFFLOAT, 0);				

//	addmess((method)random_walk_reset, "reset", 0);				
//	addmess((method)random_walk_setphasetime, "phasetime", A_DEFFLOAT, 0);				// Bind method "random_walk_assist" to assistance calls
//	addmess((method)random_walk_setphaseinc, "phaseinc", A_DEFFLOAT, 0);				// Bind method "random_walk_assist" to assistance calls
 	post("a-randomwalk~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *random_walk_new(double red)
{
	t_random_walk *x = (t_random_walk *)newobject(random_walk_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.01;

//	post("loadd with %f",red);
	x->min = -1.0f;
	x->max = 1.0f;
	x->rndwalkmin=-0.001;
	x->rndwalkmax=+0.001;
	random_walk_rndwalk(x,red);//x->val = red;
	x->friction = 1.;
		
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void random_walk_assist(t_random_walk *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method

void random_walk_min(t_random_walk *x,double m){x->min=m;}					
void random_walk_max(t_random_walk *x,double m){x->max=m;}					
void random_walk_rndwalkmin(t_random_walk *x,double m){x->rndwalkmin=m;}					
void random_walk_rndwalkmax(t_random_walk *x,double m){x->rndwalkmax=m;}
void random_walk_friction(t_random_walk *x,double m){x->friction=m;}
void random_walk_rndwalk(t_random_walk *x,double m) {
	x->rndwalkmin=-1.*m;
	x->rndwalkmax=m;
}
// Perform (signal) Method
t_int *random_walk_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_random_walk *x = (t_random_walk *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			

			*++out = x->val = Randomwalk(x);//float11(x);
			//x->val = *out;
			//randprev = x->randprev = RandomLCA(x->randprev);
			//random = randprev*ONE_OVER_RAND_MAX;
			//*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White random_walk
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void random_walk_dsp(t_random_walk *x, t_signal **sp, short *count)
{
//	x->random_walk_b0 = x->random_walk_b1 = x->random_walk_b2 = x->random_walk_b3 = x->random_walk_b4 = x->random_walk_b5 = x->random_walk_b6 = 0; 
	dsp_add(random_walk_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11(t_random_walk *x) {
	double delta;
	double r = ((double)rand()) * (double)ONE_OVER_RAND_MAX;

	delta = x->max-x->min;

	return(r*delta+x->min);

/* -1 to 2. ?
	double r1;
	
	x->randprev = RandomLCA(x->randprev);
	 r1 = ((double)x->randprev*ONE_OVER_RAND_MAX)*2.0f-1.0f;
	return r1;
*/	
}

double Randomwalk(t_random_walk *x) {
	double delta,accum,min,max;
	double r = ((double)rand()) * (double)ONE_OVER_RAND_MAX;
	double friction=x->friction;

	min = x->min;
	max = x->max;
	
	delta = x->rndwalkmax-x->rndwalkmin;

//	accum = r*delta + min;



	accum = x->val + (r*delta+min);
	
	if(accum<min){
		accum=min+(min-accum)*friction;
//		if(accum²²²²²
		}
	if(accum>max)
		accum=max-(accum-max)*friction;
		
	return accum;//x->val=accum;

}
