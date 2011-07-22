/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
dir - andrŽ sier 2005
		
			delta_x = tempNew_x - tempOld_x;
			delta_y = tempNew_y - tempOld_y;
			delta_z = tempNew_z - tempOld_z;
			azi = atan2(delta_z, delta_x) * flockPtr->r2d;
			ele = atan2(delta_y, delta_x) * flockPtr->r2d;
			speed = sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
			
			// have to remove stuff. revert to 01 version.
			// moved rest into arrow external
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

*/

#include "ext.h"
#include "ext_common.h"
#include "math.h"
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define HALFPI M_PI/2.0f;

typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet value	
	t_atom				out[3];

	double			input[3];
	double			o1[3],o2[3];
	double			r2d;
	
	double			azieledist[3], point[3];
	
	short			om;

} dir;


void *dir_new (Symbol *msg, short argc, Atom *argv);
void dir_bang (dir *x);
void dir_list (dir *x, Symbol *msg, short argc, Atom *argv);
void dir_assist(dir *x, void *b, long m, long a, char *s);

void dir_azi (dir *x, float p);
void dir_ele (dir *x, float p);
void dir_dist (dir *x, float p);
void dir_om (dir *x, long p);

void *dir_class;


void *dir_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 dir *x;
	 int i;
	 x=(dir *)newobject(dir_class);
	for(i=0;i<3;i++) {
		x->input[i] = x->o1[i] = x->o2[i] = x->azieledist[i]= x->point[i] = 0.0f;
	}

	x->r2d = 180.0/M_PI;
	x->om = 0;
	 x->c_out=listout(x);// xy dir coords
	 return(x);	
}





void dir_list (dir *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	for (i=0; i<argc; i++) {
		
		if (argv->a_type == A_FLOAT) {
			x->input[i] = argv->a_w.w_float;
			argv++;
		} 
		else if (argv->a_type == A_LONG) {
			x->input[i] = (double)argv->a_w.w_long;
			argv++;
		} 
	}

	if(!x->om) {

		// cycle points
		for(i=0;i<3;i++) {
			x->o2[i] = x->o1[i];
			x->o1[i] = x->input[i];
		}

	} else if(x->om) {
		for(i=0;i<3;i++) {
			x->o2[i] = 0.0f;
			x->o1[i] = x->input[i];
		}
	
	}

	dir_bang(x);
}

void dir_bang (dir *x)
{
	t_atom	*out;
	out	= x->out;
	double	temp[3];
	double dx, dy, dz;
	double dx2, dy2, dz2;
//	double eletemp;
/*
			delta_x = tempNew_x - tempOld_x;
			delta_y = tempNew_y - tempOld_y;
			delta_z = tempNew_z - tempOld_z;
			azi = atan2(delta_z, delta_x) * flockPtr->r2d;
			ele = atan2(delta_y, delta_x) * flockPtr->r2d;
			speed = sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

*/

	dx = x->o1[0] - x->o2[0]; dy = x->o1[1] - x->o2[1]; dz = x->o1[2] - x->o2[2];
	dx2 = dx*dx;	dy2=dy*dy;	dz2=dz*dz;
	
	// azi
	temp[0]= atan2(dx, dz) * x->r2d;
	// ele
	temp[1] =sqrt(dx2+dz2);
	temp[1] = atan2(temp[1],dy) * x->r2d;
//	temp[1] -= HALFPI;
//	temp[1] *= -1.0f;
	
	temp[1]= atan2(dy, dx) * x->r2d;
	// speed
	temp[2]= sqrt(dx2+dy2+dz2);
	
		SETFLOAT(out+0, temp[0]);
		SETFLOAT(out+1, temp[1]);
		SETFLOAT(out+2, temp[2]);

	outlet_list(x->c_out, 0L, 3, out); //xy dirular coords



}

void dir_om(dir *x, long p)
{
	if(p)
		x->om=1;
	else
		x->om=0;
}


void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&dir_class,(method)dir_new,0L,(short)sizeof(dir),0L,
 A_GIMME,0);

 addbang((method)dir_bang);
 addmess((method)dir_list,"list",A_GIMME,0);
 addmess((method)dir_om,"om",A_LONG,0);
 addmess((method)dir_assist,"assist", A_CANT, 0);

 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-dir   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void dir_assist(dir *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"bang, list 3 points"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) azi / ele / distance"); }
}


