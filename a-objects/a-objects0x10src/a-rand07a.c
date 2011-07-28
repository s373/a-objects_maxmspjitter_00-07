/*

————————————————————————————————————————————————————————————————
rand - © andré sier 2002
		
		returns random numbers within interval,
————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"
#include <stdlib.h> // for rand calling

// unsigned long max 
//#define	rndmax	4294967295
#define	rndmax	2147483647
#define	oneoverrndmax	1.0f/rndmax
//#define	rndmax	ULONG_MAX



typedef struct	
{
	t_object 	        		c_ob;			
	void 		        		*c_out;		// outlet value

	double	 			dstmin, dstmax;
	Boolean				mode; //floating point output or integer output
	unsigned long			hold; //the rand value
	int					algo;
} t_rand;


void *rand_new (Symbol *msg, short argc, Atom *argv);
void rand_bang (t_rand *x);

void rand_calc (t_rand *x);
void rand_range (t_rand *x, Symbol *msg, short argc, Atom *argv);
void rand_reset(t_rand *x);
void rand_post (t_rand *x);

void rand_assist(t_rand *x, void *b, long m, long a, char *s);
void *rand_class;

void  rand_seed (t_rand *x, int a1, int a2);
void  rand_algo (t_rand *x, int alg);

///
void   RandomInitialise(int,int);
double RandomUniform(void);
double RandomGaussian(double,double);
int    RandomInt(int,int);
double RandomDouble(double,double);


// end prototype, funcs begin

void *rand_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 t_rand *x;
	 int i;
	 
	 x=(t_rand *)newobject(rand_class);

	 x->c_out=listout(x); //integer or float
	 

	 rand_reset(x);

	if (argc) {	//typecheck to see whats the output mode
		rand_range(x,msg,argc,argv);
	}	

	 return(x);	
}


void rand_reset(t_rand *x)
{
	x->dstmin = 0.0f;
	x->dstmax = 127.0f;
//	x->hold =  0.0f;
	x->mode = 1;
	x->algo = 0;
}

void  rand_calc (t_rand *x)
{
/*
	unsigned long ring;
	
	ring = x->hold;
	
	ring = 1664525L * ring + 1013904223L; //from numerical c++ recipes, fastest random w/ broader range
*/

	x->hold = rand(); // copy inside to output
//	x->hold = Random(); // copy inside to output
//	x->hold &= 0x77FFFFFF;
}


void  rand_seed (t_rand *x, int a1, int a2)
{
	RandomInitialise(a1,a2);//1802,9373);
}

void  rand_algo (t_rand *x, int alg)
{
	x->algo = alg;
}
void rand_range (t_rand *x,Symbol *msg, short argc, Atom *argv)
{
/*	if (argc) {	//typecheck to see whats the output mode
		if ((argv[0].a_type == A_LONG)&&(argv[1].a_type == A_LONG)){
			x->mode = 0; //integer mode
			x->dstmin =  (double) argv[0].a_w.w_long;
			x->dstmax = (double) argv[1].a_w.w_long;
		} else {
			x->mode = 1; //float mode
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
			x->mode = 1; //float mode
			x->dstmin =  (double) 0.0f;
			x->dstmax = (double) argv[0].a_w.w_float;
      		  	}		

		} else if (argc==2) { //2 args
			if (argv[0].a_type == A_LONG){ //if first arg == long
				x->mode = 0; //integer mode
				x->dstmin =  (double) argv[0].a_w.w_long;
				x->dstmax = (double) argv[1].a_w.w_long;
				} else if (argv[0].a_type == A_FLOAT){ 
				x->mode = 1; //float mode
				x->dstmin =  (double) argv[0].a_w.w_float;
				x->dstmax = (double) argv[1].a_w.w_float;
			}		
		 }//end dif than 1 argument

	} else  if (!argc) { //if no args
		x->dstmin = 0.0f;
		x->dstmax = 127.0f;
		x->mode = 1; //float default
	}	

}






void rand_bang (t_rand *x)
{
	long		n;
	double	a;
		

	switch (x->algo) {
		case 1: //gaussian
		default:
				a = RandomGaussian(x->dstmin,x->dstmax);
			break;
		case 0: //gaussian
				a = RandomDouble(x->dstmin,x->dstmax);
			break;
		
	}


	
	if (x->mode) {	    //float
		outlet_float(x->c_out, a); //output the list _without_ passing it into a max atom
	} else if (!x->mode) { //int
		n = (long) ((double)a); //scale unsigned long to float wthin min max
		 outlet_int(x->c_out, n); //output the list _without_ passing it into a max atom
//		 outlet_float(x->c_out, n); //output the list _without_ passing it into a max atom
	}


/*
	    rand_calc(x);   //store next rnd at hold



	if (x->mode) {	    //float
//		f = (double) x->hold/rndmax * (x->dstmax-x->dstmin) + x->dstmin; //scale unsigned long to float wthin min max
		f = (double) x->hold*oneoverrndmax * (x->dstmax-x->dstmin) + x->dstmin; //scale unsigned long to float wthin min max
		outlet_float(x->c_out, f); //output the list _without_ passing it into a max atom
	} else if (!x->mode) { //int
		n = (long) ((double)x->hold*oneoverrndmax * (x->dstmax-x->dstmin) + x->dstmin); //scale unsigned long to float wthin min max
		 outlet_int(x->c_out, n); //output the list _without_ passing it into a max atom
//		 outlet_float(x->c_out, n); //output the list _without_ passing it into a max atom
	}

*/


	


}


void rand_post (t_rand *x)
{
	char t[27];
	t_symbol *temp = t;

if (x->mode) 
	 temp = gensym("float") ;
	else if (!x->mode)
	  temp = gensym("integer");
	
post("");
post ("rand 002 by andré sier _____  post exec^");
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
 setup((t_messlist**)&rand_class,(method)rand_new,0L,(short)sizeof(t_rand),0L,
 A_GIMME,0);

 addbang((method)rand_bang);
 addmess((method)rand_range,"range", A_GIMME, 0);
 addmess((method)rand_range,"list", A_GIMME, 0);
 addmess((method)rand_seed,"seed", A_LONG,A_LONG, 0);
 addmess((method)rand_algo,"algo", A_LONG, 0);
  addmess((method)rand_assist,"assist", A_CANT, 0);
// addmess((method)rand_post,"post", 0);
// post("_a-rand_sier_%08xd_"__TIME__"_"__DATE__,tick,0);

// post(__DATE__" \t \a \t  "__TIME__"\t %08xd            a-rand   ©   a n d r é s i e r   2 0 0 3   all rights reserved",tick, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-rand   ©   a n d r é s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void rand_assist(t_rand *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(rand)     e         c            h              o"); }
}




/////////////////// the best rnd around

#define FALSE 0
#define TRUE 1

/*
   This Random Number Generator is based on the algorithm in a FORTRAN
   version published by George Marsaglia and Arif Zaman, Florida State
   University; ref.: see original comments below.
   At the fhw (Fachhochschule Wiesbaden, W.Germany), Dept. of Computer
   Science, we have written sources in further languages (C, Modula-2
   Turbo-Pascal(3.0, 5.0), Basic and Ada) to get exactly the same test
   results compared with the original FORTRAN version.
   April 1989
   Karl-L. Noell <NOELL@DWIFH1.BITNET>
      and  Helmut  Weber <WEBER@DWIFH1.BITNET>

   This random number generator originally appeared in "Toward a Universal
   Random Number Generator" by George Marsaglia and Arif Zaman.
   Florida State University Report: FSU-SCRI-87-50 (1987)
   It was later modified by F. James and published in "A Review of Pseudo-
   random Number Generators"
   THIS IS THE BEST KNOWN RANDOM NUMBER GENERATOR AVAILABLE.
   (However, a newly discovered technique can yield
   a period of 10^600. But that is still in the development stage.)
   It passes ALL of the tests for random number generators and has a period
   of 2^144, is completely portable (gives bit identical results on all
   machines with at least 24-bit mantissas in the floating point
   representation).
   The algorithm is a combination of a Fibonacci sequence (with lags of 97
   and 33, and operation "subtraction plus one, modulo one") and an
   "arithmetic sequence" (using subtraction).

   Use IJ = 1802 & KL = 9373 to test the random number generator. The
   subroutine RANMAR should be used to generate 20000 random numbers.
   Then display the next six random numbers generated multiplied by 4096*4096
   If the random number generator is working properly, the random numbers
   should be:
           6533892.0  14220222.0  7275067.0
           6172232.0  8354498.0   10633180.0
*/

/* Globals */
double u[97],c,cd,cm;
int i97,j97;
int test = FALSE;

/*
   This is the initialization routine for the random number generator.
   NOTE: The seed variables can have values between:    0 <= IJ <= 31328
                                                        0 <= KL <= 30081
   The random number sequences created by these two seeds are of sufficient
   length to complete an entire calculation with. For example, if sveral
   different groups are working on different parts of the same calculation,
   each group could be assigned its own IJ seed. This would leave each group
   with 30000 choices for the second seed. That is to say, this random
   number generator can create 900 million different subsequences -- with
   each subsequence having a length of approximately 10^30.
*/
void RandomInitialise(int ij,int kl)
{
   double s,t;
   int ii,i,j,k,l,jj,m;

   /*
      Handle the seed range errors
         First random number seed must be between 0 and 31328
         Second seed must have a value between 0 and 30081
   */
   if (ij < 0 || ij > 31328 || kl < 0 || kl > 30081) {
		ij = 1802;
		kl = 9373;
   }

   i = (ij / 177) % 177 + 2;
   j = (ij % 177)       + 2;
   k = (kl / 169) % 178 + 1;
   l = (kl % 169);

   for (ii=0; ii<97; ii++) {
      s = 0.0;
      t = 0.5;
      for (jj=0; jj<24; jj++) {
         m = (((i * j) % 179) * k) % 179;
         i = j;
         j = k;
         k = m;
         l = (53 * l + 1) % 169;
         if (((l * m % 64)) >= 32)
            s += t;
         t *= 0.5;
      }
      u[ii] = s;
   }

   c    = 362436.0 / 16777216.0;
   cd   = 7654321.0 / 16777216.0;
   cm   = 16777213.0 / 16777216.0;
   i97  = 97;
   j97  = 33;
   test = TRUE;
   post("random engine init");
}

/* 
   This is the random number generator proposed by George Marsaglia in
   Florida State University Report: FSU-SCRI-87-50
*/
double RandomUniform(void)
{
   double uni;

   /* Make sure the initialisation routine has been called */
   if (!test) 
   	RandomInitialise(1802,9373);

   uni = u[i97-1] - u[j97-1];
   if (uni <= 0.0)
      uni++;
   u[i97-1] = uni;
   i97--;
   if (i97 == 0)
      i97 = 97;
   j97--;
   if (j97 == 0)
      j97 = 97;
   c -= cd;
   if (c < 0.0)
      c += cm;
   uni -= c;
   if (uni < 0.0)
      uni++;

   return(uni);
}

/*
  ALGORITHM 712, COLLECTED ALGORITHMS FROM ACM.
  THIS WORK PUBLISHED IN TRANSACTIONS ON MATHEMATICAL SOFTWARE,
  VOL. 18, NO. 4, DECEMBER, 1992, PP. 434-435.
  The function returns a normally distributed pseudo-random number
  with a given mean and standard devaiation.  Calls are made to a
  function subprogram which must return independent random
  numbers uniform in the interval (0,1).
  The algorithm uses the ratio of uniforms method of A.J. Kinderman
  and J.F. Monahan augmented with quadratic bounding curves.
*/
double RandomGaussian(double mean,double stddev)
{
   double  q,u,v,x,y;

	/*  
		Generate P = (u,v) uniform in rect. enclosing acceptance region 
      Make sure that any random numbers <= 0 are rejected, since
      gaussian() requires uniforms > 0, but RandomUniform() delivers >= 0.
	*/
   do {
      u = RandomUniform();
      v = RandomUniform();
   	if (u <= 0.0 || v <= 0.0) {
       	u = 1.0;
       	v = 1.0;
   	}
      v = 1.7156 * (v - 0.5);

      /*  Evaluate the quadratic form */
      x = u - 0.449871;
   	y = fabs(v) + 0.386595;
      q = x * x + y * (0.19600 * y - 0.25472 * x);

      /* Accept P if inside inner ellipse */
      if (q < 0.27597)
			break;

      /*  Reject P if outside outer ellipse, or outside acceptance region */
    } while ((q > 0.27846) || (v * v > -4.0 * log(u) * u * u));

    /*  Return ratio of P's coordinates as the normal deviate */
    return (mean + stddev * v / u);
}

/*
   Return random integer within a range, lower -> upper INCLUSIVE
*/
int RandomInt(int lower,int upper)
{
   return((int)(RandomUniform() * (upper - lower + 1)) + lower);
}

/*
   Return random float within a range, lower -> upper
*/
double RandomDouble(double lower,double upper)
{
   return((upper - lower) * RandomUniform() + lower);
}

