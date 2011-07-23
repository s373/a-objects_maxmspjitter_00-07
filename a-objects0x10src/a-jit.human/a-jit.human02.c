/* 
a-jit.human / andré sier
special 747.3 hack findbounds and arms and head and feet of user
 */

#include "jit.common.h"

typedef struct _jit_human
{
	t_object				ob;
	long					state;
	long					box_coords[4]; //min max xy
	long					arm_left[2], arm_right[2];
	long					head[2], foot_left[2], foot_right[2];
	long					exe[5];
	float					min, max;
} t_jit_human;


t_jit_human*jit_human_new(void);
void jit_human_free(t_jit_human*x);
t_jit_err jit_human_matrix_calc(t_jit_human*x, void *inputs, void *outputs);

//  1 matrices in, 0 out
void jit_human_calculate_ndim(t_jit_human*x, long dimcount, long *dim, long planecount, 
	t_jit_matrix_info *in_minfo, char *bip);

t_jit_err jit_human_init(void);
void *_jit_human_class;



t_jit_err jit_human_init(void) 
{
	long attrflags=0;
	t_jit_object *attr, *mop;
	
	_jit_human_class = jit_class_new("jit_human",(method)jit_human_new,(method)jit_human_free,
		sizeof(t_jit_human),0L);

	//add mop ; num inputs+outputs; was 2,1 in jit.eclipse
	mop = jit_object_new(_jit_sym_jit_mop,1,0);
	jit_mop_single_type(mop,_jit_sym_char);
	jit_mop_single_planecount(mop,1);
	jit_class_addadornment(_jit_human_class,mop);

	//add methods
	jit_class_addmethod(_jit_human_class, (method)jit_human_matrix_calc, 		"matrix_calc", 		A_CANT, 0L);	

	//add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;

	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"min",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_human,min));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset,"max",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_human,max));
	jit_class_addattr(_jit_human_class,attr);
	

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"exe",_jit_sym_long,5,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,exe));
	jit_class_addattr(_jit_human_class,attr);
	

	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_OPAQUE_USER;

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"box_coords",_jit_sym_long, 4,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,box_coords));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"arm_left",_jit_sym_long, 2,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,arm_left));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"arm_right",_jit_sym_long, 2,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,arm_right));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"head",_jit_sym_long, 2,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,head));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"foot_left",_jit_sym_long, 2,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,foot_left));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"foot_right",_jit_sym_long, 2,attrflags,
		(method)0L,(method)0L,0/*fix*/,calcoffset(t_jit_human,foot_right));
	jit_class_addattr(_jit_human_class,attr);

	attr = jit_object_new(_jit_sym_jit_attr_offset,"state",_jit_sym_long,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_human,state));
	jit_class_addattr(_jit_human_class,attr);

		
	jit_class_register(_jit_human_class);

	return JIT_ERR_NONE;
}


t_jit_err jit_human_matrix_calc(t_jit_human*x, void *inputs, void *outputs)
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
//		jit_human_calculate_ndim(x, dimcount, dim, planecount, &in_minfo, in_bp, &in2_minfo, in2_bp, &out_minfo, out_bp);
		jit_human_calculate_ndim(x, dimcount, dim, planecount, &in_minfo, in_bp);

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
void jit_human_calculate_ndim(t_jit_human*x, long dimcount, long *dim, long planecount, 
	t_jit_matrix_info *in_minfo, char *bip)
{

	uchar *ip;
	long		box_coords[4]; //min max xy
	long		arm_left[2], arm_right[2];
	long		head[2], feet[2];
//	uchar	mmin = (uchar) CLAMP((x->min*255.),0,255);
//	uchar	mmax = (uchar) CLAMP((x->max*255.),0,255);
	uchar	mmin = (uchar) (x->min*255.);
	uchar	mmax = (uchar) (x->max*255.);
	long	i,j;
	long	inrange0,inrange1,inrange2;
	long	min0,min1,max0,max1;
	long	dim0fr,dim1fr,dim1half; //dim 0 fraction for head/feet calc
	long	width, height;
	uchar	found_arml, found_armr, found_head, found_foot_left, found_foot_right;
	uchar	ipval,ipval2;
	
	
	inrange2 = FALSE;
	inrange1 = FALSE;
	
	for(i=0;i<4;i++)
		box_coords[i]=-1;
	for(i=0;i<2;i++)
		arm_left[i]=arm_right[i]=head[i]=feet[i]=-1;
	
	min0 = min1 = 0x7FFFFFFF;
//	min0 = dim[0] + 1; min1 = dim[1] + 1;
	max0 = max1 = -1;
			
	if (dimcount<1) return; //safety
	
	switch(dimcount) {
	case 1:
		dim[1]=1;
	case 2: // the loop
				
		width  = dim[0];
		height = dim[1];
		
        
        
		for (i = 0; i < height ; i++ ) //first pass; get the box
		{

			inrange1=FALSE; //have toset to off b4 the loop
			
	        for(j = 0; j < width; j++) 
        	 {

				inrange0=TRUE;

				ip = bip + i*in_minfo->dimstride[1] + j;				
				ipval = *ip;
				
				if (ipval < mmin)		inrange0 = FALSE;
				else if (ipval > mmax)	inrange0 = FALSE;

//				post("__char value %ld (%ld,%ld) in range is %ld",ipval, mmin, mmax,inrange0);

				
				if(inrange0==TRUE) {
					inrange1=TRUE;					
					if(j<min0) min0=j;
					else if(j>max0) max0=j;
					
				}
			}

			if(inrange1) {
				inrange2=TRUE;
				if(i<min1) min1 = i;
				else if(i>max1) max1 = i;
//				post("image line %ld - inrange - mm %ld %ld",i,min1,max1);
			}

		}
 
		//check minimuns nanning a cause do minimax findings crashing instantly
//		if (min0>width) {
//			post("::(min0>width)");
//			min0=-1; min1=-1;
//			inrange2=FALSE;
//		}


	//save state
	if (inrange2) x->state = 1;
	else x->state = 0;
	
	//check mins, maxs	
	if (min0<0x7FFFFFFF) box_coords[0] = min0;
	if (min1<0x7FFFFFFF) box_coords[2] = min1;
	if (max0>-1) box_coords[1] = max0;
	else box_coords[1] = box_coords[0];
	if (max1>-1) box_coords[3] = max1;
	else  box_coords[3] = box_coords[1];

	//so box_coords, local, is storing two points. here in the assgnment,
	//i just flip em to easier framerect commands attributes
			// box coords min max found; min0 was -1 if not set
			x->box_coords[0]=box_coords[0];
			x->box_coords[1]=box_coords[2];
			x->box_coords[2]=box_coords[1];
			x->box_coords[3]=box_coords[3];

//			post("	//state %ld  box %ld %ld %ld %ld",x->state,x->box_coords[0], x->box_coords[1], x->box_coords[2], x->box_coords[3]);

		// check second feasible

		if(inrange2==TRUE)
		{
//			post("		//second pass inside box %ld %ld %ld %ld",box_coords[0], box_coords[1], box_coords[2], box_coords[3]);
//			post("		//second pass inside box");


			//not found yet
	//		found_arml=found_armr=found_head=found_feet=FALSE;
	
	
	
			if(x->exe[0]) {
	
				//arm_left	

				found_arml = FALSE;
				
				j = min0; 
				for (i = min1; i < max1; i++) { // find the first pixel below min

					ip = bip + i*in_minfo->dimstride[1] +j; //point it right
					ipval = *ip;			

					inrange0=TRUE;

					if(ipval<mmin || ipval>mmax)
						inrange0=FALSE;

					if(inrange0) {
						// found it
						found_arml=TRUE;
						x->arm_left[0]=(long)j;
						x->arm_left[1]=(long)i;

	//					post("		//found arm_left 1st pass @ %ld %ld", j, i);
						goto arml_exit;
					}

	//				ip++;

				}
			arml_exit:
			;
				//arm_left not found
				
				//SHOULD GO FROM CORNER LEFT to MIDDLE LEFT! instead of traversing all
				
				if(!found_arml) { // arm_left is not on Y axis but on X
	//					post("		//!found arm_left :: second pass arm_left @ %ld %ld", j, i);

					//arm_left
					
					i = min1;// find the first pixel above min
	//				ip = bip + i*in_minfo->dimstride[1]; //point it right

					for (j = min0; j < max0; j++) { // traverse horizontally
						inrange0=TRUE;
						
						ip = bip + i*in_minfo->dimstride[1] + j; //point it right
						ipval = *ip;			


						if(ipval<mmin || ipval>mmax)
							inrange0=FALSE;
						if(inrange0) {
							// found it
							found_arml=TRUE;
							x->arm_left[0]=j;
							x->arm_left[1]=i;
	//						post("		//found arm_left @ %ld %ld second pass!!", j, i);
							goto arml_exit2;
						}

						//ip++;

					}
	//						post("		// arm_left NOT FOUND!!!", j, i);

		
			arml_exit2:
			;

				
				} 
				
			}



			if(x->exe[1]) {

				//arm_right	begin

				found_armr = FALSE;
				j = max0; 
				for (i = min1; i < max1; i++) { // find the first pixel below min
					ip = bip + i*in_minfo->dimstride[1] + j; //point it right	instead of ip++...		
					ipval = *ip;
					
					inrange0=TRUE;

					if(ipval<mmin || ipval>mmax)
						inrange0=FALSE;

					if(inrange0) {
						// found it
						found_armr=TRUE;
						x->arm_right[0]=j;
						x->arm_right[1]=i;
						goto armr_exit;
					}
	//				ip++;
				}
			armr_exit:
			;

				//arm_right not found
				if(!found_armr) { // arm_left is not on Y axis but on X
					i = min1;// find the first pixel above min
					for (j = max0-1; j > min0; j--) { // traverse horizontally reversed

						ip = bip + i*in_minfo->dimstride[1] + j; //point it right
						ipval = *ip;
						inrange0=TRUE;
						if(ipval<mmin || ipval>mmax)
							inrange0=FALSE;
						if(inrange0) {
							// found it
							found_armr=TRUE;
							x->arm_right[0]=j;
							x->arm_right[1]=i;
							goto armr_exit2;
						}
			//			ip++;
					}

		
			armr_exit2:
			;

				
				} 			
				// end arm_left
			
			}
			
			
	
			//head	begin
			if(x->exe[2]) {

			found_head = FALSE;
			dim0fr = (0.35*(max0-min0));
			dim1fr = (0.35*(max1-min1));
			dim1half = (max1-min1)/2;
			//for head calcs, avoid .1 extremes and peruse half box only
			//tighter loop must be dim0
			for (i = min1; i < (max1-dim1half); i++) { 
				for (j = (min0+dim0fr); j < (max0-dim0fr); j++) {

					ip = bip + i*in_minfo->dimstride[1] + j; 
					ipval = *ip++;
					ipval2 = *ip;
					
					inrange0=FALSE;

						//require 2 consecutive pixels withing threshold for head calcs
					if(ipval >= mmin && ipval<= mmax &&
					   ipval2 >= mmin && ipval2 <= mmax )
						inrange0=TRUE;


					if(inrange0) {
						// found it
						found_head=TRUE;
						x->head[0]=j;
						x->head[1]=i;
						goto head_exit;
					}
	//				ip++;
				}
		}
		head_exit:
		;

			//head not found
			if(!found_head) { 
				x->head[0] = x->head[1] = -1;
			} 			
			// end head
			
		}
			
			
			

			//foot left	begin

			if(x->exe[3]) {

			found_foot_left = FALSE;
//			dim0fr = (0.1*(max0-min0));
//			dim1fr = (0.1*(max1-min1));
//			dim1half = (max1-min1)/2;
			//for feet calcs, allow extrems avoid .1 extremes and peruse half box only from the bottim
			
			for (i = max1; i > (max1-dim1half); i--) { 
				for (j = (min0); j < (max0); j++) {					
					ip = bip + i*in_minfo->dimstride[1] + j; 
					ipval = *ip;
					
					inrange0=FALSE;

						//require 1 consecutive pixels withing threshold for feet calcs
					if ( ipval >= mmin && ipval<= mmax )
						inrange0=TRUE;


					if(inrange0) {
						// found it
						found_foot_left=TRUE;
						x->foot_left[0]=j;
						x->foot_left[1]=i;
						goto footl_exit;
					}
	//				ip++;
				}
			}
		footl_exit:
		;

			if(!found_foot_left) { 
				x->foot_left[0] = x->foot_left[1] = -1;
			} 			
			// end foot_left
	


		}

			//foot right	begin

			if(x->exe[4]) {

			found_foot_right = FALSE;
//			dim0fr = (0.1*(max0-min0));
//			dim1fr = (0.1*(max1-min1));
//			dim1half = (max1-min1)/2;
			
			dim0fr = (x->foot_left[0]>0)?x->foot_left[0]:((max0-min0)/2+min0);

			//for feet calcs, allow extrems avoid .1 extremes and peruse half box only from the bottim
				//reversed x lookup

			for (i = max1; i > (max1-dim1half); i--) { 
				for (j = (max0); j > (dim0fr); j--) {  //was min0: do not let right foot go over left
					ip = bip + i*in_minfo->dimstride[1] + j; 
					ipval = *ip;
					
					inrange0=FALSE;

						//require 1 consecutive pixels withing threshold for feet calcs
					if ( ipval >= mmin && ipval<= mmax )
						inrange0=TRUE;


					if(inrange0) {
						// found it
						found_foot_right=TRUE;
						x->foot_right[0]=j;
						x->foot_right[1]=i;
						goto footr_exit;
					}
	//				ip++;
				}
		}
		footr_exit:
		;

			if(!found_foot_right) { 
				x->foot_right[0] = x->foot_right[1] = -1;
			} 			
			// end foot_left
	
		 }

		} else if (inrange2==FALSE) {
//			post("		//no second pass inside box. inrange is FALSE. setting box=-1");

//				previously set befor loop
//			x->box_coords[0]=x->box_coords[1]=x->box_coords[2]=x->box_coords[3]=-1;
			x->arm_left[0]=x->arm_left[1]=-1;
			x->arm_right[0]=x->arm_right[1]=-1;
			x->head[0]=x->head[1]=-1;
			x->foot_left[0]=x->foot_left[1]=-1;
			x->foot_right[0]=x->foot_right[1]=-1;

//			post("		//no second pass inside box. setting box %ld %ld %ld %ld   %ld %ld   %ld %ld",		\
//			 x->box_coords[0], x->box_coords[1], x->box_coords[2], x->box_coords[3],						\
//			 x->arm_left[0],x->arm_left[1],																	\
//			 x->arm_right[0],x->arm_right[1]	);
		
		}

		
		break;
	default:
		for	(i=0;i<dim[dimcount-1];i++) {
			ip = bip + i*in_minfo->dimstride[dimcount-1];
			jit_human_calculate_ndim(x,dimcount-1,dim,planecount,in_minfo,ip);
		}
	}
}

t_jit_human*jit_human_new(void)
{
	t_jit_human*x;
	short i;
		
	if (x=(t_jit_human*)jit_object_alloc(_jit_human_class)) {

		for(i=0;i<4;i++)
			x->box_coords[i]=-1;
		for(i=0;i<2;i++)
			x->arm_left[i]=x->arm_right[i]=x->head[i]=x->foot_left[i]=x->foot_right[i]=-1;

		x->min = 0.5;
		x->max = 1.;
		
		x->state=0;
		
		x->exe[0] = x->exe[1] = x->exe[2] = x->exe[3] = x->exe[4] = 1;


		
	} else {
		x = NULL;
	}	
	return x;
}

void jit_human_free(t_jit_human*x)
{
	//nada
}


