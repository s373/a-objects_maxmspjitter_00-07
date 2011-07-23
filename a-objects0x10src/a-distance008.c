/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
a-distance - © andrŽ sier 2004

20061009 - adding clear and add node messages
20060830 - implementing extra distance methods

Euclidean distance. This is probably the most commonly chosen type of distance. It simply is the 
geometric distance in the multidimensional space. It is computed as: 
distance(x,y) = { i (xi - yi)2 }1Ú2 

Squared Euclidean distance. You may want to square the standard Euclidean distance in order to place 
progressively greater weight on objects that are further apart. This distance is computed as (see also the 
note in the previous paragraph): 
distance(x,y) = i (xi - yi)2 

City-block (Manhattan) distance. This distance is simply the average difference across dimensions. In 
most cases, this distance measure yields results similar to the simple Euclidean distance. However, note 
that in this measure, the effect of single large differences (outliers) is dampened (since they are not 
squared). The city-block distance is computed as: 
distance(x,y) = i |xi - yi| 

Chebychev distance. This distance measure may be appropriate in cases when one wants to define two 
objects as "different" if they are different on any one of the dimensions. The Chebychev distance is 
computed as: 
distance(x,y) = Maximum|xi - yi| 

Power distance. Sometimes one may want to increase or decrease the progressive weight that is placed 
on dimensions on which the respective objects are very different. This can be accomplished via the 
power distance. The power distance is computed as: 
distance(x,y) = ( i |xi - yi|p)1/r 
where r and p are user-defined parameters. A few example calculations may demonstrate how this 
measure "behaves." Parameter p controls the progressive weight that is placed on differences on 
individual dimensions, parameter r controls the progressive weight that is placed on larger differencbetween objects. If r and p are equal to 2, then this distance is equal to the Euclidean distance. 

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

#include "ext.h"
#include "ext_common.h"
#include "math.h"


#define MAXSIZ	255
//__

typedef struct	_distance
{
	t_object 	        c_ob;			
	void 		        *c_out;			// outlet	
	void 		        *c_out2;		// outlet	

	double		inpt[MAXSIZ];	// input pt to query
	double		node[MAXSIZ];	// nodes to compare distance to
	long		numnodes;		// number of nodes in the scene
	long		numdim;			// current dim count
	
	short		om;				// normalized output?
	short		activ;			// active zone

	t_atom		o[MAXSIZ];		// output vector
} distance;


//__

void *distance_new (Symbol *msg, short argc, Atom *argv);

void distance_setnodes (distance *x, Symbol *msg, short argc, Atom *argv); 
void distance_bang (distance *x); //exec
void distance_list (distance *x, Symbol *msg, short argc, Atom *argv); //input the args 
void distance_int (distance *x, long max);
void distance_float (distance *x, double max);

void distance_setnumdim (distance *x, short max);
void distance_setnumnodes (distance *x, short max);
void distance_calc (distance *x); //exec
void distance_assist(distance *x, void *b, long m, long a, char *s);
void *distance_class;

void distance_addnodes (distance *x, Symbol *msg, short argc, Atom *argv); 
void distance_clear (distance *x, Symbol *msg, short argc, Atom *argv); 

//__


void *distance_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
 distance *x;
 short i;
 
 x=(distance *)newobject(distance_class);

 x->c_out2=intout(x);
 x->c_out=listout(x);
 
 	x->numnodes = 0;
	x->numdim = 1;
	x->activ = 1;
	x->om = 0;

 for(i=0;i<MAXSIZ;i++) {
	x->inpt[i] = 0.0f;
	x->node[i] = 0.0f;
	x->o[i].a_type = A_FLOAT;
	x->o[i].a_w.w_float= 0.0f;

 }


	if (argc) {
		if (argc==1) { //if 1 arg set om
			if (argv[0].a_type == A_LONG){
			 x->om =  (short)argv[0].a_w.w_long;
			 post("om is %ld from new func", x->om); 
			} else if (argv[0].a_type == A_FLOAT){
			 x->om =  (short)argv[0].a_w.w_float; 
			 post("om is %ld from new func", x->om); 
     		  	}		

		} else if (argc>1) { //2+ args set nodes

			distance_setnodes(x,msg,argc,argv);

			}		


		
	} //end if args


 return(x);	
}


void distance_addnodes (distance *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
//	x->numnodes = argc/x->numdim;
	x->numnodes = x->numnodes; //id
	
	for(i=x->numnodes;i<argc;i++)
	{
			if (argv[i].a_type == A_LONG)
				x->node[i] = (double) argv[i].a_w.w_long;	
			else if (argv[i].a_type == A_FLOAT)
				x->node[i] = (double) argv[i].a_w.w_float;		

//		post("node %ld is %f",i,x->node[i]);
	}



	x->numnodes+= (argc/x->numdim);


}

 
void distance_setnodes (distance *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	
	x->numnodes = argc/x->numdim;
	
	for(i=0;i<argc;i++)
	{
			if (argv[i].a_type == A_LONG)
				x->node[i] = (double) argv[i].a_w.w_long;	
			else if (argv[i].a_type == A_FLOAT)
				x->node[i] = (double) argv[i].a_w.w_float;		

//		post("node %ld is %f",i,x->node[i]);
	}


}

void distance_clear (distance *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	
	x->numnodes = 0;
	
//	for(i=0;i<argc;i++)
//	{
//			if (argv[i].a_type == A_LONG)
//				x->node[i] = (double) argv[i].a_w.w_long;	
//			else if (argv[i].a_type == A_FLOAT)
//				x->node[i] = (double) argv[i].a_w.w_float;		
//
////		post("node %ld is %f",i,x->node[i]);
//	}





}


void distance_list (distance *x, Symbol *msg, short argc, Atom *argv)
{
	short i;
	
	
	x->numdim = argc;
	
	for(i=0;i<argc;i++)
	{
			if (argv[i].a_type == A_LONG)
				x->inpt[i] = (double) argv[i].a_w.w_long;	
			else if (argv[i].a_type == A_FLOAT)
				x->inpt[i] = (double) argv[i].a_w.w_float;		
	}

	distance_calc(x);
}

void distance_int (distance *x, long max)
{
	x->inpt[0] = (double)max;
	x->numdim = 1;
	distance_calc(x);
}

void distance_float (distance *x, double max)
{
	x->inpt[0] = max;
	x->numdim = 1;
	distance_calc(x);
}

void distance_setnumdim (distance *x, short max)
{
	x->numdim=max;
}

void distance_setnumnodes (distance *x, short max)
{
	x->numnodes = max;
}




void  distance_calc (distance *x)
{
	double disttemp[MAXSIZ];
	double dist;
	double mindist,sum;
	short i,j,pos;
	t_atom *o;
	
	o = x->o;
	



	switch(x->om) {


		case 0: //city block

			pos = -1;
			mindist =10000.0f; 


			for(i=0;i<x->numnodes;i++) {
				// calc dist from inpt to node point

				// - & abs
				for(j=0;j<x->numdim;j++) {
					disttemp[j] = x->node[x->numdim*i+j] - x->inpt[j];
					if(disttemp[j] < 0.0f) { disttemp[j] = -disttemp[j] ; }
				}

				// 
				dist = 0.0f;
				
				for(j=0;j<x->numdim;j++) {
					dist += disttemp[j];
				}
				//dist=sqrt(dist);
				
				if(dist<mindist) {
					mindist = dist;
					pos = i;
				}

				SETFLOAT(o+i, dist);
			}

		break;



		case 1: //squared euclidean distances

			pos = -1;
			mindist =10000.0f; 


			for(i=0;i<x->numnodes;i++) {
				// calc dist from inpt to node point

				// - & square
				for(j=0;j<x->numdim;j++) {
					disttemp[j] = x->node[x->numdim*i+j] - x->inpt[j];
					disttemp[j] *= disttemp[j];
				}

				// 
				dist = 0.0f;
				
				for(j=0;j<x->numdim;j++) {
					dist += disttemp[j];
				}
				//dist=sqrt(dist);
				
				if(dist<mindist) {
					mindist = dist;
					pos = i;
				}

				SETFLOAT(o+i, dist);
			}

		break;



		case 2: //Euclidean distance 

			pos = -1;
			mindist =10000.0f; 


			for(i=0;i<x->numnodes;i++) {
				// calc dist from inpt to node point

				// - & square
				for(j=0;j<x->numdim;j++) {
					disttemp[j] = x->node[x->numdim*i+j] - x->inpt[j];
					disttemp[j] *= disttemp[j];
				}

				// 
				dist = 0.0f;
				
				for(j=0;j<x->numdim;j++) {
					dist += disttemp[j];
				}
				dist=sqrt(dist);
				
				if(dist<mindist) {
					mindist = dist;
					pos = i;
				}

				SETFLOAT(o+i, dist);
			}

		break;

		case 3: //normalized distances

				// first pass
			pos = -1;
			mindist =0.0f; 

			sum = 0;

			for(i=0;i<x->numnodes;i++) {
				// calc dist from inpt to node point

				// - & square
				for(j=0;j<x->numdim;j++) {
					disttemp[j] = x->node[x->numdim*i+j] - x->inpt[j];
					disttemp[j] *= disttemp[j];
					disttemp[j] = 1.0f/disttemp[j] ;
					disttemp[j] *= disttemp[j];
					if(disttemp[j]<0.001) disttemp[j] = 0.001; //when 0.0 distance(happens) goes nan; not doing anything...
				}

				// 
				dist = 0.0f;
				
				for(j=0;j<x->numdim;j++) {
					dist += disttemp[j];
				}

				

				dist=sqrt(dist);
				
				
				if(dist>mindist) {
					mindist = dist;
					pos = i;
				}

				sum += dist;

				SETFLOAT(o+i, dist);
			}

				sum = 1.0/sum;
				// 2nd pass

			for(i=0;i<x->numnodes;i++) {
				
				x->o[i].a_w.w_float *= sum;
				x->o[i].a_w.w_float = pow(x->o[i].a_w.w_float,0.5);

			}




		break;




		// Chebychev distance
		case 4: 

			pos = -1;
			mindist =10000.0f; 


			for(i=0;i<x->numnodes;i++) {
				// calc dist from inpt to node point

				// - & abs
				for(j=0;j<x->numdim;j++) {
					disttemp[j] = x->node[x->numdim*i+j] - x->inpt[j];
					if(disttemp[j] < 0.0f) { disttemp[j] = -disttemp[j] ; }
				}

				// 
				dist = 0.0f;
				
				for(j=0;j<x->numdim;j++) {
					dist = MAX(disttemp[j], dist);
				}
				//dist=sqrt(dist);
				
				if(dist<mindist) {
					mindist = dist;
					pos = i;
				}

				SETFLOAT(o+i, dist);
			}

		break;






	}





	outlet_int  (x->c_out2, pos);				// output min dist element num


	outlet_list (x->c_out, 0L, x->numnodes, o); // output atom of differences



}





void distance_om (distance *x, short max)
{
	x->om = max;
}

void distance_bang (distance *x)
{
	distance_calc(x);
}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&distance_class,(method)distance_new,0L,(short)sizeof(distance),0L,
 A_GIMME,0);
 
 addbang((method)distance_bang);
 addint((method)distance_int);
 addfloat((method)distance_float);
 addmess((method)distance_list, "list", A_GIMME, 0);
 addmess((method)distance_setnodes, "nodes", A_GIMME, 0);
 addmess((method)distance_setnumnodes, "numnodes", A_DEFLONG, 0);
 addmess((method)distance_setnumdim, "numdim", A_DEFLONG, 0);
 addmess((method)distance_om, "om", A_DEFLONG, 0);
addmess((method)distance_addnodes, "addnodes", A_GIMME, 0);
addmess((method)distance_clear, "clear", A_GIMME, 0);

 addmess((method)distance_assist,"assist", A_CANT, 0);
 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                 a-distance   ©   a n d r Ž s i e r   2 0 0 4   all rights reserved",tick, 0);
}


void distance_assist(distance *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"set nodes, list"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) distances to nodes"); }
    else if (m==2&&a==1) { sprintf(s,"(int) nearest zone"); }
}


