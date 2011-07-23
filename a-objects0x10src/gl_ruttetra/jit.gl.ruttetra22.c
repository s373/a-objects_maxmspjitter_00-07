/*

	c.jit.gl.ruttetra
	
	ciels'lib first element. dyn memory allocation;
	
	sier 20041212



 */
 
 
  

#include "jit.common.h"
#include "jit.gl.h"
#include "ext_strings.h"
#include <math.h>
//#include <mem.c>
//#include <stdio.h>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif


/*
enum glmodes { 
	GL_POINTS = 0, 
	GL_LINES, 
	GL_LINE_STRIP, 
	GL_LINE_LOOP, 		
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP, 
	GL_TRIANGLE_FAN,		
	GL_QUADS, 
	GL_QUAD_STRIP, 
	GL_POLYGON
	};
*/

#define MAXGRID	512	//old style global calling; lets be safe


typedef struct _jit_gl_ruttetra 
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

	long			maxdim[2]; // store the max grid for this object
	
	float				zscale;
	
	// -- pointers to required data allocation
	
	float		*posit;
	float		*vertnorms;
	float		*texcoords;
	float		*facenorms;
	

	float		head_xyz[3];
	
	
	float		scale[3];
	float		position[3];
} t_jit_gl_ruttetra;


void *_jit_gl_ruttetra_class;

t_jit_err jit_gl_ruttetra_init(void) ;
t_jit_gl_ruttetra *jit_gl_ruttetra_new(t_symbol * dest_name);
void jit_gl_ruttetra_free(t_jit_gl_ruttetra *x);

t_jit_err jit_gl_ruttetra_shape(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_ruttetra_dim(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_ruttetra_displaylist(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_ruttetra_draw(t_jit_gl_ruttetra *x);
t_jit_err jit_gl_ruttetra_dest_changed(t_jit_gl_ruttetra *x);
void jit_gl_ruttetra_get_color(t_jit_gl_ruttetra *x, double *red, double *green, double *blue, double *alpha);

t_jit_err jit_gl_ruttetra_recalc(t_jit_gl_ruttetra *x);
t_jit_err jit_gl_ruttetra_dest_closing(t_jit_gl_ruttetra *x);

// matrix funcs
void jit_gl_ruttetra_jit_matrix(t_jit_gl_ruttetra *x, t_symbol *s, int argc, t_atom *argv);
void jit_gl_ruttetra_calculate_ndim(long dimcount, long *dim, long planecount,t_jit_matrix_info *info, \
									char *bip, float *posit, float zscsale);

//void jit_gl_ruttetra_interp(t_jit_gl_ruttetra *x, long *dim);

t_jit_err jit_gl_ruttetra_maxdim(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_ruttetra_print(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);

t_jit_err jit_gl_ruttetra_head_xyz(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);



void calc_terrain_2(t_jit_gl_ruttetra *x);


// the calc terrain funcs 
void calc_terrain(t_jit_gl_ruttetra *x);

// the terrain funcs 
void calc_getFaceNorms(t_jit_gl_ruttetra *x);
void calc_getVertNorms(t_jit_gl_ruttetra *x);
void calc_getFaceNormSegs(t_jit_gl_ruttetra *x);
void calc_getTexCoords(t_jit_gl_ruttetra *x);
void calc_drawFaceNorms(t_jit_gl_ruttetra *x);

void calc_terrain_ops(t_jit_gl_ruttetra *x);
void color_surface(t_jit_gl_ruttetra *x);

void draw_grid(t_jit_object *matrix, GLenum mode);

t_symbol *ps_etra;
t_symbol *ps_etra2;
t_symbol *ps_etra3;


//utils
short wrap(short in, short min, short max);

t_jit_err getindice(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv);

//vector 
void copy(float vec0[3], float vec1[3]);
void sub(float vec0[3], float vec1[3], float vec2[3]);
void add(float vec0[3], float vec1[3], float vec2[3]);
void scalDiv(float vec[3], float c);
void cross(float vec0[3], float vec1[3], float vec2[3]);
void norm(float vec[3]);
void set(float vec[3], float x, float y, float z);


// --------------------------------------------------------------------------------
//

t_jit_err jit_gl_ruttetra_init(void) 
{
	long attrflags=0;
	long ob3d_flags=0;
	t_jit_object *attr;
	void * ob3d;
	
	_jit_gl_ruttetra_class = jit_class_new("jit_gl_ruttetra", (method)jit_gl_ruttetra_new, (method)jit_gl_ruttetra_free,
		sizeof(t_jit_gl_ruttetra),A_CANT,0L); //A_CANT = untyped
	
	// set up object extension for 3d object, customized with flags
	ob3d = jit_ob3d_setup(_jit_gl_ruttetra_class, calcoffset(t_jit_gl_ruttetra, ob3d), ob3d_flags);

	// add attributes
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	// shape
	attr = jit_object_new(_jit_sym_jit_attr_offset,"shape",_jit_sym_symbol,attrflags,
		(method)0L,(method)jit_gl_ruttetra_shape,calcoffset(t_jit_gl_ruttetra, shape));	
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"dim",_jit_sym_long,2,attrflags,
		(method)0L,(method)jit_gl_ruttetra_dim,0/*fix*/,calcoffset(t_jit_gl_ruttetra,dim));
	jit_attr_addfilterset_clip(attr,2,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"displaylist",_jit_sym_char,attrflags,
		(method)0L,(method)jit_gl_ruttetra_displaylist,calcoffset(t_jit_gl_ruttetra, displaylist));	
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"gridmode",_jit_sym_char,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_ruttetra, gridmode));	
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	



	attr = jit_object_new(_jit_sym_jit_attr_offset,"zscale",_jit_sym_float32,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_ruttetra, zscale));	
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"head_xyz",_jit_sym_float32,3,attrflags,
		(method)0L,(method)jit_gl_ruttetra_head_xyz,0/*fix*/,calcoffset(t_jit_gl_ruttetra,head_xyz));
//s	jit_attr_addfilterset_clip(attr,2,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	



	//max gldim
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"maxdim",_jit_sym_long,2,attrflags,
		(method)0L,(method)jit_gl_ruttetra_maxdim,0/*fix*/,calcoffset(t_jit_gl_ruttetra,maxdim));
	jit_class_addattr(_jit_gl_ruttetra_class,attr);	


	// adding matrix method BEFORE ob3d
	// jkc fix
	jit_class_addmethod(_jit_gl_ruttetra_class, (method)jit_gl_ruttetra_jit_matrix, "jit_matrix",A_USURP_LOW, 0);	



	// handle draw method.  called in automatic mode by jit.gl.render, or otherwise through ob3d when banged.
	// this is A_CANT because draw setup needs to happen in the ob3d beforehand. 
	jit_class_addmethod(_jit_gl_ruttetra_class, (method)jit_gl_ruttetra_draw, "ob3d_draw", A_CANT, 0L);
	
	// handle dest_changed method.
	// this method is called by jit.render when the destination context changes: for example,
	// when the user moves the window from one monitor to another.  Anything your object keeps
	// in the OpenGL machine -- textures, display lists, vertex shaders, etc. -- will need to
	// be rebuilt here. 
	jit_class_addmethod(_jit_gl_ruttetra_class, (method)jit_gl_ruttetra_dest_changed, "dest_changed", A_CANT, 0L);
	
	// must register for ob3d	
	jit_class_addmethod(_jit_gl_ruttetra_class, (method)jit_object_register, 			"register",		A_CANT, 0L);
	jit_class_addmethod(_jit_gl_ruttetra_class, (method)jit_gl_ruttetra_dest_closing, 	"dest_closing",	A_CANT, 0L);

	jit_class_register(_jit_gl_ruttetra_class);

	ps_etra 			= gensym("quad");
	ps_etra2 			= gensym("line_strip");
	ps_etra3 			= gensym("tri");

	return JIT_ERR_NONE;
}


t_jit_gl_ruttetra *jit_gl_ruttetra_new(t_symbol * dest_name)
{
	t_jit_gl_ruttetra *x;
	long i, j;
	float *vertnorms, *posit, *facenorms,*texcoords;
	

	// make jit object
	if (x = (t_jit_gl_ruttetra *)jit_object_alloc(_jit_gl_ruttetra_class)) 
	{
		// create and attach ob3d
		jit_ob3d_new(x, dest_name);
		
		// set instance variable defaults
		x->shape = ps_etra;
		x->dim[0] = 20;
		x->dim[1] = 20;	
		x->chunk = jit_glchunk_grid_new(_jit_sym_gl_quad_grid, 12, x->dim[0], x->dim[1]);
		x->recalc = 1;
		x->displaylist = 0;
		x->dlref = 0;
		x->gridmode = 0;
		x->zscale = -1.0;

		x->head_xyz[0] = 0.;
		x->head_xyz[1] = 0.;
		x->head_xyz[2] = 0.;

	
		x->maxdim[0] = MAXGRID;
		x->maxdim[1] = MAXGRID;	
		
		
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
//		post("				init vars done:)");
	} 
	else 
	{
		x = NULL;
	}	
	return x;
}


t_jit_err jit_gl_ruttetra_maxdim(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv)
{

	long	temp[2], i,j;
	float *vertnorms, *posit, *facenorms,*texcoords;


	if (argc&&argv) {
		temp[0] = jit_atom_getlong(argv+0);
		temp[1] = jit_atom_getlong(argv+1);
	}



		x->maxdim[0] = CLIP(temp[0], 512, 65535);
		x->maxdim[1] = CLIP(temp[1], 512, 65535);
		
		// replace with resize pointer!! important! and defer mem calling
		// alloc memory here
		
		if (x->posit) {
 			sysmem_freeptr(x->posit);
			post("just free 'd posit");
		}
		
		
		x->posit= sysmem_newptr(x->maxdim[0]*x->maxdim[1]*sizeof(float));
		if(x->posit) { 
			post("alloced posit %ld * %ld * %ld \(sizeof\(float\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*sizeof(float));   
		} else {
			error("couldn't build x->posit");
		}
		

		if (x->vertnorms) {
 			sysmem_freeptr(x->vertnorms);
			post("just free 'd vertnorms");
		}


		x->vertnorms= sysmem_newptr(x->maxdim[0]*x->maxdim[1]* 3 *sizeof(float));
		if(x->vertnorms) { 
			post("alloced vertnorms %ld * %ld * %ld * 3 *\(sizeof\(float\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*3*sizeof(float));   
		} else {
			error("couldn't build x->vertnorms");
		}
		


		if (x->texcoords) {
 			sysmem_freeptr(x->texcoords);
			post("just free 'd texcoords");
		}


		x->texcoords= sysmem_newptr(x->maxdim[0]*x->maxdim[1]* 2 *sizeof(float));
		if(x->texcoords) { 
			post("alloced vertnorms %ld * %ld * %ld * 3 *\(sizeof\(float\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*3*sizeof(float));   
		} else {
			error("couldn't build x->texcoords");
		}
		
		if (x->facenorms) {
 			sysmem_freeptr(x->facenorms);
			post("just free 'd facenorms");
		}
	
		
		x->facenorms= sysmem_newptr(2 * x->maxdim[0]*x->maxdim[1]* 3 *sizeof(float));
		if(x->facenorms) { 
			post("alloced facenorms 2*  %ld * %ld * %ld * 3 *\(sizeof\(float\)\) = %ld bytes",x->maxdim[0], x->maxdim[1],sizeof(float),x->maxdim[0]*x->maxdim[1]*3*sizeof(float));   
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





t_jit_err jit_gl_ruttetra_shape(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->shape = jit_atom_getsym(argv);
		x->recalc = 1;		
	}
	
	return JIT_ERR_NONE;
}


t_jit_err jit_gl_ruttetra_dim(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv&&x->chunk&&x->chunk->m_vertex) {
		t_jit_matrix_info info;

		jit_object_method(x->chunk->m_vertex,_jit_sym_dim, argc, argv);
		jit_object_method(x->chunk->m_vertex,_jit_sym_getinfo,&info);
		x->dim[0] = info.dim[0];
		x->dim[1] = info.dim[1];

//		x->recalc = 1;	

		//important to		
//			calc_getTexCoords(x);
		// when dimensions change	



		calc_getFaceNorms(x);
		calc_getVertNorms(x);
		calc_getTexCoords(x);


	 x->recalc =1;


	}
	
	return JIT_ERR_NONE;
}


t_jit_err jit_gl_ruttetra_displaylist(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->displaylist = jit_atom_getlong(argv);
		x->recalc = 1;		
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_gl_ruttetra_recalc(t_jit_gl_ruttetra *x)
{
	if (x->chunk&&x->chunk->m_vertex) {
		if (x->shape==ps_etra) {
			calc_terrain(x);
		} /*else if /*(x->shape==ps_etra2) {  //whatever symbol, if unmatched before, do terrain2
			calc_terrain2(x);*/
	  
  	}	
	return JIT_ERR_NONE;
}



void jit_gl_ruttetra_free(t_jit_gl_ruttetra *x)
{
	

		if (x->facenorms) {
 			sysmem_freeptr(x->facenorms);
			post("just free 'd facenorms");
		}

		if (x->texcoords) {
 			sysmem_freeptr(x->texcoords);
			post("just free 'd texcoords");
		}

		if (x->vertnorms) {
 			sysmem_freeptr(x->vertnorms);
			post("just free 'd vertnormz");
		}

		if (x->posit) {
 			sysmem_freeptr(x->posit);
			post("just free 'd posit");
		}






	if (x->chunk)
		jit_glchunk_delete(x->chunk);
	if (jit_ob3d_set_context(x)==JIT_ERR_NONE) {
		if (x->dlref)
			glDeleteLists(x->dlref,1);
	}
	jit_ob3d_free(x);
}

t_jit_err jit_gl_ruttetra_dest_closing(t_jit_gl_ruttetra *x)
{
	if (x->dlref) {
		glDeleteLists(x->dlref,1);
		x->dlref=NULL;
		x->recalc=1;
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_gl_ruttetra_draw(t_jit_gl_ruttetra *x)
{
	t_jit_err result = JIT_ERR_NONE;
	GLenum prim;

	prim = (x->gridmode) ? GL_TRIANGLE_STRIP : GL_QUAD_STRIP;

	
	switch(x->gridmode) {
			case 0: prim = GL_POINTS; break;
			case 1: prim = GL_LINES; break;
			case 2: prim = GL_LINE_STRIP; break; 
			case 3: prim = GL_LINE_LOOP; break; 		
			case 4: prim = GL_TRIANGLES; break;
			case 5: prim = GL_TRIANGLE_STRIP; break; 
			case 6: prim = GL_TRIANGLE_FAN; break;		
			case 7: prim = GL_QUADS; break; 
			case 8: prim = GL_QUAD_STRIP; break; 
			case 9: prim = GL_POLYGON; break;
	}
	
	if (x->recalc) {
		jit_gl_ruttetra_recalc(x);
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

t_jit_err jit_gl_ruttetra_dest_changed(t_jit_gl_ruttetra *x)
{
	if (x->dlref) x->recalc=1;
	return JIT_ERR_NONE;
}

void jit_gl_ruttetra_get_color(t_jit_gl_ruttetra *x, double *red, double *green, double *blue, double *alpha)
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

void jit_gl_ruttetra_jit_matrix(t_jit_gl_ruttetra *x, t_symbol *s, int argc, t_atom *argv)
{
	void *matrix;
	t_jit_matrix_info info;
	t_symbol *matrix_name=jit_atom_getsym(argv);
	char *mdata=NULL;
	long savelock;
//	float *posit;
	float	zscale = x->zscale *  0.00392157 ; // / 255.; // simply copy them to gl struct	
	//missing here
	t_jit_err err=JIT_ERR_NONE;
	long i,dimcount,planecount,dim[JIT_MATRIX_MAX_DIMCOUNT];

	
	//data for the rest parts (calculate ndim)	
	//[...] never here! its a recursive function
	
	
	matrix = jit_object_findregistered(matrix_name);

	if (!matrix) 
		return;
	


	savelock = (long) jit_object_method(matrix,_jit_sym_lock,1);

	jit_object_method(matrix, _jit_sym_getinfo, &info);
	jit_object_method(matrix,_jit_sym_getdata,&mdata);
		


	//do something with the matrix	

	if (!mdata) { err=JIT_ERR_GENERIC; goto out;}


	//compatible types? // updated for double32 ; NO DIVISION per pixel boost

// _jit_sym_char, _jit_sym_float32
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

	

// 1 matrix, no need to find < dim



	//importante
	for (i=0;i<dimcount;i++) {
		//if dimsize is 1, treat as infinite domain across that dimension.
		//otherwise truncate if less than the output dimsize
		dim[i] = MIN(info.dim[i],info.dim[0]);
		}


//	posit=x->posit;


	jit_gl_ruttetra_calculate_ndim(dimcount, dim, planecount, &info, mdata, x->posit, zscale);




	//TEST THIS
	// PERHAPS THIS IS FUCKING THINGS, SINCE THE MATRIX ALREADY STORES THE DATA
	// in the required pointer..




		calc_getFaceNorms(x);
		calc_getVertNorms(x);
		calc_getTexCoords(x);

	 x->recalc =1;






/*

	if (dimcount<1) return; //safety


*/

/*	} else {
		return JIT_ERR_INVALID_PTR;
	}
*/

out:
	jit_object_method(matrix,_jit_sym_lock,savelock);					
}	




//	jit_gl_ruttetra_calculate_ndim(dimcount, dim, planecount, NULL, &info, mdata);

void jit_gl_ruttetra_calculate_ndim(long dimcount, long *dim, long planecount,t_jit_matrix_info *info, char *bip, float *posit, float zscale)
{

	long i,j,width,height;
	uchar *ip,ipval;
	float t;

//	double *posit;
//	float zscale = x->zscale * 0.00392157; // /255.

	if (dimcount<1) return; //safety


	

	switch(dimcount) {
	case 1:
		dim[1]=1;
	case 2:
				
		width  = dim[0];
		height = dim[1];

			for (i=0;i<height;i++){
			
			
				for (j=0;j<width;j++) {


						ip = (bip + i*info->dimstride[1] + j); //jitter
//						posit = (posit + i*info->dimstride[1] + j); // mine; point the posit pointer at coord instead of incrementing it;

						ipval = *ip;
						t = ((float)ipval * zscale);
						
					    *posit = t ; // / 255.; // simply copy them to gl struct
						posit++;

//						t = (double)((*ip+0)) /255.0f; // / 255.; // simply copy them to gl struct
						
						
//						*posit = (double)(((*ip+0)) * zscale * 0.00392157) ; // / 255.; // simply copy them to gl struct


					//	*posit = (double)(ABS((*ip+0)) * zscale * 0.00392157) ; // / 255.; // simply copy them to gl struct
						
//						*posit = (double)(ABS((*ip+0)) * zscale * 0.00392157) ; // / 255.; // simply copy them to gl struct
						
							
						//posit++;
						
						//posit+=1;
											
//						posit[i][j] = t; // copying to aux to interpolate from!

/*						*op++ = *(ip+1);
						*op++ = *(ip+2);
						*op++ = *(ip+3);
*/			//			goto ick1;
				
					
					ick1:
					;
					//ip+=resolution; // this changed from 4 to 1 and is looking more resolution
					ip+=1; // this changed from 4 to 1 and is looking more resolution
							// must be 4!...
							
							// ip must only advance one step, or else terrain gets repeated 

				}
			}	
	break;
	default:
		for	(i=0;i<dim[dimcount-1];i++) {
			ip = bip + i*info->dimstride[dimcount-1];
			jit_gl_ruttetra_calculate_ndim(dimcount-1,dim,planecount,info,ip, posit,zscale); //dont need to pass object
		}
	}
}






void color_surface(t_jit_gl_ruttetra *x)
{
	double *p;
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
	
	jit_gl_ruttetra_get_color(x,&red,&green,&blue,&alpha);			
	
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








void calc_terrain(t_jit_gl_ruttetra *x)
{
// importante: fixed calls to x and y!!


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
	double xx, xy;
		
	float *posit,*texcoords,*facenorms,*vertnorms;
	
	posit=x->posit;
	texcoords=x->texcoords;
	facenorms=x->facenorms;	
	vertnorms=x->vertnorms;
		
	if (!x->chunk)
		return;

	if (!(m=x->chunk->m_vertex))
		return;
	
	jit_object_method(m,_jit_sym_getinfo,&info);
	jit_object_method(m,_jit_sym_getdata,&bp);
	
	if (!bp)
		return;
	
	rowstride = info.dimstride[1];
	
	jit_gl_ruttetra_get_color(x,&red,&green,&blue,&alpha);
		

	//tem de ser aqui que leio so alguma parte do vector que tem o dobro do tamanho
	//ler sempre o tamanho dele, logo o eixo x; por agora, no maximo, e 64 elementos e no minimo
	//sera 2 ou qq outra coisa.
	

	//a posicao de escrita actual sera sempre o maximo do terreno, o horizonte
	//todas as restantes posições serão em decrescendo, para baixo da posição actual
	//wrapped à volta do mínimo e máximo do vector!...


	for (i=0; i<=nummajor ; i++) {
		//double x,y;
		xy = i*majorstep-1.;
		
//		indice = wrap (k, 0, (MAXGRID*2-1)); // aqui teremos sempre o indice que se deve ler da array

		p = (float *)(bp + i*rowstride);
		
		for (j=0;j<=numminor;j++) {
			xx = 1.-j*minorstep;

			//vertex
			*p++ = xx; 
			*p++ = xy;
			*p++ = (float)*posit++;
			//texture
			*p++ = (float)*texcoords++; 
			*p++ = (float)*texcoords++;

			//normals; no normals here
			*p++ = (float)*vertnorms++; 
			*p++ = (float)*vertnorms++; 
			*p++ = (float)*vertnorms++; 
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


/* face normals - for flat shading */
void calc_getFaceNorms(t_jit_gl_ruttetra *x)
{
	int i,j;
    float vec0[3], vec1[3], vec2[3], norm0[3], norm1[3];
    float geom0[3], geom1[3], geom2[3], geom3[3];
    float *posit,*facenorms;
    
    posit=x->posit;
    facenorms=x->facenorms;//facenorms is go at 0

    for (i = 0; i < (x->dim[0]-2); ++i)
    {
        for (j = 0; j < (x->dim[1]-2); ++j)
        {
   

            /* get vectors from geometry points */

/*            geom0[0] = i; geom0[1] = j; geom0[2] = x->posit[i][j];
            geom1[0] = i; geom1[1] = j+1; geom1[2] = x->posit[i][j+1];
            geom2[0] = i+1; geom2[1] = j; geom2[2] = x->posit[i+1][j];
            geom3[0] = i+1; geom3[1] = j+1; geom3[2] = x->posit[i+1][j+1];
*/

			// read values from pos pointer
			// c[i][j][k] == *(&c[0][0][0]+[j]*[k]*i+[k]*j+k)
           geom0[0] = i; geom0[1] = j; geom0[2] = 		(float)(*posit+x->dim[1]*i+j);
            geom1[0] = i; geom1[1] = j+1; geom1[2] = 	(float)(*posit+x->dim[1]*i+(j+1));
            geom2[0] = i+1; geom2[1] = j; geom2[2] = 	(float)(*posit+x->dim[1]*(i+1)+j);
            geom3[0] = i+1; geom3[1] = j+1; geom3[2] = 	(float)(*posit+x->dim[1]*(i+1)+j+1);
			
	//		posit+=1; // increment the pointer position


            sub( vec0, geom1, geom0 );
            sub( vec1, geom1, geom2 );
            sub( vec2, geom1, geom3 );

            /* get triangle face normals from vectors & normalize them */
            cross( norm0, vec0, vec1 );
            norm( norm0 );

            cross( norm1, vec1, vec2 ); 
            norm( norm1 );

//            copy( x->facenorms[0][i][j], norm0 );
  //          copy( x->facenorms[1][i][j], norm1 );
  
  			// assign, then increment

			*facenorms++ = norm0[0];
			*facenorms++ = norm0[1];
			*facenorms++ = norm0[2];
			*facenorms++ = norm1[0];
			*facenorms++ = norm1[1];
			*facenorms++ = norm1[2];

        }
    }

}


void calc_getVertNorms(t_jit_gl_ruttetra *x)
{
	int i,j;
    float avg[3], temp[3];
    // (x->dim[0]-1) = max x
    // (x->dim[1]-1) = max y
 
    float *vertnorms,*facenorms, *t;
    
    vertnorms=x->vertnorms;
    facenorms=x->facenorms;
    
 
 
 
    
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
                // need to calc the facenorm pointer postition, based on i,j
 
 
                temp[0]=(*facenorms + x->dim[1]*6*i + 6*j + 0);
                temp[1]=(*facenorms + x->dim[1]*6*i + 6*j + 1);
                temp[2]=(*facenorms + x->dim[1]*6*i + 6*j + 2);
 
 			// c[i][j][k] == *(&c[0][0][0]+[j]*[k]*i+[k]*j+k)

 
               // add( avg, avg, x->facenorms[0][i][j] );
             	add( avg, avg, temp );
            }
            /* Right & below */
            if (j < (x->dim[1]-2) && i > 0)
            {
                // need to calc the facenorm pointer postition, based on i,j
 
                // this is first 3 elements
                temp[0]=(*facenorms + x->dim[1]*6*(i-1) + 6*j + 0);
                temp[1]=(*facenorms + x->dim[1]*6*(i-1) + 6*j + 1);
                temp[2]=(*facenorms + x->dim[1]*6*(i-1) + 6*j + 2);
              
  //               add( avg, avg, x->facenorms[0][i-1][j] );
                add( avg, avg, temp );

                // this is below 3 elements
                temp[0]=(*facenorms + x->dim[1]*6*(i-1) + 6*j + 3);
                temp[1]=(*facenorms + x->dim[1]*6*(i-1) + 6*j + 4);
                temp[2]=(*facenorms + x->dim[1]*6*(i-1) + 6*j + 5);
  //              add( avg, avg, x->facenorms[1][i-1][j] );
 
                add( avg, avg, temp );
            }
            /* Left & above */
            if (j > 0 && i < (x->dim[0]-2))
            {

                // this is first 3 elements
                temp[0]=(*facenorms + x->dim[1]*6*i + 6*(j-1)+ 0);
                temp[1]=(*facenorms + x->dim[1]*6*i + 6*(j-1) + 1);
                temp[2]=(*facenorms + x->dim[1]*6*i + 6*(j-1) + 2);
//                add( avg, avg, x->facenorms[0][i][j-1] );
                add( avg, avg, temp );

                // this is below 3 elements
                temp[0]=(*facenorms + x->dim[1]*6*i + 6*(j-1)+ 3);
                temp[1]=(*facenorms + x->dim[1]*6*i + 6*(j-1) + 4);
                temp[2]=(*facenorms + x->dim[1]*6*i + 6*(j-1) + 5);

   //             add( avg, avg, x->facenorms[1][i][j-1] );
                add( avg, avg, temp );
            }
            /* Left & below */
            if (j > 0 && i > 0)
            {

                // this is first 3 elements
                temp[0]=(*facenorms + x->dim[1]*6*(i-1) + 6*(j-1)+ 3);
                temp[1]=(*facenorms + x->dim[1]*6*(i-1) + 6*(j-1) + 4);
                temp[2]=(*facenorms + x->dim[1]*6*(i-1) + 6*(j-1) + 5);

//                add( avg, avg, x->facenorms[1][i-1][j-1] );
                add( avg, avg, temp );
            }

            /* Normalize */
            norm( avg );
//          copy( x->vertnorms[i][j], avg );
	
	
//  multi[row][col] = 1;     *(*(multi + row) + col) = 1;
//  c[i][j][k] = 1;     *(*(multi + i) + col) = 1;

			
			(*(vertnorms + x->dim[1]*3*i + 3*j + 0))=avg[0];
			(*(vertnorms + x->dim[1]*3*i + 3*j + 1))=avg[1];
			(*(vertnorms + x->dim[1]*3*i + 3*j + 2))=avg[2];

        }
    }

}


void calc_getTexCoords(t_jit_gl_ruttetra *x)
{
	int i,j;
	float *texcoords;
	
	texcoords=x->texcoords;
	
    for (i = 0; i < (x->dim[0]-1); ++i)
    {
        for (j = 0; j < (x->dim[1]-1); ++j)
        {
            (*(texcoords+i*x->dim[0]*2+j*2+0)) = (float)j/(float)(x->dim[0]-2);
            (*(texcoords+i*x->dim[0]*2+j*2+1)) = (float)i/(float)(x->dim[1]-2);
        }
    }
}

/*
void calc_drawFaceNorms(t_jit_gl_ruttetra *x)
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






t_jit_err jit_gl_ruttetra_head_xyz(t_jit_gl_ruttetra *x, void *attr, long argc, t_atom *argv)
{
	
	float scale[3], position[3];
	float stridex,stridey;
//	void *o;
	long ac,i;

	float	pt0,pt1,pt2,pt3;
	long	cx,cz;
	float	dx,dz,val;
	float	*posit;


	if (argc&&argv) {
	
		posit = x->posit;
	
		x->head_xyz[0] = (float)jit_atom_getfloat(argv);
		x->head_xyz[1] = (float)jit_atom_getfloat(argv+1);
		x->head_xyz[2] = (float)jit_atom_getfloat(argv+2);


//		jit_object_method(x, gensym("getscale"), &scale);



		CLAMP(x->head_xyz[0],0.,1.);
		CLAMP(x->head_xyz[2],0.,1.);
		
		(x->head_xyz[0]<0.)?0.:(x->head_xyz[0]>1.)?1.:x->head_xyz[0];
		(x->head_xyz[2]<0.)?0.:(x->head_xyz[2]>1.)?1.:x->head_xyz[2];


		stridex=1.0/(float)x->dim[0];
		stridey=1.0/(float)x->dim[1];
		
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
		p = (double *)(bp + i*rowstride);
		p2 = (double *)(bp + (i+1)*rowstride);
		
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

void set(float vec[3], float x, float y, float z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}


short wrap(short in, short min, short max)
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




