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

#ifndef RAND_MAX
#define RAND_MAX 	2147483647
#endif
#define ONE_OVER_RAND_MAX 1.0/RAND_MAX


void *gauss_noise_class;		// Required. Global pointing to this class

static long randx = 1;	// Seed

typedef struct _gauss_noise	// Data Structure for this object
{
	t_pxobject x_obj;
	int gauss_noise_mode;
//	float gauss_noise_b0, gauss_noise_b1, gauss_noise_b2, gauss_noise_b3, gauss_noise_b4, gauss_noise_b5, gauss_noise_b6; // Coefficients for the "pinking" filter
	int	gauss_param_q;
	float gauss_q, gauss_c1, gauss_c2, gauss_c3;

} t_gauss_noise;

// Prototypes for methods: need a method for each incoming message type
void *gauss_noise_new(long mode);							// New Object Creation Method
void gauss_param (t_gauss_noise *x, Symbol *msg, short argc, Atom *argv);
t_int *gauss_noise_perform(t_int *w);						// An MSP Perform (signal) Method
void gauss_noise_dsp(t_gauss_noise *x, t_signal **sp, short *count);		// DSP Method
void gauss_noise_assist(t_gauss_noise *x, void *b, long m, long a, char *s);	// Assistance Method
void gauss_noise_int(t_gauss_noise *x, long mode);					// Int Method
void gauss_noise_param(t_gauss_noise *x, long amount);					// Int Method
void gauss_noise_fparam(t_gauss_noise *x, float amount);					// Int Method
//void gauss_noise_free(t_gauss_noise *x);


/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&gauss_noise_class, (method)gauss_noise_new, (method)dsp_free, (short)sizeof(t_gauss_noise), 0L, A_DEFLONG, 0);

	addmess((method)gauss_noise_dsp, "dsp", A_CANT, 0);					// Bind method "gauss_noise_dsp" to the DSP call from MSP
	addmess((method)gauss_noise_assist, "assist", A_CANT, 0);				// Bind method "gauss_noise_assist" to assistance calls
	addint((method)gauss_noise_param);									// Bind int method to mode
	addfloat((method)gauss_noise_fparam);									// Bind int method to mode
 	post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-gaussnoise~   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
//	taptools_init();
}


/************************************************************************************/
// Object Creation Method

void *gauss_noise_new(long mode)
{
	t_gauss_noise *x = (t_gauss_noise *)newobject(gauss_noise_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

//	x->gauss_param_q = mode;					// Default Mode
	gauss_noise_param(x,mode);	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void gauss_noise_assist(t_gauss_noise *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// float Method
void gauss_noise_param(t_gauss_noise *x, long amount)
{
	x->gauss_param_q = amount;
	x->gauss_c1 = (1 << x->gauss_param_q) -1;
	x->gauss_c2 = ((int)(x->gauss_c1 / 3)) + 1;
	x->gauss_c3 = 1.f/x->gauss_c1;
}

// float Method
void gauss_noise_fparam(t_gauss_noise *x, float amount)
{
	x->gauss_q = amount;
	x->gauss_c1 = (1 << x->gauss_param_q) -1;
// 	x->gauss_c1 = ((float)1. << x->gauss_q) -1;

	x->gauss_c2 = ((int)(x->gauss_c1 / 3)) + 1;
	x->gauss_c3 = 1.f/x->gauss_c1;
}


/*
// Memory Deallocation
void gauss_noise_free(t_gauss_noise *x)
{
	notify_free((t_object *)x);
	dsp_free((t_pxobject *)x);
}
*/

// Perform (signal) Method
t_int *gauss_noise_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_gauss_noise *x = (t_gauss_noise *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	unsigned long i;					// used by random gen.
	float	random;
	float	c1 = x->gauss_c1;
	float	c2 = x->gauss_c2;
	float	c3 = x->gauss_c3;

			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			random = ((float)rand() * (float) ONE_OVER_RAND_MAX);/// (float)(RAND_MAX + 1));
			*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White gauss_noise
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void gauss_noise_dsp(t_gauss_noise *x, t_signal **sp, short *count)
{
//	x->gauss_noise_b0 = x->gauss_noise_b1 = x->gauss_noise_b2 = x->gauss_noise_b3 = x->gauss_noise_b4 = x->gauss_noise_b5 = x->gauss_noise_b6 = 0; 
	dsp_add(gauss_noise_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}