/*
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
a-fiddle - © ASier 2002

fiddle aid to construct terrains of sound

constroi listas de saida com
pares de posicao (frequencia do componente) / amplitude (altura,quantidade)

analisa a lista que entra de dois em dois valores. o primeiro valor estabelece
o pico, a localizacao que armazena o segundo valor


modos: 	so os picos (todos os outros valores a zero - ou a offset determinado)
(todo)	linear de abertura lambda (tudo a zero, rampas para os valores e dos valores a 0)
		
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"

#define MAXSIZE 256



typedef struct {
	t_object 			x_ob;

	t_atom			x_inlist[MAXSIZE];	//store input list inside
	short				x_tsillen; 			//the lenght - max 256
	Boolean			x_fire;			//bang on calc?
	Boolean			x_clear;			//clearmode
	double				x_offset;											

	void 			       *x_outlet;
} afiddle;


void *afiddle_new 		(long lenght);
void  afiddle_assist		(afiddle *x, void *b, long m, long a, char *s);			
void  afiddle_bang		(afiddle *x);												
void  afiddle_in1			(afiddle *x, long n);											
void  afiddle_list			(afiddle *x,Symbol *s, short argc, Atom *argv);											
void  afiddle_fire		(afiddle *x, long n);											
void  afiddle_clearm		(afiddle *x, long n);											
void  afiddle_clear		(afiddle *x);											
void  afiddle_offset		(afiddle *x, double n);											
void  afiddle_free		(afiddle *x);	//not required?												

void *afiddle_class		; 					



void  afiddle_free(afiddle *x){ notify_free((t_object *)x);}												
void  afiddle_fire(afiddle *x, long n){ x->x_fire = n;}												
void  afiddle_clearm(afiddle *x, long n){ x->x_clear = n;}												
void  afiddle_offset(afiddle *x, double n){ x->x_offset = n;}												
void  afiddle_in1 (afiddle *x, long n)
{
	if(n < 1) x->x_tsillen = 100;//register outlistsize
	else x->x_tsillen = n;
}

void  afiddle_clear(afiddle *x)
{
 	short i;
	
	for (i=0; i < MAXSIZE; i++) x->x_inlist[i].a_w.w_float = x->x_offset;
 }


void  afiddle_list(afiddle *x,Symbol *s, short argc, Atom *argv)
{
	t_atom      *input;
	
	short		i, where, index;
	
	input  = x->x_inlist;


	if(x->x_clear)	//zero it out first to make sure all is zero
	for (i=0; i < MAXSIZE; i++) x->x_inlist[i].a_w.w_float = x->x_offset;

	//parse & set em
	for (i=0; i < argc; i++) {
	
		where = i % 2;

		if (!where) { // estamos a ver o indice na lista final
			if (argv[i].a_type==A_LONG) {
				index = argv[i].a_w.w_long;
			}
			else if (argv[i].a_type==A_FLOAT) {
				index =(short) argv[i].a_w.w_float;
			}
			else if (argv[i].a_type==A_SYM) {
				error("wrong argument type");
			}
		
	    } else if (where) { // estamos a ver valor a colocar no indice
			if (argv[i].a_type==A_LONG) {
				x->x_inlist[index].a_w.w_float = (double)argv[i].a_w.w_long;
			}
			else if (argv[i].a_type==A_FLOAT) {
				x->x_inlist[index].a_w.w_float = (double)argv[i].a_w.w_float;
			}
			else if (argv[i].a_type==A_SYM) {
				error("wrong argument type");
			}
		}
	 }


	if(x->x_fire) afiddle_bang(x);

	return;
}


void  afiddle_bang (afiddle *x)
{
	t_atom		*output;
	short 		i;
	
	output = x->x_inlist;
	
	for(i=0; i<x->x_tsillen; i++){
		output[i].a_w.w_float = x->x_inlist[i].a_w.w_float ;
		SETFLOAT(output+i, output[i].a_w.w_float);
	}
	
	outlet_list(x->x_outlet,0L,x->x_tsillen, output);	
}





void afiddle_assist(afiddle *x, void *b, long m, long a, char *s)		
{
 if ((m==1)&&(a==0)) 		sprintf(s,"(list), bang");
 else if ((m==1)&&(a==1)) 	sprintf(s,"(int) list lenght");
 else if ((m==2)&&(a==0)) 	sprintf(s,"(list)fiddle out");
}

void *afiddle_new (long lenght)
{
	short i;
	afiddle *x;
	x = (afiddle *)newobject(afiddle_class);
	intin(x,1); 

	if(lenght < 1) x->x_tsillen = 100;//register outlistsize
	else x->x_tsillen = lenght;

	//zero doubles on the atom.double struct
	for (i=0;i<MAXSIZE;i++) {
		x->x_inlist[i].a_w.w_float=0.000f;
		}

	x->x_outlet = listout(x);	
	x->x_fire = 1;
	x->x_clear = 1;
	x->x_offset = 0.00f;
	return (x);
} 

void main(void)
{
	long int tick = gettime();
		
	setup((t_messlist **)&afiddle_class, (method)afiddle_new,\
	(method)afiddle_free, (short)sizeof(afiddle), 0L, A_DEFLONG, 0); //initial arg default len
	addmess((method)afiddle_list,"list",A_GIMME,0);
	addinx((method)afiddle_in1, 1);
	addbang((method)afiddle_bang);
	addmess((method)afiddle_offset,"offset", A_FLOAT, 0);
	addmess((method)afiddle_in1,"size", A_LONG, 0);
	addmess((method)afiddle_fire,"fire", A_LONG, 0);
	addmess((method)afiddle_clearm,"clearmode", A_LONG, 0);
	addmess((method)afiddle_clear,"clear", 0);
	addmess((method)afiddle_assist,"assist",A_CANT,0);
	post(__DATE__" \t \a \t  "__TIME__"\t %07xd",tick, 0);
}