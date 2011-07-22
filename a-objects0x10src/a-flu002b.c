/*

————————————————————————————————————————————————————————————————
flu - © andré sier 2002
		
		returns fluom numbers within interval,
————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"
#include <stdlib.h> // for rand calling

// unsigned long max 
//#define	rndmax	4294967295
//#define	rndmax	32767
//#define	rndmax	65535
#define	rndmax	2147483647


typedef struct	
{
	t_object 	        		c_ob;			
	void 		        		*c_out;		// outlet value

	double	 			dstmin, dstmax;
	Boolean				mode; //floating point output or integer output
	unsigned long			hold; //the flu value
	
	double				input, noise; // socrates near the end on |Io|, the real greek island, composing
	short				flag;
} t_flu;


void *flu_new (Symbol *msg, short argc, Atom *argv);
void flu_bang (t_flu *x);
void flu_output (t_flu *x);
void flu_int (t_flu *x, long n);
void flu_float (t_flu *x, double n);

void flu_flag (t_flu *x, long n);

void flu_calc (t_flu *x);
void flu_range (t_flu *x, Symbol *msg, short argc, Atom *argv);
void flu_reset(t_flu *x);
void flu_post (t_flu *x);

void flu_assist(t_flu *x, void *b, long m, long a, char *s);
void *flu_class;


// end prototype, funcs begin

void *flu_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 t_flu *x;
	 int i;
	 
	 x=(t_flu *)newobject(flu_class);

	 x->c_out=listout(x); //integer or double
	 

	 flu_reset(x);

	if (argc) {	//typecheck to see whats the output mode
		if (argc==1) { //if 1 arg
			if (argv[0].a_type == A_LONG){
			x->mode = 0; //integer mode
			x->dstmin =  (double) 0.0f;
			x->dstmax = (double) argv[0].a_w.w_long; //important!> 0 is first!
			} else if (argv[0].a_type == A_FLOAT){
			x->mode = 1; //double mode
			x->dstmin =  (double) 0.0f;
			x->dstmax = (double) argv[0].a_w.w_float;
      		  	}		

		} else if (argc==2) { //2 args
			if (argv[0].a_type == A_LONG){ //if first arg == long
				x->mode = 0; //integer mode
				x->dstmin =  (double) argv[0].a_w.w_long;
				x->dstmax = (double) argv[1].a_w.w_long;
				} else if (argv[0].a_type == A_FLOAT){ 
				x->mode = 1; //double mode
				x->dstmin =  (double) argv[0].a_w.w_float;
				x->dstmax = (double) argv[1].a_w.w_float;
			}		
		 }//end dif than 1 argument

	} else  if (!argc) { //if no args
		x->dstmin = 0.0f;
		x->dstmax = 127.0f;
		x->mode = 1; //double default
	}	

	 return(x);	
}


void flu_reset(t_flu *x)
{
	x->dstmin = 0.0f;
	x->dstmax = 127.0f;
//	x->hold =  0.0f;
	x->mode = 1;
	x->input = 0.0f;
//	x->output = 0.0f;
	x->flag = 1;
	flu_bang(x); //inti random
}

void flu_flag (t_flu *x, long n)
{
	if (n) x->flag =  1;
	else if (!n) x->flag = 0;
}

void flu_int (t_flu *x, long n)
{
	short flag = x->flag;
	
	if (flag)			
	    flu_calc(x);   //store next rnd at hold
	x->mode = 0; //integer mode
	x->input = (double) n;
	flu_output(x);
}

void flu_float (t_flu *x, double n)
{
	short flag = x->flag;
	
	if (flag)			
	    flu_calc(x);   //store next rnd at hold
	x->mode = 1; //double mode
	x->input = (double) n;
	flu_output(x);
}


void  flu_calc (t_flu *x)
{

	x->hold = rand(); // copy inside to output
}



void flu_list (t_flu *x,Symbol *msg, short argc, Atom *argv)
{

}

void flu_range (t_flu *x,Symbol *msg, short argc, Atom *argv)
{
/*	if (argc) {	//typecheck to see whats the output mode
		if ((argv[0].a_type == A_LONG)&&(argv[1].a_type == A_LONG)){
			x->mode = 0; //integer mode
			x->dstmin =  (double) argv[0].a_w.w_long;
			x->dstmax = (double) argv[1].a_w.w_long;
		} else {
			x->mode = 1; //double mode
			x->dstmin =  (double) argv[0].a_w.w_float;
			x->dstmax = (double) argv[1].a_w.w_float;
		}		
	}	
*/

	if (argc) {	//typecheck to see whats the output mode
		if (argc==1) { //if 1 arg
			if (argv[0].a_type == A_LONG){
			x->mode = 0; //integer mode
			x->dstmin =  (double) 0.0f;
			x->dstmax = (double) argv[0].a_w.w_long; //important!> 0 is first!
			} else if (argv[0].a_type == A_FLOAT){
			x->mode = 1; //double mode
			x->dstmin =  (double) 0.0f;
			x->dstmax = (double) argv[0].a_w.w_float;
      		  	}		

		} else if (argc==2) { //2 args
			if (argv[0].a_type == A_LONG){ //if first arg == long
				x->mode = 0; //integer mode
				x->dstmin =  (double) argv[0].a_w.w_long;
				x->dstmax = (double) argv[1].a_w.w_long;
				} else if (argv[0].a_type == A_FLOAT){ 
				x->mode = 1; //double mode
				x->dstmin =  (double) argv[0].a_w.w_float;
				x->dstmax = (double) argv[1].a_w.w_float;
			}		
		 }//end dif than 1 argument

	} else  if (!argc) { //if no args
		x->dstmin = 0.0f;
		x->dstmax = 127.0f;
		x->mode = 1; //double default
	}	

}






void flu_bang (t_flu *x)
{
	    flu_calc(x);   //store next rnd at hold


	flu_output(x);

}


void flu_output (t_flu *x)
{
	long		n;
	double	f;
	double	i = x->input;
	

// should precalc the scaling jitter only in the calc method. to do

	if (x->mode) {	    //double
		f = (double) x->hold/rndmax * (x->dstmax-x->dstmin) + x->dstmin; //scale unsigned long to double wthin min max
		f += i;
		outlet_float(x->c_out, f); //output the list _without_ passing it into a max atom

	} else if (!x->mode) { //int
		n = (long) ((double)x->hold/rndmax * (x->dstmax-x->dstmin) + x->dstmin); //scale unsigned long to double wthin min max
		n += (long)i;
		 outlet_int(x->c_out, n); //output the list _without_ passing it into a max atom
//		 outlet_float(x->c_out, n); //output the list _without_ passing it into a max atom
	}


}



void flu_post (t_flu *x)
{
t_symbol *temp;
if (x->mode) 
	 temp = gensym("double") ;
	else if (!x->mode)
	  temp = gensym("integer");
	
post("");
post ("flu 002 by andré sier _____  post exec^");
post("");
post ("   o_m %ld %s", x->mode, temp);
post ("   dstmin %f", x->dstmin);
post ("   dstmax %f", x->dstmax);
post ("   rndhold %ld", x->hold);
post("");
}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&flu_class,(method)flu_new,0L,(short)sizeof(t_flu),0L,
 A_GIMME,0);

 addbang((method)flu_bang);
 addint((method)flu_int);
 addfloat((method)flu_float);
 addmess((method)flu_range,"list", A_GIMME, 0);
 addmess((method)flu_range,"range", A_GIMME, 0);
addmess((method)flu_flag,"om", A_DEFLONG, 0);
  addmess((method)flu_assist,"assist", A_CANT, 0);
// addmess((method)flu_post,"post", 0);
// post("_a-flu_sier_%08xd_"__TIME__"_"__DATE__,tick,0);
 post(__TIME__" \t"__DATE__"\t  %08xd",tick,0);
}


void flu_assist(t_flu *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(flu)     e         c            h              o"); }
}


