/*



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

#define rosx(y,z)		-((y)+(z))
#define rosy(x,y,a)		(x)+((a)*(y))
#define rosz(x,z,b,c)	(b)+((x)*(z))-((c)*(z))





void *rossler_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _rossler	// Data Structure for this object
{
	t_pxobject x_obj;
//	double rossler_b0, rossler_b1, rossler_b2, rossler_b3, rossler_b4, rossler_b5, rossler_b6; // Coefficients for the "pinking" filter

//	unsigned long randprev;
	
	double		a, b, c, nx, ny, nz,dt; //eq variables

	short dim,interpon;

	double interp,source,target,delta,actual;

	double min, max;
} t_rossler;

// Prototypes for methods: need a method for each incoming message type
void *rossler_new(double red);							// New Object Creation Method
t_int *rossler_perform(t_int *w);						// An MSP Perform (signal) Method
void rossler_dsp(t_rossler *x, t_signal **sp, short *count);		// DSP Method
void rossler_assist(t_rossler *x, void *b, long m, long a, char *s);	// Assistance Method

void rossler_reset(t_rossler *x);		
void rossler_min(t_rossler *x,double m);					
void rossler_max(t_rossler *x,double m);		
void rossler_dim(t_rossler *x,short m);	
void rossler_a (t_rossler *x, double echo);
void rossler_b (t_rossler *x, double echo);
void rossler_c (t_rossler *x, double echo);
void rossler_nx (t_rossler *x, double echo);
void rossler_ny (t_rossler *x, double echo);
void rossler_nz (t_rossler *x, double echo);
void rossler_dt (t_rossler *x, double echo);

void rossler_interp (t_rossler *x, double echo);
			
//void rossler_free(t_rossler *x);
double Randomfloat11(t_rossler *x);//double Randomfloat11() ;
/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&rossler_class, (method)rossler_new, (method)dsp_free, (short)sizeof(t_rossler), 0L, A_DEFFLOAT, 0);

	addmess((method)rossler_dsp, "dsp", A_CANT, 0);					
	addmess((method)rossler_assist, "assist", A_CANT, 0);				
//	addint((method)rossler_param);									
//	addfloat((method)rossler_setphaseinc);									
	addmess((method)rossler_interp, "interp",A_DEFFLOAT, 0);				

	addmess((method)rossler_min, "min",A_DEFFLOAT, 0);				
	addmess((method)rossler_max, "max",A_DEFFLOAT, 0);				
	addmess((method)rossler_dim, "dim",A_DEFLONG, 0);				



 addmess((method)rossler_dt,"dt", A_DEFFLOAT, 0);
 addmess((method)rossler_a,"a", A_DEFFLOAT, 0);
 addmess((method)rossler_b,"b", A_DEFFLOAT, 0);
 addmess((method)rossler_c,"c", A_DEFFLOAT, 0);
 addmess((method)rossler_nx,"x", A_DEFFLOAT, 0);
 addmess((method)rossler_ny,"y", A_DEFFLOAT, 0);
 addmess((method)rossler_nz,"z", A_DEFFLOAT, 0);

 addmess((method)rossler_reset,"reset", 0);
//	addmess((method)rossler_reset, "reset", 0);				
//	addmess((method)rossler_setphasetime, "phasetime", A_DEFFLOAT, 0);				// Bind method "rossler_assist" to assistance calls
//	addmess((method)rossler_setphaseinc, "phaseinc", A_DEFFLOAT, 0);				// Bind method "rossler_assist" to assistance calls
 	post("a-rossler~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *rossler_new(double red)
{
	t_rossler *x = (t_rossler *)newobject(rossler_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.1;

//	post("loadd with %f",red);
	rossler_reset(x);
	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void rossler_assist(t_rossler *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method
void rossler_reset(t_rossler *x) {

	 //classic rossler
		 x->a=0.2;
		 x->b=0.2; 
		 x->c=5.7; 
		 x->nx=1; 
		 x->ny=1; 
		 x->nz=1;
		 x->dt=0.01;

	x->min = -1;
	x->max = 1;
	
	x->dim = 0;
	x->interpon=0;
	x->interp = 0.;
	x->source=0.;
	x->target=0.;
	x->actual=0.;

}

void rossler_min(t_rossler *x,double m){x->min=m;}					
void rossler_max(t_rossler *x,double m){x->max=m;}					
void rossler_a (t_rossler *x, double echo){x->a=echo;}
void rossler_b (t_rossler *x, double echo){x->b=echo;}
void rossler_c (t_rossler *x, double echo){x->c=echo;}
void rossler_nx (t_rossler *x, double echo){x->nx=echo;}
void rossler_ny (t_rossler *x, double echo){x->ny=echo;}
void rossler_nz (t_rossler *x, double echo){x->nz=echo;}
void rossler_dt (t_rossler *x, double echo){x->dt=echo;}
void rossler_dim (t_rossler *x, short echo){x->dim=echo;}
void rossler_interp (t_rossler *x, double echo){x->interp=echo; if(echo!=0.){x->interpon=1;}else{x->interpon=0;}}
void  rossler_calc (t_rossler *x)
{
	double	a, b, c, dt, x0, y0, z0;
	double  fx, fy, fz, ffx, ffy, ffz, dt2;
	double	dtfx, dtfy, dtfz;
	
	//local copies
	a = x->a;
	b = x->b;
	c = x->c;
	dt = x->dt;
	dt2 = dt/2.;
	x0 = x->nx;
	y0 = x->ny;
	z0 = x->nz;


	fx = rosx(y0,z0);
	fy = rosy(x0,y0,a);
	fz = rosz(x0,z0,b,c);

	ffx = rosx((y0+dt*fy), (z0+(dt*fz)));
	ffy = rosy((x0+dt*x0), (y0+dt*y0), a);
	ffz = rosz((x0+dt*fx), (z0+dt*z0), b, c);

	x->nx += (dt2*(fx+ffx));
	x->ny += (dt2*(fy+ffy));
	x->nz += (dt2*(fz+ffz));
}


// Perform (signal) Method
t_int *rossler_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_rossler *x = (t_rossler *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	double interp=x->interp;
	short om= x->interpon;/*(interp==0.)?0:1;*/
	double actual;
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			
		if(!om) {
			rossler_calc(x);
			
			switch(x->dim) {
				default: case 0:*++out = x->nx; break;
				case 1:*++out = x->ny; break;
				case 2:*++out = x->nz; break;
			}
		} else {
		
			actual = x->actual += interp;
			
			if(actual>=1.0f) {
				rossler_calc(x);
				x->source = x->target;
				switch(x->dim) {
					default: case 0:x->target = x->nx; break;
					case 1:x->target = x->ny; break;
					case 2:x->target = x->nz; break;
				}
				x->delta = x->target-x->source;
				actual = x->actual-=1.0f;

			}
			
			*++out = x->source + (actual*x->delta);

			
		
		}
			//*++out = Randomfloat11(x);
			
			//randprev = x->randprev = RandomLCA(x->randprev);
			//random = randprev*ONE_OVER_RAND_MAX;
			//*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White rossler
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void rossler_dsp(t_rossler *x, t_signal **sp, short *count)
{
//	x->rossler_b0 = x->rossler_b1 = x->rossler_b2 = x->rossler_b3 = x->rossler_b4 = x->rossler_b5 = x->rossler_b6 = 0; 
	dsp_add(rossler_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11(t_rossler *x) {
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
