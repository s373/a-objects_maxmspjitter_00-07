/*

a-jit.accum 

a multiporpose object to receive 1 matrix and keep accuming it,
or, more, and receive n matrices and clear framebuffer after that


*/


#include "jit.common.h"

typedef struct _jit_accum 
{
	t_object				ob;
	long					mode, oper;
	
	float					threshold;
	float					mix;
	float					mult;
	float					add;
	
	int						clear; // how many frames to wait to clear framebuf. if 0 never clear, else N inputs clear
	int						clearcount;
} t_jit_accum;

void *_jit_accum_class;

t_jit_accum *jit_accum_new(void);
void jit_accum_free(t_jit_accum *x);
t_jit_err jit_accum_matrix_calc(t_jit_accum *x, void *inputs, void *outputs);
void jit_accum_calculate_ndim(t_jit_accum *x, long dimcount, long *dim, long planecount, 
	t_jit_matrix_info *in_minfo, char *bip,/* t_jit_matrix_info *in2_minfo, char *bip2, */t_jit_matrix_info *out_minfo, char *bop);
t_jit_err jit_accum_init(void);


t_jit_err jit_accum_init(void) 
{
	long attrflags=0;
	t_jit_object *attr, *mop;
	
	_jit_accum_class = jit_class_new("jit_accum",(method)jit_accum_new,(method)jit_accum_free,
		sizeof(t_jit_accum),0L);

	//add mop
	mop = jit_object_new(_jit_sym_jit_mop,1,1);
	jit_mop_single_type(mop,_jit_sym_char);
	jit_mop_single_planecount(mop,1);
	jit_class_addadornment(_jit_accum_class,mop);

	//add methods
	jit_class_addmethod(_jit_accum_class, (method)jit_accum_matrix_calc, 		"matrix_calc", 		A_CANT, 0L);	

	//add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;

	// mode -- switches eclipsing states
	attr = jit_object_new(_jit_sym_jit_attr_offset,"mode",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,mode));
	jit_class_addattr(_jit_accum_class,attr);

	// op -- 0 is add, 1 is multiply
	attr = jit_object_new(_jit_sym_jit_attr_offset,"op",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,oper));
	jit_class_addattr(_jit_accum_class,attr);



		//threshold + add + mix + mult
	attr = jit_object_new(_jit_sym_jit_attr_offset,"threshold",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,threshold));
	jit_class_addattr(_jit_accum_class,attr);
	attr = jit_object_new(_jit_sym_jit_attr_offset,"add",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,add));
	jit_class_addattr(_jit_accum_class,attr);
	attr = jit_object_new(_jit_sym_jit_attr_offset,"mult",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,mult));
	jit_class_addattr(_jit_accum_class,attr);
	attr = jit_object_new(_jit_sym_jit_attr_offset,"mix",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,mix));
	jit_class_addattr(_jit_accum_class,attr);
	attr = jit_object_new(_jit_sym_jit_attr_offset,"clear",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,clear));
	jit_class_addattr(_jit_accum_class,attr);


//unsettable ungettable argument
	attrflags = JIT_ATTR_GET_OPAQUE_USER | JIT_ATTR_SET_OPAQUE_USER;

	attr = jit_object_new(_jit_sym_jit_attr_offset,"clearcount",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_accum,clearcount));
	jit_class_addattr(_jit_accum_class,attr);


		
	jit_class_register(_jit_accum_class);

	return JIT_ERR_NONE;
}


t_jit_err jit_accum_matrix_calc(t_jit_accum *x, void *inputs, void *outputs)
{
//matrixcalc called whenever matrix arrives in inlet!

	t_jit_err err=JIT_ERR_NONE;
	long in_savelock,in2_savelock,out_savelock;
	t_jit_matrix_info in_minfo,in2_minfo,out_minfo;
	char *in_bp,*in2_bp,*out_bp;
	long i,dimcount,planecount,dim[JIT_MATRIX_MAX_DIMCOUNT];
	void *in_matrix, *in2_matrix, *out_matrix;

	in_matrix = jit_object_method(inputs, _jit_sym_getindex, 0);
//	in2_matrix = jit_object_method(inputs, _jit_sym_getindex, 1);
	out_matrix = jit_object_method(outputs,_jit_sym_getindex, 0);

	if (x&&in_matrix/*&&in2_matrix*/&&out_matrix) {
		
		in_savelock = (long) jit_object_method(in_matrix,_jit_sym_lock,1);
//		in2_savelock = (long) jit_object_method(in2_matrix,_jit_sym_lock,1);
		out_savelock = (long) jit_object_method(out_matrix,_jit_sym_lock,1);
		
		jit_object_method(in_matrix,_jit_sym_getinfo,&in_minfo);
//		jit_object_method(in2_matrix,_jit_sym_getinfo,&in2_minfo);
		jit_object_method(out_matrix,_jit_sym_getinfo,&out_minfo);
		
		jit_object_method(in_matrix,_jit_sym_getdata,&in_bp);
//		jit_object_method(in2_matrix,_jit_sym_getdata,&in2_bp);
		jit_object_method(out_matrix,_jit_sym_getdata,&out_bp);
		
		if (!in_bp) { err=JIT_ERR_GENERIC; goto out;}
//		if (!in2_bp) { err=JIT_ERR_GENERIC; goto out;}
		if (!out_bp) { err=JIT_ERR_GENERIC; goto out;}
		
		//compatible types?
		if ((in_minfo.type!=_jit_sym_char)||(in_minfo.type!=out_minfo.type)) { 
			err=JIT_ERR_MISMATCH_TYPE; 
			goto out;
		}		

		//compatible planes?			4 planes only for now 
		if ((in_minfo.planecount!=4)||(out_minfo.planecount!=4)) { 
			err=JIT_ERR_MISMATCH_PLANE; 
			goto out;
		}		

/*		
		//compatible types?
		if ((in2_minfo.type!=_jit_sym_char)||(in2_minfo.type!=out_minfo.type)) { 
			err=JIT_ERR_MISMATCH_TYPE;
			post("type not ok");
			goto out;
		}		

		//compatible planes?
		if ((in2_minfo.planecount!=4)) { 
			err=JIT_ERR_MISMATCH_PLANE; 
			post("planes not ok");
			goto out;
		}		

*/
		//get dimensions/planecount
		dimcount   = out_minfo.dimcount;
		planecount = out_minfo.planecount;			
		for (i=0;i<dimcount;i++) {
			dim[i] = MIN(in_minfo.dim[i],out_minfo.dim[i]);
		}		
				
		//calculate
		jit_accum_calculate_ndim(x, dimcount, dim, planecount, &in_minfo, in_bp, /*&in2_minfo, in2_bp,*/ &out_minfo, out_bp);

	} else {
		return JIT_ERR_INVALID_PTR;
	}
	
out:
	jit_object_method(out_matrix,_jit_sym_lock,out_savelock);
	jit_object_method(in_matrix,_jit_sym_lock,in_savelock);
//	jit_object_method(in2_matrix,_jit_sym_lock,in2_savelock);
	return err;
}

//recursive function to handle higher dimension matrices, by processing 2D sections at a time 
void jit_accum_calculate_ndim(t_jit_accum *x, long dimcount, long *dim, long planecount, 
	t_jit_matrix_info *in_minfo, char *bip, /*t_jit_matrix_info *in2_minfo, char *bip2,*/ t_jit_matrix_info *out_minfo, char *bop)
{
	uchar *ip,*op;
	long i,j,width,height;
	
	float	mix, add, threshold, mult;
	int clear, clearcount;
	int doclear = 0;
	
	
	
	mix = x->mix;
	add = x->add;
	threshold = x->threshold;
	mult = x->mult;

			
	if (dimcount<1) return; //safety

	// check if clear matrix for next frame
	clear = x->clear;
	clearcount = x->clearcount++;
	if(clear) {
		if(clearcount>clear) {
			x->clearcount = 0;
			doclear = 1;
		}
	}


	
	switch(dimcount) {
	case 1:
		dim[1]=1;
	case 2:
				
		width  = dim[0];
		height = dim[1];
		
		
		for(j=0; j < height; j++) {
			for(i=0; i < width; i++) {
			
				ip = bip + i*in_minfo->dimstride[1] + j;
				op = bop + i*out_minfo->dimstride[1] + j;

			}
		}
		
		
		
		
		break;
	default:
		for	(i=0;i<dim[dimcount-1];i++) {
			ip = bip + i*in_minfo->dimstride[dimcount-1];
			op = bop + i*out_minfo->dimstride[dimcount-1];
			jit_accum_calculate_ndim(x,dimcount-1,dim,planecount,in_minfo,ip,/*in2_minfo,bip2,*/out_minfo,op);
		}
	}
}

t_jit_accum *jit_accum_new(void)
{
	t_jit_accum *x;
	short i;
		
	if (x=(t_jit_accum *)jit_object_alloc(_jit_accum_class)) {

		x->threshold = 0.;
		x->mix = 0.9;
		x->mult =0.1f;
		x->add = 0.0f;


		x->clear = 0;
		x->clearcount = 0;

		
	} else {
		x = NULL;
	}	
	return x;
}

void jit_accum_free(t_jit_accum *x)
{
	//nada
}


