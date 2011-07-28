/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
Lhline - © ASier 2002
renamed 		a-listscramble		dez 2003 

		A Intdelline, floatdelline © Richard Dudas 1995 remake.

		The same as those objects in one (float output) plus more modes:

			mode 0 <¡> left->right output <modes in those objects>
			mode 1 <¡> right->left output
			mode 2 <¡> center->sides output
			mode 3 <¡> rnd/urn output

		The difference uptil now is that standard output is float list always.
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include <stdlib.h>	//for random

#define MAXSIZE 256



typedef struct {
	t_object 			x_ob;
	t_atom			x_tsilist[MAXSIZE];	//store output list inside
	t_atom			x_inlist[MAXSIZE];	//store input list inside
	t_atom			x_index[MAXSIZE];	//store indexes list inside: 4 urn output
	short				x_tsillen; 			//the lenght - max 256
	short				x_break, x_step; 	//num breakpoints: 4 int, float methods
	short				x_mode; 			//the modes
	Boolean			x_fire;			//bang on calc?
	void 			       *x_outlet;											
} Lhline;


void *Lhline_new 		(long lenght);
void  Lhline_assist		(Lhline *x, void *b, long m, long a, char *s);			
void  Lhline_bang		(Lhline *x);												
void  Lhline_int			(Lhline *x, long n);											
void  Lhline_in1		(Lhline *x, long n);											
void  Lhline_float		(Lhline *x, float n);											
void  Lhline_list		(Lhline *x,Symbol *s, short argc, Atom *argv);											
void  Lhline_mode		(Lhline *x, long n);											
void  Lhline_fire		(Lhline *x, long n);											
void  Lhline_break		(Lhline *x, long n);											
//void  Lhline_free		(Lhline *x);	//not required?												

void *Lhline_class		; 					



//void  Lhline_free(Lhline *x){ notify_free((t_object *)x);}												

void main(void)
{
	setup((t_messlist **)&Lhline_class, (method)Lhline_new,0L, \
	 (short)sizeof(Lhline), 0L, A_DEFLONG, 0); //initial arg default len
	addmess((method)Lhline_list,"list",A_GIMME,0);
	addint((method)Lhline_int);
	addinx((method)Lhline_in1, 1);
	addbang((method)Lhline_bang);
	addfloat((method)Lhline_float);
	addmess((method)Lhline_in1,"size", A_LONG, 0);
	addmess((method)Lhline_break,"numpoints", A_LONG, 0);
	addmess((method)Lhline_mode,"mode", A_LONG, 0);
	addmess((method)Lhline_fire,"fire", A_LONG, 0);
	addmess((method)Lhline_assist,"assist",A_CANT,0);
	post(__DATE__" \t \a \t  "__TIME__,0);
}

void  Lhline_fire(Lhline *x, long n){ x->x_fire = n;}												
void  Lhline_break(Lhline *x, long n){ x->x_break = n;}												
void  Lhline_mode(Lhline *x, long n){ x->x_mode = n;}												
void  Lhline_in1 (Lhline *x, long n)
{
	if(!n || n < 0) x->x_tsillen = 100;//register outlistsize
	else x->x_tsillen = n;
}


void  Lhline_list	(Lhline *x,Symbol *s, short argc, Atom *argv)
{
	double	poslisratio=0.0, incremento=0.0, 
			a=0.0, b=0.0, 
			indice=0.0;
	short int	i=0, indi=0, 
			len = x->x_tsillen;
	t_atom     *output, *input;
	
	output = x->x_tsilist;
	input  = x->x_inlist;

	for (i=0; i < argc; i++) {
		if (argv[i].a_type==A_LONG) {
			x->x_inlist[i].a_w.w_float = (float)argv[i].a_w.w_long;
		}
		else if (argv[i].a_type==A_FLOAT) {
			x->x_inlist[i].a_w.w_float = argv[i].a_w.w_float;
		}
		else if (argv[i].a_type==A_SYM) {
			error("wrong argument type for list gliding");
		//	i--;
		}
	 }

//	if (argc > len) return; 
//	if (argc == len) goto stemall;

	poslisratio = (double)len / argc;

	x->x_tsilist[0].a_w.w_float = x->x_inlist[0].a_w.w_float; //1st arg is same
	
	for(i=1; i<len-1; i++) {			//from second to one but last arg
		indice = (double)(i/poslisratio);
		indi = (int)(indice);		
		a = (double)x->x_inlist[indi].a_w.w_float;
		b = (double)x->x_inlist[indi+1].a_w.w_float;
		
//		incremento = (double) (((-b+a)/poslisratio)*((indi+1)-indice));
		incremento = (double) (((-b+a)/poslisratio)*((indi)-indice));		//Ã linda!!
		x->x_tsilist[i].a_w.w_float = x->x_inlist[indi].a_w.w_float + incremento;
	 }

	x->x_tsilist[len-1].a_w.w_float = x->x_inlist[argc-1].a_w.w_float; //last arg is same

	if(x->x_fire) Lhline_bang(x);

	return;
/*
stemall:
	for(i=0; i<len; i++)
		x->x_tsilist[i].a_w.w_float = x->x_inlist[i].a_w.w_float;
	if(x->x_fire) Lhline_bang(x);
	
	return;
*/
}

void  Lhline_float (Lhline *x, float n)
{
	double	poslisratio=0.0, incremento=0.0, 
			a=0.0, b=0.0, 
			indice=0.0;
	short int	i=0, indi=0, 
			len = x->x_tsillen,
//			counter = x->x_break,
			dummy = 0, dist2dum =0;
	t_atom     *output, *input;
	
	output = x->x_tsilist;
	input  = x->x_inlist;

//	if (x->x_mode>0) counter /=2; post("counter %ld", counter,0);

	x->x_inlist[x->x_step].a_w.w_float = n;

	if (!x->x_mode) {
		x->x_step +=1;
		if(x->x_step>(x->x_break-1))x->x_step =0;
		if(x->x_fire) Lhline_bang(x);
//		return;
		}
	else if (x->x_mode==1) {
		x->x_step -=1;
		if(x->x_step<0 ) x->x_step =(x->x_break-1);
		if(x->x_fire) Lhline_bang(x);
//		return;
		}
	else if (x->x_mode==2) { //center to sides: o que se passa e que vao duas para o lado esquerdo, tb e bonito
		dummy = (x->x_break-1)/2; //center breakpoint
		dist2dum = dummy + x->x_step;

		x->x_inlist[dist2dum].a_w.w_float = n; //set right side


		//continua a decrementar o step desde metade do caminho ate 0	: left side	
		x->x_step -=1;
		if(x->x_step<0 ) x->x_step =dummy;
		if(x->x_fire) Lhline_bang(x);
//		return;
		}
	else if (x->x_mode==3) { //center to sides: o que se passa e que vao duas para o lado esquerdo, tb e bonito
		dummy = (x->x_break-1)/2; //center breakpoint
		dist2dum = 2*dummy + x->x_step;

		x->x_inlist[dist2dum].a_w.w_float = n; //set right side
		x->x_step -=1;
		if(x->x_step<0 ) x->x_step =dummy;
		if(x->x_fire) Lhline_bang(x);
		}

	for(i=0; i<len; i++){
		x->x_tsilist[i].a_w.w_float = x->x_inlist[i].a_w.w_float ;
	}

}


void  Lhline_int (Lhline *x, long n)
{
	double	poslisratio=0.0, incremento=0.0, 
			a=0.0, b=0.0, 
			indice=0.0;
	short int	i=0, indi=0, 
			len = x->x_tsillen;
	t_atom     *output, *input;
	
	output = x->x_tsilist;
	input  = x->x_inlist;

	x->x_inlist[x->x_step].a_w.w_float = (float)n;
//	if (x->x_break > len) return; 

	poslisratio = (double)len / x->x_break;

	x->x_tsilist[0].a_w.w_float = x->x_inlist[0].a_w.w_float; //1st arg is same
	
	for(i=1; i<len-1; i++){
		indice = (double)(i/poslisratio);
		indi = (int)(indice);		
		a = (double)x->x_inlist[indi].a_w.w_float;
		b = (double)x->x_inlist[indi+1].a_w.w_float;
		
		incremento = (double) ((-b+a)/poslisratio)*((indi)-indice);
		x->x_tsilist[i].a_w.w_float = x->x_inlist[indi].a_w.w_float + incremento;
	 }

	x->x_tsilist[len-1].a_w.w_float = x->x_inlist[x->x_break-1].a_w.w_float; //last arg is same

	x->x_step +=1;
	if(x->x_step>(x->x_break-1))x->x_step =0;
	if(x->x_fire) Lhline_bang(x);
}


void  Lhline_bang (Lhline *x)
{
	t_atom		*output;
	short 		i, j, k,len = x->x_tsillen, center, temp;
	
	output = x->x_tsilist;
	center = len / 2;  // the center argument takes first value
	
	if(x->x_mode == 0) { //l2r
		for(i=0; i<x->x_tsillen; i++){
			output[i].a_w.w_float = x->x_tsilist[i].a_w.w_float ;
			SETFLOAT(output+i, output[i].a_w.w_float);
		}
	}
	
	//este modo e q esta a fazer center->sides!!! e descarta metade dos argumentos
	else if(x->x_mode == 1) { //r2l
		for(i=0, j=(len-1);  i < len ; i++, j--){
			output[i].a_w.w_float = x->x_tsilist[j].a_w.w_float ;
			SETFLOAT(output+i, output[i].a_w.w_float);
		}
	}

	//este modo e q esta a fazer center->sides!!! e descarta metade dos argumentos
	else if(x->x_mode == 2) { //r2l
		for(i=0, j=(len-1);  i <= center ; i++, j--){
			output[i].a_w.w_float = x->x_tsilist[j].a_w.w_float ;
			SETFLOAT(output+i, output[i].a_w.w_float);
			SETFLOAT(output+j, output[j].a_w.w_float);
		}
	}

	else if(x->x_mode == 3) { //center->sides: this will use only +/-half of the stored values
		
		for(i=0, j=center, k=center;  i <= center ; i++, j++, k--){

			output[j].a_w.w_float = x->x_tsilist[i].a_w.w_float ;
			output[k].a_w.w_float = x->x_tsilist[i].a_w.w_float ;
			SETFLOAT(output+i, output[i].a_w.w_float);
		}
	}

	else if(x->x_mode == 4) { //random one
		
		for(i=0, j=center ;  i <= center ; i++, j++){
			temp = (short) ((float)rand()/(float)RAND_MAX * len);
			output[j].a_w.w_float = x->x_tsilist[temp].a_w.w_float ;
//			output[k].a_w.w_float = x->x_tsilist[temp].a_w.w_float ;
			SETFLOAT(output+i, output[i].a_w.w_float);
		}
	}

	else if(x->x_mode == 5) { //random two
		
		for(i=0, j=center, k=center;  i <= center ; i++, j++, k--){
			temp = (short) ((float)rand()/(float)RAND_MAX * len);
			output[j].a_w.w_float = x->x_tsilist[temp].a_w.w_float ;
			output[k].a_w.w_float = x->x_tsilist[temp].a_w.w_float ;
			SETFLOAT(output+i, output[i].a_w.w_float);
		}
	}


	outlet_list(x->x_outlet,0L,x->x_tsillen, output);	
}





void Lhline_assist(Lhline *x, void *b, long m, long a, char *s)		
{
 if ((m==1)&&(a==0)) 		sprintf(s,"(list), (int), (float), bang");
 else if ((m==1)&&(a==1)) 	sprintf(s,"(int) list lenght");
 else if ((m==2)&&(a==0)) 	sprintf(s,"(list) hglide out");
}

void *Lhline_new (long lenght)
{
	short i;
	Lhline *x;
	x = (Lhline *)newobject(Lhline_class);
	intin(x,1); 

	if(!lenght || lenght < 0) x->x_tsillen = 100;//register outlistsize
	else x->x_tsillen = lenght;

	//zero floats on the atom.float struct
	for (i=0;i<MAXSIZE;i++) {
		x->x_inlist[i].a_w.w_float =  0.000f;
		x->x_tsilist[i].a_w.w_float = 0.00f;		
		x->x_index[i].a_w.w_long =   0;		
		}

	x->x_outlet = listout(x);	
	x->x_fire = 1;
	x->x_mode = 0;
	x->x_break = 10, x->x_step=0 ; //default numbreakpoints, start increment point
	return (x);
} //line339