/*





a poisson trigger


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

void *poissont_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _poissont	// Data Structure for this object
{
	t_pxobject x_obj;
//	double poissont_b0, poissont_b1, poissont_b2, poissont_b3, poissont_b4, poissont_b5, poissont_b6; // Coefficients for the "pinking" filter

//	unsigned long randprev;
	
//	double min,max;
	double threshold,rate;
	long interp,interpmax,interpon;
	
	double source, target, delta, phase, phaseinc;
	
} t_poissont;

// Prototypes for methods: need a method for each incoming message type
void *poissont_new(double red);							// New Object Creation Method
t_int *poissont_perform(t_int *w);						// An MSP Perform (signal) Method
void poissont_dsp(t_poissont *x, t_signal **sp, short *count);		// DSP Method
void poissont_assist(t_poissont *x, void *b, long m, long a, char *s);	// Assistance Method

				
void poissont_rate(t_poissont *x,double m);		
void poissont_interp(t_poissont *x,long m);
			
//void poissont_free(t_poissont *x);
double Randomfloat11();
double Randomfloat12();//double Randomfloat11() ;
/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&poissont_class, (method)poissont_new, (method)dsp_free, (short)sizeof(t_poissont), 0L, A_DEFFLOAT, 0);

	addmess((method)poissont_dsp, "dsp", A_CANT, 0);					
	addmess((method)poissont_assist, "assist", A_CANT, 0);				
//	addint((method)poissont_param);									
	addfloat((method)poissont_rate);									

	addmess((method)poissont_interp, "interp",A_DEFLONG, 0);				


 	post("a-pt2~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *poissont_new(double red)
{
	t_poissont *x = (t_poissont *)newobject(poissont_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.1;
		
		
		x->source= x->target= x->delta= x->phase= x->phaseinc=0.0f;
		
		//x->interp=0;
		poissont_interp(x,1);
		
		poissont_rate(x,red);

	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void poissont_assist(t_poissont *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method

void poissont_rate(t_poissont *x,double m){
	x->rate=m;
	x->threshold=1.0-m;
	
}					


void poissont_interp(t_poissont *x,long m){
	x->interp=m;
	x->interpmax=m;
	x->interpon=0;
	
}					


// Perform (signal) Method
t_int *poissont_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_poissont *x = (t_poissont *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	
	double excitation=0.0f;
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{

			excitation = (Randomfloat12() > x->threshold) ? Randomfloat11():0.0f;


			if (excitation!=0.0) {
				x->interpon = 1;
					x->source=x->target;////0.;
					x->target=excitation;
					x->delta=excitation*0.5;//up&down
					x->phase = 0.0;
					x->phaseinc = 1.0/x->interp;
			}
			
			
			if(x->interpon) {
				if(x->phase>=0. && x->phase<=1.) {
					x->phase+=x->phaseinc;
					excitation = x->source + x->phase*x->delta;
					}

				if(x->phase>=1. && x->phase<=2.) {
					double val = 1.0f - ((x->phase+=x->phaseinc)-1.); // de 1 a 0
					excitation = x->source + val*x->delta;
					}
				
				
				if(x->phase>2.0) {
					x->interpon=0;
					x->phase-=2.0f;// = 0.;
				}
				
				
				*++out = excitation;
			
			} else {
				*++out = excitation;
			}
			
			
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void poissont_dsp(t_poissont *x, t_signal **sp, short *count)
{
//	x->poissont_b0 = x->poissont_b1 = x->poissont_b2 = x->poissont_b3 = x->poissont_b4 = x->poissont_b5 = x->poissont_b6 = 0; 
	dsp_add(poissont_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11() {
	double r = ((double)rand()) * (double)ONE_OVER_RAND_MAX;
	return(r*2-1);
}

double Randomfloat12() {
	return (((double)rand()) * (double)ONE_OVER_RAND_MAX);
//	returnr;
}


/* -1 to 2. ?
	double r1;
	
	x->randprev = RandomLCA(x->randprev);
	 r1 = ((double)x->randprev*ONE_OVER_RAND_MAX)*2.0f-1.0f;
	return r1;
*/	

