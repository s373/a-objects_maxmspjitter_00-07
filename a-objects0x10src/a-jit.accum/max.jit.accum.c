/*
a-jit.accum max file, 2nd go at 070715

*/


#include "jit.common.h"
#include "max.jit.mop.h"

typedef struct _max_jit_accum 
{
	t_object		ob;
	void			*obex;
} t_max_jit_accum;

t_jit_err jit_accum_init(void); 

void *max_jit_accum_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_accum_free(t_max_jit_accum *x);
void *max_jit_accum_class;
		 	
void main(void)
{	
	void *p,*q;
	
	jit_accum_init();	
	setup(&max_jit_accum_class, max_jit_accum_new, (method)max_jit_accum_free, (short)sizeof(t_max_jit_accum), 
		0L, A_GIMME, 0);

	p = max_jit_classex_setup(calcoffset(t_max_jit_accum,obex));
	q = jit_class_findbyname(gensym("jit_accum"));    
    max_jit_classex_mop_wrap(p,q,0); 		
    max_jit_classex_standard_wrap(p,q,0); 	
    addmess((method)max_jit_mop_assist, "assist", A_CANT,0);  

	post("a-jit.accum © andré sier 2007");
}

void max_jit_accum_free(t_max_jit_accum *x)
{
	max_jit_mop_free(x);
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_accum_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_accum *x,*o;

	if (x=(t_max_jit_accum *)max_jit_obex_new(max_jit_accum_class,gensym("jit_accum"))) {
		if (o=jit_object_new(gensym("jit_accum"))) {
			max_jit_mop_setup_simple(x,o,argc,argv);			
			max_jit_attr_args(x,argc,argv);
		} else {
			error("jit.accum: could not allocate object");
			freeobject(x);
		}
	}
	return (x);
}