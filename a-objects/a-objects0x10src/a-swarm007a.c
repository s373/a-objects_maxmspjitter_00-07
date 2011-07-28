/*

————————————————————————————————————————————————————————————————
a-swarm - © andré sier 2004
		
		
	// rewrite attempt with daniel shiffman source code
	12 april 2007	

	max attemp to draw a simple swarming flock that converges
	to a certain xy pt. loosely inspired on Duncan Crombie's
	implementation of Craig Reynold's BOIDS algorithmn in the
	swarming class
	


	-20030322
		implement repulsion toggle for every cell



	-20030315
		implement repulsion

	todo,
	
	modes here,
	wander
	chase, 
	flee, c
	check craig reynolds algo


————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"
#include <math.h>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif
// unsigned long max 
#define	rndmax	4294967295


#define	MAXBIRDS			200


// ___________________________________________________________________________
// ___________________________s t r u c t s___________________________________
// ___________________________________________________________________________




typedef struct	_bird		//each bird's data struct
{
	float		bx;
	float		by;
	float		btheta;
	float		bspeed;
	short		id;			// added to try prevent self division
//	float		width, height;	// in the swarm typedef
//	float		maxturn;
} bird;

// ___________________________________________________________________________

typedef struct	_pt		// a pt datum
{
	float	x, y;
} pt;

// ___________________________________________________________________________


typedef struct	_swarm
{
	t_object 	        s_ob;			
	void 		        *s_out; 			// the outlet		
	bird				bee[MAXBIRDS];		// the bees in the swarm ; should be pointers
											// but how do i alloc the space for it?
	float				width, height;			// the w&h of the 2d_space
	short				numbirds;				// how many birds are there
	float				stride;					// the overall speed of the system
	float				maxturn;				// the max turn each bird may do
	float				mindist, maxdist; 		// the min & max dist -- influence between birds
	float				minspeed, maxspeed; 		// the min & max speed of the flock
	char				paused;
	char				flocking;

	float				dstx, dsty;				// the target pt for the swarm
	float				heading;				// the general heading of the flock 
	unsigned long		hold_rnd;
	Atom				out[5]; //outlet list: bee index, bee x, bee y, bee angle, bee speed

	short				id;		// lets shoot some birds
	Boolean				onR[MAXBIRDS];
	Boolean				onA[MAXBIRDS];

	float				weightdst;

} swarm;



// ___________________________________________________________________________
// ___________________________p r o t o t y p e n_____________________________
// ___________________________________________________________________________


void swarm_bang (swarm *x); // all around throttle

//bird prototypes
void swarm_bird_setbird (swarm *x, int which, float bx, float by, float btheta); 
void swarm_bird_move (swarm *x, float heading);
void swarm_bird_addbird (swarm *x); 
void swarm_bird_removebird (swarm *x); 
void swarm_bird_reset (swarm *x); // random birds locat within the arena

//swarming prototypes
void swarm_init (swarm *x); 
void swarm_move (swarm *x); 
void swarm_list (swarm *x, Symbol *msg, short argc, Atom *argv); //input through 'list'
float swarm_general_heading (swarm *x, bird beee); //calc general heading from bees


void swarm_onRToggle (swarm *x, Symbol *msg, short argc, Atom *argv); 
void swarm_onAToggle (swarm *x, Symbol *msg, short argc, Atom *argv); 


//utilz
double	sizeOfpt (pt p);									// calc a pts size
float getDistance (bird bee1, bird bee2);
pt	normalisePoint(pt p, double n);
pt	sumPoint(pt p1, double w1, pt p2, double w2);
void rand_calc(swarm *x);

// required max boring stuff
void swarm_assist(swarm *x, void *b, long m, long a, char *s);	//		poets wanted
void *swarm_new (Symbol *msg, short argc, Atom *argv);			//		A_GIMME
void *swarm_class;
void swarm_setnum (swarm *x, int n);
void swarm_setdst (swarm *x, float c, float v);
void swarm_sethv (swarm *x, float c, float v);
void swarm_setstride (swarm *x, float c);
void swarm_setturn (swarm *x, float c);
void swarm_setdist (swarm *x, float c, float v);

void swarm_setweightdst (swarm *x, float c);


// ___________________________________________________________________________
// _______________________________n e w_______________________________________
// ___________________________________________________________________________
void *swarm_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
 swarm *x;
 x=(swarm *)newobject(swarm_class);
 x->s_out=listout(x);
 swarm_init(x);

	if (argc) { // 
		if(argv->a_w.w_sym == gensym("@hv")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->width = argv->a_w.w_float;	argv++;
				x->height = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->width = (float)argv->a_w.w_long;	argv++;
				x->height = (float)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@dist")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->mindist = argv->a_w.w_float;	argv++;
				x->maxdist = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->mindist = (float)argv->a_w.w_long;	argv++;
				x->maxdist = (float)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@num")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->numbirds = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->numbirds = argv->a_w.w_long;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@stride")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->stride = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->stride = (float)argv->a_w.w_long;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@dst")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->dstx = argv->a_w.w_float;	argv++;
				x->dsty = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->dstx = (float)argv->a_w.w_long;	argv++;
				x->dsty = (float)argv->a_w.w_long;	argv++;			
			}
		}
		if(argv->a_w.w_sym == gensym("@turn")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->maxturn = argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->maxturn = (float)argv->a_w.w_long;	argv++;
			}
		}


	} // end if args
 return(x);	
}
// ___________________________________________________________________________
// _______________________________f u n c s___________________________________
// ___________________________________________________________________________

void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&swarm_class,(method)swarm_new,0L,(short)sizeof(swarm),0L,A_GIMME,0);
 addbang((method)swarm_bang);
 addmess((method)swarm_bird_reset, "reset", 0);
 addmess((method)swarm_setnum, "num", A_DEFLONG, 0);
 addmess((method)swarm_setdst, "dst", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)swarm_sethv, "hv", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)swarm_setdist, "dist", A_DEFFLOAT, A_DEFFLOAT, 0);
 addmess((method)swarm_setstride, "stride", A_DEFFLOAT, 0);
 addmess((method)swarm_setweightdst, "weightdst", A_DEFFLOAT, 0);
 addmess((method)swarm_setturn, "turn", A_DEFFLOAT, 0);
 addmess((method)swarm_onRToggle, "rtoggle", A_GIMME, 0);
 addmess((method)swarm_onAToggle, "atoggle", A_GIMME, 0);
 addmess((method)swarm_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t %08xd                                 a-swarm   ©   a n d r é s i e r   2 0 0 4   all rights reserved",tick, 0);
// post("%08xd %02f %ld", sizeof(swarm), sizeof(swarm), sizeof(swarm),0);
}
// ___________________________________________________________________________
// _______________________________f u n c s___________________________________
// ___________________________________________________________________________

void swarm_setnum (swarm *x, int n) { if (n > 2 && n < MAXBIRDS) x->numbirds = n; }
void swarm_setdst (swarm *x, float c, float v) { x->dstx = c; x->dsty = v; }
void swarm_sethv (swarm *x, float c, float v) { x->width = c; x->height = v; }
void swarm_setstride (swarm *x, float c) { x->stride = c;}
void swarm_setdist (swarm *x, float c, float v) { x->mindist = c; x->maxdist = v; }
void swarm_setturn (swarm *x, float c) { x->maxturn = c;}

void swarm_setweightdst (swarm *x, float c) { x->weightdst = c;}


void swarm_init (swarm *x)
{
	short i;
	float tx, ty, tt; //hold rnd x, y & angle
	
	x->id = 0;
	x->numbirds = 20;
	x->minspeed = x->maxspeed = 5.0;
	x->width = x->height = 127.0;
	x->stride = 1.0;
	x->maxturn = 15.0;
	x->mindist = 30.0;
	x->maxdist = 60.0;
	x->weightdst = 1.0;

	for (i=0; i<MAXBIRDS; i++) { //zero out the whole flock of birds
		x->bee[i].bx = 0.0f;
		x->bee[i].by = 0.0f;
		x->bee[i].bspeed = 0.0f;
		x->bee[i].btheta = 0.0f;
		x->bee[i].id = (short)i; //assign the id
	}


	for (i=0; i<MAXBIRDS; i++) { // init _all_ to rnd locats
		rand_calc(x);		tx = (float) x->hold_rnd/rndmax * x->width;
		rand_calc(x);		ty = (float) x->hold_rnd/rndmax * x->height;
		rand_calc(x);		tt = (float) x->hold_rnd/rndmax * 360;
		x->bee[i].bx = tx;
		x->bee[i].by = ty;
		x->bee[i].btheta = tt;
	}

	for (i=0; i<MAXBIRDS; i++) { // init repulsion toogles
		x->onR[i] = 1;
		x->onA[i] = 1;
	}

}



void swarm_bird_reset (swarm *x)
{ // random birds locat within the arena
	short i;
	float tx, ty, tt; //hold rnd x, y & angle
	
	for (i=0; i<x->numbirds; i++) {
		rand_calc(x);		tx = (float) x->hold_rnd/rndmax * x->width;
		rand_calc(x);		ty = (float) x->hold_rnd/rndmax * x->height;
		rand_calc(x);		tt = (float) x->hold_rnd/rndmax * 360;
		x->bee[i].bx = tx;
		x->bee[i].by = ty;
		x->bee[i].btheta = tt;
	}


}


void swarm_onRToggle (swarm *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	if (argc>1) {
		for (i=0; i<argc;i++)
		{
			if(argv->a_type == A_FLOAT) {
				x->onR[i] = (Boolean) argv->a_w.w_float;
				argv++;
			} else if(argv->a_type == A_LONG) {
				x->onR[i] = (Boolean)argv->a_w.w_long;
				argv++;
			}
		}
	} else if (argc==1) {
		for (i=0; i<MAXBIRDS;i++)
		{
			if(argv->a_type == A_FLOAT) {
				x->onR[i] = (Boolean) argv->a_w.w_float;
			} else if(argv->a_type == A_LONG) {
				x->onR[i] = (Boolean)argv->a_w.w_long;
			}
	
		}
	}


}
void swarm_onAToggle (swarm *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	if (argc>1) {
		for (i=0; i<argc;i++)
		{
			if(argv->a_type == A_FLOAT) {
				x->onA[i] = (Boolean) argv->a_w.w_float;
				argv++;
			} else if(argv->a_type == A_LONG) {
				x->onA[i] = (Boolean)argv->a_w.w_long;
				argv++;
			}
		}
	} else if (argc==1) {
		for (i=0; i<MAXBIRDS;i++)
		{
			if(argv->a_type == A_FLOAT) {
				x->onA[i] = (Boolean) argv->a_w.w_float;
			} else if(argv->a_type == A_LONG) {
				x->onA[i] = (Boolean)argv->a_w.w_long;
			}
	
		}
	}


}

void swarm_list (swarm *x, Symbol *msg, short argc, Atom *argv) //input the args 
{

}

// ___________________________________________________________________________
// _______________________________f u n c s___________________________________
// ___________________________________________________________________________


void swarm_bang (swarm *x)
{
	Atom	*out;
	short i;
	out = x->out;

	swarm_move(x); // main_display_of_cpu_instrukts

	for (i=0; i < x->numbirds; i++) {	// bring out the datum
		SETLONG  (out+0, i);
		SETFLOAT (out+1, x->bee[i].bx);
		SETFLOAT (out+2, x->bee[i].by);
		SETFLOAT (out+3, x->bee[i].btheta);
		SETFLOAT (out+4, x->bee[i].bspeed);
		outlet_list(x->s_out, 0L, 5, out);
	}
}


// ___________________________________________________________________________
// _______________________________c o r e_____________________________________
// ___________________________________________________________________________

void swarm_bird_move (swarm *x, float heading)
{
	//x->bee[i]->theta is not being used...
	float change = 0.;
	float left, right, itheta, stride = x->stride;
	short id = x->id;
	
	
	left  = (float) ((int)(heading - x->bee[id].btheta + 360) % 360);
	right = (float) ((int)(x->bee[id].btheta - heading + 360) % 360);

	if(left<right)
		change = MIN (x->maxturn, left);
	else
		change = - MIN(x->maxturn, right);

	x->bee[id].btheta = (float) ((int)(x->bee[id].btheta + change + 360) % 360);
	x->bee[id].bx += (float) (stride*((int) x->maxspeed * cos(x->bee[id].btheta*M_PI/180))+x->width);
	x->bee[id].bx = (((int) x->bee[id].bx) % (int)x->width); 
	x->bee[id].by -= (float) (stride*((int) x->maxspeed * sin(x->bee[id].btheta*M_PI/180))-x->height);
	x->bee[id].by = (((int) x->bee[id].by) % (int)x->height); 
}


void swarm_move (swarm *x)
{
	short i;
	bird*  btemp;
	float tempheading;
	
	for (i=0; i<x->numbirds; i++) {
		btemp = &x->bee[i]; //works??
		x->id = i;
		tempheading = swarm_general_heading(x, *btemp);	// calc each birds dst heading
		swarm_bird_move (x,	tempheading); 					// move each bird
	}
}



float swarm_general_heading (swarm *x, bird bee) //receives a bee as a parameter
{
	bird*	temp; // a pointer to the bird
	pt		ptarget; //the target point 
	pt		pdist;
	pt		palign; //the align point 
	pt		patract; //the align point 
	pt		tempt, tempt2; //store the temp xy point coords of bee
	
	int		numbirds = 0, 
			i, badconversion;
	float	distance, ttheta;
	Boolean tooclose;
	double	weight;
		
		
//	ptarget.x = 0.0f;
//	ptarget.y = 0.0f;

	ptarget.x = x->dstx;
	ptarget.y = x->dsty;

	for (i=0; i<x->numbirds;i++) { // for all birds in array
		// get element i
		//get distance from bee to bee[i], not counting self distance

		temp = &x->bee[i];
		
		//prevent self distance, this will eventually check self

		distance = getDistance(*temp, bee);


//		if (distance>0. && distance<= x->maxdist) { //if within...
//		if ((x->bee[i].id != i) && distance>0. && distance<= x->maxdist) { //if within and id is not self...
		if ((bee.id != i) && distance>0. && distance<= x->maxdist) { //if within and id is not self...

/*
 * If the neighbour is a sibling the algorithm tells the boid to align its 
 * direction with the other Bird.  If the distance between them differs from 
 * minDISTANCE then a weighted forces is applied to move it towards that 
 * distance.  This force is stronger when the boids are very close or towards 
 * the limit of detection.
 */

			if(x->onA[i])
			{
			ttheta = temp->btheta;
			palign.x =  (100.*cos(ttheta)*M_PI/180.);
			palign.y = (-100.*sin(ttheta)*M_PI/180.);
			palign = normalisePoint(palign, 100); 

			tooclose = (distance < x->mindist);
			weight = 200.0;
			if(tooclose) weight *= pow(1.0 - (double)(distance/x->mindist),2);
			else weight *= - pow( (double)(distance-x->mindist) / (x->maxdist - x->mindist), 2);

			tempt.x = temp->bx; tempt.y = temp->by;
			tempt2.x = bee.bx; tempt2.y = bee.by;
			patract = sumPoint (tempt, -1.0, tempt2, 1.0);
			patract = normalisePoint(patract, weight);
			pdist = sumPoint (palign, 1.0, patract, 1.0);
			pdist = normalisePoint(pdist, 100);
			ptarget = sumPoint (ptarget, 1.0, pdist, 1.0);
			}
			
		}
		
/*
 * In repulsion the target point moves away from the other Bird with a force 
 * that is weighted according to a distance squared rule.
 */

        else { // repulsion
			if(x->onR[i])
			{
			tempt.x = temp->bx; tempt.y = temp->by;
			tempt2.x = bee.bx; tempt2.y = bee.by;

			pdist = sumPoint (tempt2, 1.0, tempt, -1.0);
			pdist = normalisePoint (pdist, 1000);
			weight = pow(1.0 - (double) (distance/x->maxdist), 2);
			ptarget = sumPoint(ptarget, 1.0, pdist, weight);
			}
		}

		numbirds++;

		}
//	}//end check all
	
	if (numbirds == 0) return bee.btheta;
	else {//average target points and add to position
		tempt2.x = bee.bx; tempt2.y = bee.by;
		ptarget = sumPoint (tempt2, x->weightdst, ptarget, 1/(double)numbirds);
	}

//	ttheta =( (180./M_PI * atan2(tempt2.y - ptarget.y, ptarget.x - tempt2.x)) );
	badconversion =(int)( (180./M_PI * atan2(tempt2.y - ptarget.y, ptarget.x - tempt2.x)) + 360 ) %360;
	
//	x->id = i;

//	return ((ttheta+360.)%360;
	return ((float)badconversion); // angle for Bird to steer towards

}






float getDistance (bird bee1, bird bee2)
{
	float dx, dy;	
	dx = bee2.bx - bee1.bx;
	dy = bee2.by - bee2.by;
	return (float) (sqrt( (dx*dx)+(dy*dy) ) );
} 


pt	sumPoint(pt p1, double w1, pt p2, double w2)
{
	pt	temp;
	temp.x = w1*p1.x + w2*p2.x;
	temp.y = w1*p1.y + w2*p2.y;
	return temp;
}

pt	normalisePoint(pt p, double n)
{
	pt	temp;
	double sizept = sqrt((p.x*p.x)+(p.y*p.y));
	double weight;
	
	if(sizept==0.0) return p;
	else{
	 	weight = n / sizept;
	 	temp.x = p.x * weight;
	 	temp.y = p.y * weight;
	 	return temp;
	}
}


void rand_calc(swarm *x)
{
	unsigned long ring;
	ring = x->hold_rnd;
	ring = 1664525L * ring + 1013904223L; //from numerical c++ recipes, fastest random w/ broader range
	x->hold_rnd = ring; // copy inside to output
}



void swarm_assist(swarm *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) swarm     e         c            h              o"); }
 //   else if (m==2&&a==1) { sprintf(s,"(float) y       e             c                h                   o"); }
}


