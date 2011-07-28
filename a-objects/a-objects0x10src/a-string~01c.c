/*





	a-gaussnoise~

	gaussian noise generation


a-string~


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

Force = magnitude * velocity^2 
magnitude = 0.5*c*rho*area



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

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define M_PIx6 (6.0f*M_PI)


void *string_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _string	// Data Structure for this object
{
	t_pxobject x_obj;

	double feedback;

	double min, max;
	
// Spring simulation constants
double mass,massinv;   // Mass
double k;  // Spring constant
double damping;  // Damping
double rest ;    // Rest position

// Spring simulation variables
double stringpos ;// Position
double stringvel ;  // Velocity
double stringacc;    // Acceleration
double force ;     // Force	
	short om; //no, now is friction mode stokes / newton//clip,bounce
	
	double param1,param2;//param3 is damping
	double magnitude;
	
} t_string;

// Prototypes for methods: need a method for each incoming message type
void *string_new(double red);							// New Object Creation Method
t_int *string_perform(t_int *w);						// An MSP Perform (signal) Method
void string_dsp(t_string *x, t_signal **sp, short *count);		// DSP Method
void string_assist(t_string *x, void *b, long m, long a, char *s);	// Assistance Method

void string_reset(t_string *x);	
void string_min(t_string *x,double m);					
void string_max(t_string *x,double m);		
void string_feedback(t_string *x,double m);	
void string_mass(t_string *x,double m);					
void string_k(t_string *x,double m);	
void string_damping(t_string *x,double m);					
void string_rest(t_string *x,double m);	
void string_pos(t_string *x,double m);	
void string_vel(t_string *x,double m);	
void string_acc(t_string *x,double m);	
void string_param1(t_string *x,double m);	
void string_param2(t_string *x,double m);	
void string_magnitude(t_string *x,double m);	
void string_om(t_string *x,long m);	
			
//void string_free(t_string *x);

/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&string_class, (method)string_new, (method)dsp_free, (short)sizeof(t_string), 0L, A_DEFFLOAT, 0);

	addmess((method)string_dsp, "dsp", A_CANT, 0);					
	addmess((method)string_assist, "assist", A_CANT, 0);				
//	addint((method)string_param);									
//	addfloat((method)string_setphaseinc);									
	addmess((method)string_feedback, "feedback",A_DEFFLOAT, 0);
	addmess((method)string_min, "min",A_DEFFLOAT, 0);				
	addmess((method)string_max, "max",A_DEFFLOAT, 0);				
	addmess((method)string_mass, "mass",A_DEFFLOAT, 0);				
	addmess((method)string_k, "k",A_DEFFLOAT, 0);				
	addmess((method)string_damping, "damping",A_DEFFLOAT, 0);				
	addmess((method)string_rest, "rest",A_DEFFLOAT, 0);				
	addmess((method)string_pos, "pos",A_DEFFLOAT, 0);				
	addmess((method)string_vel, "vel",A_DEFFLOAT, 0);				
	addmess((method)string_acc, "acc",A_DEFFLOAT, 0);				

	addmess((method)string_param1, "param1",A_DEFFLOAT, 0);	
	addmess((method)string_param2, "param2",A_DEFFLOAT, 0);	

	addmess((method)string_magnitude, "magnitude",A_DEFFLOAT, 0);	
		
	addmess((method)string_om, "om",A_DEFLONG, 0);	
	addmess((method)string_reset, "reset", 0);				
//	addmess((method)string_setphasetime, "phasetime", A_DEFFLOAT, 0);				// Bind method "string_assist" to assistance calls
//	addmess((method)string_setphaseinc, "phaseinc", A_DEFFLOAT, 0);				// Bind method "string_assist" to assistance calls
 	post("a-string~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *string_new(double red)
{
	t_string *x = (t_string *)newobject(string_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.1;

//	post("loadd with %f",red);
	string_reset(x);
	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void string_assist(t_string *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method

void string_reset(t_string *x){


	x->mass = 0.8;   // Mass
	x->massinv = 1.0f/x->mass;
	x->k = 0.2;   // Spring constant
	x->damping = 0.99993;  // Damping
	x->rest = 0.;//60;    // Rest position

		// Spring simulation variables
	x->stringpos = 0.;//60.0; // Position
	x->stringvel = 0.0;  // Velocity
	x->stringacc = 0.;    // Acceleration
	x->force = 0.;     // Force	

	x->min = -1.0f;
	x->max = 1.0f;
	
	x->om = 0;

	x->feedback = 0.;
	
	x->magnitude = 0.1;


}



void string_min(t_string *x,double m)	{x->min=m;}					
void string_max(t_string *x,double m)	{x->max=m;}					

void string_mass(t_string *x,double m)		{x->mass=m; x->massinv=1.0/m;}					
void string_k(t_string *x,double m)			{x->k=m;}	
void string_damping(t_string *x,double m)	{x->damping=m;}					
void string_rest(t_string *x,double m)		{x->rest=m;}	
void string_pos(t_string *x,double m)		{x->stringpos=m;}	
void string_vel(t_string *x,double m)		{x->stringvel=m;}	
void string_acc(t_string *x,double m)		{x->stringacc=m;}
void string_feedback(t_string *x,double m)		{x->feedback=m;}
void string_magnitude(t_string *x,double m)		{x->magnitude=m;}	
void string_param1(t_string *x,double m)		{x->param1=m;}	
void string_param2(t_string *x,double m)		{x->param2=m;}	
void string_om(t_string *x,long m)			{x->om=m;}
/*
// Memory Deallocation
void string_free(t_string *x)
{
	notify_free((t_object *)x);
	dsp_free((t_pxobject *)x);
}
*/

// Perform (signal) Method
t_int *string_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_string *x = (t_string *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	
//	double phase, phaseinc;//, source, target;
//	double delta;

	double mass,k,damping,rest,pos,vel,acc;
	double force,massinv;
	double min, max,feedback,f0,f1;
	double magnitude;
	short om;
	
	mass=x->mass;
	massinv=x->massinv;
	k=x->k;
	damping=x->damping;
	rest=x->rest;
	pos=x->stringpos;
	vel=x->stringvel;
	acc=x->stringacc;
	min=x->min;
	max=x->max;
	om = x->om;
	feedback = x->feedback;
	f0 = feedback;
	f1 = 1.0f - f0;
	magnitude = x->magnitude;
	
//	phaseinc = x->phaseinc;

	
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			

		  // Update the string position
		//  if(!move) {

			//force = -k * (pos - rest);    // f=-ky
			//acc = force * massinv; //as = f / M;           // Set the acceleration, f=ma == a=f/m
			switch(om) {
				default: 
				case 0:	
					force = M_PIx6 * magnitude * vel;//-k * (pos - rest);    // f=-ky
					acc = force * massinv; 
					vel = damping * (vel+acc);//(M_PIx6 * magnitude * damping) * (vel + acc);  
					//vel = damping * (vel + acc);
					break;
				case 1:	
					force = magnitude*vel*vel;// -k * (pos - rest);    // f=-ky
					acc = force * massinv; 
					vel = damping * (vel + acc);
					//vel = (M_PIx6 * radius * damping) * (vel + acc);  
					break;
			}
	//		vel = damping * (vel + acc); //vs = D * (vs + as);   // Set the velocity
			pos+= vel;//ps = ps + vs;         // Updated position

					if(pos<min) 
						pos+=(min-pos);
					//	pos = min;
						//pos+=(min-pos);

					if(pos>max) 
						pos-=(pos-max);
						//pos = max; //pos+=(pos-max);



/*
			switch(om) {
				default:
				case 0:
					if(pos<min) {
						pos+=(min-pos);
					//	vel = -vel;
						}
					if(pos>max) {
						pos+=(pos-max);
					//	vel = -vel;
						
						}
				 break;

				case 1:
					if(pos<min)
						pos=min;
					if(pos>max)
						pos=max;
				 break;

				case 2:
					if(pos<min)
						pos=min;
					if(pos>max)
						pos=max;
				 break;


				case 3:
					if(pos<min) {
						pos+=(min-pos);
						vel = -vel;
						}
					if(pos>max) {
						pos+=(pos-max);
						vel = -vel;
						
						}
				 break;


				case 4:
					if(pos<min) {
						pos=max;//(min-pos);
						vel = -vel;
						}
					if(pos>max) {
						pos=min;//+=(pos-max);
						vel = -vel;
						
						}
				 break;

				case 5:
					if(pos<min) {
						pos=max;//(min-pos);
					//	vel = -vel;
						}
					if(pos>max) {
						pos=min;//+=(pos-max);
					//	vel = -vel;
						
						}
				 break;


			}
			
			x->stringpos = f0*x->stringpos + f1* pos;
			x->stringvel = f0*x->stringvel + f1* vel;
*/

		 // }
//		  if(abs(vs) < 0.1) {
//			vs = 0.0;
//		  }


			*++out = pos;//x->source + (phase*x->delta);


	/*		phase = x->phase+=phaseinc;
			
			if(phase>=1.0) {
					x->source = x->target;
					x->target = Randomfloat11(x);
					x->delta = x->target-x->source;
					x->phase -= 1.0f;		
				//	post("phasesync");			
			}

			*++out = x->source + (phase*x->delta);
	*/		
			//randprev = x->randprev = RandomLCA(x->randprev);
			//random = randprev*ONE_OVER_RAND_MAX;
			//*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White string
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void string_dsp(t_string *x, t_signal **sp, short *count)
{
//	x->string_b0 = x->string_b1 = x->string_b2 = x->string_b3 = x->string_b4 = x->string_b5 = x->string_b6 = 0; 
	dsp_add(string_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11(t_string *x) {
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
