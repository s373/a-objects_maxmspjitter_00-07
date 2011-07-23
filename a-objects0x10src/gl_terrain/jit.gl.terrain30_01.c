// jit.gl.terrain.c
// draw 3d terrains. 
// from the gl.gridshape in the jitter sdk
// author: jkc
// © 2002 cycling '74

// 20030920 - vers 0x01
// 20031007 - vers 0x03
// this is simply a hack to build proper terrains under jitter
// not quite so simple. a very hard port! i'm still using limited 64 max settings.
//later, get dynamical spawning of the global vector posit, vert, vertnorms, vertnormsegs and texts
// author: andré sier
// © André Sier 2003 sier@alunos.fcsh.unl.pt
 
 
// 20031023 - vers 0x04
//first to use terrains
 //IMPORTANT: CHANGED from sqrt call to  jit_math_sqrt
 // see norm call
 
 /*
from jit common, the input float array methods 
 
long jit_attr_getfloat_array(void *x, t_symbol *s, long max, float *vals);
t_jit_err jit_attr_setfloat_array(void *x, t_symbol *s, long count, float *vals);

 
 */
 /*
// 20031024 - vers 0x05
	removed called calcops post
	added triggers to calc and draw norms
 
// 20031025 - vers 0x07
	adding shifting methods


// 20031201 - vers 0x08
	changing terrain copying routines into circular buffers, thanks to jasch!!

	no entanto, para calcular os vertnorms, etc, preciso sempre dos mecanismos de leitura
	q sao usados na funcao calc_terrain, que usa o wrapping para ler o terreno duma array
	com o dobro do tamanho





// 20040502 -- arranjado bug importante nos terrenos; ainda falta implementar o buffer circular

 */
 
 
 
 
 
 
 /*
 by the way, what does mop and ob3d stand for?

mop = "Matrix Operator"
ob3d = "Object 3D"


what do i need to put a matrix method inside a jit.gl.gridshape kind of object?


All you need to do is add the following to your class definition:

jit_class_addmethod(_jit_gl_foo_class, (method)jit_gl_foo_jit_matrix, "jit_matrix",A_USURP_LOW, 0);	

Then define your jit_matrix method something like the following

void jit_gl_foo_jit_matrix(t_jit_gl_foo *x, t_symbol *s, int argc, t_atom *argv)
{
	void *matrix;
	t_jit_matrix_info info;
	t_symbol *matrix_name=jit_atom_getsym(argv);
	char *mdata=NULL;
	long savelock;
	
	matrix = jit_object_findregistered(matrix_name);
	if (!matrix)
		return; // not a registered matrix name
	
	jit_object_method(matrix, _jit_sym_getinfo, &info);
	savelock = (long) jit_object_method(matrix,_jit_sym_lock,1);
	jit_object_method(matrix,_jit_sym_getdata,&mdata);
		
	//do something with the matrix	
		
	jit_object_method(matrix,_jit_sym_lock,savelock);					
}	


are mop and ob3d compatible?

Certain aspects are, but the object argument conventions are not compatible. If you really need functionality exposed by the mop implementation and ob3d implementation in one object, then I can try to code up an example, but from what you describe, it doesn't sound like you will need more than the above (we haven't needed it for jit.gl.render/videoplane/text2d/text3d).


-Joshua


 
 
 
 
 
 
 
 
 */
 
 
 
 

#include "jit.common.h"
#include "jit.gl.h"
#include "ext_strings.h"
#include <math.h>
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif


#define MAXGRID	256	//lets be safe


typedef struct _jit_gl_terrain 
{
	t_object			ob;
	void				*ob3d;
	t_jit_glchunk		*chunk;
	long				recalc;
	t_symbol			*shape;
	long				dim[2];
	char				gridmode;
	char				displaylist;
	GLuint				dlref; 
	float				pos[MAXGRID]; // a max of 64 args for the list
	char				draw_norms;
	char				calc_norms;
	char				shift;
	char				dir;
	short			ind;
	Boolean			om, getindice;

	float				coords[2]; // the point to get coords from

	long				tail;
	long				tend;

	// -- pointers to required data allocation
	
	float		*posit;
	float		*vertnorms;
	float		*texcoords;
	float		*facenorms;


	float		head_xyz[3];
	float		scale[3];
	float		position[3];

	long			maxdim[2]; // store the max grid for this object
} t_jit_gl_terrain;


void *_jit_gl_terrain_class;

t_jit_err jit_gl_terrain_init(void) ;
t_jit_gl_terrain *jit_gl_terrain_new(t_symbol * dest_name);
void jit_gl_terrain_free(t_jit_gl_terrain *x);

t_jit_err jit_gl_terrain_shape(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terrain_dim(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terrain_displaylist(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terrain_draw(t_jit_gl_terrain *x);
t_jit_err jit_gl_terrain_dest_changed(t_jit_gl_terrain *x);
void jit_gl_terrain_get_color(t_jit_gl_terrain *x, float *red, float *green, float *blue, float *alpha);

t_jit_err jit_gl_terrain_recalc(t_jit_gl_terrain *x);
t_jit_err jit_gl_terrain_dest_closing(t_jit_gl_terrain *x);

//func to acess float list
t_jit_err jit_gl_terrain_pos(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);

t_jit_err jit_gl_terrain_om(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);

t_jit_err jit_gl_terrain_shift(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terrain_coords(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);

// matrix funcs
//void jit_gl_terrain_jit_matrix(t_jit_gl_terrain *x, t_symbol *s, int argc, t_atom *argv);
//void jit_gl_terrain_calculate_ndim(long dimcount, long *dim, long planecount,t_jit_matrix_info *info, char *mdata);

//t_jit_err jit_gl_terrain_maxdim(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv)

t_jit_err jit_gl_terrain_maxdim(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terrain_head_xyz (t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);

/*
t_jit_err jit_gl_terrain_matrix_calc(t_jit_gl_terrain *x, void *inputs, void *outputs);
void jit_gl_terrain_calculate_ndim(long dimcount, long *dim, long planecount, void *vecdata, t_jit_matrix_info *in1_minfo, char *bip1, 
	t_jit_matrix_info *in2_minfo, char *bip2, t_jit_matrix_info *out_minfo, char *bop);
void jit_gl_terrain_vector_char(long n, void *vecdata, t_jit_op_info *in1, t_jit_op_info *in2, t_jit_op_info *out) ;
void jit_gl_terrain_vector_char_4plane(long n, void *vecdata, t_jit_op_info *in1, t_jit_op_info *in2, t_jit_op_info *out) ;
*/

// the calc terrain funcs 
void calc_terrain(t_jit_gl_terrain *x);

// the terrain funcs 
void calc_getFaceNorms(t_jit_gl_terrain *x);
void calc_getVertNorms(t_jit_gl_terrain *x);
void calc_getFaceNormSegs(t_jit_gl_terrain *x);
void calc_getTexCoords(t_jit_gl_terrain *x);
void calc_drawFaceNorms(t_jit_gl_terrain *x);

void calc_terrain_ops(t_jit_gl_terrain *x);

/* old style global code
//global carve stuff
float	    temp[2*MAXGRID][MAXGRID];			        //double position circular buffer
float	    posit[MAXGRID][MAXGRID];			        //position
float	    vertNorms[MAXGRID][MAXGRID][3];				//normals
float	    faceNorms[2][MAXGRID][MAXGRID][3];  		//facenormals
float	    faceNormSegs[2][2][MAXGRID][MAXGRID][3];	//facenormal segments
float 		texCoords[MAXGRID][MAXGRID][2];				//texture coordinates

float		pos[MAXGRID][MAXGRID];
*/

void color_surface(t_jit_gl_terrain *x);

void draw_grid(t_jit_object *matrix, GLenum mode);

t_symbol *ps_terrain;

//utils
long wrap(long in, long min, long max);
t_jit_err getindice(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv);

//vector 
void copy(float vec0[3], float vec1[3]);
void sub(float vec0[3], float vec1[3], float vec2[3]);
void add(float vec0[3], float vec1[3], float vec2[3]);
void scalDiv(float vec[3], float c);
void cross(float vec0[3], float vec1[3], float vec2[3]);
void norm(float vec[3]);
void norm_good(float vec[3]);
void set(float vec[3], float x, float y, float z);
void cheap_norm(float vec[3]) ;


// --------------------------------------------------------------------------------
//

t_jit_err jit_gl_terrain_init(void) 
{
	long attrflags=0;
	long ob3d_flags=0;
	t_jit_object *attr;
	void * ob3d;

	//try to add matrix inside
		t_atom a;
	void *o,*mop;

	_jit_gl_terrain_class = jit_class_new("jit_gl_terrain", (method)jit_gl_terrain_new, (method)jit_gl_terrain_free,
		sizeof(t_jit_gl_terrain),A_CANT,0L); //A_CANT = untyped
	
	// set up object extension for 3d object, customized with flags
	ob3d = jit_ob3d_setup(_jit_gl_terrain_class, calcoffset(t_jit_gl_terrain, ob3d), ob3d_flags);

/*
	//add mop
	mop = jit_object_new(_jit_sym_jit_mop,2,1);
	jit_mop_single_type(mop,_jit_sym_char);	
	o = jit_object_method(mop,_jit_sym_getinput,2);
	jit_attr_setlong(o,_jit_sym_dimlink,0);
	jit_atom_setlong(&a,256);
	jit_object_method(o,_jit_sym_mindim,1,&a);
	jit_object_method(o,_jit_sym_maxdim,1,&a);
	jit_class_addadornment(_jit_gl_terrain_class,mop);

*/

	// add attributes
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;

	// jkc email method
//	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_gl_terrain_jit_matrix, "jit_matrix",A_USURP_LOW, 0);	



	// shape
	attr = jit_object_new(_jit_sym_jit_attr_offset,"shape",_jit_sym_symbol,attrflags,
		(method)0L,(method)jit_gl_terrain_shape,calcoffset(t_jit_gl_terrain, shape));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"dim",_jit_sym_long,2,attrflags,
		(method)0L,(method)jit_gl_terrain_dim,0/*fix*/,calcoffset(t_jit_gl_terrain,dim));
	jit_attr_addfilterset_clip(attr,2,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terrain_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"displaylist",_jit_sym_char,attrflags,
		(method)0L,(method)jit_gl_terrain_displaylist,calcoffset(t_jit_gl_terrain, displaylist));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"gridmode",_jit_sym_char,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_terrain, gridmode));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	


	attr = jit_object_new(_jit_sym_jit_attr_offset,"calc_norms",_jit_sym_char,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_terrain, calc_norms));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	

	//doesnt work right now. cant call gl ops from draw_grid
	attr = jit_object_new(_jit_sym_jit_attr_offset,"draw_norms",_jit_sym_char,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_terrain, draw_norms));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	

	attr = jit_object_new(_jit_sym_jit_attr_offset,"shift",_jit_sym_char,attrflags,
		(method)0L,(method)jit_gl_terrain_shift,calcoffset(t_jit_gl_terrain, shift));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	

	attr = jit_object_new(_jit_sym_jit_attr_offset,"dir",_jit_sym_char,attrflags,
		(method)0L,(method)jit_gl_terrain_shift,calcoffset(t_jit_gl_terrain, shift));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	

//	post("before attrib_jit_sym_float32\, MAXGRID\, attrflags\,");
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"pos",_jit_sym_float32, MAXGRID, attrflags,		//add 64floats to pos
		(method)0L,(method)jit_gl_terrain_pos,0/*fix*/,calcoffset(t_jit_gl_terrain, pos));	
//	jit_attr_addfilterset_clip(attr,MAXGRID,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terrain_class,attr);	
//	post("after attrib_jit_sym_float32\, MAXGRID\, attrflags\,");


	attr = jit_object_new(_jit_sym_jit_attr_offset,"om",_jit_sym_char,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_terrain, om));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	

	attr = jit_object_new(_jit_sym_jit_attr_offset,"getindice",0L,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_terrain, getindice));	
	jit_class_addattr(_jit_gl_terrain_class,attr);	


	//added 27.4.2004
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"coords",_jit_sym_float32,2,attrflags,
		(method)0L,(method)jit_gl_terrain_coords,0/*fix*/,calcoffset(t_jit_gl_terrain,coords));
	jit_class_addattr(_jit_gl_terrain_class,attr);	



	//heaedxyz method from gl_ruttetra; get to find which y we want
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"head_xyz",_jit_sym_float32,3,attrflags,
		(method)0L,(method)jit_gl_terrain_head_xyz,0/*fix*/,calcoffset(t_jit_gl_terrain,head_xyz));
//s	jit_attr_addfilterset_clip(attr,2,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terrain_class,attr);	
	
	
		//max gldim
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"maxdim",_jit_sym_long,2,attrflags,
		(method)0L,(method)jit_gl_terrain_maxdim,0/*fix*/,calcoffset(t_jit_gl_terrain,maxdim));
	jit_class_addattr(_jit_gl_terrain_class,attr);	

	// handle draw method.  called in automatic mode by jit.gl.render, or otherwise through ob3d when banged.
	// this is A_CANT because draw setup needs to happen in the ob3d beforehand. 
	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_gl_terrain_draw, "ob3d_draw", A_CANT, 0L);
	
	// handle dest_changed method.
	// this method is called by jit.render when the destination context changes: for example,
	// when the user moves the window from one monitor to another.  Anything your object keeps
	// in the OpenGL machine -- textures, display lists, vertex shaders, etc. -- will need to
	// be rebuilt here. 
	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_gl_terrain_dest_changed, "dest_changed", A_CANT, 0L);


	
	// must register for ob3d	
	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_object_register, 			"register",		A_CANT, 0L);
	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_gl_terrain_dest_closing, 	"dest_closing",	A_CANT, 0L);


	//add matrix methods
//	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_gl_terrain_matrix_calc, 		"matrix_calc", 		A_CANT, 0L);


	// jkc fix
//	jit_class_addmethod(_jit_gl_terrain_class, (method)jit_gl_terrain_jit_matrix, "jit_matrix",A_USURP_LOW, 0);	



	jit_class_register(_jit_gl_terrain_class);

	ps_terrain 			= gensym("terrain");

	return JIT_ERR_NONE;
}


t_jit_gl_terrain *jit_gl_terrain_new(t_symbol * dest_name)
{
	t_jit_gl_terrain *x;
	long i, j;
	float *vertnorms, *posit, *facenorms,*texcoords;

	// make jit object
	if (x = (t_jit_gl_terrain *)jit_object_alloc(_jit_gl_terrain_class)) 
	{
		// create and attach ob3d
		jit_ob3d_new(x, dest_name);
		
		// set instance variable defaults
		x->shape = ps_terrain;
		x->dim[0] = 20;
		x->dim[1] = 20;	
		x->chunk = jit_glchunk_grid_new(_jit_sym_gl_quad_grid, 12, x->dim[0], x->dim[1]);
		x->recalc = 1;
		x->displaylist = 0;
		x->dlref = 0;
		x->gridmode = 0;

		x->ind = 0;
		x->om = 0;

		x->coords[0] = 0.1;
		x->coords[1] = 0.1;	
	
		x->calc_norms = 1;
		x->draw_norms=0;
		
		x->tail = MAXGRID;

		for (i=0; i<MAXGRID; i++) {
			x->pos[i] = 0.0f;

//			for (j=0; j<MAXGRID; j++) {
//				temp[i][j] = 0.0f;
//				temp[i+MAXGRID][j] = 0.0f;
//			}
		}
		
		
		// get mem code here
		x->maxdim[0] = MAXGRID;
		x->maxdim[1] = MAXGRID;	
		
//		post("init vars done");


		// alloc memory here
		
		x->posit= sysmem_newptr(x->maxdim[0]*x->maxdim[1]*sizeof(float));
		if(x->posit) { 
//			post("alloced posit %ld * %ld * %ld \(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(double),x->maxdim[0]*x->maxdim[1]*sizeof(double));   
		} else {
			error("couldn't build x->posit");
		}
		
		x->vertnorms= sysmem_newptr(x->maxdim[0]*x->maxdim[1]* 3 *sizeof(float));
		if(x->vertnorms) { 
//			post("alloced vertnorms %ld * %ld * %ld * 3 *\(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(double),x->maxdim[0]*x->maxdim[1]*3*sizeof(double));   
		} else {
			error("couldn't build x->vertnorms");
		}
		
		x->texcoords= sysmem_newptr(x->maxdim[0]*x->maxdim[1]* 2 *sizeof(float));
		if(x->texcoords) { 
//			post("alloced texcoords %ld * %ld * %ld * 3 *\(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(double),x->maxdim[0]*x->maxdim[1]*3*sizeof(double));   
		} else {
			error("couldn't build x->texcoords");
		}


		
		x->facenorms= sysmem_newptr(2 * x->maxdim[0]*x->maxdim[1]* 3 *sizeof(float));
		if(x->facenorms) { 
//			post("alloced vertnorms %ld * %ld * %ld * 3 *\(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(double),x->maxdim[0]*x->maxdim[1]*3*sizeof(double));   
		} else {
			error("couldn't build x->facenorms");
		}
		
		
//		post("igniting alloced memory");


		vertnorms=x->vertnorms;
		posit=x->posit;
		facenorms=x->facenorms;
		texcoords=x->texcoords;

		
		//memset(x->vertnorms, 0, x->maxdim[0]*x->maxdim[1]*sizeof(double));

		
		for (i=0; i<MAXGRID; i++) {
			for (j=0; j<MAXGRID; j++) {

				*posit=0.0f;
				posit++;
				
				//(double)((*posit+0))=0.0f; posit++;

				*vertnorms=0.0f; vertnorms++;
				*vertnorms=0.0f; vertnorms++;
				*vertnorms=0.0f; vertnorms++;

				*texcoords=0.0f; texcoords++;
				*texcoords=0.0f; texcoords++;

				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;


/*
				x->vertnorms[i][j][1]=0.0f;
				x->vertnorms[i][j][2]=0.0f;

				x->texcoords[i][j][0]=0.0f;
				x->texcoords[i][j][1]=0.0f;

				x->facenorms[0][i][j][0]=0.0f;
				x->facenorms[0][i][j][1]=0.0f;
				x->facenorms[0][i][j][2]=0.0f;
				x->facenorms[1][i][j][0]=0.0f;
				x->facenorms[1][i][j][1]=0.0f;
				x->facenorms[1][i][j][2]=0.0f;
*/

			}
		}
	} 
	else 
	{
		x = NULL;
	}	
	return x;
}

t_jit_err jit_gl_terrain_shape(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->shape = jit_atom_getsym(argv);
		x->recalc = 1;		
	}
	
	return JIT_ERR_NONE;
}


t_jit_err jit_gl_terrain_dim(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv&&x->chunk&&x->chunk->m_vertex) {
		t_jit_matrix_info info;

		jit_object_method(x->chunk->m_vertex,_jit_sym_dim, argc, argv);
		jit_object_method(x->chunk->m_vertex,_jit_sym_getinfo,&info);
		x->dim[0] = info.dim[0];
		x->dim[1] = info.dim[1];
		x->recalc = 1;	
	
//		CLIP(x->dim[0], 0, x->maxdim[0]);
//		CLIP(x->dim[1], 0, x->maxdim[1]);


		//important to		
			calc_getTexCoords(x);
		// when dimensions change	


	}

	return JIT_ERR_NONE;
	
}


t_jit_err jit_gl_terrain_maxdim(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{

	long	temp[2], i,j;
	float *vertnorms, *posit, *facenorms,*texcoords;


	if (argc&&argv) {
		temp[0] = jit_atom_getlong(argv+0);
		temp[1] = jit_atom_getlong(argv+1);
	}



		x->maxdim[0] = CLIP(temp[0], 256, 65535);
		x->maxdim[1] = CLIP(temp[1], 256, 65535);
		
		// replace with resize pointer!! important! and defer mem calling
		// alloc memory here
		
		if (x->posit) {
 			sysmem_freeptr(x->posit);
			post("just free 'd posit");
		}
		
		
		x->posit= sysmem_newptr(x->maxdim[0]*x->maxdim[1]*sizeof(float));
		if(x->posit) { 
			post("alloced posit %ld * %ld * %ld \(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*sizeof(float));   
		} else {
			error("couldn't build x->posit");
		}
		

		if (x->vertnorms) {
 			sysmem_freeptr(x->vertnorms);
			post("just free 'd vertnorms");
		}


		x->vertnorms= sysmem_newptr(x->maxdim[0]*x->maxdim[1]* 3 *sizeof(float));
		if(x->vertnorms) { 
			post("alloced vertnorms %ld * %ld * %ld * 3 *\(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*3*sizeof(float));   
		} else {
			error("couldn't build x->vertnorms");
		}
		


		if (x->texcoords) {
 			sysmem_freeptr(x->texcoords);
			post("just free 'd texcoords");
		}


		x->texcoords= sysmem_newptr(x->maxdim[0]*x->maxdim[1]* 2 *sizeof(float));
		if(x->texcoords) { 
			post("alloced vertnorms %ld * %ld * %ld * 3 *\(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*3*sizeof(float));   
		} else {
			error("couldn't build x->texcoords");
		}
		
		if (x->facenorms) {
 			sysmem_freeptr(x->facenorms);
			post("just free 'd facenorms");
		}
	
		
		x->facenorms= sysmem_newptr(2 * x->maxdim[0]*x->maxdim[1]* 3 *sizeof(float));
		if(x->facenorms) { 
			post("alloced facenorms 2*  %ld * %ld * %ld * 3 *\(sizeof\(double\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*3*sizeof(float));   
		} else {
			error("couldn't build x->facenorms");
		}
		
		
		post("igniting alloced memory");


		vertnorms=x->vertnorms;
		posit=x->posit;
		facenorms=x->facenorms;
		texcoords=x->texcoords;

		
		for (i=0; i<MAXGRID; i++) {
			for (j=0; j<MAXGRID; j++) {

				*posit=0.0f;
				posit++;
				
				//(double)((*posit+0))=0.0f; posit++;

				*vertnorms=0.0f; vertnorms++;
				*vertnorms=0.0f; vertnorms++;

				*texcoords=0.0f; texcoords++;
				*texcoords=0.0f; texcoords++;

				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;
				*facenorms=0.0f; facenorms++;



			}
		}
//		post("				init vars done:)");
	


	return JIT_ERR_NONE;


}

//my_pos func
t_jit_err jit_gl_terrain_pos(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	long j, i,k,tend = 0;
	float val = 0.0f;
	short ind = x->ind; // o indice a seguir
	


	if (argc&&argv) {
		for (j=0; j < argc; j++) {
			x->pos[j] = jit_atom_getfloat(argv+j);
		}

		calc_terrain_ops(x);

/*

		for (j=0; j < argc; j++) {
			posit[x->tail][j] = jit_atom_getfloat(argv+j);
		}

	
		x->tail -= 1;													// moving backwards
		if (x->tail < 0){
				x->tail = MAXGRID; 										// ensure writepoint goes back to  MAXGRID
				}




		if (!x->dir){																	
			for (i = 0; i<x->dim[1]; i++){ 								
				for(j=(x->dim[0]-1); j>=0; --j){
					tend = wrap (i + x->tail + 1, (long) 0, (long)(MAXGRID));   // simply  wrap the coords across the valid N*num_len range
					posit[i][j] = pos[tend][j];
				}
			}
		// tá a crashar...	claro, i estava a incrementar
		} else if (x->dir){
			for (i = x->dim[1]-1, k=0; i>=0; i--, k++){ 								
				for(j=(x->dim[0]-1); j>=0; --j){
					tend = wrap (i + x->tail + 1, (long) 0, (long)(MAXGRID));   // simply  wrap the coords across the valid N*num_len range
					posit[k][j] = pos[tend][j];
				}
			}


		}
*/


	x->recalc = 1;		

	}


		//resultado, estamos sempre a por a posicao no buffer, incrementando o indice,
		//que dará a posicao de escrita actual

//	for (j=0; j<x->dim[1]; j++)
//		temp[ind][j] = x->pos[j]; //cpy the chunk


	// depois de colocar o valor, aqui e que estamos a incrementar o indice

	x->ind+=1;
	if (x->ind>MAXGRID*2) x->ind = 0;	     // o indice ira no maximo a 2 vezes o tamanho maximo do vector
//	if (x->ind > (x->dim[0]*2)) x->ind = 0; // o indice ira no maximo a 2 vezes o tamanho do vector



/*
	
	//calc_fifo_terrain;
	for(i=x->dim[1]-1; i>=0; --i) // i é o comprimento
		for(j=x->dim[0]-1; j>=0; --j){ // j é a largura
			 posit[i+1][j] = posit[i][j];	//_copy	next
		}	


*/
	//the posit vectors were calculated now, calc the rest

	//calc_vert_norms(x);
	//this is the low version, so dont calc normals
//	if (x->calc_norms) {


//		calc_getFaceNorms(x);
//		calc_getVertNorms(x);
	
	return JIT_ERR_NONE;
}



t_jit_err jit_gl_terrain_shift(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	long i = 0;
	long j = 0;
	char val;
	
	if (argc&&argv) {
		for (i=0; i < argc; i++) 
			val=jit_atom_getlong(argv+i);
	}
	//changed directions to fit
	if (!val) { //shifting destra
		for(i=x->dim[1]-1; i>=0; --i)
		   for(j=x->dim[0]-1; j>=0; --j)
			 x->posit[(i+1)*x->dim[0]+j] = x->posit[(i)*x->dim[0]+j];	//shift 1 destra			

	} else if (val) { //shifting sinistra
		for(i=x->dim[1]-1; i>=0; --i)
		   for(j=x->dim[0]-1; j>=0; --j)
   			// posit[i+1][j+1] = posit[i][j];	//shift 1 sinistra			
			 x->posit[(i+1)*x->dim[0]+j+1] = x->posit[(i)*x->dim[0]+j];	//shift 1 destra			
	}

	x->recalc = 1;		//this should be enough to recalc
					//but it doesnt calc norms

	//high_version, so lets calc normals
	
		calc_getFaceNorms(x);
		calc_getVertNorms(x);

	
//	calc_terrain_ops(x);

}

t_jit_err jit_gl_terrain_coords(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{




}

t_jit_err jit_gl_terrain_displaylist(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->displaylist = jit_atom_getlong(argv);
		x->recalc = 1;		
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_gl_terrain_recalc(t_jit_gl_terrain *x)
{
	if (x->chunk&&x->chunk->m_vertex) {
		if (x->shape==ps_terrain) {
			calc_terrain(x);
		} else /*if (x->shape==ps_terrain2) */{  //whatever symbol, if unmatched before, do terrain2
			calc_terrain(x);
	  }
  	}	
	return JIT_ERR_NONE;
}

void jit_gl_terrain_free(t_jit_gl_terrain *x)
{
	
	if (x->chunk)
		jit_glchunk_delete(x->chunk);
	if (jit_ob3d_set_context(x)==JIT_ERR_NONE) {
		if (x->dlref)
			glDeleteLists(x->dlref,1);
	}
	jit_ob3d_free(x);
}

t_jit_err jit_gl_terrain_dest_closing(t_jit_gl_terrain *x)
{
	if (x->dlref) {
		glDeleteLists(x->dlref,1);
		x->dlref=NULL;
		x->recalc=1;
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_gl_terrain_draw(t_jit_gl_terrain *x)
{
	t_jit_err result = JIT_ERR_NONE;
	GLenum prim;
	
	prim = (x->gridmode) ? GL_TRIANGLE_STRIP : GL_QUAD_STRIP;
	
	if (x->recalc) {
		jit_gl_terrain_recalc(x);
		if (x->displaylist) {
			if (x->dlref) {
				glDeleteLists(x->dlref,1);
				x->dlref = 0;
			}
			if (x->dlref=glGenLists(1)) {
				glNewList(x->displaylist, GL_COMPILE);
				if (x->chunk&&x->chunk->m_vertex) 
					draw_grid(x->chunk->m_vertex, prim);
				glEndList();
			}
		}		
		x->recalc = 0;
	}
	
	// draw our chunk of OpenGL geometry. 
	if (x->chunk&&x->chunk->m_vertex) {
		if (!jit_attr_getlong(x,gensym("matrixoutput"))) {
			if (x->displaylist&&x->dlref)
				glCallList(x->dlref);
			else 
				draw_grid(x->chunk->m_vertex, prim);
		} else{
			color_surface(x);
			result = jit_ob3d_draw_chunk(x->ob3d, x->chunk); //output matrix
		}
	}	
	
	return result;
}

t_jit_err jit_gl_terrain_dest_changed(t_jit_gl_terrain *x)
{
	if (x->dlref) x->recalc=1;
	return JIT_ERR_NONE;
}

void jit_gl_terrain_get_color(t_jit_gl_terrain *x, float *red, float *green, float *blue, float *alpha)
{
	long ac=0;
	t_atom *av=NULL;
	
	jit_object_method(x,gensym("getcolor"),&ac,&av);
	if ((ac==4)&&av) {
		*red 	= jit_atom_getfloat(av);
		*green 	= jit_atom_getfloat(av+1);
		*blue 	= jit_atom_getfloat(av+2);
		*alpha 	= jit_atom_getfloat(av+3);
		jit_freebytes(av,ac*sizeof(t_atom));
	}
}






void jit_gl_terrain_jit_matrix(t_jit_gl_terrain *x, t_symbol *s, int argc, t_atom *argv)
{
	void *matrix;
	t_jit_matrix_info info;
	t_symbol *matrix_name=jit_atom_getsym(argv);
	char *mdata=NULL;
	long savelock, matrix_in;
	
	//missing here
	t_jit_err err=JIT_ERR_NONE;
	long i,dimcount,planecount,dim[JIT_MATRIX_MAX_DIMCOUNT];

	
	//data for the rest parts (calculate ndim)	
	//[...] never here! its a recursive function
	
	
	matrix = jit_object_findregistered(matrix_name);

	if (!matrix) 
		return;
	

	matrix_in = 0;


	savelock = (long) jit_object_method(matrix,_jit_sym_lock,1);

	jit_object_method(matrix, _jit_sym_getinfo, &info);
	jit_object_method(matrix,_jit_sym_getdata,&mdata);
		


	//do something with the matrix	

	if (!mdata) { err=JIT_ERR_GENERIC; goto out;}


	//compatible types?
	if ((info.type!=_jit_sym_char)) { 
		err=JIT_ERR_MISMATCH_TYPE; 
		goto out;
	}		

	//get dimensions/planecount
	dimcount   = info.dimcount;
	planecount = info.planecount;			





	// 1 PLANE

	if (info.planecount > 1) {
		err=JIT_ERR_MISMATCH_PLANE;
		goto out;

	}


	// if dim > 256x256 ERR
	if (info.dim[0]>256 || info.dim[1]>256) {
		err=JIT_ERR_MISMATCH_DIM;
		goto out;
	}
	



	for (i=0;i<dimcount;i++) {
		//if dimsize is 1, treat as infinite domain across that dimension.
		//otherwise truncate if less than the output dimsize
		dim[i] = info.dim[i];
	}


	jit_gl_terrain_calculate_ndim(dimcount, dim, planecount, &info, mdata);
//	matrix_in = 1;



		x->recalc = 1;		


/*

	if (dimcount<1) return; //safety


*/

/*	} else {
		return JIT_ERR_INVALID_PTR;
	}
*/

out:
	jit_object_method(matrix,_jit_sym_lock,savelock);
	
	

//	if(matrix_in)

	
					
}	




//	jit_gl_terrain_calculate_ndim(dimcount, dim, planecount, NULL, &info, mdata);

void jit_gl_terrain_calculate_ndim(long dimcount, long *dim, long planecount,t_jit_matrix_info *info, char *bip)
{

	long i,j,width,height;
	char *ip;
	float t;



	if (dimcount<1) return; //safety



	switch(dimcount) {
	case 1:
		dim[1]=1;
	case 2:
				
		width  = dim[0];
		height = dim[1];

			for (i=0;i<height;i++){
			
			ip = bip + i*info->dimstride[1];
			
				for (j=0;j<width;j++) {

// its a 1 planner..

						//*op++ = *ip; // alpha -- nothing here
				
						t = (float)*ip / 255.; //ranges 0.1
	//					x->posit[i*x->dim[0]+j] = t; // simply copy them to gl struct  //posit, not pos


/*						*op++ = *(ip+1);
						*op++ = *(ip+2);
						*op++ = *(ip+3);
*/						goto ick1;
				
					
					ick1:
					;
					ip+=4;

				}
			}	
	break;
	default:
		for	(i=0;i<dim[dimcount-1];i++) {
			ip = bip + i*info->dimstride[dimcount-1];
			jit_gl_terrain_calculate_ndim(dimcount-1,dim,planecount,info,ip); //dont need to pass object
		}
	}




}




/*

t_jit_err jit_gl_terrain_matrix_calc(t_jit_gl_terrain *x, void *inputs, void *outputs)
{
	t_jit_err err=JIT_ERR_NONE;
	long in1_savelock,in2_savelock,out_savelock;
	t_jit_matrix_info in1_minfo,in2_minfo,out_minfo;
	char *in1_bp,*in2_bp,*out_bp;
	long i,dimcount,planecount,dim[JIT_MATRIX_MAX_DIMCOUNT];
	void *in1_matrix,*in2_matrix,*out_matrix;
	
	in1_matrix 	= jit_object_method(inputs,_jit_sym_getindex,0);
	in2_matrix 	= jit_object_method(inputs,_jit_sym_getindex,1);
	out_matrix 	= jit_object_method(outputs,_jit_sym_getindex,0);

	if (x&&in1_matrix&&in2_matrix&&out_matrix) {
		in1_savelock = (long) jit_object_method(in1_matrix,_jit_sym_lock,1);
		in2_savelock = (long) jit_object_method(in2_matrix,_jit_sym_lock,1);
		out_savelock = (long) jit_object_method(out_matrix,_jit_sym_lock,1);
		
		jit_object_method(in1_matrix,_jit_sym_getinfo,&in1_minfo);
		jit_object_method(in2_matrix,_jit_sym_getinfo,&in2_minfo);
		jit_object_method(out_matrix,_jit_sym_getinfo,&out_minfo);
		
		jit_object_method(in1_matrix,_jit_sym_getdata,&in1_bp);
		jit_object_method(in2_matrix,_jit_sym_getdata,&in2_bp);
		jit_object_method(out_matrix,_jit_sym_getdata,&out_bp);
		
		if (!in1_bp) { err=JIT_ERR_INVALID_INPUT; goto out;}
		if (!in2_bp) { err=JIT_ERR_INVALID_INPUT; goto out;}
		if (!out_bp) { err=JIT_ERR_INVALID_OUTPUT; goto out;}
		
		//compatible types?
		if ((in1_minfo.type!=in2_minfo.type)||(in1_minfo.type!=out_minfo.type)) { 
			err=JIT_ERR_MISMATCH_TYPE; 
			goto out;
		}		

		//get dimensions/planecount
		dimcount   = out_minfo.dimcount;
		planecount = out_minfo.planecount;			

		if (in2_minfo.dim[0]<256) {
			err=JIT_ERR_MISMATCH_DIM;
		}
		
		for (i=0;i<dimcount;i++) {
			//if dimsize is 1, treat as infinite domain across that dimension.
			//otherwise truncate if less than the output dimsize
			dim[i] = out_minfo.dim[i];
			if ((in1_minfo.dim[i]<dim[i])&&in1_minfo.dim[i]>1) {
				dim[i] = in1_minfo.dim[i];
			}
		}
				
		jit_gl_terrain_calculate_ndim(dimcount, dim, planecount, NULL, &in1_minfo, in1_bp, &in2_minfo, in2_bp, &out_minfo, out_bp);
	} else {
		return JIT_ERR_INVALID_PTR;
	}
	
out:
	jit_object_method(out_matrix,gensym("lock"),out_savelock);
	jit_object_method(in2_matrix,gensym("lock"),in2_savelock);
	jit_object_method(in1_matrix,gensym("lock"),in1_savelock);
	return err;
}

void jit_gl_terrain_calculate_ndim(long dimcount, long *dim, long planecount, void *vecdata, t_jit_matrix_info *in1_minfo, char *bip1, 
	t_jit_matrix_info *in2_minfo, char *bip2, t_jit_matrix_info *out_minfo, char *bop)
{
	long i,j,n;
	char *ip1=bip1,*ip2=bip2,*op=bop;
	t_jit_op_info in1_opinfo,in2_opinfo,out_opinfo;
		
	if (dimcount<1) return; //safety
	
	switch(dimcount) {
	case 1:
		dim[1] = 1;
	case 2:
		//if planecount the same, flatten planes - treat as single plane data for speed
		n = dim[0];
		in1_opinfo.stride = in1_minfo->dim[0]>1?in1_minfo->planecount:0;
		in2_opinfo.stride = in2_minfo->dim[0]>1?in2_minfo->planecount:0;
		out_opinfo.stride = out_minfo->dim[0]>1?out_minfo->planecount:0;
		if (in1_minfo->type==_jit_sym_char) {
			if ((in1_opinfo.stride==4)&&(in2_opinfo.stride==4)&&(in2_opinfo.stride==4)) {
				in2_opinfo.p = bip2;	
				for (i=0;i<dim[1];i++){
					in1_opinfo.p = bip1 + i*in1_minfo->dimstride[1];
					out_opinfo.p = bop  + i*out_minfo->dimstride[1];
					jit_gl_terrain_vector_char_4plane(n,vecdata,&in1_opinfo,&in2_opinfo,&out_opinfo);
				}
			} else {			
				for (i=0;i<dim[1];i++){
					for (j=0;j<planecount;j++) {
						in1_opinfo.p = bip1 + i*in1_minfo->dimstride[1] + j%in1_minfo->planecount;
						in2_opinfo.p = bip2 + j%in2_minfo->planecount;
						out_opinfo.p = bop  + i*out_minfo->dimstride[1] + j%out_minfo->planecount;
						jit_gl_terrain_vector_char(n,vecdata,&in1_opinfo,&in2_opinfo,&out_opinfo);
					}
				}
			}
		}
		break;
	default:
		for	(i=0;i<dim[dimcount-1];i++) {
			ip1 = bip1 + i*in1_minfo->dimstride[dimcount-1];
			op  = bop  + i*out_minfo->dimstride[dimcount-1];
			jit_gl_terrain_calculate_ndim(dimcount-1,dim,planecount,vecdata,in1_minfo,ip1,in2_minfo,bip2,out_minfo,op);
		}
	}
}


void jit_gl_terrain_vector_char(long n, void *vecdata, t_jit_op_info *in1, t_jit_op_info *in2, t_jit_op_info *out) 
{
	uchar *ip1,*ip2,*op;
	long is1,is2,os;
		
	ip1 = ((uchar *)in1->p);
	ip2 = ((uchar *)in2->p);
	op  = ((uchar *)out->p);
	is1 = in1->stride; 
	is2 = in2->stride; 
	os  = out->stride; 
	
	++n;
	while (--n) {
		*op = ip2[(*ip1)*is2];
		op += os; ip1 += is1; 
	}
	
}

void jit_gl_terrain_vector_char_4plane(long n, void *vecdata, t_jit_op_info *in1, t_jit_op_info *in2, t_jit_op_info *out) 
{
	uchar *ip1,*ip2,*op;
	uchar *p0,*p1,*p2,*p3;
		
	ip1 = ((uchar *)in1->p);
	op  = ((uchar *)out->p);	
	p0  = ((uchar *)in2->p);
	p1  = p0 + 1;
	p2  = p0 + 2;
	p3  = p0 + 3;
		
	++n;--op;--ip1;
	while (--n) {
		*++op = p0[(*++ip1)*4];
		*++op = p1[(*++ip1)*4];
		*++op = p2[(*++ip1)*4];
		*++op = p3[(*++ip1)*4];
	}	
}

*/



void color_surface(t_jit_gl_terrain *x)
{
	float *p;
	char *bp=NULL;
	void *m;
	int i,j,rowstride;
	t_jit_matrix_info info;
	float red,green,blue,alpha;

	if (!x->chunk)
		return;

	if (!(m=x->chunk->m_vertex))
		return;
	
	jit_object_method(m,_jit_sym_getinfo,&info);
	jit_object_method(m,_jit_sym_getdata,&bp);
	
	if (!bp)
		return;
	
	rowstride = info.dimstride[1];
	
	jit_gl_terrain_get_color(x,&red,&green,&blue,&alpha);			
	
	p = (float *)bp;
	if ((p[8]==red)&&(p[9]==green)&&(p[10]==blue)&&(p[11]==alpha))			
		return;
	for (i=0;i<info.dim[1];i++) {
		p = (float *)(bp + i*rowstride);
		for (j=0;j<info.dim[0];j++) {   	
		    p += 8; 
			//color rgba
			*p++ = red;
			*p++ = green;
			*p++ = blue;
			*p++ = alpha;
			
		}
    }
}



t_jit_err getindice(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	int i,j,rowstride, k;
	short	indice;
	
	for (i=0, k = x->ind; i<=x->dim[0] ; i++, k--) {
		indice = wrap (k, 0, (MAXGRID*2-1)); // aqui teremos sempre o indice que se deve ler da array
		post("    %ld", indice);
	}

	return JIT_ERR_NONE;


}

t_jit_err jit_gl_terrain_head_xyz(t_jit_gl_terrain *x, void *attr, long argc, t_atom *argv)
{
	
	double scale[3], position[3];
	double stridex,stridey;
//	void *o;
	long ac,i;

	float	pt0,pt1,pt2,pt3;
	long	cx,cz;
	float	dx,dz,val;
	float	*posit;


	if (argc&&argv) {
	
		posit = x->posit;
	
		x->head_xyz[0] = (double)jit_atom_getfloat(argv);
		x->head_xyz[1] = (double)jit_atom_getfloat(argv+1);
		x->head_xyz[2] = (double)jit_atom_getfloat(argv+2);


//		jit_object_method(x, gensym("getscale"), &scale);



		CLAMP(x->head_xyz[0],0.,1.);
		CLAMP(x->head_xyz[2],0.,1.);
		
		(x->head_xyz[0]<0.)?0.:(x->head_xyz[0]>1.)?1.:x->head_xyz[0];
		(x->head_xyz[2]<0.)?0.:(x->head_xyz[2]>1.)?1.:x->head_xyz[2];


		stridex=1.0/(double)x->dim[0];
		stridey=1.0/(double)x->dim[1];
		
		dx = stridex - ((long)stridex);
		dz = stridey - ((long)stridey);


		//this floors
		cx = x->head_xyz[0] * (float)x->dim[0];
		cz = x->head_xyz[2] * (float)x->dim[1];
		//&c[0][0][0]+[j]*[k]*i+[k]*j+k)
		val = (float)*(posit + x->dim[1]*cx + cz);


		x->head_xyz[1] = val;

//		dx = x->head_xyz[0] - 

			post("posit coords %ld %ld & val %f",cx,cz,val);
			post("dxdz %f %f",dx,dz);

		for(i=0;i<3;i++) {
			post("head_xyz %ld %f",i,x->head_xyz[i]);

//			post("scale %ld %f",i,scale[i]);
//			post("position %ld %f",i,position[i]);


		}


//		x->recalc = 1;		


	}
	
	return JIT_ERR_NONE;



}


void calc_terrain(t_jit_gl_terrain *x)
{

	// this is the plane calc method. fix it to call the terrain calcs
	// and put all the info into the float pointer *p
	
	int nummajor = x->dim[1]-1;
	int numminor = x->dim[0]-1;
	double majorstep = (2.0/(double)nummajor);
	double minorstep = (2.0/(double)numminor);
	double nummajor_inv = 1./(double)nummajor;
	double numminor_inv = 1./(double)numminor;
	float *p;
	char *bp=NULL;
	void *m;
	int i,j,rowstride, k;
	t_jit_matrix_info info;
	float red,green,blue,alpha;
	short	indice;
		
	if (!x->chunk)
		return;

	if (!(m=x->chunk->m_vertex))
		return;
	
	jit_object_method(m,_jit_sym_getinfo,&info);
	jit_object_method(m,_jit_sym_getdata,&bp);
	
	if (!bp)
		return;
	
	rowstride = info.dimstride[1];
	
	jit_gl_terrain_get_color(x,&red,&green,&blue,&alpha);
		

	//tem de ser aqui que leio so alguma parte do vector que tem o dobro do tamanho
	//ler sempre o tamanho dele, logo o eixo x; por agora, no maximo, e 64 elementos e no minimo
	//sera 2 ou qq outra coisa.
	

	//a posicao de escrita actual sera sempre o maximo do terreno, o horizonte
	//todas as restantes posições serão em decrescendo, para baixo da posição actual
	//wrapped à volta do mínimo e máximo do vector!...


	for (i=0, k = x->ind; i<=nummajor ; i++, k--) {
		double xx,xy;
		int a;
		xy = i*majorstep-1.;
		
		indice = wrap (k, 0, (MAXGRID*2-1)); // aqui teremos sempre o indice que se deve ler da array

		p = (float *)(bp + i*rowstride);
		
		for (j=0;j<=numminor;j++) {
			xx = 1.-j*minorstep;

			//vertex
			*p++ = xx; 
			*p++ = xy; 
//			if(!x->om)
         //				*p++ = temp[indice][j]; // z == posit[i][j]
//			else if (x->om) *p++ = posit[i][j]; // z == posit[i][j]
			
			//*p++ = posit[i][j]; // z == posit[i][j]
			a=i*nummajor+j;
			
			*p++ = x->posit[a];

			//texture
//			*p++ = j*numminor_inv; 
//			*p++ = i*nummajor_inv;
			*p++ = x->texcoords[a+0]; 
			*p++ = x->texcoords[a+1]; 

			//normals; no normals here
			*p++ = x->vertnorms[a+0]; 
			*p++ = x->vertnorms[a+1]; 
			*p++ = x->vertnorms[a+2]; 
//			*p++ = 0.; 
//			*p++ = 1.; 
//			*p++ = 0.; 
			//color rgba
			*p++ = red;
			*p++ = green;
			*p++ = blue;
			*p++ = alpha;
		}
	}
}



void calc_terrain_ops(t_jit_gl_terrain *x)
{
	long i, j;
	
//	post("calc_terrain_ops called");

	//cpy pos 2 posit[0][y]
	for (j=0; j<x->dim[1]; j++)
		x->posit[0+j] = x->pos[j];
	
	//calc_fifo_terrain;
	for(i=x->dim[0]-1; i>=0; --i)
		for(j=x->dim[1]-1; j>=0; --j){
			 x->posit[(i+1)*x->dim[0]+j] = x->posit[i*x->dim[0]+j];	//_copy	next
		}	

	//the posit vectors were calculated now, calc the rest

	//calc_vert_norms(x);
	//this is the low version, so dont calc normals
//	if (x->calc_norms) {
		calc_getFaceNorms(x);
		calc_getVertNorms(x);
//	}

/*
	//draw_face_norms(x);
	if (x->draw_norms) {
		calc_getFaceNormSegs(x);
		calc_drawFaceNorms(x);
	}
*/	
	//calc_...(x);
}



/* face normals - for flat shading */
void calc_getFaceNorms(t_jit_gl_terrain *x)
{
	int i,j;
    float vec0[3], vec1[3], vec2[3], norm0[3], norm1[3];
    float geom0[3], geom1[3], geom2[3], geom3[3];

    for (i = 0; i < (x->dim[0]-2); ++i)
    {
        for (j = 0; j < (x->dim[1]-2); ++j)
        {
            /* get vectors from geometry points */
            geom0[0] = i; geom0[1] = j; geom0[2] = x->posit[i*x->dim[0]+j];
            geom1[0] = i; geom1[1] = j+1; geom1[2] = x->posit[i*x->dim[0]+j+1];
            geom2[0] = i+1; geom2[1] = j; geom2[2] = x->posit[(i+1)*x->dim[0]+j];
            geom3[0] = i+1; geom3[1] = j+1; geom3[2] = x->posit[(i+1)*x->dim[0]+j+1];

            sub( vec0, geom1, geom0 );
            sub( vec1, geom1, geom2 );
            sub( vec2, geom1, geom3 );

            /* get triangle face normals from vectors & normalize them */
            cross( norm0, vec0, vec1 );
            cheap_norm( norm0 );

            cross( norm1, vec1, vec2 ); 
            cheap_norm( norm1 );

 //           copy( x->facenorms[0+i*x->dim[0]+j], norm0 );
			x->facenorms[0+i*x->dim[0]+j+0] =  norm0[0];
			x->facenorms[0+i*x->dim[0]+j+1] =  norm0[1];
			x->facenorms[0+i*x->dim[0]+j+2] =  norm0[2]; 

 //           copy( x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j], norm1 );

			x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+0] =  norm1[0];
			x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+1] =  norm1[1];
			x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+2] =  norm1[2]; 


        }
    }

}


void calc_getVertNorms(t_jit_gl_terrain *x)
{
	int i,j;
    float avg[3];
    // (x->dim[0]-1) = max x
    // (x->dim[1]-1) = max y
    
    for (i = 0; i < (x->dim[0]-1); ++i)
    {
        for (j = 0; j < (x->dim[1]-1); ++j)
        {
            /* For each vertex, average normals from all faces sharing */
            /* vertex.  Check each quadrant in turn */
            set(avg, 0.0, 0.0, 0.0);

            /* Right & above */
            if (j < (x->dim[1]-2) && i < (x->dim[0]-2))
            {
 //               add( avg, avg, x->facenorms[0+i*x->dim[0]+j] );
               avg[0] += x->facenorms[0+i*x->dim[0]+j] ;
               avg[1] += x->facenorms[0+i*x->dim[0]+j+1] ;
               avg[2] += x->facenorms[0+i*x->dim[0]+j+2] ;
            }
            /* Right & below */
            if (j < (x->dim[1]-2) && i > 0)
            {
  //              add( avg, avg, x->facenorms[0+i*x->dim[0]+j]);//faceNorms[0][i-1][j] );
              avg[0] += x->facenorms[0+(i-1)*x->dim[0]+j] ;
               avg[1] += x->facenorms[0+(i-1)*x->dim[0]+j+1] ;
               avg[2] += x->facenorms[0+(i-1)*x->dim[0]+j+2] ;

  //              add( avg, avg,  x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j];//faceNorms[1][i-1][j] );
            
              avg[0] += x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j] ;
               avg[1] += x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+1] ;
               avg[2] += x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+2] ;
			
			}
            /* Left & above */
            if (j > 0 && i < (x->dim[0]-2))
            {
 //               add( avg, avg, x->facenorms[0+i*x->dim[0]+j-1]);//faceNorms[0][i][j-1] );
              avg[0] += x->facenorms[0+i*x->dim[0]+j-1] ;
               avg[1] += x->facenorms[0+i*x->dim[0]+j+0] ;
               avg[2] += x->facenorms[0+i*x->dim[0]+j+1] ;

  //              add( avg, avg, x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j-1]);///faceNorms[1][i][j-1] );
    
			   avg[0] += x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j-1] ;
               avg[1] += x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+0] ;
               avg[2] += x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+1] ;

					        }
            /* Left & below */
            if (j > 0 && i > 0)
            {
 //               add( avg, avg, x->facenorms[x->dim[0]*x->dim[1]+(i-1)*x->dim[0]+j-1];//faceNorms[1][i-1][j-1] );
   			   avg[0] += x->facenorms[x->dim[0]*x->dim[1]+(i-1)*x->dim[0]+j-1] ;
               avg[1] += x->facenorms[x->dim[0]*x->dim[1]+(i-1)*x->dim[0]+j+0] ;
               avg[2] += x->facenorms[x->dim[0]*x->dim[1]+(i-1)*x->dim[0]+j+1] ;
         }

            /* Normalize */
            cheap_norm( avg );
			
			x->vertnorms[i*x->dim[0]+j] = avg[0];
			x->vertnorms[i*x->dim[0]+j+1] = avg[1];
			x->vertnorms[i*x->dim[0]+j+2] = avg[2];
			
 //           copy( x->vertnorms[i*x->dim[0]+j], avg );
        }
    }

}


void calc_getFaceNormSegs(t_jit_gl_terrain *x)
{
	int i,j;
    float center0[3], center1[3], normSeg0[3], normSeg1[3];
    float geom0[3], geom1[3], geom2[3], geom3[3];
 
    for (i = 0; i < (x->dim[0]-2); ++i)
    {
        for (j = 0; j < (x->dim[1]-2); ++j)
        {
            geom0[0] = i; geom0[1] = j; geom0[2] =x->posit[i*x->dim[0]+j];
            geom1[0] = i; geom1[1] = j+1; geom1[2] = x->posit[i*x->dim[0]+j+1];
            geom2[0] = i+1; geom2[1] = j; geom2[2] = x->posit[(i+1)*x->dim[0]+j];
            geom3[0] = i+1; geom3[1] = j+1; geom3[2] = x->posit[(i+1)*x->dim[0]+j+1];

            /* find center of triangle face by averaging three vertices */
            add( center0, geom2, geom0 );
            add( center0, center0, geom1 );
            scalDiv( center0, 3.0 );

            add( center1, geom2, geom1 );
            add( center1, center1, geom3 );
            scalDiv( center1, 3.0 );

         //        add( avg, avg, x->facenorms[0+i*x->dim[0]+j-1]);//faceNorms[0][i][j-1] );
           //     add( avg, avg, x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j-1]);///faceNorms[1][i][j-1] );

			/* translate normal to center of triangle face to get normal segment */
//            add( normSeg0, center0, x->facenorms[0+i*x->dim[0]+j]);
			normSeg0[0] = center0[0] + x->facenorms[0+i*x->dim[0]+j];
			normSeg0[1] = center0[1] + x->facenorms[0+i*x->dim[0]+j+1];
			normSeg0[2] = center0[2] + x->facenorms[0+i*x->dim[0]+j+2];
			
			
 //           add( normSeg1, center1, x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j]);
			normSeg1[0] = center1[0] + x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j];
			normSeg1[1] = center1[1] + x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+1];
			normSeg1[2] = center1[2] + x->facenorms[x->dim[0]*x->dim[1]+i*x->dim[0]+j+2];

/*
            copy( faceNormSegs[0][0][i][j], center0 );
            copy( faceNormSegs[1][0][i][j], center1 );

            copy( faceNormSegs[0][1][i][j], normSeg0 );
            copy( faceNormSegs[1][1][i][j], normSeg1 );
  */
		      }
    }
}

void calc_getTexCoords(t_jit_gl_terrain *x)
{
	int i,j;

    for (i = 0; i < (x->dim[0]-1); ++i)
    {
        for (j = 0; j < (x->dim[1]-1); ++j)
        {
            x->texcoords[i*x->dim[0]+j+0] = (float)j/(float)(x->dim[0]-2);
             x->texcoords[i*x->dim[0]+j+1] = (float)i/(float)(x->dim[1]-2);
        }
    }
}

/*
void calc_drawFaceNorms(t_jit_gl_terrain *x)
{
    int i,j;
    glColor3f(1.0,1.0,1.0);
    for (i = 0; i < (x->dim[0]-1); ++i)
    {
        for (j = 0; j < (x->dim[1]-1); ++j)
        {
            glBegin(GL_LINES);
            glVertex3fv(faceNormSegs[0][0][i][j]);
            glVertex3fv(faceNormSegs[0][1][i][j]);
            glEnd();

            glBegin(GL_LINES);
            glVertex3fv(faceNormSegs[1][0][i][j]);
            glVertex3fv(faceNormSegs[1][1][i][j]);
            glEnd();
        }
    }
}

*/


//_____________________________________________________
// util

#define _I_X	0
#define _I_Y	1
#define _I_Z	2
#define _I_S	3
#define _I_T	4
#define _I_NX	5
#define _I_NY	6
#define _I_NZ	7
#define _I_R	8
#define _I_G	9
#define _I_B	10
#define _I_A	11


//transform a vector into music

//not drawing per vertex color in this object

void draw_grid(t_jit_object *matrix, GLenum mode)
{
	float *p,*p2;
	char *bp=NULL;
	int i,j,rowstride,width,height,planecount,up=0;
	t_jit_matrix_info info;

	if (!matrix) return;

	jit_object_method(matrix,_jit_sym_getinfo,&info);
	jit_object_method(matrix,_jit_sym_getdata,&bp);

	if (!bp) return;

	planecount	= info.planecount;
	rowstride 	= info.dimstride[1];
	height 		= info.dim[1]-1;
	width 		= info.dim[0];
		
	for (i=0;i<height;i++) {
		p = (float *)(bp + i*rowstride);
		p2 = (float *)(bp + (i+1)*rowstride);
		
		glBegin(mode);
		switch (planecount) {		
		case 16:
		case 15:
		case 14:
		case 13:
		case 12:
			for (j=0;j<width;j++) {
				
				glNormal3f(p[_I_NX],p[_I_NY],p[_I_NZ]);
//				glColor4f(p[_I_R],p[_I_G],p[_I_B],p[_I_A]);
				glTexCoord2f(p[_I_S],p[_I_T]);
				glVertex3f(p[_I_X],p[_I_Y],p[_I_Z]);

				glNormal3f(p2[_I_NX],p2[_I_NY],p2[_I_NZ]);
//				glColor4f(p2[_I_R],p2[_I_G],p2[_I_B],p2[_I_A]);
				glTexCoord2f(p2[_I_S],p2[_I_T]);
				glVertex3f(p2[_I_X],p2[_I_Y],p2[_I_Z]);
				
				p+=planecount;
				p2+=planecount;	    	
			}
			break;
		case 11:
		case 10:
		case 9:
		case 8:
			for (j=0;j<width;j++) {
			
				glNormal3f(p[_I_NX],p[_I_NY],p[_I_NZ]);
				glTexCoord2f(p[_I_S],p[_I_T]);
				glVertex3f(p[_I_X],p[_I_Y],p[_I_Z]);

				glNormal3f(p2[_I_NX],p2[_I_NY],p2[_I_NZ]);
				glTexCoord2f(p2[_I_S],p2[_I_T]);
				glVertex3f(p2[_I_X],p2[_I_Y],p2[_I_Z]);

				p+=planecount;
				p2+=planecount;	    	
			}
			break;
		case 7:
		case 6:
		case 5:
			for (j=0;j<width;j++) {
			
				glTexCoord2f(p[_I_S],p[_I_T]);
				glVertex3f(p[_I_X],p[_I_Y],p[_I_Z]);

				glTexCoord2f(p2[_I_S],p2[_I_T]);
				glVertex3f(p2[_I_X],p2[_I_Y],p2[_I_Z]);

				p+=planecount;
				p2+=planecount;	    	
			}
			break;
		case 4:
		case 3:
			for (j=0;j<width;j++) {
			
				glVertex3f(p[_I_X],p[_I_Y],p[_I_Z]);

				glVertex3f(p2[_I_X],p2[_I_Y],p2[_I_Z]);

				p+=planecount;
				p2+=planecount;	    	
			}
			break;
		}
		glEnd();
	}

	//draw_face_norms(x);
/*	if (x->draw_norms) {
		calc_getFaceNormSegs(x);
		calc_drawFaceNorms(x);
	}
*/


// draw face norms?

/*
//    if (x->drawFaceNorms)    
  //  {
	    glDisable(GL_TEXTURE_2D);
        calc_getFaceNormSegs(x);
        calc_drawFaceNorms(x);
    //}


*/
}



// math -- VECTOR3 ops LIB
void copy(float vec0[3], float vec1[3])
{
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];
}

void sub(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[0] - vec2[0];
    vec0[1] = vec1[1] - vec2[1];
    vec0[2] = vec1[2] - vec2[2];
}

void add(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[0] + vec2[0];
    vec0[1] = vec1[1] + vec2[1];
    vec0[2] = vec1[2] + vec2[2];
}

void scalDiv(float vec[3], float c)
{
    vec[0] /= c; vec[1] /= c; vec[2] /= c;
}

void cross(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    vec0[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    vec0[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

void norm(float vec[3]) //IMPORTANT: CHANGED from sqrt call to  jit_math_sqrt
{
    float c = jit_math_sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    scalDiv(vec, c); 
}

void cheap_norm(float vec[3]) 
{
    float c = (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    scalDiv(vec, c); 
}

void set(float vec[3], float x, float y, float z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}


long wrap(long in, long min, long max)
{
	if((in >= min) && (in <= max))
		return in;		
	else if(in < min)
		return((max + 1) - (min - in));
	else
		return((min - 1) + (in - max));	
}




/*----------------------------------------------------------
	EOF
----------------------------------------------------------*/




