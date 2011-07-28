/*

————————————————————————————————————————————————————————————————
a-celerador - © Copyright sier 2002
		
		displacement via euler method with simple colision detection;
		throttle, speed, drag, mass, colision, bounce
		
- 15.09.2006
		-added thrust2; should be array and summed;;
		
		
+ 17.06.2003
	-colision message now outputs dir if desired as well


+ 14.06.2003
	-colision message 4 master colision object,

+ 13.06.2003
	-colision detect improvement,
	-colision direction output


+ 11.06.2003
	-2 doubles,
	-maxvel, maxpos
	-clipping arg
	-prevent output on no change
	-bouce? mode
————————————————————————————————————————————————————————————————
*/

#include "ext.h"




typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;	//vel		
	void 		        *c_out2;		//pos	
	void 		        *c_out3;			//colisao

	double 	thrust, thrust2;
	double	dragcoef;
	double 	velocity;
	double 	mass;
	double 	displacement;
	double 	dt;
	double	maxvel[2], maxpos[2]; //+ 11.06.2003
	Boolean	clip[2];
	
	char		mode, dif;

} motion;


void *motion_new (Symbol *msg, short argc, Atom *argv);
void motion_bang (motion *x);
void motion_reset (motion *x);
void   motion_calc (motion *x,double dt);
void motion_dt (motion *x, double dt);
void motion_thrust (motion *x, double thrust, double thrust2);
void motion_dragcoef (motion *x, double drag);
void motion_velocity (motion *x, double v);
void motion_mass (motion *x, double mass);
void motion_displacement (motion *x, double disp);

void motion_clip(motion *x, int dt, int dtt);
void motion_mode (motion *x, int thrust);
void motion_maxvel (motion *x, double disp, double deep);
void motion_maxpos (motion *x, double disp, double deep);

void motion_colide (motion *x);

void motion_assist(motion *x, void *b, long m, long a, char *s);
void *motion_class;

// 						-

void *motion_new (Symbol *msg, short argc, Atom *argv) 
{
 motion *x;
char i;

 x=(motion *)newobject(motion_class);

 x->c_out3=intout(x); //colision dir
 x->c_out=floatout(x); //vel
 x->c_out2=floatout(x); //pos
 
 motion_reset(x);
 
 	if (argc) {

		for (i=0; i<argc/2; i++){ //jitter style setup
		
		if(argv->a_w.w_sym == gensym("@dt")){
			argv++;
			x->dt = argv->a_w.w_float;argv++;
			}

		if(argv->a_w.w_sym == gensym("@thrust")){
			argv++;
			x->thrust = argv->a_w.w_float;argv++;
			}

		if(argv->a_w.w_sym == gensym("@drag")){
			argv++;
			x->dragcoef = argv->a_w.w_float;argv++;
			}

		if(argv->a_w.w_sym == gensym("@mass")){
			argv++;
			x->mass = argv->a_w.w_float;argv++;
			}

		if(argv->a_w.w_sym == gensym("@pos")){
			argv++;
			x->displacement = argv->a_w.w_float;argv++;
			}

		if(argv->a_w.w_sym == gensym("@vel")){
			argv++;
			x->velocity = argv->a_w.w_float;argv++;
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
		if(argv->a_w.w_sym == gensym("@mode")){
			argv++;
			x->mode = argv->a_w.w_long;argv++;
			}
		if(argv->a_w.w_sym == gensym("@colisão")){
			argv++;
			x->mode = argv->a_w.w_long;argv++;
			}

		}
	}
 return(x);	
}

void motion_reset(motion *x)
{
	 x->thrust	=1.00f;
	 x->thrust2	=0.00f;
	x->dragcoef	=0.9f;
	 x->velocity	=1.0f;
	x->mass	=1.0f;
	 x->displacement	=0.0f;
	 x->dt = 0.001f;
	 
	 x->maxvel[0] = -5.;
	 x->maxvel[1] =   5.;
	 x->maxpos[0] = -25.;
	 x->maxpos[1] =   25.;
	 
	 x->clip[0] = x->clip[1] = 0;
	 x->mode = 0; x->dif = 0;
	 
}

void  motion_calc (motion *x, double dt)
{
	double F; //total force
	double A; //aceleration
	double Vnew; //new velocity at t+dt
	double Snew; //new position at t+dt
	// copies
	double T = x->thrust;
	double C = x->dragcoef;
	double V = x->velocity;
	double M = x->mass;
	double S = x->displacement;
	
	T += x->thrust2;

	//calc force
	F =  ( T - (C * V));
	//calc acceleration
	A = F / M;

	//calc new velocity at t+dt where V is velocity at t
	Vnew = V + A * dt;
	if (x->clip[0]) {
		if (Vnew < x->maxvel[0]) Vnew = x->maxvel[0];
		if (Vnew > x->maxvel[1]) Vnew = x->maxvel[1];
	}
	
	//calc new displacement at t+dt where S is displacement at t
	Snew = S + Vnew * dt;
	if (x->clip[1]) {
		 if (Snew > x->maxpos[0] && Snew < x->maxpos[1] ) x->dif = 0;
		if (Snew < x->maxpos[0]) Snew = x->maxpos[0];
		if (Snew > x->maxpos[1]) Snew = x->maxpos[1];
		if (x->mode >0) {
			if (Snew <= x->maxpos[0]){
 				Vnew = -Vnew; //invert speed if @ maxpos
 				//Snew = Snew + 0.01;
 				Snew = Snew + Vnew * dt; //impact
				if (x->mode >1) {x->dif = 1;}

 			}

			if (Snew >= x->maxpos[1]){
 				Vnew = -Vnew; //invert speed if @ maxpos
 				//Snew = Snew - 0.01;
 				Snew = Snew + Vnew * dt; //impact
				if (x->mode >1) {x->dif = 2;}
 			}
			
		}
	}

	//calc new displacement at t+dt where S is displacement at t
//	Snew = S + Vnew * dt;

	//update new velocity and new displacement
	x->velocity = Vnew;
	x->displacement = Snew;

}

void motion_colide (motion *x)
{
	x->velocity = -x->velocity;

	motion_calc(x, x->dt);

     if (x->mode >1) {
	if (x->velocity >0) {x->dif = 1;} //we hit left
	else if (x->velocity <0) {x->dif = 2;} //we hit right
	else  {x->dif = 0;} //we hit left
    }
	//of course, no duplicates
	    outlet_float(x->c_out,   x->velocity);
	    outlet_float(x->c_out2, x->displacement);
     if (x->mode >1) 
             outlet_int(x->c_out3, x->dif);


}

void motion_dt (motion *x, double dt)
{
 x->dt = dt;
}

void motion_clip(motion *x, int dt, int dtt)
{
 x->clip[0] = dt;
 x->clip[1] = dtt;
}

void motion_mode (motion *x, int thrust)
{
	x->mode = thrust;
}
void motion_thrust (motion *x, double thrust,  double thrust2)
{
	x->thrust = thrust;
	x->thrust2 = thrust2;
}
void motion_dragcoef (motion *x, double drag)
{
	x->dragcoef = drag;
}

void motion_velocity (motion *x, double v)
{
	x->velocity = v;
}

void motion_mass (motion *x, double mass)
{
	x->mass = mass;
}

void motion_displacement (motion *x, double disp)
{
	x->displacement = disp;
}

void motion_maxvel (motion *x, double disp, double deep)
{
	x->maxvel[0] = disp;
	x->maxvel[1] = deep;
}

void motion_maxpos (motion *x, double disp, double deep)
{
	x->maxpos[0] = disp;
	x->maxpos[1] = deep;
}


void motion_bang (motion *x)
{
	double av, ap;
	char  dif;
	
	av = x->velocity;
	ap = x->displacement;
	if (x->mode >1) {dif = x->dif;}

    motion_calc(x, x->dt);
    
	if (av!=x->velocity)
	    outlet_float(x->c_out, x->velocity);
	if (ap!=x->displacement)
	    outlet_float(x->c_out2, x->displacement);
	if (x->mode >1) 
		if (dif!=x->dif)
	    		outlet_int(x->c_out3, x->dif);
	
	

}



void main(void)
{

	long int tick = gettime();

setup((t_messlist**)&motion_class,(method)motion_new,0L,(short)sizeof(motion),0L,
 A_GIMME,0);

 addbang((method)motion_bang);
 addmess((method)motion_dt,"dt", A_DEFFLOAT, 0);
 addmess((method)motion_thrust,"thrust", A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)motion_dragcoef,"dragcoef", A_DEFFLOAT, 0);
 addmess((method)motion_mass,"mass", A_DEFFLOAT, 0);
 addmess((method)motion_maxvel,"mvel", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)motion_maxpos,"mpos", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)motion_displacement,"displacement", A_DEFFLOAT, 0);
 addmess((method)motion_displacement,"pos", A_DEFFLOAT, 0);
 addmess((method)motion_velocity,"velocity", A_DEFFLOAT, 0);
 addmess((method)motion_velocity,"vel", A_DEFFLOAT, 0);
 addmess((method)motion_reset,"reset", 0);
 addmess((method)motion_clip,"clip", A_DEFLONG, A_DEFLONG, 0);
 addmess((method)motion_mode,"colisão", A_DEFLONG,  0);
 addmess((method)motion_colide,"colide", 0);


 addmess((method)motion_assist,"assist", A_CANT, 0);


 post("	 \t \a \t  %08xd	  			a-celerador © A-Sier 2003"__TIME__,tick,0);
}

void motion_assist(motion *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"a-celerador !!      e          c           h                o"); }   
    else if (m==2&&a==0) { sprintf(s,"(float) v e l o c i d a d e"); }
    else if (m==2&&a==1) { sprintf(s,"(float) p o s i ç ã o"); }
    else if (m==2&&a==2) { sprintf(s,"(int) d i r e c ç ã o   d a   c o l i s ã o"); }
}