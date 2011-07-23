/* 
a-jit.mass / andré sier

cv.jit.mass was fucking intel<->ppc port
hence a remake as ub
 */

#include "jit.common.h"


typedef struct _jit_mass
{
	t_object				ob;
	long					mass,om;
		
} t_jit_mass;



t_jit_mass *jit_mass_new(void);
void jit_mass_free(t_jit_mass *x);
t_jit_err jit_mass_matrix_calc(t_jit_mass *x, void *inputs, void *outputs);

//  1 matrices in, 0 out
void jit_mass_calculate_ndim(t_jit_mass *x, long dimcount, long *dim, long planecount, 
	t_jit_matrix_info *in_minfo, char *bip);

t_jit_err jit_mass_init(void);
void *_jit_mass_class;



t_jit_err jit_mass_init(void) 
{
	long attrflags=0;
	t_jit_object *attr, *mop;
	
	_jit_mass_class = jit_class_new("jit_mass",(method)jit_mass_new,(method)jit_mass_free,
		sizeof(t_jit_mass),0L);

	//add mop ; num inputs+outputs; was 2,1 in jit.eclipse
	mop = jit_object_new(_jit_sym_jit_mop,1,0);
	jit_mop_single_type(mop,_jit_sym_char);
	jit_mop_single_planecount(mop,1);
	jit_class_addadornment(_jit_mass_class,mop);

	//add methods
	jit_class_addmethod(_jit_mass_class, (method)jit_mass_matrix_calc, 		"matrix_calc", 		A_CANT, 0L);	

	//add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;

	attr = jit_object_new(_jit_sym_jit_attr_offset,"om",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_mass,om));
	jit_class_addattr(_jit_mass_class,attr);

	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_OPAQUE_USER;

	attr = jit_object_new(_jit_sym_jit_attr_offset,"mass",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_mass,mass));
	jit_class_addattr(_jit_mass_class,attr);

		
	jit_class_register(_jit_mass_class);

	return JIT_ERR_NONE;
}


t_jit_err jit_mass_matrix_calc(t_jit_mass*x, void *inputs, void *outputs)
{
	t_jit_err err=JIT_ERR_NONE;
	long in_savelock;
	t_jit_matrix_info in_minfo;
	char *in_bp;
	long i,dimcount,planecount,dim[JIT_MATRIX_MAX_DIMCOUNT];
	void *in_matrix;

	in_matrix = jit_object_method(inputs, _jit_sym_getindex, 0);

	if (x&&in_matrix) {
		in_savelock = (long) jit_object_method(in_matrix,_jit_sym_lock,1);
		jit_object_method(in_matrix,_jit_sym_getinfo,&in_minfo);
		jit_object_method(in_matrix,_jit_sym_getdata,&in_bp);
		if (!in_bp) { err=JIT_ERR_GENERIC; goto out;}
		//compatible types?
		if ((in_minfo.type!=_jit_sym_char)) { 
			err=JIT_ERR_MISMATCH_TYPE; 
			goto out;
		}		
		//compatible planes?
		if ((in_minfo.planecount!=1)) { 
			err=JIT_ERR_MISMATCH_PLANE; 
			goto out;
		}		
		//get dimensions/planecount
		dimcount   = in_minfo.dimcount;
		planecount = in_minfo.planecount;			
		for (i=0;i<dimcount;i++) {
//			dim[i] = MIN(in_minfo.dim[i],out_minfo.dim[i]);
			dim[i] = in_minfo.dim[i];
		}		
				
		//calculate
//		jit_mass_calculate_ndim(x, dimcount, dim, planecount, &in_minfo, in_bp, &in2_minfo, in2_bp, &out_minfo, out_bp);
		jit_mass_calculate_ndim(x, dimcount, dim, planecount, &in_minfo, in_bp);
	} else {
		return JIT_ERR_INVALID_PTR;
	}
	
out:
//	jit_object_method(out_matrix,_jit_sym_lock,out_savelock);
	jit_object_method(in_matrix,_jit_sym_lock,in_savelock);
//	jit_object_method(in2_matrix,_jit_sym_lock,in2_savelock);
	return err;
}

//recursive function to handle higher dimension matrices, by processing 2D sections at a time 
void jit_mass_calculate_ndim(t_jit_mass *x, long dimcount, long *dim, long planecount, 
	t_jit_matrix_info *in_minfo, char *bip)
{

	uchar *ip;
	long		mass,width,height,i,j,om;
	
	
	if (dimcount<1) return; //safety
	
	switch(dimcount) {
	case 1:
		dim[1]=1;
	case 2: // the loop
				
		width  = dim[0];
		height = dim[1];
		
        mass=0;
		om = x->om;
		
		if(om) {
        
		for (i = 0; i < height ; i++ ) {
	        for(j = 0; j < width; j++) {
				
				ip = bip + i*in_minfo->dimstride[1] + j;				
				mass += *ip;
			}
		}

		}else {
		//	long val;
		for (i = 0; i < height ; i++ ) {
	        for(j = 0; j < width; j++) {
				
				ip = bip + i*in_minfo->dimstride[1] + j;
								
				if (*ip>0) mass += 1;
			}
		}

		
		}

	//save state
		x->mass=mass;
	

		
		break;
	default:
		for	(i=0;i<dim[dimcount-1];i++) {
			ip = bip + i*in_minfo->dimstride[dimcount-1];
			jit_mass_calculate_ndim(x,dimcount-1,dim,planecount,in_minfo,ip);
		}
	}
}

t_jit_mass*jit_mass_new(void)
{
	t_jit_mass*x;
//	short i;
		
	if (x=(t_jit_mass*)jit_object_alloc(_jit_mass_class)) {

		
		x->mass=0;
		x->om=0;		

		
	} else {
		x = NULL;
	}	
	return x;
}

void jit_mass_free(t_jit_mass*x)
{
	//nada
}


