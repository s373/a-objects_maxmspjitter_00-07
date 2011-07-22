/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
spring - © andrŽ sier 2002
		
		mola el]astica unidimensional
		a=Vo/w;
		b=Zo;

		w = Ã (k/m)
		Ä = w/2pi
		
		Z(t) = A sin wt + B cos wt;		A,B pontos minimo e maximo
		
		ou
		
		Z(t) = C sin (wt + ¶)
		
			onde
			
			C = Ã ( Vo^2/W^2 + Zo^2 )	C, ¶ amplitude das oscilacoes e fase
			e ¶ = sin ^ -1 Zo/C







    A simple version of a physics simulation

    This is a text-only version of the simple spring simulation.



    Part of the www.MyPhysicsLab.com physics simulation applet.
    Copyright (c) 2001  Erik Neumann



    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.



    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.



    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



    Contact Erik Neumann at erikn@MyPhysicsLab.com or
    610 N. 65th St. Seattle WA 98103



x = position of the block
v = velocity of the block
m = mass of the block
R = rest length of the spring
k = spring stiffness







  Single spring & single mass, in one dimension (moving horizontally)



  vars[0] = position (x) with origin as above
  vars[1] = velocity (v=x')
  R = rest length
  len = current length of spring = x - origin.x
  L = how much spring is stretched from rest length
  L = len - R = x - origin.x - R
  k = spring constant
  b = damping constant
  F = m a  (force = mass * acceleration) leads to:
  F = -kL -bv = -k(x - origin.x - R) -bv = m v'
  so diffeq's are:
  x' = v
  v' = -(k/m)(x - origin.x - R) -(b/m)v



		
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"




typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;			
	void 		        *c_out2;			

	double	vars[2];	// 0 pos, 1 vel
	double	h;			// dt interval...
	double	time;
	
	double	pos;
	double 	vel;
	double 	mass;
	double 	rest_lenght;
	double		len;		//		 current lenght of spring
	double 	k; 			//		 spring stiffness constant
	double 	b; 			//		 damping constant
	double 	dt;
	double		origin;		//		 original spring position
	
	double	maxvel[2], maxpos[2]; //+ 11.06.2003
	Boolean	clip[2], mode;
	


} spring;


void *spring_new (Symbol *msg, short argc, Atom *argv);
void spring_bang (spring *x);
void spring_reset (spring *x);
void spring_calc (spring *x,double dt);

void spring_position (spring *x, double p);
void spring_velocity (spring *x, double v);
void spring_mass (spring *x, double mass);
void spring_rest_lenght (spring *x, double rest);
void spring_len (spring *x, double len);
void spring_k (spring *x, double k);
void spring_dt (spring *x, double dt);
void spring_origin (spring *x, double origin);
void spring_b (spring *x, double damp);

void spring_clip(spring *x, int dt, int dtt);
void spring_mode(spring *x, int dt);
void spring_maxvel (spring *x, double disp, double deep);
void spring_maxpos (spring *x, double disp, double deep);


double diffeq0 (spring *x, double t, double var[2]);
double diffeq1 (spring *x, double t, double var[2]);
double diffeq2 (spring *x, double t, double var[2]);
double diffeq3 (spring *x, double t, double var[2]);
double diffeq4 (spring *x, double t, double var[2]);
double diffeq5 (spring *x, double t, double var[2]);
double diffeq6 (spring *x, double t, double var[2]);
double diffeq7 (spring *x, double t, double var[2]);

double evaluate (spring *x, int i, double t, double var[2]);
void  solve (spring *x,double t, double h);

void spring_h (spring *x, double h);


void spring_assist(spring *x, void *b, long m, long a, char *s);
void *spring_class;

// 						-

void *spring_new (Symbol *msg, short argc, Atom *argv) 
{
	 spring *x;
	short	i;

	 x=(spring *)newobject(spring_class);

	 x->c_out=floatout(x);
	 x->c_out2=floatout(x);
	 
	 spring_reset(x);

	if (argc) {

		for (i=0; i<argc/2; i++){ //jitter style setup
		
		if(argv->a_w.w_sym == gensym("@pos")){
			argv++;
		 	x->pos = argv->a_w.w_float;
		 	x->vars[0] = argv->a_w.w_float; argv++;
			}

		if(argv->a_w.w_sym == gensym("@vel")){
			argv++;
		 	x->vel = argv->a_w.w_float;
		 	x->vars[1] = argv->a_w.w_float; argv++;
			}

		if(argv->a_w.w_sym == gensym("@mass")){
			argv++;
		 	x->mass = argv->a_w.w_float; argv++;
			}

		if(argv->a_w.w_sym == gensym("@rest")){
			argv++;
		 	x->rest_lenght = argv->a_w.w_float; argv++;
			}

		if(argv->a_w.w_sym == gensym("@k")){
			argv++;
		 	x->k = argv->a_w.w_float; argv++;
			}

		if(argv->a_w.w_sym == gensym("@h")){
			argv++;
		 	x->h = argv->a_w.w_float; argv++;
			}
		if(argv->a_w.w_sym == gensym("@maxvel")){
			argv++;
			x->maxvel[0] = argv->a_w.w_float;argv++;
			x->maxvel[1] = argv->a_w.w_float;argv++;
			}
		if(argv->a_w.w_sym == gensym("@maxpos")){
			argv++;
			x->maxpos[0] = argv->a_w.w_float;argv++;
			x->maxpos[1] = argv->a_w.w_float;argv++;
			}
		if(argv->a_w.w_sym == gensym("@mvel")){
			argv++;
			x->maxvel[0] = argv->a_w.w_float;argv++;
			x->maxvel[1] = argv->a_w.w_float;argv++;
			}
		if(argv->a_w.w_sym == gensym("@mpos")){
			argv++;
			x->maxpos[0] = argv->a_w.w_float;argv++;
			x->maxpos[1] = argv->a_w.w_float;argv++;
			}
		if(argv->a_w.w_sym == gensym("@clip")){
			argv++;
			x->clip[0] = argv->a_w.w_long;argv++;
			x->clip[1] = argv->a_w.w_long;argv++;
			}
		if(argv->a_w.w_sym == gensym("@colis‹o")){
			argv++;
			x->mode = argv->a_w.w_long;argv++;
			}
		if(argv->a_w.w_sym == gensym("@mode")){
			argv++;
			x->mode = argv->a_w.w_long;argv++;
			}



		}
	}	 
	 return(x);	
}

void spring_reset(spring *x)
{
	 x->pos=0.0f;
	 x->vel=0.0f;
	 x->mass=1.0f;
	 x->rest_lenght=2.5f;
	 x->k = 3.0f;
	 x->dt = 0.1f;
	 x->b = 0.2f;
	 x->origin = 0.000f;
	 
	 x->vars[0] =  x->vars[1] = 0.0f;
	 x->h = 0.1f;
	 x->time = -9990.0f;

}

void  spring_calc (spring *x, double dt)
{

/*
  vars[0] = position (x) with origin as above
  vars[1] = velocity (v=x')
  R = rest length
  len = current length of spring = x - origin.x
  L = how much spring is stretched from rest length
  L = len - R = x - origin.x - R
  k = spring constant
  b = damping constant
  F = m a  (force = mass * acceleration) leads to:
  F = -kL -bv = -k(x - origin.x - R) -bv = m v'
  so diffeq's are:
  x' = v
  v' = -(k/m)(x - origin.x - R) -(b/m)v

*/
	double Vnew; //new velocity at t+dt
	double Snew; //new position at t+dt

	double Vel, Pos, Mass, Rest, K, Len, L, B;
		
	// copies

	Vel = x->vel;
	Pos = x->pos - x->origin;
	Mass = x->mass;
	Rest = x->rest_lenght;
	Len = x->len;
	L = Len - Rest - x->origin;
	K = x->k;
	B = x->b;

	// calc new pos && vel	
	Snew = Pos + (Vel * dt);
//	Vnew = -(K/Mass) * L - (B/Mass)*Vel*dt;
	Vnew = -K*(Pos - Rest) - (B * Vel *dt);
	Vnew = Vnew / Mass;

	// update new pos && vel
	x->pos = Snew;
	x->vel = Vnew;
}





// user interface


void spring_clip(spring *x, int dt, int dtt)
{
 x->clip[0] = dt;
 x->clip[1] = dtt;
}

void spring_mode(spring *x, int dt)
{
 x->mode= dt;
}

void spring_maxvel (spring *x, double disp, double deep)
{
	x->maxvel[0] = disp;
	x->maxvel[1] = deep;
}

void spring_maxpos (spring *x, double disp, double deep)
{
	x->maxpos[0] = disp;
	x->maxpos[1] = deep;
}


void spring_position (spring *x, double p)
{
 	x->pos = p;
 	x->vars[0] = p;
}
void spring_velocity (spring *x, double v)
{
 	x->vel = v;
 	x->vars[1] = v;
}
void spring_mass (spring *x, double mass)
{
 	x->mass = mass;
}
void spring_rest_lenght (spring *x, double rest)
{
 	x->rest_lenght = rest;
}
void spring_len (spring *x, double len)
{
 	x->len = len;
}
void spring_k (spring *x, double k)
{
 	x->k = k;
}
void spring_dt (spring *x, double dt)
{
 	x->dt = dt;
}
void spring_origin (spring *x, double origin)
{
 	x->origin = origin;
}
void spring_b (spring *x, double damp)
{
 	x->b = damp;
}

void spring_h (spring *x, double h)
{
 	x->h = h;
}




double diffeq0 (spring *x, double t, double var[2])
{
	double v = var[1];
	
	if (x->clip[0]) {
	if (v < x->maxvel[0]) v = x->maxvel[0];
	if (v > x->maxvel[1]) v = x->maxvel[1];
	var[1] = v;
	}

	
	return var[1]; // x' = v
}

double diffeq1 (spring *x, double t, double var[2])
{
	double r, z;
	
	r = -x->k * (var[0] - x->origin - x->rest_lenght) - x->b * var[1];
	z = r/x->mass;
	if (x->clip[1]) {
		if (z < x->maxpos[0]) z = x->maxpos[0];
		if (z > x->maxpos[1]) z = x->maxpos[1];
		if (x->mode ==1) {
			x->vars[1] = -x->vars[1];
			z = z * x->vars[1] * t;
		}
	}

	return (z);
}


double diffeq2 (spring *x, double t, double var[2]) { return 0; }
double diffeq3 (spring *x, double t, double var[2]) { return 0; }
double diffeq4 (spring *x, double t, double var[2]) { return 0; }
double diffeq5 (spring *x, double t, double var[2]) { return 0; }
double diffeq6 (spring *x, double t, double var[2]) { return 0; }
double diffeq7 (spring *x, double t, double var[2]) { return 0; }

double evaluate (spring *x, int i, double t, double var[2])
{
	switch (i)
	{
		case 0: return diffeq0 (x, t, x->vars);
		case 1: return diffeq1 (x, t, x->vars);
		case 2: return diffeq2 (x, t, x->vars);
		case 3: return diffeq3 (x, t, x->vars);
		case 4: return diffeq4 (x, t, x->vars);
		case 5: return diffeq5 (x, t, x->vars);
		case 6: return diffeq6 (x, t, x->vars);
		case 7: return diffeq7 (x, t, x->vars);
		default: error("error in func evaluate"); return 0;
	}

}

void  solve (spring *x,double t, double h)
{
	static const int N = 2;
	int i;
	
	double inp[N];
	double k1[N];
	double k2[N];
	double k3[N];
	double k4[N];
	
	for (i=0; i<N; i++)
		k1[i] = evaluate (x, i, t, x->vars);	//evauluate at t

	for (i=0; i<N; i++)
		inp[i] = x->vars[i] + k1[i]*h/2; 		//set input to dif equations

	for (i=0; i<N; i++)
		k2[i] = evaluate (x, i, t + h/2, inp);	//evauluate at t + h/2

	for (i=0; i<N; i++)
		inp[i] = x->vars[i] + k2[i]*h/2; 		//set input to dif equations

	for (i=0; i<N; i++)
		k3[i] = evaluate (x, i, t + h/2, inp);	//evauluate at t + h/2

	for (i=0; i<N; i++)
		inp[i] = x->vars[i] + k3[i]*h; 		//set input to dif equations

	for (i=0; i<N; i++)
		k4[i] = evaluate (x, i, t + h, inp);	//evauluate at t + h

	for (i=0; i<N; i++)
		x->vars[i] = x->vars[i] + (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]) *h/6; 		

}



void spring_bang (spring *x)
{
//	short i; //waste ram
//    spring_calc(x, x->dt);
//    outlet_float(x->c_out, x->pos);
//    outlet_float(x->c_out2, x->vel);
double sim_time = x->time;
double h = x->h;

if (sim_time < 0) { sim_time = 0.2; ;}

	solve(x, sim_time, h);
	
    outlet_float(x->c_out2,  x->vars[0]);
    outlet_float(x->c_out ,  x->vars[1]);
    
    sim_time +=h;
    
    x->time = sim_time;
}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&spring_class,(method)spring_new,0L,(short)sizeof(spring),
 0L, A_GIMME,0);

 addbang((method)spring_bang);
 addmess((method)spring_position,"position", A_DEFFLOAT, 0);
 addmess((method)spring_position,"pos", A_DEFFLOAT, 0);
 addmess((method)spring_velocity,"velocity", A_DEFFLOAT, 0);
 addmess((method)spring_velocity,"vel", A_DEFFLOAT, 0);
 addmess((method)spring_mass,"mass", A_DEFFLOAT, 0);
 addmess((method)spring_rest_lenght,"rest_lenght", A_DEFFLOAT, 0);
 addmess((method)spring_rest_lenght,"R", A_DEFFLOAT, 0);
 addmess((method)spring_len,"len", A_DEFFLOAT, 0);
 addmess((method)spring_k,"k", A_DEFFLOAT, 0);
 addmess((method)spring_b,"damp", A_DEFFLOAT, 0);
 addmess((method)spring_origin,"origin", A_DEFFLOAT, 0);
 addmess((method)spring_dt,"dt", A_DEFFLOAT, 0);
 addmess((method)spring_h,"h", A_DEFFLOAT, 0);
 addmess((method)spring_maxvel,"mvel", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)spring_maxpos,"mpos", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)spring_clip,"clip", A_DEFLONG, A_DEFLONG, 0);
 addmess((method)spring_mode,"colis‹o", A_DEFLONG, 0);

 addmess((method)spring_reset,"reset", 0);
 addmess((method)spring_assist,"assist", A_CANT, 0);



 post("	 \t \a \t  %08xd	  			a-spring © A-Sier 2003"__TIME__,tick,0);
}


void spring_assist(spring *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"bang(beat), c o m a n d o s"); }   
    else if (m==2&&a==0) { sprintf(s,"(double) p o s i ; ' a o"); }
    else if (m==2&&a==1) { sprintf(s,"(double) v e l o c i d a d e"); }
}