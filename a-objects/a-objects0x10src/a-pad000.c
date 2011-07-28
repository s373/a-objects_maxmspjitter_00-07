/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
a-pad - © andrŽ sier 2004
		
		pad recebe uma lista e junta N elementos entre os valores

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"

#define MAXSIZE 257

typedef struct	_pad
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet	

	t_atom 				o[MAXSIZE], i[MAXSIZE]; //i and o lists
	short				n; //how many
	float				value; // the offset value
	short				ni;	// # inputs on the list
	short				no;
} pad;



void *pad_new (Symbol *msg, short argc, Atom *argv);
void pad_calc (pad *x); //exec rotation
void pad_bang (pad *x); //exec rotation
void pad_list (pad *x, Symbol *msg, short argc, Atom *argv); //input the args 

void pad_many (pad *x, long n); 
void pad_value (pad *x, float value); 

void pad_assist(pad *x, void *b, long m, long a, char *s);
void *pad_class;




void pad_many (pad *x, long n) 
{
	n = CLIP (n, 0, MAXSIZE);
	x->n = n;
}
void pad_value (pad *x, float value)
{
	x->value = value;
} 


void *pad_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
 pad *x;
 int i;
 
 x=(pad *)newobject(pad_class);


 x->c_out=listout(x);


	if (argc) {
		if (argc==1) { //if 1 arg
			if (argv[0].a_type == A_LONG){
//				x->n = pad_many(x, argv[0].a_w.w_long);
				x->n = argv[0].a_w.w_long;
			} else if (argv[0].a_type == A_FLOAT){
				x->n = (long)argv[0].a_w.w_float;
     		  	}		

		} else if (argc>=2) { //2 args
			if (argv[0].a_type == A_LONG){ //if first arg == long
				x->n = argv[0].a_w.w_long;
				x->value = argv[1].a_w.w_long;
			} else if (argv[0].a_type == A_FLOAT){ 
				x->n = argv[0].a_w.w_float;
				x->value =argv[1].a_w.w_float;
			}		

		}	
	} else if (!argc) { //end if args
			x->n = 2;
			x->value = 0;
	}

 return(x);	
}

void pad_list (pad *x, Symbol *msg, short argc, Atom *argv) //input the args 
{
	short i;
	t_atom *input;
	
	input = x->i;

	if (argc) {

		x->ni = argc;
		for (i=0; i< argc; i++) {	//set the list inside
			switch (argv[i].a_type) {
				case A_FLOAT:
					x->i[i].a_w.w_float = argv[i].a_w.w_float;
					break;
				case A_LONG:
					x->i[i].a_w.w_float = (float)argv[i].a_w.w_long;
					break;
				case A_SYM:
					error("now, that's a sym isn't it?");
					break;
			}		

		}
		pad_calc(x);
	}
}

void  pad_calc (pad *x)
{
	short i, j, k, num = x->ni, no;
	t_atom *o, *input;
	float	temp[MAXSIZE];

	
	o = x->o;
	input = x->i;

	k = 0;
	
	temp[k] = x->i[0].a_w.w_float; //first element is the number
	
	for (i=1; i< (num-1); i++) {	
		for (j=0; j<x->n; j++){
			k++;
			temp[k] = x->value;
		}	
		k++;
		temp[k] = x->i[i].a_w.w_float;
	}
	
	k++;
	temp[k] = x->i[num-1].a_w.w_float;
	
	// do this better
	no = CLIP (k, 0, MAXSIZE);
	x->no = no;
	for (i=0; i<no; i++)
		SETFLOAT (o+i, temp[i]);
		
	outlet_list(x->c_out,0L,no, o);	
}

void pad_bang (pad *x)
{
	pad_calc(x);
}


void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&pad_class,(method)pad_new,0L,(short)sizeof(pad),0L,
 A_GIMME,0);
 
 addbang((method)pad_bang);
 addmess((method)pad_list, "list", A_GIMME, 0);
 addmess((method)pad_many, "num", A_LONG, 0);
 addmess((method)pad_value, "val", A_FLOAT, 0);
 addfloat((method)pad_value);
 addmess((method)pad_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t %08xd                                 a-pad   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);
//new post method 20040124
}


void pad_assist(pad *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) pad     e         c            h              o"); }
 //   else if (m==2&&a==1) { sprintf(s,"(float) y       e             c                h                   o"); }
}


