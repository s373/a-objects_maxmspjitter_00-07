/*





	a-gaussnoise~

	gaussian noise generation


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

void *red_noise_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _red_noise	// Data Structure for this object
{
	t_pxobject x_obj;
//	double red_noise_b0, red_noise_b1, red_noise_b2, red_noise_b3, red_noise_b4, red_noise_b5, red_noise_b6; // Coefficients for the "pinking" filter

//	unsigned long randprev;
	
	double phase, phaseinc, source, target;
	double delta;

	double min, max;
} t_red_noise;

// Prototypes for methods: need a method for each incoming message type
void *red_noise_new(double red);							// New Object Creation Method
t_int *red_noise_perform(t_int *w);						// An MSP Perform (signal) Method
void red_noise_dsp(t_red_noise *x, t_signal **sp, short *count);		// DSP Method
void red_noise_assist(t_red_noise *x, void *b, long m, long a, char *s);	// Assistance Method

void red_noise_reset(t_red_noise *x);	
void red_noise_setphaseinc(t_red_noise *x,double phasei);
void red_noise_setphasetime(t_red_noise *x,double millis);					
void red_noise_min(t_red_noise *x,double m);					
void red_noise_max(t_red_noise *x,double m);					
//void red_noise_free(t_red_noise *x);
unsigned long RandomLCA(unsigned long previous);
double Randomfloat11(t_red_noise *x);//double Randomfloat11() ;
/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&red_noise_class, (method)red_noise_new, (method)dsp_free, (short)sizeof(t_red_noise), 0L, A_DEFFLOAT, 0);

	addmess((method)red_noise_dsp, "dsp", A_CANT, 0);					
	addmess((method)red_noise_assist, "assist", A_CANT, 0);				
//	addint((method)red_noise_param);									
	addfloat((method)red_noise_setphaseinc);									

	addmess((method)red_noise_min, "min",A_DEFFLOAT, 0);				
	addmess((method)red_noise_max, "max",A_DEFFLOAT, 0);				

	addmess((method)red_noise_reset, "reset", 0);				
//	addmess((method)red_noise_setphasetime, "phasetime", A_DEFFLOAT, 0);				// Bind method "red_noise_assist" to assistance calls
//	addmess((method)red_noise_setphaseinc, "phaseinc", A_DEFFLOAT, 0);				// Bind method "red_noise_assist" to assistance calls
 	post("a-rednoise~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *red_noise_new(double red)
{
	t_red_noise *x = (t_red_noise *)newobject(red_noise_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.1;

//	post("loadd with %f",red);
	red_noise_reset(x);

	x->phaseinc = red;
//	x->min = -1.0f;
//	x->max = 1.0f;
	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void red_noise_assist(t_red_noise *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method

void red_noise_reset(t_red_noise *x){
	x->phase=0.; 
	x->phaseinc=0.1;
	x->source=Randomfloat11(x);
	x->target=Randomfloat11(x);
	x->delta = x->target-x->source;
	
	x->min=-1.0f;
	x->max=1.0f;
//	post("srctar %f %f %f",x->source,x->target,x->delta);
}


void red_noise_setphaseinc(t_red_noise *x,double phasei){
	if(phasei||phasei==0.)
		x->phaseinc = phasei;
//	post("phaseinc %f",x->phaseinc);
}


void red_noise_min(t_red_noise *x,double m){x->min=m;}					
void red_noise_max(t_red_noise *x,double m){x->max=m;}					

void red_noise_setphasetime(t_red_noise *x,double millis){
	double sr = sys_getsr();//double sys_getsr(void);			// returns current sampling rate
//	double to = 1.0f / (millis * 0.001 * sr); 
//	double samples = (millis * sr)/1000.;// *0.01 * sr;
	
	//millis is seconds!
	double samples = (millis * sr * 0.01);// *0.01 * sr;	
	if(samples==0.)
		return;
	x->phaseinc = 1.0f/samples;// 0.1 inc is 10 samples//1.0f/samples;
	
//	x->phaseinc = to;//Mmillis / sr;
	post("phaseinc %f %f",x->phaseinc,sr);
}

/*
// Memory Deallocation
void red_noise_free(t_red_noise *x)
{
	notify_free((t_object *)x);
	dsp_free((t_pxobject *)x);
}
*/

// Perform (signal) Method
t_int *red_noise_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_red_noise *x = (t_red_noise *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	
	double phase, phaseinc;//, source, target;
//	double delta;
	
	phaseinc = x->phaseinc;

	
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			
			phase = x->phase+=phaseinc;
			
			if(phase>=1.0) {
					x->source = x->target;
					x->target = Randomfloat11(x);
					x->delta = x->target-x->source;
					x->phase -= 1.0f;		
				//	post("phasesync");			
			}

			*++out = x->source + (phase*x->delta);
			
			//randprev = x->randprev = RandomLCA(x->randprev);
			//random = randprev*ONE_OVER_RAND_MAX;
			//*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White red_noise
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void red_noise_dsp(t_red_noise *x, t_signal **sp, short *count)
{
//	x->red_noise_b0 = x->red_noise_b1 = x->red_noise_b2 = x->red_noise_b3 = x->red_noise_b4 = x->red_noise_b5 = x->red_noise_b6 = 0; 
	dsp_add(red_noise_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11(t_red_noise *x) {
	double delta,off;
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
