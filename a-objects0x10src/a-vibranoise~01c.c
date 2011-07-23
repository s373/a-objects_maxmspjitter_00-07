/*
vibra noise is red noise that ensures vibration of strings:)

friction is the key!!!
http://search.cpan.org/~smueller/Physics-Springs-Friction-1.01/lib/Physics/Springs/Friction.pm


On Stokes' Friction

The formula to calculate Stokes' friction of a sphere of radius r is

Force = 6 * Pi * r * eta * velocity Trivially, the force is antiparallel
to the velocity. Stokes' friction is usually applied when a rather small
object moves rather slowly through a fluid or gas of viscosity eta.

In the Physics::Springs::Friction implementation, the friction is simply
computed as

Force = magnitude * velocity

Hence: magnitude = 6 * Pi * r * eta (for small spheres) The magnitude is
passed as the second argument to the add_friction() method. This
behaviour was chosen to allow for flexibility concerning geometry and
viscosity.


On Newtonian Friction

The formula to calculate Newtonian friction of a body in a fluid:

Force = 1/2 * c * rho * area * velocity^2 rho is the density of the
fluid, the area is the projected area of the body in direction of
movement, and c is coefficient that is determined by the body's
geometry. For hydro-dynamically good geometries: c < 1. For spheres: c =
about 1. For hydro-dynamically inefficient geometries: c > 1.

Newtonian friction is usually applied in cases of high velocity and
bodies of significant size. As with Stokes' friction, all constants are
summed up into the magnitude property of the friction as follows:

Force = magnitude * velocity^2 magnitude = 0.5*c*rho*area

  
  
*/

#include "ext.h"
#include "ext_common.h"
#include "z_dsp.h"
#include <math.h>

#include <stdlib.h> // for rand calling
#ifndef RAND_MAX
#define RAND_MAX 	2147483647
#endif
#define ONE_OVER_RAND_MAX 1.0/RAND_MAX

#ifndef M_PI
#define M_PI 	3.14159265358979323846
#endif



void *vibra_noise_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _vibra_noise	// Data Structure for this object
{
	t_pxobject x_obj;
//	double vibra_noise_b0, vibra_noise_b1, vibra_noise_b2, vibra_noise_b3, vibra_noise_b4, vibra_noise_b5, vibra_noise_b6; // Coefficients for the "pinking" filter

//	unsigned long randprev;
	
	double phase, phaseinc, source, target;
	double delta;

	double min, max;
	
	long vibra, vibracount;
	short side; //var to toggle string sides
	double frequency;
} t_vibra_noise;

// Prototypes for methods: need a method for each incoming message type
void *vibra_noise_new(double red);							// New Object Creation Method
t_int *vibra_noise_perform(t_int *w);						// An MSP Perform (signal) Method
void vibra_noise_dsp(t_vibra_noise *x, t_signal **sp, short *count);		// DSP Method
void vibra_noise_assist(t_vibra_noise *x, void *b, long m, long a, char *s);	// Assistance Method

void vibra_noise_reset(t_vibra_noise *x);	
void vibra_noise_setphaseinc(t_vibra_noise *x,double phasei);
void vibra_noise_setphasetime(t_vibra_noise *x,double millis);					
void vibra_noise_min(t_vibra_noise *x,double m);					
void vibra_noise_max(t_vibra_noise *x,double m);	
void vibra_noise_vibra(t_vibra_noise *x,long m);					
//void vibra_noise_free(t_vibra_noise *x);
unsigned long RandomLCA(unsigned long previous);
double Randomfloat11(t_vibra_noise *x);//double Randomfloat11() ;
double Randomfloat10(t_vibra_noise *x);
/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&vibra_noise_class, (method)vibra_noise_new, (method)dsp_free, (short)sizeof(t_vibra_noise), 0L, A_DEFFLOAT, 0);

	addmess((method)vibra_noise_dsp, "dsp", A_CANT, 0);					
	addmess((method)vibra_noise_assist, "assist", A_CANT, 0);				
//	addint((method)vibra_noise_param);									
	addfloat((method)vibra_noise_setphasetime);//vibra_noise_setphaseinc);									

	addmess((method)vibra_noise_min, "min",A_DEFFLOAT, 0);				
	addmess((method)vibra_noise_max, "max",A_DEFFLOAT, 0);				
//	addmess((method)vibra_noise_setphasetime, "frequency",A_DEFFLOAT, 0);
		addmess((method)vibra_noise_vibra, "vibra",A_DEFLONG, 0);				

	addmess((method)vibra_noise_reset, "reset", 0);				
 	post("a-vibranoise~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *vibra_noise_new(double red)
{
	t_vibra_noise *x = (t_vibra_noise *)newobject(vibra_noise_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.1;

//	post("loadd with %f",red);
	vibra_noise_reset(x);

	vibra_noise_setphasetime(x,red);//x->phaseinc = red;
	
	x->side = 0; 
	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void vibra_noise_assist(t_vibra_noise *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method

void vibra_noise_reset(t_vibra_noise *x){
	x->phase=0.; 
	x->phaseinc=0.1;
	x->source=Randomfloat11(x);
	x->target=Randomfloat11(x);
	x->delta = x->target-x->source;
	
	x->min=0.0f;//-1.0f;
	x->max=1.0f;
	
	x->vibra=1;
//	post("srctar %f %f %f",x->source,x->target,x->delta);
}


void vibra_noise_setphaseinc(t_vibra_noise *x,double phasei){
	x->phaseinc = phasei;
//	post("phaseinc %f",x->phaseinc);
}


void vibra_noise_min(t_vibra_noise *x,double m){x->min=m;}					
void vibra_noise_max(t_vibra_noise *x,double m){x->max=m;}					

void vibra_noise_vibra(t_vibra_noise *x,long m) {
	x->vibra=m;
}


void vibra_noise_setphasetime(t_vibra_noise *x,double millis){
	double sr = sys_getsr();//double sys_getsr(void);			// returns current sampling rate
//	double to = 1.0f / (millis * 0.001 * sr); 
//	double samples = (millis * sr)/1000.;// *0.01 * sr;
	
	//millis is seconds!
//	double samples = (millis * sr * 0.01);// *0.01 * sr;	
	if(millis==0.)
		millis=0.000001;

	x->phaseinc = 1.0f/((1.0f/millis)*sr);// 0.1 inc is 10 samples//1.0f/samples;
	
//	x->phaseinc = to;//Mmillis / sr;
//	post("phaseinc %f %f",x->phaseinc,sr);
}

/*
// Memory Deallocation
void vibra_noise_free(t_vibra_noise *x)
{
	notify_free((t_object *)x);
	dsp_free((t_pxobject *)x);
}
*/

// Perform (signal) Method
t_int *vibra_noise_perform(t_int *w)
{
//	t_float *out = (t_float *)(w[1]);	// Output
//   	t_vibra_noise *x = (t_vibra_noise *)(w[2]);		// Pointer
//	int n = (int)(w[3]);				// Vector Size		

	t_vibra_noise *x = (t_vibra_noise *) w[1];
	t_float *in,*out;
	int n ;//= (int)(w[3]);				// Vector Size		

	in = (t_float *)(w[2]);
	out = (t_float *)(w[3]); // my output vector
	n = (int)(w[4]); // my vector size
	
	
	double phase, phaseinc;//, source, target;
//	double delta;
	double sr = sys_getsr();
	double f;
	
	
	
//	phaseinc = x->phaseinc;

	
	
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			//1.0f/((1.0f/millis)*sr);
			f = 1.0f/((1.0f / *++in) * sr);
			phase = x->phase+=f;//freq now//phaseinc;
			
			
			if(phase>=1.0) {
					x->source = x->target;
					x->target = Randomfloat10(x);
					x->delta = x->target-x->source;
					x->phase -= 1.0f;		
				//	post("phasesync");			
			}

			*++out = x->source + (phase*x->delta);
			
			//randprev = x->randprev = RandomLCA(x->randprev);
			//random = randprev*ONE_OVER_RAND_MAX;
			//*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White vibra_noise
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void vibra_noise_dsp(t_vibra_noise *x, t_signal **sp, short *count)
{
	// add to the dsp chain -- i need my class, my input vector, my output vector, and my vector size...
	dsp_add(vibra_noise_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
//	dsp_add(ftom_perform, 5, x, sp[0]->s_vec-1, sp[1]->s_vec-1, sp[0]->s_n+1); //from ftom~ perolate
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11(t_vibra_noise *x) {
	double delta,off;
	double r = ((double)rand()) * (double)ONE_OVER_RAND_MAX;

	delta = x->max-x->min;

	return(r*delta+x->min);
}


double Randomfloat10(t_vibra_noise *x) {
	double delta,off;
	double r = ((double)rand()) * (double)ONE_OVER_RAND_MAX;

	delta = x->max-x->min;
	
	//handle side here
	if(x->vibracount++%x->vibra==0)
		x->side!=x->side;
	

	if(x->side)
		return(r*delta+x->min);
	else
		return(-1.0*(r*delta+x->min));
}
