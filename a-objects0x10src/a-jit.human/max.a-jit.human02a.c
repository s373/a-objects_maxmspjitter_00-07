/* 
		important max.a-jit.human02.c is crashing. 2a is first version that gets all coords..
*/
/* 
		
*/


#include "jit.common.h"
#include "max.jit.mop.h"

typedef struct _max_jit_human
{
	t_object		ob;
	void			*obex;
		//outlets
	void			*out_box_coords; //min max xy
	void			*out_arm_left, *out_arm_right;
	void			*out_head, *out_foot_left, *out_foot_right;
	void			*out_state;
} t_max_jit_human;




t_jit_err jit_human_init(void); 
void *max_jit_human_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_human_free(t_max_jit_human*x);
void max_jit_human_mproc(t_max_jit_human *x, void *mop);			
void max_jit_human_assist(t_max_jit_human *x, void *b, long m, long a, char *s);
void *max_jit_human_class;
			
t_symbol *ps_getbox_coords,
		*ps_getarm_left, *ps_getarm_right, 
		*ps_gethead, 
		*ps_getfoot_left, *ps_getfoot_right,
		*ps_getstate;
			



void main(void)
{	
	void *p,*q;
	
	jit_human_init();	
	setup(&max_jit_human_class, max_jit_human_new, (method)max_jit_human_free, (short)sizeof(t_max_jit_human), 
		0L, A_GIMME, 0);

	p = max_jit_classex_setup(calcoffset(t_max_jit_human,obex));
	q = jit_class_findbyname(gensym("jit_human"));    
//    max_jit_classex_mop_wrap(p,q,0); 		
    max_jit_classex_mop_wrap(p,q,MAX_JIT_MOP_FLAGS_OWN_BANG|MAX_JIT_MOP_FLAGS_OWN_OUTPUTMATRIX); //custom bang/outputmatrix 		
    max_jit_classex_mop_mproc(p,q,max_jit_human_mproc); 	//custom mproc
    max_jit_classex_standard_wrap(p,q,0); 	
    addmess((method)max_jit_human_assist, "assist", A_CANT,0);  

	ps_getstate = gensym("getstate");
	ps_getbox_coords = gensym("getbox_coords");
	ps_getarm_left = gensym("getarm_left");
	ps_getarm_right = gensym("getarm_right");
	ps_gethead = gensym("gethead");
	ps_getfoot_left = gensym("getfoot_left");
	ps_getfoot_right = gensym("getfoot_right");


	post("a-jit.human  - andrŽ sier - "__DATE__"  "__TIME__);
}

void max_jit_human_mproc(t_max_jit_human *x, void *mop)
{
	t_jit_err err;
	long ac=0;
	t_atom *av=NULL;
	void *o;
//	t_atom temp[4];
	
//	av = temp;
	
	
	o=max_jit_obex_jitob_get(x);

	if (err=(t_jit_err) jit_object_method(
		o,
		_jit_sym_matrix_calc,
		jit_object_method(mop,_jit_sym_getinputlist),
		jit_object_method(mop,_jit_sym_getoutputlist))) 
	{
		jit_error_code(x,err); 

	} else {

//		ac = 2;

		jit_object_method(o,ps_getstate,&ac,&av);
		outlet_int(x->out_state,jit_atom_getlong(av));
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;


		jit_object_method(o,ps_getfoot_right,&ac,&av);
		outlet_anything(x->out_foot_right,_jit_sym_list,ac,av);
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;

		jit_object_method(o,ps_getfoot_left,&ac,&av);
		outlet_anything(x->out_foot_left,_jit_sym_list,ac,av);
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;

//		ac = 2;
		jit_object_method(o,ps_gethead,&ac,&av);
		outlet_anything(x->out_head,_jit_sym_list,ac,av);
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;

//		ac = 2;
		jit_object_method(o,ps_getarm_right,&ac,&av);
		outlet_anything(x->out_arm_right,_jit_sym_list,ac,av);
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;

//		ac = 2;
		jit_object_method(o,ps_getarm_left,&ac,&av);
		outlet_anything(x->out_arm_left,_jit_sym_list,ac,av);
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;

//		ac = 4;
		jit_object_method(o,ps_getbox_coords,&ac,&av);
		outlet_anything(x->out_box_coords,_jit_sym_list,ac,av);
		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
		av=NULL; ac=0;

//		switch(ac) {
//		case 1:
//			outlet_float(x->maxout,jit_atom_getfloat(av));
//			break;
//		default:		
//			outlet_anything(x->maxout,_jit_sym_list,ac,av);
//			break;
//		}
//		if (av) jit_freebytes(av,(ac)*sizeof(t_atom));
//		av=NULL; ac=0;
	}
}


void max_jit_human_assist(t_max_jit_human *x, void *b, long m, long a, char *s)
{
	if (m == 1) { //input
		max_jit_mop_assist(x,b,m,a,s);
	} else { //output
		switch (a) {
		case 0:
			sprintf(s,"(list) box min/max");
			break;  			
		case 1:
			sprintf(s,"(list) arm left");
			break;  			
		case 2:
			sprintf(s,"(list) arm right");
			break;  			
		case 3:
			sprintf(s,"(list) head");
			break;  			
		case 4:
			sprintf(s,"(list) foot left");
			break;  			
		case 5:
			sprintf(s,"(list) foot right");
			break;  			
		case 6:
			sprintf(s,"(int) state");
			break; 			
		case 7:
			sprintf(s,"dumpout");
			break; 			
		}
	}
}


void max_jit_human_free(t_max_jit_human*x)
{
	max_jit_mop_free(x);
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_human_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_human*x,*o;

	if (x=(t_max_jit_human*)max_jit_obex_new(max_jit_human_class,gensym("jit_human"))) {
		if (o=jit_object_new(gensym("jit_human"))) {
			max_jit_mop_setup_simple(x,o,argc,argv);			
			//add additional non-matrix output, right2left
			x->out_state 	= outlet_new(x,0L);	
			x->out_foot_right 	= outlet_new(x,0L);	
			x->out_foot_left 	= outlet_new(x,0L);	
			x->out_head 	= outlet_new(x,0L);	
			x->out_arm_right 	= outlet_new(x,0L);	
			x->out_arm_left 	= outlet_new(x,0L);	
			x->out_box_coords 	= outlet_new(x,0L);	
			max_jit_attr_args(x,argc,argv);
		} else {
			error("jit.human: could not allocate object");
			freeobject(x);
		}
	}
	return (x);
}


