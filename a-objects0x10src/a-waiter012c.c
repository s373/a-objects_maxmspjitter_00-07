/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
a-waiter  
por andrŽ sier 2006
		
		objecto que recebe conjuntos de valores validos de output 
				
		escolhe aleatoriamente um deles, outro random onde calcula
		um enorme e faz modulo para a parte final, e uma urna, 
		uma urna para mais tarde. finalmente aprender como se faz isso
			
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ
#include "ext.h"
#include "ext_common.h"
#include <stdlib.h>
//#define mRAND_MAX 65535
#define RAND_LIMIT 2147483647.0f
#define mRAND_MAX 65535
#define MAXSIZE 256

/*
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif
*/

//#define pi2		2 * M_PI

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ


typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out[2];		// outlet value	+ zone outlet value

	double			value;			// o valor de saida registado
	t_atom			list[MAXSIZE];		// a lista dos elementos de seleccao
	int			num;			// o numero de elementos de intervalo de
									// seleccao. algo como argc / 2									
	int			curr_zone;
	int			om;
	int			head, head2, swit, pal1, pal2;
	
	int			indy[MAXSIZE];
	int			clear, indy_count, clear_bang;
	
//	double 			*datum;
//	int			size;
} waiter;

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ

void *waiter_new 		(Symbol *msg, int argc, Atom *argv);
void waiter_bang 		(waiter *x);
void waiter_serve 		(waiter *x);
void waiter_shuffleindy (waiter *x);

//int _MSL_CDECL rand(void);

int	mayrand(int	min, 	int	max);

void waiter_get 		(waiter *x, long echo);
void waiter_head 		(waiter *x, long echo);
void waiter_list 		(waiter *x, Symbol *msg, int argc, Atom *argv);
void waiter_clear_bang 	(waiter *x);

void waiter_om 			(waiter *x, long echo);
void waiter_post 		(waiter *x);
void waiter_assist		(waiter *x, void *b, long m, long a, char *s);
void *waiter_class;

//double mapvalue 			(double x, double a, double b, double c, double d) ; // the double mapping version

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ

void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&waiter_class,(method)waiter_new,0L,(int)sizeof(waiter),0L,
 A_GIMME,0);

 addbang((method)waiter_bang);
 addmess((method)waiter_list,"list",A_GIMME,0);
 addmess((method)waiter_om,"om",A_DEFLONG,0);
 addmess((method)waiter_head,"head",A_DEFLONG,0);
 addmess((method)waiter_get,"get",A_DEFLONG,0);
 addmess((method)waiter_post,"post",0);
 addmess((method)waiter_clear_bang,"clear",0);
 addmess((method)waiter_assist,"assist", A_CANT, 0);

 post("	 \t \a \t  %08xd	  			a-waiter 2006   "__TIME__,tick,0);
}


void *waiter_new (Symbol *msg, int argc, Atom *argv) //input the args 
{
	waiter *x;
	t_atom *list;
	int i;
	int t;
	double ftest; int stest; unsigned int utest;
	 	 
	x=(waiter *)newobject(waiter_class); // primeira ordem. importante.

	list = x->list; // store list in the list vector

	x->c_out[1]=intout(x);		// outlet da zona actual
	x->c_out[0]=floatout(x);	// outlet do valor

	x->clear = 1;
	x->num = MAXSIZE;
	x->om = x->head = x->head2 = x->swit = 0;

	if (argc) { 
		waiter_list(x, msg, argc, argv);
	} else if (!argc)  {
		 x->om = 1;
		 x->num = 2; //two default args
		 x->list[0].a_w.w_float = 0.;
		 x->list[1].a_w.w_float = 1.;
   		 x->curr_zone = 0;	
	}	

	 return(x);	
}

void waiter_shuffleindy (waiter *x)
{
	int i,t;
	int tval;
	int n1=x->num-1;

	//shuffleindy

	// reset all normal positions
	for(i=0;i<(x->num);i++) {
		x->indy[i] = i;
//		post("fill indy in %ld = %ld", i, x->indy[i]);
	}


	for(i=n1 ; i>0 ; i--) {
		t = mayrand(0,i);
		tval = x->indy[t];
		x->indy[t]=x->indy[i];
		x->indy[i]=tval;
//		post("swap indy in %ld = %ld with t %ld", i, x->indy[i], t);
	}

}

void waiter_post (waiter *x)
{

	int i;
post(" 	");
post(" 	");
post("  a-waiter 004 20031128");
post(" 	");
post(" value        = %f", x->value);
post(" num_values   = %ld", x->num);

	for (i=0; i<x->num; i++)
		post("    interval_point[%ld]    = %f", i, x->list[i].a_w.w_float);

post(" om           = %ld", x->om);
post(" num          = %ld", x->num);
post(" higlighted   = %ld", x->curr_zone);
post(" 		");
post(" 	");

//	for (i=0; i<x->num; i++)
//		post("    indylist[%ld]    = %2d", i, x->indy[i]);


}




void waiter_list (waiter *x, Symbol *msg, int argc, Atom *argv)
{
	int i;

	if (argc) { 
		x->num = argc; 
		x->clear=1;			//importante: assim que entra uma lista, refazer a lista de shuffle
		x->curr_zone = 0;	

		for (i=0; i<argc; i++) {
			if (argv[i].a_type==A_LONG) {
			  x->list[i].a_w.w_float = (double)argv[i].a_w.w_long; //copiar a lista da entrada 
			}													  //para o interior do objecto
			else if (argv[i].a_type==A_FLOAT) {
			  x->list[i].a_w.w_float = argv[i].a_w.w_float; //copiar a lista da entrada 
			}													  //para o interior do objecto
			else if (argv[i].a_type==A_SYM) {
				error("%s wrong argument type for waiter", argv[i].a_w.w_sym,0);
			}
		}			
	}	
	
}



void waiter_om (waiter *x, long echo)
{
	x->om = echo;
}

void waiter_clear_bang 	(waiter *x)
{
/*	switch(x->om)
	{
		case 3:
		case 4: x->clear = 1; break;
		case 5: 
		case 6:
		case 7: x->head = mayrand(0,(x->num-1)); x->head2 = x->num-1-x->head; break;
	}
*/
		x->clear = 1; 

		x->head = mayrand(0,(x->num-1)); 
		x->head2 = x->num-1-x->head; 
}
void waiter_get (waiter *x, long echo)
{
	int t;
	double val;
	t = echo % x->num;
	val = x->list[t].a_w.w_float;
	outlet_int	(x->c_out[1], t); 
	outlet_float(x->c_out[0], val); 
}

void waiter_head (waiter *x, long echo)
{
	x->head = CLIP(echo, 0, x->num-1);
	x->head2 = x->num - 1 - x->head;
	
}


void waiter_bang (waiter *x)
{
	waiter_serve(x);
	outlet_int	(x->c_out[1], x->curr_zone); 
	outlet_float(x->c_out[0], x->value); 
}

void waiter_serve (waiter *x)
{
	int i,t;
	double tp;
	unsigned int rhold;
	
	if(x->clear) {
		waiter_shuffleindy(x);
		x->clear = x->indy_count = 0;
	}
	

	switch(x->om) 
	{
		 case 0: // rand within bounds
		 {
		  	rhold = rand();			
		  	tp = (double) rhold/mRAND_MAX;
//		  	post("waiter om 0 rand hold %ld and result %f",rhold,tp);
			tp *= x->num;		  	
		  	x->head = t = (int)(tp);
		  	x->curr_zone = t;
		  //	post("t %ld", t);
		  //	CLIP(t, 0, x->num);
		  	x->value = x->list[t].a_w.w_float;
		  	break;
		 }	
		 case 1: // random within bounds
		 {
		  	rhold = Random();
		  	tp = (double) rhold/mRAND_MAX;
		  	tp *= x->num;		  	
		  	x->head = t = (int)(tp);
		  	x->curr_zone = t;
		  //	post("t %ld", t);
		  //	CLIP(t, 0, x->num);
		  	x->value = x->list[t].a_w.w_float;
		  	break;
		 }	
		 case 2: // random within bounds & modulus
		 {
		  	rhold = Random();
		  	tp = (double) rhold/mRAND_MAX;
		  	tp *= (rand());	
		  	t = (int) tp;	  	
		  	t = t %x->num;
		  	x->head = x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	break;
		 }	
		 case 3: // urn like shuffle autoclear
		 {
			t = x->indy[x->indy_count++];
			if (x->indy_count==x->num) { x->clear = 1; }
		  	x->head = x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	break;
		 }	
		 case 4: // urn like shuffle needs clear message
		 {
			t = x->indy[x->indy_count++];
			if (x->indy_count==x->num) { x->indy_count = 0; }
		  	x->head = x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	break;
		 }	
		 case 5: // count up
		 {
		  	t = (int) x->head++;	  	
		  	x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	(x->head > (x->num-1)) ? (x->head = 0) : x->head;
		  	break;
		 }	
		 case 6: // count down
		 {
		  	t = (int) x->head--;	  	
		  	(x->head < 0) ? (x->head=(x->num-1)) : x->head;
		  	x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	break;
		 }	
		 case 7: // crossing counters
		 {
	//		post("swit %ld");
			if(x->swit) {
			  	t = (int) x->head2--;	  	
			  	(x->head2 < 0) ? (x->head2=(x->num-1)) : x->head2;
			 } else if (!x->swit) {
			  	t = (int) x->head++;	  	
			  	(x->head > (x->num-1)) ? (x->head = 0) : x->head;			 
			 }
		  	x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	x->swit = !x->swit;
		  	break;
		 }	
		 case 8: // palidrome
		 {
		  	if(!x->swit) { t = (int) x->head++; if (x->head > (x->num-1)) {x->swit = 1;} } 
		  	if( x->swit) { t = (int) x->head--; if (x->head < 1) {x->swit = 0;} } 
		  	x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
		  	(x->head > (x->num-1)) ? (x->head = 0) : x->head;
		  	break;
		 }	
		 case 9: // crossing palidromes
		 {
			if(!x->swit) {
			  	if(!x->pal1) { 
			  		t = (int) x->head++; 
			  		if (x->head > (x->num-1)) {
			  			x->pal1 = 1;
			  		} 
			  	}else if(x->pal1) { 
			  		t = (int) x->head--; 
			  		if (x->head < (1)) {
			  			x->pal1 = 0;
			  		} 
			  	} 
			  } else if (x->swit) {
			  	if(!x->pal2) { 
			  		t = (int) x->head2--; 
			  		if (x->head2 < (1)) {
			  			x->pal2 = 1;
			  		} 
			  	}else if(x->pal2) { 
			  		t = (int) x->head2++; 
			  		if (x->head2 > (x->num-1)) {
			  			x->pal2 = 0;
			  		} 
			  	} 
			  
			  }

		  	x->curr_zone = t;
		  	x->value = x->list[t].a_w.w_float;
			
			x->swit = !x->swit;

		  	break;
		 }	

	}
}




void waiter_assist(waiter *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"e c ho    e   c  h   o"); }   
    else if (m==2&&a==0)  { sprintf(s,"(double) waiter   e         c            h              o"); }
	else if (m==2&&a==1)  {sprintf(s,"(int) waiter zone  e         c            h             o"); }
}




int	mayrand(int min, int max)
{
	int dist = max-min;
	unsigned int rhold;
	double t;
	
	rhold = Random();
	t = ((double)rhold/mRAND_MAX)*dist+min;
//	double t = ((((double)Random()+mRAND_MAX)/2*mRAND_MAX))*dist+min;
	return ( (int)t);
}

