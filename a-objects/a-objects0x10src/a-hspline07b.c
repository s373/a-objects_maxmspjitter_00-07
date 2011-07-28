/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
hspline - andrŽ sier 2005
		
using hermite splines (less elaborate than b-splines) on a bunch
of equally spaced ctrl input points (the stride parameter of the 
equation)

smooth out path making

hist:

20050902 - version 01 departs from hangar
20050907 - added moveto, after days of fun with stack
20050907 - added percent in out points (to leave 0.1. range if desired)

20051007 - missed added stack method!! earlier, for the conference still, re-added alpha2 today

20060930 - stack method was broken with deffloats, replaced with A_GIMME and is working great

20070920 - updated bang method to stop if no points in
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

*/

#include "ext.h"
#include "ext_common.h"
//#include "math.h"


typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet value	
	void				*c_out2;
	void				*c_out3;

	double			coords[3];		// as xy de saida
	double			move[3];		// deslocamos o centro ?
	double			steps;			// quantos passos por hsplineulo
	double			step;			// em q passo estamos
	double			stride;			// a dist de passo a passo
	t_atom			out[3];			// nao preciso de mais q 3 de saida

	Boolean			end, om; // se chegou ao fim do stride, e o modo de saida
	double			a[3], b[3]; // as coordenadas de cada ponto
	double			c[3], d[3]; // as coordenadas de cada ponto; precisamos de 4 pontos para este spline
	short			numpoints, whichpoint; // quantos argumentos a lista tem, qual Ž o primeiro ponto...
	double			inlist[256]; // armazenar os pontos a iterar sobre
	double			alpha;
	double			alpha2;
	
	int				stackid;
	
	double			range[2];
	int endd;

} hspline;


void *hspline_new (Symbol *msg, short argc, Atom *argv);
void hspline_bang (hspline *x);

void hspline_calc (hspline *x);
void hspline_move (hspline *x, double echo, double ech, double ec);
void hspline_stride (hspline *x, double echo);
void hspline_steps (hspline *x, double echo);
void hspline_step (hspline *x, double echo);
void hspline_st(hspline *x, int n);

void hspline_echo (hspline *x, double echo);
void hspline_int(hspline *x, int n);
void hspline_double(hspline *x, double n);

void hspline_alpha(hspline *x, Symbol *msg, short argc, Atom *argv);

void hspline_stack (hspline *x, Symbol *msg, short argc, Atom *argv);
void hspline_moveto (hspline *x, double echo, double ech, double ec);


void hspline_a (hspline *x, double echo, double ech, double ec);
void hspline_b (hspline *x, double echo, double ech, double ec);
void hspline_c (hspline *x, double echo, double ech, double ec);
void hspline_d (hspline *x, double echo, double ech, double ec);
void hspline_ang_len (hspline *x, double azi, double ele, double dist);


void hspline_mode (hspline *x, long echo);
void hspline_om (hspline *x, long echo);

void hspline_endd (hspline *x, long endd);

void hspline_list (hspline *x, Symbol *msg, short argc, Atom *argv);
void hspline_setpt (hspline *x, long echo);


void hspline_post (hspline *x);

void hspline_assist(hspline *x, void *b, long m, long a, char *s);
void *hspline_class;


void *hspline_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 hspline *x;
	 int i;
	 
	 x=(hspline *)newobject(hspline_class);


	 x->c_out3=bangout(x);// output bang at end points
	 x->c_out2=bangout(x);// output bang at end
	 x->c_out=listout(x);// xy hspline coords
	 

	x->endd=0;
		x->coords[0] = 0.;
		x->coords[1] = 0.;
		x->coords[2] = 0.;
		x->move[0] 	 = 0.; //so that the convolution is about
		x->move[1] 	 = 0.; // the ooo origin
		x->move[2] 	 = 0.; // the ooo origin
		x->om = 0;
	
		x->a[0] = 0.0;
		x->a[1] = 0.0;
		x->a[2] = 0.0;
		
		x->b[0] = 0.1;
		x->b[1] = 1.0;
		x->b[2] = 1.0;
		
		x->c[0] = 0.2;
		x->c[1] = 0.0;
		x->c[2] = 0.0;
		
		x->d[0] = 0.3;
		x->d[1] = 1.0;
		x->d[2] = 1.0;

		x->alpha = 0.0;
		x->alpha2 = 0.0;

		x->range[0] = 0.0;
		x->range[1] = 1.0;

		x->steps = 1.;
		x->step = 0.;
		x->stride = 0.01;

		x->numpoints = 2;
		x->whichpoint = 0;
		x->end = 0;

		for(i=0;i<256;i++)
			x->inlist[i] = 0.0;



	 return(x);	
}


void hspline_endd (hspline *x, long endd)
{
	x->endd=endd;
}


void hspline_a (hspline *x, double echo, double ech, double ec)
{
	x->a[0]= echo;
	x->a[1]= ech;
	x->a[2]= ec;

	x->numpoints = 2;
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}

void hspline_stack (hspline *x, Symbol *msg, short argc, Atom *argv)
{

	double	xyz[3];
	short i;


	if(x->numpoints!=1) x->numpoints=1; //special; avoids calling inlist and sets immediatly

	
//	x->numpoints = (short)argc/3;
//	post("numpoint %ld",x->numpoints);
	 
	for (i=0; i<argc; i++) {
		
		if (argv->a_type == A_FLOAT) {
			xyz[i] = (double)argv->a_w.w_float;
			argv++;
		} 
		else if (argv->a_type == A_LONG) {
			xyz[i] = (double)argv->a_w.w_long;
			argv++;
		} 
	
	
	}


	
//	post("input to stack is %f %f %f",xyz[0], xyz[1], xyz[2]);

		x->a[0]= x->b[0];		x->a[1]= x->b[1];		x->a[2]= x->b[2];	
		x->b[0]= x->c[0];		x->b[1]= x->c[1];		x->b[2]= x->c[2];	
		x->c[0]= x->d[0];		x->c[1]= x->d[1];		x->c[2]= x->d[2];	
		x->d[0]= xyz[0];			x->d[1]= xyz[1];			x->d[2]= xyz[2];	

}

void hspline_moveto (hspline *x, double echo, double ech, double ec)
{

	if(x->numpoints!=1) x->numpoints=1; //special; avoids calling inlist and sets immediatly


		x->a[0]= echo;		x->a[1]= ech;		x->a[2]= ec;	
		x->b[0]= echo;		x->b[1]= ech;		x->b[2]= ec;	
		x->c[0]= echo;		x->c[1]= ech;		x->c[2]= ec;	
		x->d[0]= echo;			x->d[1]= ech;			x->d[2]= ec;	

}

void hspline_range (hspline *x, double echo, double ech)
{
	x->range[0]= echo;
	x->range[1]= ech;
}

void hspline_b (hspline *x, double echo, double ech, double ec)
{
	x->b[0]= echo;
	x->b[1]= ech;
	x->b[2]= ec;

	if(x->numpoints!=2) x->numpoints = 2;

	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}
void hspline_c (hspline *x, double echo, double ech, double ec)
{
	x->c[0]= echo;
	x->c[1]= ech;
	x->c[2]= ec;

	if(x->numpoints!=2) x->numpoints = 2;
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}
void hspline_d (hspline *x, double echo, double ech, double ec)
{
	x->d[0]= echo;
	x->d[1]= ech;
	x->d[2]= ec;

	if(x->numpoints!=2) x->numpoints = 2;

	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}

void hspline_ang_len (hspline *x, double azi, double ele, double dist)
{
	
}
void hspline_post (hspline *x)
{
	post("a is : %f %f %f", x->a[0], x->a[1], x->a[2],0L);
	post("b is : %f %f %f", x->b[0], x->b[1], x->b[2],0L);
	post("c is : %f %f %f", x->c[0], x->c[1], x->c[2],0L);
	post("d is : %f %f %f", x->d[0], x->d[1], x->d[2],0L);
	post("");
}

void hspline_alpha (hspline *x, Symbol *msg, short argc, Atom *argv)
{
	double temp[2]; int i;

	if(argc==1) {
		if (argv->a_type == A_FLOAT) {
			temp[0] = argv->a_w.w_float;
		} 
		else if (argv->a_type == A_LONG) {
			temp[0] = (double)argv->a_w.w_long;
		} 
		x->alpha = temp[0];
		x->alpha2 = temp[0];
	} else {
		for (i=0;i<2;i++) {
			if (argv->a_type == A_FLOAT) {
				temp[i] = argv->a_w.w_float;
				argv++;
			} 
			else if (argv->a_type == A_LONG) {
				temp[i] = (double)argv->a_w.w_long;
				argv++;
			} 
		}
		x->alpha = temp[0];
		x->alpha2 = temp[1];	
	}



	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}


void hspline_list (hspline *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	x->numpoints = (short)argc/3;
//	post("numpoint %ld",x->numpoints);
	 
	for (i=0; i<argc; i++) {
		
		if (argv->a_type == A_FLOAT) {
			x->inlist[i] = argv->a_w.w_float;
			argv++;
		} 
		else if (argv->a_type == A_LONG) {
			x->inlist[i] = (double)argv->a_w.w_long;
			argv++;
		} 
	
	
	}

}

void  hspline_calc (hspline *x)
{
	double 	value, step = x->step, a[3], b[3], c[3], d[3], move[3], stride = x->stride;
	short	pt = x->whichpoint, i;
	double	step2, step3, alpha, alpha2, m0[3], m1[3], block[4];

	if (x->numpoints > 2) // se tivermos mais do que dois argumentos
	{						// fazer um mŽtodo que faa wrapping de tudo isto..

		if (x->numpoints == 3 || pt == 0) {		// 		special first point case			
			x->a[0] = x->inlist[3*pt+0];
			x->a[1] = x->inlist[3*pt+1];
			x->a[2] = x->inlist[3*pt+2];
			x->b[0] = x->inlist[3*pt+0];
			x->b[1] = x->inlist[3*pt+1];
			x->b[2] = x->inlist[3*pt+2];
			x->c[0] = x->inlist[3*pt+3];
			x->c[1] = x->inlist[3*pt+4];
			x->c[2] = x->inlist[3*pt+5];
			x->d[0] = x->inlist[3*pt+6];
			x->d[1] = x->inlist[3*pt+7];
			x->d[2] = x->inlist[3*pt+8];
		}else if (pt == (x->numpoints-2)) {		// 		special last point case					
			x->a[0] = x->inlist[3*(pt-1)+0];
			x->a[1] = x->inlist[3*(pt-1)+1];
			x->a[2] = x->inlist[3*(pt-1)+2];
			x->b[0] = x->inlist[3*(pt-1)+3];
			x->b[1] = x->inlist[3*(pt-1)+4];
			x->b[2] = x->inlist[3*(pt-1)+5];
			x->c[0] = x->inlist[3*(pt-1)+6];
			x->c[1] = x->inlist[3*(pt-1)+7];
			x->c[2] = x->inlist[3*(pt-1)+8];
			x->d[0] = x->inlist[3*(pt-1)+6];
			x->d[1] = x->inlist[3*(pt-1)+7];
			x->d[2] = x->inlist[3*(pt-1)+8];
		}else {									// 		all other points case
			x->a[0] = x->inlist[3*(pt-1)+0];
			x->a[1] = x->inlist[3*(pt-1)+1];
			x->a[2] = x->inlist[3*(pt-1)+2];
			x->b[0] = x->inlist[3*(pt-1)+3];
			x->b[1] = x->inlist[3*(pt-1)+4];
			x->b[2] = x->inlist[3*(pt-1)+5];
			x->c[0] = x->inlist[3*(pt-1)+6];
			x->c[1] = x->inlist[3*(pt-1)+7];
			x->c[2] = x->inlist[3*(pt-1)+8];
			x->d[0] = x->inlist[3*(pt-1)+9];
			x->d[1] = x->inlist[3*(pt-1)+10];
			x->d[2] = x->inlist[3*(pt-1)+11];

		}
	}

	a[0]= x->a[0];	a[1]= x->a[1];	a[2]= x->a[2];
	b[0]= x->b[0];	b[1]= x->b[1];	b[2]= x->b[2];
	c[0]= x->c[0];	c[1]= x->c[1];	c[2]= x->c[2];
	d[0]= x->d[0];	d[1]= x->d[1];	d[2]= x->d[2];

//	post("a is : %f %f %f", x->a[0], x->a[1], x->a[2],0L);
//	post("b is : %f %f %f", x->b[0], x->b[1], x->b[2],0L);
//	post("c is : %f %f %f", x->c[0], x->c[1], x->c[2],0L);
//	post("d is : %f %f %f", x->d[0], x->d[1], x->d[2],0L);
//	post("");
	
	move[0] = x->move[0];
	move[1] = x->move[1];
	move[2] = x->move[2];
	
	// hermite spline's method
	// p(t) = (2t^3 - 3t^2 + 1)p0 + (t^3 -2t^2+t)m0 + (t^3-t^2)m1 + (-2t^3 + 3t^2)p1
	// m_i = ((1-Œ)/2) ((p_i - p_i-1)+(p_i+1 - p_i)) 

	step2 = step*step;
	step3 = step2*step;
	alpha = x->alpha;
	alpha2 = x->alpha2;

	// eq multiplier blocks
	block[0] = 2*step3 - 3*step2 + 1;
	block[1] = step3 - 2*step2 + step;
	block[2] = step3 - step2;
	block[3] = -2*step3 + 3*step2;
	
	for(i=0;i<3;i++) {
		m0[i] = ((1.0-alpha) * 0.5) * ((b[i] - a[i]) + (c[i] - b[i])); 
		m1[i] = ((1.0-alpha2) * 0.5) * ((c[i] - b[i]) + (d[i] - c[i])); 
	}

	for(i=0;i<3;i++) {
		x->coords[i] = block[0]*b[i] + block[1]*m0[i] + block[2]*m1[i] + block[3]*c[i];
	}
	
	
	if (move[0] !=0. || move[1] != 0. || move[2] != 0.){
 	 x->coords[0] += move[0];	
 	 x->coords[1] += move[1];	
 	 x->coords[2] += move[2];	
	}

	// advance stride and bang before last if if
	x->step += stride;
	value = x->step + stride;

	if (value > x->steps) 
	{
		if(x->endd==0) {
			x->step = 0.0;
			x->end = 1;
		} else {
			x->step = x->steps;//0.0;
		}
	}
	if (value < 0.0f)
	{
		x->step = x->steps;
		x->end = 1;
	}

}

void hspline_echo (hspline *x, double echo) //compute directly from number, no iter
{
	t_atom	*out;
	double 	value,  a[3], b[3], move[3];

	out	= x->out;

	a[0]= x->a[0];
	a[1]= x->a[1];
	a[2]= x->a[2];
	b[0]= x->b[0];
	b[1]= x->b[1];
	b[2]= x->b[2];
	move[0] = x->move[0];
	move[1] = x->move[1];
	move[2] = x->move[2];
	
	x->coords[0] = echo * (x->b[0] - x->a[0]) + x->a[0];
	x->coords[1] = echo * (x->b[1] - x->a[1]) + x->a[1];
	x->coords[2] = echo * (x->b[2] - x->a[2]) + x->a[2];
	
	if (move[0] !=0. || move[1] != 0. || move[2] != 0.){
 	 x->coords[0] += move[0];	
 	 x->coords[1] += move[1];	
 	 x->coords[2] += move[2];	
	}
	
		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
		SETFLOAT(out+2, x->coords[2]);

	outlet_list(x->c_out, 0L, 3, out); //xy hsplineular coords
}

void hspline_int(hspline *x, int n)     { hspline_echo(x, (double)n);}
void hspline_double(hspline *x, double n) { hspline_echo(x, n);}



void hspline_move (hspline *x, double echo, double ech, double ec)
{
	x->move[0] = echo;
	x->move[1] = ech;
	x->move[2] = ec;
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}


void hspline_setpt (hspline *x, long echo)
{
	x->whichpoint = (short)echo; //
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}
void hspline_stride (hspline *x, double echo)
{
	x->stride = echo; //
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}
void hspline_st(hspline *x, int n)
{
	if (n>0) {
		x->stride = x->steps/n;
	}
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}

void hspline_steps (hspline *x, double echo)
{
	x->steps = echo; //
//	if (x->steps<0.1)
//		x->steps = 100.;
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}

void hspline_step (hspline *x, double echo)
{
	x->step = echo; //
//	if (x->step<0.1)
//		x->step = 100.;
	if (x->om)	// output do novo hsplineulo apenas se nao ha silencio
		hspline_bang(x);
}





void hspline_om (hspline *x, long echo)
{
	x->om = echo;
}


void hspline_bang (hspline *x)
{
	t_atom	*out;
	out	= x->out;
	
	hspline_calc(x);


		SETFLOAT(out+0, x->coords[0]);
		SETFLOAT(out+1, x->coords[1]);
		SETFLOAT(out+2, x->coords[2]);

	outlet_list(x->c_out, 0L, 3, out); //xy hsplineular coords

	if (x->end) {

		x->end = 0;

		outlet_bang(x->c_out2);

		if (x->numpoints>2) {
			x->whichpoint +=1; // avanamos o ponto que queremos ouvir
			if (x->whichpoint> (x->numpoints-2)) {// por agora: deve ser uma array com 4 pontos sempre 
				x->whichpoint = 0;					// activos.. ver isto bem..
				outlet_bang(x->c_out3);
			}
		x->end = 0;

		}

	}

}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&hspline_class,(method)hspline_new,0L,(short)sizeof(hspline),0L,
 A_GIMME,0);

 addbang((method)hspline_bang);
 addmess((method)hspline_list,"list",A_GIMME,0);
 addmess((method)hspline_stack,"stack",A_GIMME, 0);
 addmess((method)hspline_moveto,"moveto",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)hspline_range,"range",A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)hspline_a,"a",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)hspline_b,"b",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)hspline_c,"c",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)hspline_d,"d",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)hspline_move,"move",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0);
 addmess((method)hspline_stride,"stride",A_DEFFLOAT,0);
 addmess((method)hspline_st,"steps",A_DEFLONG,0);							//
 addfloat((method)hspline_double);
 addint((method)hspline_int);
 addmess((method)hspline_setpt,"set",A_DEFLONG,0);
 addmess((method)hspline_alpha,"alpha",A_GIMME,0);
 addmess((method)hspline_endd,"end",A_DEFLONG,0);

 addmess((method)hspline_post,"post",0);

 addmess((method)hspline_assist,"assist", A_CANT, 0);

 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-hspline3d   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void hspline_assist(hspline *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"bang, messages, list"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) hspline xyz    e         c            h              o"); }
    else if (m==2&&a==1) { sprintf(s,"(ping) hspline end ramp    e         c            h              o"); }
    else if (m==2&&a==2) { sprintf(s,"(ping) hspline end points ramp    e         c            h              o"); }
}


