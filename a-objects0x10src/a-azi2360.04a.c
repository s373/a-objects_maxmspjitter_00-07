/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
azi - © andr sier 2004

utilitario para converter angulos 0-360, como na escola, 
para azimutes de 0 a +-180
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"
#include "math.h"


typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet value	
	
	void				*c_out2;
	t_atom				out[1];

	Boolean			 om; 

	double in, azi;
	
} azi;


void *azi_new (Symbol *msg, short argc, Atom *argv);
void azi_bang (azi *x);

void azi_calc (azi *x);

void azi_echo (azi *x, double echo);
void azi_int(azi *x, int n);
void azi_float(azi *x, double n);


void azi_om (azi *x, long echo);
double mapvalue (double x, double a, double b, double c, double d) ;
double wrap(double in, double min, double max);

void azi_assist(azi *x, void *b, long m, long a, char *s);
void *azi_class;


void *azi_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 azi *x;
	 int i;
	 
	 x=(azi *)newobject(azi_class);


	 x->c_out=floatout(x);// xy azi coords
	 

	x->om = 0;
	x->in = 0.; 
	x->azi = 0.;

	if (argc) { // 

			if(argv->a_type == A_FLOAT) {//1¼
				x->om = (Boolean)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->om = (Boolean)argv->a_w.w_long;	argv++;
			}

			if(argv->a_type == A_FLOAT) {//2«
				x->in = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->in = (double)argv->a_w.w_long;	argv++;
			}


	}
	
	
	 return(x);	
}



void  azi_calc (azi *x)
{
	double 	in = x->in;
	Boolean	om = x->om;


	if (x->om) {	// 360 2 azi

		if (in>=0. && in<=90.) {			// quadrant 1
//			mapvalue(in, 0., 90., 90., 0.);
			in = 90. - in;  
		}	else if (in>90. && in<=180.) {	// quadrant 2
			in = mapvalue(in, 90., 180., 0., -90.);
//			in = 90. - in;  
		}	else if (in>180. && in<=270.) {	// quadrant 3
			in = mapvalue(in, 180., 270., -90., -180.);
//			in = 90. - in;  
		}	else if (in>270. && in<=360.) {	// quadrant 4
			in = mapvalue(in, 270., 360., 180., 90.);
//			in = 90. - in;  
		}		
	

	
	} else if (!x->om) {	// azi 2 360

//	post("azi 2 360");

		if (in>0. && in<=90.) {			// quadrant 1
			in = mapvalue(in, 0., 90., 90., 0.);
		}	else if (in>-90. && in<=0.) {	// quadrant 2
			in = mapvalue(in, -90., 0., 180., 90.);
		}	else if (in>=-180. && in<=-90.) {	// quadrant 3
			in = mapvalue(in, -180., -90., 270., 180.);
		}	else if (in>90. && in<=180.) {	// quadrant 4
			in = mapvalue(in, 90., 180., 360., 270.);
		}		
		

	
	}


	x->azi = in;
}

void azi_echo (azi *x, double echo) //set the number in
{
	if (x->om)  {     // 360 2 azi
		x->in = wrap(echo, 0., 360.);
//		post("x->in = %f", x->in);
	} else if (!x->om) {// azi 2 360
		x->in = wrap(echo, -180., 180.);
//		post("x->in = %f", x->in);
	}
	azi_bang(x);
}

void azi_int(azi *x, int n)     { azi_echo(x, (double)n);}
void azi_float(azi *x, double n) { azi_echo(x, n);}






void azi_om (azi *x, long echo)
{
	x->om = echo;
}


void azi_bang (azi *x)
{
	azi_calc(x);

	outlet_float(x->c_out, x->azi); //xy aziular coords


}

//
// maps x in the range a-b to the range c-d  returns a double 
//	
double mapvalue (double x, double a, double b, double c, double d) 
{	
	return((x - a) * (d-c)/(b-a) + c);
}

double wrap(double in, double min, double max)
{
//	double a, b, c;
	
/*	
	
	if((in >= min) && (in <= max)) 
		return in;
		
	else if(in < min) 
		return((max + 1) - (min - in));
	else 
		return((min - 1) + (in - max));	

*/
	if((in >= min) && (in <= max)) 
		return in;

	if (in>max) {
		while (in>max)
			in = ((min-1) + (in-max));
	} else if (in<min) {
		while (in<min)
			in = ((max + 1) - (min - in));
	}

		return in;

}


void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&azi_class,(method)azi_new,0L,(short)sizeof(azi),0L,
 A_GIMME,0);

 addbang((method)azi_bang);
 addfloat((method)azi_float);
 addint((method)azi_int);
 addmess((method)azi_om,"om", A_LONG, 0);
 addmess((method)azi_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-azi2360   ©   a n d r  s i e r   2 0 0 4   all rights reserved",tick, 0);

 alias("a-azito360");
}


void azi_assist(azi *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) {
    	if (x->om)
    	 { sprintf(s,"(double) azi ang    e         c            h              o"); }
    	else if (!x->om)
    	 { sprintf(s,"(double) 360 ang    e         c            h              o"); }
    
    }
}


