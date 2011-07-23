/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
kin - © andrŽ sier 2002
		
		output kinema scope coords


20031029	adicionar a msg "size" q se traduz em 
			dist max - dist min -> centre * 2; e
			
20031102	05:13 da manha. levantar a meio da noite
			para arranjar:
				
				- varias saidas para os pares conjugados
					xy, x'y, xy',x'y'
	
	20040828 updated double version		
	
	2004 04 18 --- internals to double, phps crashes intel
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"



typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out[4];		// value	

	double			coords[4]; // so preciso de 2! os mirrors de cada coordenada!
	double			centre[2];
	double			input[2];
	double			move[2];
	t_atom			out[255];	
	char			mode, om;
} kin;


void *kin_new (Symbol *msg, short argc, Atom *argv);
void kin_bang (kin *x);

void kin_calc (kin *x);
void kin_mode (kin *x, long echo);
void kin_om (kin *x, long echo);

void kin_echo (kin *x, double echo);
void kin_int(kin *x, int n);
void kin_double(kin *x, double n);
void kin_list (kin *x, Symbol *msg, short argc, Atom *argv);

void kin_centre (kin *x, double echo, double ech);
void kin_input (kin *x, double echo, double ech);
void kin_move (kin *x, double echo, double ech);
void kin_do_move (kin *x);
void kin_size (kin *x, double echo, double ech);

void kin_post (kin *x);

void kin_assist(kin *x, void *b, long m, long a, char *s);
void *kin_class;

double map (double x, double a, double b, double c, double d) ;


void *kin_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 kin *x;
	 int i, j;
	 double s, t, temp[3];
	 
	 x=(kin *)newobject(kin_class);

	 x->c_out[3]=listout(x);
	 x->c_out[2]=listout(x);
	 x->c_out[1]=listout(x);
	 x->c_out[0]=listout(x);
	 

		x->centre[0] = 160.;
		x->centre[1] = 120.;
		x->input[0] = 0.;
		x->input[1] = 0.;
		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->mode = 1;
		x->move[0] = 0.;
		x->move[1] = 0.;
		x->om = 0;


	if (argc) { // 
		if(argv->a_w.w_sym == gensym("@size")){
			argv++;
	
				for(j=0;j<2;j++) {
				if(argv->a_type == A_FLOAT) {
					temp[j] = (double)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[j] = (double)argv->a_w.w_long;	
					argv++;
				}
			}
			s = temp[0];
			t = temp[1];
			kin_size (x, s, t);
		}
		if(argv->a_w.w_sym == gensym("@center")){
			argv++;
	
				for(j=0;j<2;j++) {
				if(argv->a_type == A_FLOAT) {
					temp[j] = (double)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[j] = (double)argv->a_w.w_long;	
					argv++;
				}
			}
			s = temp[0];
			t = temp[1];
			kin_centre (x, s, t);

		}
		if(argv->a_w.w_sym == gensym("@c")){
			argv++;
				for(j=0;j<2;j++) {
				if(argv->a_type == A_FLOAT) {
					temp[j] = (double)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[j] = (double)argv->a_w.w_long;	
					argv++;
				}
			}
			s = temp[0];
			t = temp[1];
			kin_centre (x, s, t);
		}
		if(argv->a_w.w_sym == gensym("@move")){
			argv++;
				for(j=0;j<2;j++) {
				if(argv->a_type == A_FLOAT) {
					temp[j] = (double)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[j] = (double)argv->a_w.w_long;	
					argv++;
				}
			}
			s = temp[0];
			t = temp[1];
			kin_move (x, s, t);
		}
		if(argv->a_w.w_sym == gensym("@om")){
			x->om = argv->a_w.w_long;
		}

	} else   {
		x->centre[0] = 160;
		x->centre[1] = 120;
		x->input[0] = 0;
		x->input[1] = 0;
		x->coords[0] = 0;
		x->coords[1] = 0;
		x->mode = 1;
		x->move[0] = 0;
		x->move[1] = 0;
		x->om = 0;
	}	

	 return(x);	
}


void  kin_calc (kin *x)
{
	double	right = x->centre[0]*2;
	double	bottom = x->centre[1]*2;
	

		x->coords[0] =  x->input[0];//x
		x->coords[1] =  x->input[1];//y
		x->coords[2] = right - x->input[0];//x' - x		! mirror about the x-axis
		x->coords[3] = bottom - x->input[1];//y' - y	! mirror about the y-axis

}

void kin_centre (kin *x, double echo, double ech)
{
	x->centre[0] = echo;
	x->centre[1] = ech;
	kin_bang(x);
}
void kin_input (kin *x, double echo, double ech)
{
	x->input[0] = echo;
	x->input[1] = ech;
	kin_bang(x);
}
void kin_move (kin *x, double echo, double ech)
{
	x->move[0] = echo;
	x->move[1] = ech;
	kin_bang(x);
}

void kin_do_move (kin *x)
{
	if (x->move[0] == 0. && x->move[1] == 0.)
		return;
	else	{
	x->coords[0] += x->move[0];
	x->coords[1] += x->move[1];
	x->coords[2] += x->move[0];
	x->coords[3] += x->move[1];
	}
}

void kin_size (kin *x, double echo, double ech)
{
	x->centre[0] = echo/2; //the center coords will be /2
	x->centre[1] = ech/2;

}

void kin_post (kin *x)
{
post(" 	");
post(" 	");
post(" 		a-kin v1.5 20031102");
post(" 	");
post(" x-center  = %ld", x->centre[0]);
post(" y-center  = %ld", x->centre[1]);
post(" coords[0] = %ld", x->coords[0]);
post(" coords[1] = %ld", x->coords[1]);
post(" coords[2] = %ld", x->coords[2]);
post(" coords[3] = %ld", x->coords[3]);
post(" om        = %ld", x->om);
post(" input[0]  = %ld", x->input[0]);
post(" input[1]  = %ld", x->input[1]);
post(" move[0]   = %ld", x->move[0]);
post(" move[1]   = %ld", x->move[1]);
post(" 		");
post(" 	");
}


void kin_echo (kin *x, double echo)
{

}

void kin_int(kin *x, int n)     { kin_echo(x, n);}
void kin_double(kin *x, double n) { kin_echo(x, n);}

void kin_mode (kin *x, long echo)
{
	x->mode = echo;
}

void kin_om (kin *x, long echo)
{
	x->om = echo;
}


void kin_bang (kin *x)
{
	t_atom	*out;
	out	= x->out;
	
	kin_calc(x);
	kin_do_move(x);


		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out[0], 0L, 2, out); //output first outlet input pair 

		SETFLOAT(out+0, x->coords[2]);
		SETFLOAT(out+1, x->coords[1]);
	outlet_list(x->c_out[1], 0L, 2, out); //output x'y

		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[3]);
	outlet_list(x->c_out[2], 0L, 2, out); //output xy'

		SETFLOAT(out+0, x->coords[2]);
		SETFLOAT(out+1, x->coords[3]);
	outlet_list(x->c_out[3], 0L, 2, out); //output x'y'

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&kin_class,(method)kin_new,0L,(short)sizeof(kin),0L,
 A_GIMME,0);

 addbang((method)kin_bang);
 
 addmess((method)kin_input,"input",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)kin_centre,"center",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)kin_size,"size",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)kin_mode,"mode",A_DEFFLOAT,0);
 addmess((method)kin_move,"move",A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)kin_om,"om",A_DEFLONG,0);
 addmess((method)kin_post,"post",0);

// addint((method)kin_int);
// adddouble((method)kin_double);
 addmess((method)kin_assist,"assist", A_CANT, 0);
 post("	 \t \a \t  %08xd	  			a-kin3ma © A-Sier 2003"__TIME__,tick,0);
}


void kin_assist(kin *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) xy     e         c            h              o"); }
    else if (m==2&&a==1) { sprintf(s,"(list) x'y     e         c            h              o"); }
    else if (m==2&&a==2) { sprintf(s,"(list) xy'     e         c            h              o"); }
    else if (m==2&&a==3) { sprintf(s,"(list) x'y'     e         c            h              o"); }
}


