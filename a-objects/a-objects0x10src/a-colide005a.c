/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
a-colide - © andrŽ sier 2004

should only output when a collision occurs & how much to move colidee
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"
#include "math.h"

#ifndef pi
#define pi 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068
#endif
#define divide_pi pi/180.
#define two_pi 2.*pi
#define divide_two_pi 1./two_pi

#define MAXOBJECTS	52

typedef struct	_littleobjects
{
	double	pos[3];
	double	radius;

} littleobjects;


typedef struct	_colide
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet	

	littleobjects		littleobj[MAXOBJECTS];
	littleobjects		player;
	short				activeobject;
	short				numobjects;

	t_atom o[12];				//output atom
} colide;



void *colide_new (Symbol *msg, short argc, Atom *argv);
void colide_set_active_object (colide *x, short max);
void copy3 (double to[3], double from[3]);
void colide_list (colide *x, Symbol *msg, short argc, Atom *argv); //input the args 
void colide_calc (colide *x); 	//exec calcs
void colide_set_num_object (colide *x, short max);
void colide_post (colide *x);
void colide_bang (colide *x); //exec
void colide_readxyz (colide *x, Symbol *msg, short argc, Atom *argv); //input the args 


void colide_player (colide *x, Symbol *msg, short argc, Atom *argv); //input the args 
void colide_radius (colide *x, Symbol *msg, short argc, Atom *argv); //input the args 


void colide_assist(colide *x, void *b, long m, long a, char *s);
void *colide_class;


//____imPORTANT myroundING FUNC fixed: fix HV
long myround (float tomyround);
double piwrap(double f);


//__


void *colide_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 colide *x;
	 int i;
	 
	 x=(colide *)newobject(colide_class);


	 x->c_out=listout(x);

	 //init
	 x->activeobject=0;
	 x->numobjects=0;
	 for(i=0;i<MAXOBJECTS;i++) {
		x->littleobj[i].pos[0]=0.0f;
		x->littleobj[i].pos[1]=0.0f;
		x->littleobj[i].pos[2]=0.0f;
		x->littleobj[i].radius=1.0f;
	 
	 }

		x->player.pos[0]=0.0f;
		x->player.pos[1]=0.0f;
		x->player.pos[2]=0.0f;
		x->player.radius=1.0f;
	 


	 return(x);	
}

void colide_list (colide *x, Symbol *msg, short argc, Atom *argv) //input the args 
{


	if (argc) {
	
		float	temp[4];
		double	tpos[3];
		int i;

		if (argc==3) { //3+ args


			for(i=0;i<3;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}



			for(i=0;i<3;i++) {
//				if (argv[i].a_type == A_LONG){ 
//					tpos[i] = (double) argv[i].a_w.w_long;
//				}else if (argv[i].a_type == A_FLOAT){ 
					tpos[i] = (double)temp[i];
				}
				
				copy3(x->littleobj[x->activeobject].pos,tpos);
			
			
			

			} else 		if (argc==4) { //3+ args

			for(i=0;i<4;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}



			for(i=0;i<3;i++) {
//				if (argv[i].a_type == A_LONG){ 
//					tpos[i] = (double) argv[i].a_w.w_long;
//				}else if (argv[i].a_type == A_FLOAT){ 
					tpos[i] = (double)temp[i];
				}
				
				copy3(x->littleobj[x->activeobject].pos,tpos);
			



	//			i=3;
//				if (argv[i].a_type == A_LONG){ 
//					tpos[2] = (double) argv[i].a_w.w_long;
//				}else if (argv[i].a_type == A_FLOAT){ 
					tpos[2] = (double) temp[3];
//				}

				
				x->littleobj[x->activeobject].radius = tpos[2];
			
			}

		
		
		}
		
		
		
		

	
	
//	colide_calc(x);



}

void colide_readxyz (colide *x, Symbol *msg, short argc, Atom *argv) //input the args 
{


	if (argc) {
	
		float	temp[4]={0.0f,0.0f,0.0f,0.0f};
		double	tpos[3];
		int i;
		int id;

		if (argc==4) { //3+ args

			id = argv->a_w.w_long;
			x->activeobject=id;
			argv++;
			
			for(i=0;i<3;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}

			for(i=0;i<3;i++) {
					tpos[i] = (double)temp[i];
			}
				
			copy3(x->littleobj[x->activeobject].pos,tpos);
			


			
			}

		else if (argc==5) { //extra_for radius

			id = argv->a_w.w_long;
			x->activeobject=id;
			argv++;
			
			for(i=0;i<3;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}

			for(i=0;i<3;i++) {
					tpos[i] = (double)temp[i];
			}
				
			copy3(x->littleobj[x->activeobject].pos,tpos);
			

			temp[0]=(float)argv->a_w.w_float;
			x->littleobj[x->activeobject].radius=(double)temp[0];
			
			}

	}

		



}




void colide_player (colide *x, Symbol *msg, short argc, Atom *argv)
{
	if (argc) {
	
		float	temp[4]={0.0f,0.0f,0.0f,0.0f};
		double	tpos[3];
		int i;
		int id;

		if (argc==3) { //3+ args
			
			for(i=0;i<3;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}

			for(i=0;i<3;i++) {
					tpos[i] = (double)temp[i];
			}
				
			copy3(x->player.pos,tpos);
			


			
			}

		else if (argc==4) { //extra_for radius
			
			for(i=0;i<3;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}

			for(i=0;i<3;i++) {
					tpos[i] = (double)temp[i];
			}
				
			copy3(x->player.pos,tpos);
			

			temp[0]=(float)argv->a_w.w_float;
			x->player.radius=(double)temp[0];
			
			}

	}

		

}

void colide_radius (colide *x, Symbol *msg, short argc, Atom *argv)
{

	float	temp[4]={0.0f,0.0f,0.0f,0.0f};
	double	tpos[3];
	int i;

	if (argc) {
	


			for(i=0;i<argc;i++) {
				if(argv->a_type == A_FLOAT) {
					temp[i] = (float)argv->a_w.w_float;	
					argv++;
				} else if(argv->a_type == A_LONG) {
					temp[i] = (float)argv->a_w.w_long;	
					argv++;
				}
			}

			for(i=0;i<argc;i++) {
					tpos[i] = (double)temp[i];
			}
				


			for(i=0;i<x->numobjects;i++) {
				x->littleobj[i].radius=(double)temp[0];
			}




		}		

}




void copy3 (double to[3], double from[3])
{
	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void colide_set_active_object (colide *x, short max)
{
	x->activeobject = CLIP(max,0,MAXOBJECTS);
	post("	active object is %ld", x->activeobject);
}

void colide_set_num_object (colide *x, short max)
{
	x->numobjects = CLIP(max,1,MAXOBJECTS);
	post("	num objects is %ld", x->numobjects);
}

void colide_post (colide *x)
{
	short i;
	
	for(i=0;i<x->numobjects;i++) {
	post("	object %ld  pos+rad is  %f %f %f    %f", i,x->littleobj[i].pos[0],x->littleobj[i].pos[1],x->littleobj[i].pos[2],x->littleobj[i].radius );
	}
}



void  colide_calc (colide *x)
{

	littleobjects		player;
	short i;
	double	cdist[3];
	double r; //radial distance.. whatever, from marbles, 
	double theta; // the angle around x and z?
	double move[3]={0.,0.,0.};
	double radii;
	double	newpos[3];


	t_atom *o = x->o;
	
	o = x->o;

//	copy3(player.pos,x->littleobj[0].pos);
//	player.radius = x->littleobj[0].radius;
	copy3(player.pos,x->player.pos);
	player.radius = x->player.radius;

	for(i=0;i<x->numobjects;i++) { //start at 1; now at 0!
		cdist[0] = player.pos[0]-x->littleobj[i].pos[0];
		cdist[1] = player.pos[1]-x->littleobj[i].pos[1];
		cdist[2] = player.pos[2]-x->littleobj[i].pos[2];
		
		

		r = sqrt(cdist[0]*cdist[0]+cdist[1]*cdist[1]+cdist[2]*cdist[2]);
//		theta = atan2(cdist[1],cdist[0]); // stoopid!! should be nested after to reduce atan2 calls!"!!
		
		radii = player.radius + x->littleobj[i].radius;

		if(r<radii) { // theres a hit
		
			theta = atan2(cdist[1],cdist[0]); 

			move[0] =(radii-r)*cos(theta);
			move[1] =(radii-r)*sin(theta);
//			move[2] = 
			newpos[0]= x->littleobj[i].pos[0] - move[0] ;
			newpos[1]= x->littleobj[i].pos[1] - move[1] ;
//			x->littleobj[i].pos[1] -= move[1] ;
			SETLONG(o+0, i);
			SETFLOAT(o+1, move[0]);
			SETFLOAT(o+2, move[1]);
			SETFLOAT(o+3, newpos[0]);
			SETFLOAT(o+4, newpos[1]);
			SETFLOAT(o+5, x->littleobj[i].pos[2]);
			outlet_list (x->c_out, 0L, 6, o);
		}
	}



//	outlet_list (x->c_out, 0L, 12, o);
	
	
	
	


}




void colide_bang (colide *x)
{


	
	colide_calc(x);


}


void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&colide_class,(method)colide_new,0L,(short)sizeof(colide),0L,
 A_GIMME,0);
 
 addbang((method)colide_bang);
 addmess((method)colide_readxyz, "readxyz", A_GIMME, 0);
 addmess((method)colide_player, "player", A_GIMME, 0);
 addmess((method)colide_radius, "radius", A_GIMME, 0);
 addmess((method)colide_list, "list", A_GIMME, 0);
 addmess((method)colide_set_active_object, "set", A_DEFLONG, 0);
 addmess((method)colide_set_num_object, "num", A_DEFLONG, 0);
 addmess((method)colide_post, "post", 0);
 addmess((method)colide_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                 a-colide   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);
}


void colide_assist(colide *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) colide     e         c            h              o"); }
 //   else if (m==2&&a==1) { sprintf(s,"(float) y       e             c                h                   o"); }
}


long myround (float tomyround)
{

 long res;
 
	if (tomyround<0)
	 	tomyround-=0.5;
	else
		tomyround+=0.5;

	res = (long)tomyround;
	
 return res;
}



double piwrap(double f)
{
	double op1, op3;
	long op2;
	if (f >= 0) {														
		op1 = f + pi;
		}
		else {
		op1 = f - pi;
		}
		op2 = (long)(op1 * divide_two_pi);
		op3 = (double)op2 * two_pi;
		f -= op3;
		return f;
}
