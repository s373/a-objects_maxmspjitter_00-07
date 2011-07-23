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


#define lorx(x, y, a) 		(a) * ((y) - (x));		
#define lory(x, y, z, a) 	((x) * ((r) - (z))) - (y);
#define lorz(x, y, z, c) 	((x) * (y)) - ((z) * (c));




void *lorenz_class;		// Required. Global pointing to this class

//static long randx = 1;	// Seed

typedef struct _lorenz	// Data Structure for this object
{
	t_pxobject x_obj;
//	double lorenz_b0, lorenz_b1, lorenz_b2, lorenz_b3, lorenz_b4, lorenz_b5, lorenz_b6; // Coefficients for the "pinking" filter

//	unsigned long randprev;
	
	double		a, r, c, nx, ny, nz,dt; //eq variables

	short dim,interpon;

	double interp,source,target,delta,actual;

	double min, max;
} t_lorenz;

// Prototypes for methods: need a method for each incoming message type
void *lorenz_new(double red);							// New Object Creation Method
t_int *lorenz_perform(t_int *w);						// An MSP Perform (signal) Method
void lorenz_dsp(t_lorenz *x, t_signal **sp, short *count);		// DSP Method
void lorenz_assist(t_lorenz *x, void *b, long m, long a, char *s);	// Assistance Method

void lorenz_reset(t_lorenz *x);		
void lorenz_min(t_lorenz *x,double m);					
void lorenz_max(t_lorenz *x,double m);		
void lorenz_dim(t_lorenz *x,short m);	
void lorenz_a (t_lorenz *x, double echo);
void lorenz_r (t_lorenz *x, double echo);
void lorenz_c (t_lorenz *x, double echo);
void lorenz_nx (t_lorenz *x, double echo);
void lorenz_ny (t_lorenz *x, double echo);
void lorenz_nz (t_lorenz *x, double echo);
void lorenz_dt (t_lorenz *x, double echo);

void lorenz_interp (t_lorenz *x, double echo);
			
//void lorenz_free(t_lorenz *x);
double Randomfloat11(t_lorenz *x);//double Randomfloat11() ;
/************************************************************************************/
// Main() Function

void main(void)				// main recieves a copy of the Max function macros table
{

 long int tick = gettime();

	setup((t_messlist **)&lorenz_class, (method)lorenz_new, (method)dsp_free, (short)sizeof(t_lorenz), 0L, A_DEFFLOAT, 0);

	addmess((method)lorenz_dsp, "dsp", A_CANT, 0);					
	addmess((method)lorenz_assist, "assist", A_CANT, 0);				
//	addint((method)lorenz_param);									
//	addfloat((method)lorenz_setphaseinc);									
	addmess((method)lorenz_interp, "interp",A_DEFFLOAT, 0);				

	addmess((method)lorenz_min, "min",A_DEFFLOAT, 0);				
	addmess((method)lorenz_max, "max",A_DEFFLOAT, 0);				
	addmess((method)lorenz_dim, "dim",A_DEFLONG, 0);				



 addmess((method)lorenz_dt,"dt", A_DEFFLOAT, 0);
 addmess((method)lorenz_a,"a", A_DEFFLOAT, 0);
 addmess((method)lorenz_r,"r", A_DEFFLOAT, 0);
 addmess((method)lorenz_c,"c", A_DEFFLOAT, 0);
 addmess((method)lorenz_nx,"x", A_DEFFLOAT, 0);
 addmess((method)lorenz_ny,"y", A_DEFFLOAT, 0);
 addmess((method)lorenz_nz,"z", A_DEFFLOAT, 0);

 addmess((method)lorenz_reset,"reset", 0);
//	addmess((method)lorenz_reset, "reset", 0);				
//	addmess((method)lorenz_setphasetime, "phasetime", A_DEFFLOAT, 0);				// Bind method "lorenz_assist" to assistance calls
//	addmess((method)lorenz_setphaseinc, "phaseinc", A_DEFFLOAT, 0);				// Bind method "lorenz_assist" to assistance calls
 	post("a-lorenz~ © andrŽ sier 2007 "__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd ",tick, 0);
	dsp_initclass();												// Setup object's class to work with MSP
}


/************************************************************************************/
// Object Creation Method

void *lorenz_new(double red)
{
	t_lorenz *x = (t_lorenz *)newobject(lorenz_class);	
	dsp_setup((t_pxobject *)x,1);				// Create Object and 1 Inlet (last argument)
	outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet  

	if(!red)
		red = 0.1;

//	post("loadd with %f",red);
	lorenz_reset(x);
	
	return (x);							// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void lorenz_assist(t_lorenz *x, void *b, long msg, long arg, char *dst)
{
/*
	if(msg==1)	// Inlet
		strcpy(dst, "(int) Amount");
	else if(msg==2) // Outlet
		strcpy(dst, "(signal) gaussian noise output");
*/
}


// double Method
void lorenz_reset(t_lorenz *x) {

	 //classic lorenz
		 x->a=10.0;
		 x->r=28.0; 
		 x->c= 2.67; 
		 x->nx=0.1; 
		 x->ny=0.1; 
		 x->nz=0.1;
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

void lorenz_min(t_lorenz *x,double m){x->min=m;}					
void lorenz_max(t_lorenz *x,double m){x->max=m;}					
void lorenz_a (t_lorenz *x, double echo){x->a=echo;}
void lorenz_r (t_lorenz *x, double echo){x->r=echo;}
void lorenz_c (t_lorenz *x, double echo){x->c=echo;}
void lorenz_nx (t_lorenz *x, double echo){x->nx=echo;}
void lorenz_ny (t_lorenz *x, double echo){x->ny=echo;}
void lorenz_nz (t_lorenz *x, double echo){x->nz=echo;}
void lorenz_dt (t_lorenz *x, double echo){x->dt=echo;}
void lorenz_dim (t_lorenz *x, short echo){x->dim=echo;}
void lorenz_interp (t_lorenz *x, double echo){x->interp=echo; if(echo!=0.){x->interpon=1;}else{x->interpon=0;}}
void  lorenz_calc (t_lorenz *x)
{
	double	a, r, c, dt, x0, y0, z0;
	double  fx, fy, fz, ffx, ffy, ffz, dt2;
	double	dtfx, dtfy, dtfz;
	
	//local copies
	a = x->a;
	r = x->r;
	c = x->c;
	dt = x->dt;
	dt2 = dt/2.;
	x0 = x->nx;
	y0 = x->ny;
	z0 = x->nz;


	fx = lorx   (x0,y0,a);
	fz = lorz(x0,y0,z0,c);
	fy = lory(x0,y0,z0,r);

	dtfx = x0 + (dt*fx);
	dtfy = y0 + (dt*fy);
	dtfz = z0 + (dt*fz);

	ffx = lorx	   (dtfx,dtfy,a);
	ffz = lorz(dtfx,dtfy,dtfz,c);
	ffy = lory(dtfx,dtfy,dtfz,r);

	x->nx += (dt2*(fx+ffx));
	x->ny += (dt2*(fy+ffy));
	x->nz += (dt2*(fz+ffz));
}


// Perform (signal) Method
t_int *lorenz_perform(t_int *w)
{
	t_float *out = (t_float *)(w[1]);	// Output
   	t_lorenz *x = (t_lorenz *)(w[2]);		// Pointer
	int n = (int)(w[3]);				// Vector Size		
	double interp=x->interp;
	short om= x->interpon;/*(interp==0.)?0:1;*/
	double actual;
			
	if (x->x_obj.z_disabled) return (w+4);

		while (--n) 
		{
			
		if(!om) {
			lorenz_calc(x);
			
			switch(x->dim) {
				default: case 0:*++out = x->nx; break;
				case 1:*++out = x->ny; break;
				case 2:*++out = x->nz; break;
			}
		} else {
		
			actual = x->actual += interp;
			
			if(actual>=1.0f) {
				lorenz_calc(x);
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
			//*++out = (2.f * ((random * c2) + (random*c2) + (random*c2)) -3.f * (c2 - 1.f)) * c3;							// Output White lorenz
		}	
	return (w + 4);				// Return a pointer to the NEXT object in the DSP call chain
}


// DSP Method
void lorenz_dsp(t_lorenz *x, t_signal **sp, short *count)
{
//	x->lorenz_b0 = x->lorenz_b1 = x->lorenz_b2 = x->lorenz_b3 = x->lorenz_b4 = x->lorenz_b5 = x->lorenz_b6 = 0; 
	dsp_add(lorenz_perform, 3, sp[1]->s_vec-1, x, sp[0]->s_n+1);	
}



////// rand LCA linear congruential algorithm
/*Calculate pseudo-random 32bit number using
the LCA*/
unsigned long RandomLCA(unsigned long previous) {
	return ((previous * 196314165) + 907633515);
}

double Randomfloat11(t_lorenz *x) {
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
