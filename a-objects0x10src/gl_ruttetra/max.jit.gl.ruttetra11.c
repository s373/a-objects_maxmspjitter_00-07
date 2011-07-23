// max.jit.gl.gridshape.c
// draw 3d gridshapes.
//
// author: jkc
// © 2002 cycling '74




/*
added outlets players[8]

TRYING TO ADD OUTLETS AND INPUTS FOR PLAYER METHODS

player mode
	walk
	fly
	auto
	force
	bounce


*/


#include "jit.common.h"
#include "jit.gl.h"




typedef struct _max_jit_gl_ruttetra 
{
	t_object		ob;
	void			*obex;


} t_max_jit_gl_ruttetra;

t_jit_err jit_gl_ruttetra_init(void); 

void *max_jit_gl_ruttetra_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_gl_ruttetra_free(t_max_jit_gl_ruttetra *x);
t_class *max_jit_gl_ruttetra_class;
		 	

void main(void)
{	
	void *classex, *jitclass;
	
	jit_gl_ruttetra_init();	
	setup((t_messlist **)&max_jit_gl_ruttetra_class, (method)max_jit_gl_ruttetra_new, (method)max_jit_gl_ruttetra_free, (short)sizeof(t_max_jit_gl_ruttetra), 
		0L, A_GIMME, 0);

	classex = max_jit_classex_setup(calcoffset(t_max_jit_gl_ruttetra, obex));
	jitclass = jit_class_findbyname(gensym("jit_gl_ruttetra"));	
    max_jit_classex_standard_wrap(classex, jitclass, 0); 				// getattributes/dumpout/maxjitclassaddmethods/etc    
    addmess((method)max_jit_ob3d_assist, "assist", A_CANT,0);  

    post(" gl_ruttetra © andré sier 2004/6   "__DATE__"       "__TIME__);


	// add methods for 3d drawing
    max_ob3d_setup();
    
}

void max_jit_gl_ruttetra_free(t_max_jit_gl_ruttetra *x)
{
	max_jit_ob3d_detach(x);
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_gl_ruttetra_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_gl_ruttetra *x;
	void *jit_ob;
	long attrstart;
	t_symbol *dest_name_sym = _jit_sym_nothing;
	short i;

	if (x = (t_max_jit_gl_ruttetra *)max_jit_obex_new(max_jit_gl_ruttetra_class, gensym("jit_gl_ruttetra"))) 
	{
		//get normal args
		attrstart = max_jit_attr_args_offset(argc,argv);
		if (attrstart&&argv) 
		{
			jit_atom_arg_getsym(&dest_name_sym, 0, attrstart, argv);
		}

		if (jit_ob = jit_object_new(gensym("jit_gl_ruttetra"), dest_name_sym)) 
		{
			max_jit_obex_jitob_set(x, jit_ob);
			max_jit_obex_dumpout_set(x, outlet_new(x,NULL));
			max_jit_attr_args(x, argc, argv);		
			
			// attach the jit object's ob3d to a new outlet for sending drawing messages.	
			max_jit_ob3d_attach(x, jit_ob, outlet_new(x, "jit_matrix"));



/*
		for(i=0;i<8;i++) { //nu m players
			x->players[i]=outlet_new(x,0L);	
		}
*/

		} 
		else 
		{
			error("jit.gl.gridshape: could not allocate object");
			freeobject((t_object *)x);
			x = NULL;
		}
	}
	return (x);
}


