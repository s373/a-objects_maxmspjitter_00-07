/*
special hard version for 747.3
try to get the terrain moving scale units in its position

walk's scale units, nice
016 version working like a charm. just removing posts, etc

 */
 
 

#include "jit.common.h"
#include "jit.gl.h"
#include "ext_strings.h"
#include <math.h>
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif


#define MAXGRID 256	//lets be safe
#define MAXZONAS 16	//lets be safe


typedef struct _Terreno
{
	int id;
	int		resx,resz;
	float		posit[MAXGRID][MAXGRID];
	float	    vertNorms[MAXGRID][MAXGRID][3];				//normals
//	float	    faceNorms[2][MAXGRID][MAXGRID][3];  		//facenormals
//	float	    faceNormSegs[2][2][MAXGRID][MAXGRID][3];	//facenormal segments
//	float 		texCoords[MAXGRID][MAXGRID][2];				//texture coordinates

} Terreno;


typedef struct _Zona
{
	int z_id;
	Terreno *z_ter;
	int z_ter_id;
	float	z_center[3]; //3d point on center
	float	z_box[4];// 2 points box defines

} Zona;



typedef struct _jit_gl_terreno 
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

// head
	float				head_xyz[3];
//	float				head_xyz2[3];
	float				object_xyz[3]; // was 6 because trying to smuggle in scale

//	float				walk[3]; //how much to walk regarding scale in 3d
	t_atom				atomics[3]; // to pass around values to position

// gridzonas code

	float		spacing,goneoverspacing;//ter spacing
	float			user[3];//user pos
	int			terrenodim[2];//how many grid squares to render per frame


	Terreno		*ter;
//	long			numters; //1 ter
	

	long			build_ter; //config one ter; dummy var to get method
	long			build_zona; //config one ter; dummy var to get method
	
	
	float Ymax[3];//get y max vert at xz pos
	

} t_jit_gl_terreno;


void *_jit_gl_terreno_class;

t_jit_err jit_gl_terreno_init(void) ;
t_jit_gl_terreno *jit_gl_terreno_new(t_symbol * dest_name);
void jit_gl_terreno_free(t_jit_gl_terreno *x);

//terreno func
void TERRENO_build(t_jit_gl_terreno *x);
void TERRENO_init(t_jit_gl_terreno *x);
void TERRENO_norm(t_jit_gl_terreno *x);

void jit_gl_terreno_build_ter(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);
void jit_gl_terreno_terrenodim(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);
void jit_gl_terreno_zonater(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);
void jit_gl_terreno_spacing(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);
void jit_gl_terreno_postzonas(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);
void jit_gl_terreno_postters(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);


void jit_gl_terreno_user(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);
void jit_gl_terreno_gridrender(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);

void jit_gl_terreno_zonacenter(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);

void jit_gl_terreno_Ymax(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv);


//zonas to prototype
void ZONAS_make_ters(t_jit_gl_terreno *x, long numTers);
void ZONAS_make_zonas(t_jit_gl_terreno *x, long numZx,long numZz);
void terrs_init(t_jit_gl_terreno *x);
void zonas_init(t_jit_gl_terreno *x);


t_jit_err jit_gl_terreno_shape(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terreno_dim(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terreno_rad_minor(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terreno_displaylist(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terreno_draw(t_jit_gl_terreno *x);
t_jit_err jit_gl_terreno_dest_changed(t_jit_gl_terreno *x);
void jit_gl_terreno_get_color(t_jit_gl_terreno *x, float *red, float *green, float *blue, float *alpha);

t_jit_err jit_gl_terreno_recalc(t_jit_gl_terreno *x);
t_jit_err jit_gl_terreno_dest_closing(t_jit_gl_terreno *x);



t_jit_err jit_gl_terreno_head_xyz(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terreno_head_xyz2(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_gl_terreno_object_xyz(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv);
void jit_gl_terreno_get_position(t_jit_gl_terreno *x, float * posx, float *posy, float *posz);
void jit_gl_terreno_get_scale(t_jit_gl_terreno *x, float * scalx, float *scaly, float *scalz);
void jit_gl_terreno_set_position(t_jit_gl_terreno *x);
float my_map (float in, float min, float max, float imin, float imax);

// the calc terrain funcs 
void calc_terrain(t_jit_gl_terreno *x);


t_symbol *ps_getcolor;
t_symbol *ps_getposition;
t_symbol *ps_setposition;
t_symbol *ps_getscale;
t_symbol *ps_dim;
t_symbol *ps_getcolor;
t_symbol *ps_matrixoutput;


void color_surface(t_jit_gl_terreno *x);

void draw_grid(t_jit_object *matrix, GLenum mode);

t_symbol	*ps_terrain;

float mapf (float x, float a, float b, float c, float d) ;

//vector 
void copy(float vec0[3], float vec1[3]);
void sub(float vec0[3], float vec1[3], float vec2[3]);
void add(float vec0[3], float vec1[3], float vec2[3]);
void scalDiv(float vec[3], float c);
void cross(float vec0[3], float vec1[3], float vec2[3]);
void norm(float vec[3]);
void set(float vec[3], float x, float y, float z);

void mult(float vec0[3], float vec1[3]);


// --------------------------------------------------------------------------------
//

t_jit_err jit_gl_terreno_init(void) 
{
	long attrflags=0;
	long ob3d_flags=0;
	t_jit_object *attr;
	void * ob3d;
	
	_jit_gl_terreno_class = jit_class_new("jit_gl_terreno", (method)jit_gl_terreno_new, (method)jit_gl_terreno_free,
		sizeof(t_jit_gl_terreno),A_CANT,0L); //A_CANT = untyped
	
	// set up object extension for 3d object, customized with flags
	ob3d = jit_ob3d_setup(_jit_gl_terreno_class, calcoffset(t_jit_gl_terreno, ob3d), ob3d_flags);

	// add attributes
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	// shape
	attr = jit_object_new(_jit_sym_jit_attr_offset,"shape",_jit_sym_symbol,attrflags,
		(method)0L,(method)jit_gl_terreno_shape,calcoffset(t_jit_gl_terreno, shape));	
	jit_class_addattr(_jit_gl_terreno_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"dim",_jit_sym_long,2,attrflags,
		(method)0L,(method)jit_gl_terreno_dim,0/*fix*/,calcoffset(t_jit_gl_terreno,dim));
	jit_attr_addfilterset_clip(attr,2,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terreno_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"displaylist",_jit_sym_char,attrflags,
		(method)0L,(method)jit_gl_terreno_displaylist,calcoffset(t_jit_gl_terreno, displaylist));	
	jit_class_addattr(_jit_gl_terreno_class,attr);	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"gridmode",_jit_sym_char,attrflags,
		(method)0L,(method)0L,calcoffset(t_jit_gl_terreno, gridmode));	
	jit_class_addattr(_jit_gl_terreno_class,attr);	


	attr = jit_object_new(_jit_sym_jit_attr_offset,"build_ter",_jit_sym_long, attrflags,		//build ter
		(method)0L,(method)jit_gl_terreno_build_ter,0/*fix*/,calcoffset(t_jit_gl_terreno, build_ter));	
//	jit_attr_addfilterset_clip(attr,MAXGRID,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terreno_class,attr);	
	

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"terrenodim",_jit_sym_long, 2, attrflags,		//zonater
		(method)0L,(method)jit_gl_terreno_terrenodim,0/*fix*/,calcoffset(t_jit_gl_terreno, terrenodim));	
//	jit_attr_addfilterset_clip(attr,MAXGRID,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terreno_class,attr);	

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"user",_jit_sym_float32, 3, attrflags,		//walk
		(method)0L,(method)jit_gl_terreno_user,0/*fix*/,calcoffset(t_jit_gl_terreno, user));	
//	jit_attr_addfilterset_clip(attr,MAXGRID,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terreno_class,attr);	


		attr = jit_object_new(_jit_sym_jit_attr_offset,"spacing",_jit_sym_float32, attrflags,		//spacing
		(method)0L,(method)jit_gl_terreno_spacing,0/*fix*/,calcoffset(t_jit_gl_terreno, spacing));	
//	jit_attr_addfilterset_clip(attr,MAXGRID,0,TRUE,FALSE);
	jit_class_addattr(_jit_gl_terreno_class,attr);	

	

	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"Ymax",_jit_sym_float32, 3, attrflags,		//head_xyz
		(method)0L,(method)jit_gl_terreno_Ymax,0/*fix*/,calcoffset(t_jit_gl_terreno, Ymax));	
	jit_class_addattr(_jit_gl_terreno_class,attr);	


//Ymax

	
	// handle draw method.  called in automatic mode by jit.gl.render, or otherwise through ob3d when banged.
	// this is A_CANT because draw setup needs to happen in the ob3d beforehand. 
	jit_class_addmethod(_jit_gl_terreno_class, (method)jit_gl_terreno_draw, "ob3d_draw", A_CANT, 0L);
	
	// handle dest_changed method.
	// this method is called by jit.render when the destination context changes: for example,
	// when the user moves the window from one monitor to another.  Anything your object keeps
	// in the OpenGL machine -- textures, display lists, vertex shaders, etc. -- will need to
	// be rebuilt here. 
	jit_class_addmethod(_jit_gl_terreno_class, (method)jit_gl_terreno_dest_changed, "dest_changed", A_CANT, 0L);
	
	// must register for ob3d	
	jit_class_addmethod(_jit_gl_terreno_class, (method)jit_object_register, 			"register",		A_CANT, 0L);
	jit_class_addmethod(_jit_gl_terreno_class, (method)jit_gl_terreno_dest_closing, 	"dest_closing",	A_CANT, 0L);

	jit_class_register(_jit_gl_terreno_class);

	ps_terrain 			= gensym("terrain");

	ps_getcolor= gensym("getcolor");
	ps_getposition= gensym("getposition");
	ps_setposition= gensym("position");
	ps_getscale= gensym("getscale");
	ps_dim=gensym("dim");
	ps_getcolor=gensym("getcolor");
	ps_matrixoutput=gensym("matrixoutput");


	return JIT_ERR_NONE;
}


t_jit_gl_terreno *jit_gl_terreno_new(t_symbol * dest_name)
{
	t_jit_gl_terreno *x;
//	long i;
//	float *vertnorms, *posit, *facenorms,*texcoords, *facenormsegs;
	
	// make jit object
	if (x = (t_jit_gl_terreno *)jit_object_alloc(_jit_gl_terreno_class)) 
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

		//initing code for zonas
		
		//make some ters first
//		x->numters = 10;
		//ter = 
//		ZONAS_make_ters(x,x->numters);

		x->terrenodim[0] =(int)(0.5*x->dim[0]); //10;//2*x->dim[0];	important: gridrender always halfdim.. gets rid of nasty crash at loadbang start
		x->terrenodim[1] = (int)(0.5*x->dim[1]);//10;//2*x->dim[1];
		x->spacing = 10.;//100.;
		x->goneoverspacing = 0.1;//1.;//0.01;
//		x->zonasdim[0] =		x->zonasdim[1] =  100.0f;

//		x->defaultzone=0;

		//ZONAS_make_ters(x,10);
		TERRENO_build(x);
//		int num = rand()%9 + 1;
		//num*=num;
//		ZONAS_make_zonas(x,num,num);




	} 
	else 
	{
		x = NULL;
	}	
	return x;
}


void jit_gl_terreno_free(t_jit_gl_terreno *x)
{

/*	if(x->zonas){
		jit_disposeptr((void *)x->zonas);//, sizeof(Zona)*x->numzonas);
		post("freed zonas %ld %ld",x->numzonas,sizeof(Zona)*x->numzonas);
	}
*/
	if(x->ter){
		jit_disposeptr((void *)x->ter);//, sizeof(Terreno)*x->numters);
		post("freed ters %ld %ld",1/*x->numters*/,sizeof(Terreno));//*x->numters);
	}


	
	if (x->chunk)
		jit_glchunk_delete(x->chunk);
	if (jit_ob3d_set_context(x)==JIT_ERR_NONE) {
		if (x->dlref)
			glDeleteLists(x->dlref,1);
	}
	jit_ob3d_free(x);
}



void TERRENO_build(t_jit_gl_terreno *x)
{
	if(x->ter)
		jit_disposeptr((void *)x->ter);//, sizeof(Terreno)*numTers);

	x->ter = (Terreno *) jit_newptr(sizeof(Terreno)/*numTers*/);//jit_getbytes(sizeof(Terreno)*numTers);
//	x->numters = numTers;
	memset((Terreno *)x->ter,0,sizeof(Terreno)/*numTers*/);

//	post("built terreno");//,numTers);
	TERRENO_init(x);
}

void TERRENO_init(t_jit_gl_terreno *x)
{
//	int i;
	int k,l,maxk,maxl;
	
//	for(i=0;i<x->numters;i++) {
//		x->ter[i].id = i;
		maxk= x->ter->resx = rand()%90+5;
		maxl= x->ter->resz = rand()%90+5;
		
		
			for(l=0;l<maxl;l++)		{
		for(k=0;k<maxk;k++) {
//			for(l=0;l<maxl;l++)		{
			
			x->ter->posit[k][l] = ((rand()%1000)*0.001)*(10.0f/*i*7+1*/);//(pow(i+1,2));//0-1*i
			
	//		post("making ter %ld casa %ld %ld posit %f ",i,k,l,x->ter[i].posit[k][l]);
			
			}
			
		}

	//should make norms etc

	
	
	TERRENO_norm(x);



}

void TERRENO_norm(t_jit_gl_terreno *x)
{
	int i,j;
	float loca[3],locb[3],locc[3];
	float v1[3],v2[3],temp[3];
	float spacing = x->spacing;

//	for(k=0;k<x->numters;k++) {
		int rx = x->ter->resx;
		int rz = x->ter->resz;

		for(j=0;j<rz;j++) {
		for(i=0;i<rx;i++) {
			int ii = i+1;
			int jj = j+1;
			
			set(loca, i*spacing, x->ter->posit[i][j], j*spacing);

			if(ii==rx)
				ii=0;
			if(jj==rz)
				jj=0;

			set(locb, ((i+1)*spacing), x->ter->posit[ii][j], j*spacing);
			set(locc, ((i)*spacing), x->ter->posit[i][jj], ((j+1)*spacing) );
				
				sub(v1, loca, locb);
				sub(v2, loca, locc);
		
			cross(temp,v1,v2);
			
			norm(temp);
			multf(temp,-1.); //
			
			copy(x->ter->vertNorms[i][j],temp);
//			post("ter%ld casa %ld %ld norms %f %f %f",k,i,j,x->ter[k].vertNorms[i][j][0],x->ter[k].vertNorms[i][j][1],x->ter[k].vertNorms[i][j][2]);
		}
		}

	

}


/*
void ZONAS_make_ters(t_jit_gl_terreno *x, long numTers){
	//if not NULL, deallocate memory before assigning new memory
	//could check for whether flockPtr->number changed in value but not really 
	//necessary as speed isn't the biggest issue in setting this param

	if(x->ter)
		jit_disposeptr((void *)x->ter);//, sizeof(Terreno)*numTers);
		
	x->ter = (Terreno *) jit_newptr(sizeof(Terreno)*numTers);//jit_getbytes(sizeof(Terreno)*numTers);
	x->numters = numTers;
	memset((Terreno *)x->ter,0,sizeof(Terreno)*numTers);

	post("built %ld ters",numTers);
	terrs_init(x);
}


void ZONAS_make_zonas(t_jit_gl_terreno *x, long numZx,long numZz){
	//if not NULL, deallocate memory before assigning new memory
	//could check for whether flockPtr->number changed in value but not really 
	//necessary as speed isn't the biggest issue in setting this param

	if(x->zonas)
		jit_disposeptr((void *)x->zonas);//, sizeof(Zona)*x->numzonas);
		
	int numz = x->numzonas = numZx*numZz;
	x->numZx = numZx; x->numZz = numZz;
		
	x->zonas = (Zona *) jit_newptr(sizeof(Zona)*numz);
	//talvez tenha que fazer memset
	//x->zonas = (Zona *) memset
	
	memset((Zona *)x->zonas,0,sizeof(Zona)*numz);
	
		post("built %ld zonas %ld %ld",numz,numZx,numZz);

	zonas_init(x);

}


void terrs_init(t_jit_gl_terreno *x){
	int i;
	int k,l,maxk,maxl;
	
	for(i=0;i<x->numters;i++) {
		x->ter[i].id = i;
		maxk= x->ter[i].resx = rand()%90+5;
		maxl= x->ter[i].resz = rand()%90+5;
		
		
			for(l=0;l<maxl;l++)		{
		for(k=0;k<maxk;k++) {
//			for(l=0;l<maxl;l++)		{
			
			x->ter[i].posit[k][l] = ((rand()%1000)*0.001)*(i*7+1);//(pow(i+1,2));//0-1*i
			
	//		post("making ter %ld casa %ld %ld posit %f ",i,k,l,x->ter[i].posit[k][l]);
			
			}
			
		}

	//should make norms etc

	}
	
	terrs_norm(x);

}

void ter_norm(t_jit_gl_terreno *x, int k){
	int i,j;
	float loca[3],locb[3],locc[3];
	float v1[3],v2[3],temp[3];
	float spacing = x->spacing;

//	for(k=0;k<x->numters;k++) {
		int rx = x->ter[k].resx;
		int rz = x->ter[k].resz;

		for(j=0;j<rz;j++) {
		for(i=0;i<rx;i++) {
			int ii = i+1;
			int jj = j+1;
			
			set(loca, i*spacing, x->ter[k].posit[i][j], j*spacing);

			if(ii==rx)
				ii=0;
			if(jj==rz)
				jj=0;

			set(locb, ((i+1)*spacing), x->ter[k].posit[ii][j], j*spacing);
			set(locc, ((i)*spacing), x->ter[k].posit[i][jj], ((j+1)*spacing) );
				
				sub(v1, loca, locb);
				sub(v2, loca, locc);
		
			cross(temp,v1,v2);
			
			norm(temp);
			multf(temp,-1.); //
			
			copy(x->ter[k].vertNorms[i][j],temp);
//			post("ter%ld casa %ld %ld norms %f %f %f",k,i,j,x->ter[k].vertNorms[i][j][0],x->ter[k].vertNorms[i][j][1],x->ter[k].vertNorms[i][j][2]);
		}
		}

	
}



void terrs_norm(t_jit_gl_terreno *x){
	int k,i,j;
	float loca[3],locb[3],locc[3];
	float v1[3],v2[3],temp[3];
	float spacing = x->spacing;

	for(k=0;k<x->numters;k++) {
		int rx = x->ter[k].resx;
		int rz = x->ter[k].resz;

		for(j=0;j<rz;j++) {
		for(i=0;i<rx;i++) {
			int ii = i+1;
			int jj = j+1;
			
			set(loca, i*spacing, x->ter[k].posit[i][j], j*spacing);

			if(ii==rx)
				ii=0;
			if(jj==rz)
				jj=0;
			//i+1*spacing points at correct coords while ii*spacing does not
			set(locb, ((i+1)*spacing), x->ter[k].posit[ii][j], j*spacing);
			set(locc, ((i)*spacing), x->ter[k].posit[i][jj], ((j+1)*spacing) );
				
				sub(v1, loca, locb);
				sub(v2, loca, locc);
		
			cross(temp,v1,v2);
			
			norm(temp);
			multf(temp,-1.); //
		//	temp[1]=-temp[1];
			
			copy(x->ter[k].vertNorms[i][j],temp);
			post("ter%ld casa %ld %ld norms %f %f %f",k,i,j,x->ter[k].vertNorms[i][j][0],x->ter[k].vertNorms[i][j][1],x->ter[k].vertNorms[i][j][2]);
		}
		}

	}
}

void zonas_init(t_jit_gl_terreno *x){
	int i,j;
	int k;//,l;
//	Terreno *ter;
//hanle zbox
	int numx = x->numZx;
	int numz = x->numZz;
	float zonasdimx=x->zonasdim[0],zonasdimz=x->zonasdim[1];

// type all these into memdyn
//  int zonamap[numx][numz][4];// = new int[x->numZx][numzdivision][4];//logo com 4 para jardar para dentro das outras
//  float zonadiv[numx][numz][2];
//  float zonadivx[numx+1];//accomodate for limit ending
//  float zonadivz[numz+1];

float* zonamapa;
float * zonadivX;
float *zonadivZ;

	zonamapa = (float *) jit_newptr(sizeof(float)*numx*numz*4);
	zonadivX = (float *) jit_newptr(sizeof(float)*(numx+1));
	zonadivZ = (float *) jit_newptr(sizeof(float)*(numz+1));	
	if(!zonamapa||!zonadivX||!zonadivZ)
		return;

  float dimxstride = 1./(float)numx;
  float dimzstride = 1./(float)numz;
//  float goneover = x->goneoverspacing;
  float spacing = x->spacing;

//	zonadivx[0]=zonadivz[0] = 0.;
	*(zonadivX+0) =0.;
	*(zonadivZ+0) =0.;
	*(zonadivX+numx) =1.;
	*(zonadivZ+numz) =1.;
	
//	zonadivx[numx] = 1.;
//	zonadivz[numz] = 1.;

	for(i=1;i<numx;i++){//was -1
//			zonadivx[i] = i * dimxstride + (((rand()%1000)*0.001)-0.5)*dimxstride;
			*(zonadivX+i)= (float) i * dimxstride + (((rand()%1000)*0.001)-0.5)*dimxstride;
	}

	for(j=1;j<numz;j++){
//			zonadivz[j] = j * dimzstride + (((rand()%1000)*0.001)-0.5)*dimzstride;
			*(zonadivZ+j) = (float) j * dimzstride + (((rand()%1000)*0.001)-0.5)*dimzstride;
	}//was j fucking things


//	for(i=0;i<numx+1;i++)
//		post("zonadivx %ld %f",i,*(zonadivX+i));

//	for(i=0;i<numz+1;i++)
//		post("zonadivz %ld %f",i,*(zonadivZ+i));


	for(j=0;j<numz;j++){
	for(i=0;i<numx;i++){
		int idx = i*numz+j;
		float val = *(zonadivX+i);		*(zonamapa+idx+0) =  mapf(val,0.,1.,-1*zonasdimx,zonasdimx); // post("idx %ld zonamapa 0 %f val %f",idx,*(zonamapa+idx+0),val);
			  val = *(zonadivZ+j);		*(zonamapa+idx+1) =  mapf(val,0.,1.,-1*zonasdimz,zonasdimz); // post("idx %ld zonamapa 1 %f val %f",idx,*(zonamapa+idx+1),val);
			  val = *(zonadivX+i+1);	*(zonamapa+idx+2) =  mapf(val,0.,1.,-1*zonasdimx,zonasdimx); // post("idx %ld zonamapa 2 %f val %f",idx,*(zonamapa+idx+2),val);
			  val = *(zonadivZ+j+1);	*(zonamapa+idx+3) =  mapf(val,0.,1.,-1*zonasdimz,zonasdimz); // post("idx %ld zonamapa 3 %f val %f",idx,*(zonamapa+idx+3),val);

	//	 post("idx %ld zonamapa %f %f %f %f",idx,*(zonamapa+idx+0),*(zonamapa+idx+1),*(zonamapa+idx+2),*(zonamapa+idx+3));

		// so consigo manter os valores de cima dentro deste loop... sen‹o os bounds da zonamapa s‹o diferentes


		x->zonas[idx].z_id = idx;
		int index = rand()%x->numters;
	//	ter = &x->ter[index];
		x->zonas[idx].z_ter = &x->ter[index];//ter;//x->ter[rand()%x->numters];
		x->zonas[idx].z_ter_id = index;
		
		x->zonas[idx].z_box[0] =(float)	*(zonamapa +idx+0);//*(zonamapa +i*numz+j+0);//[i][j][0]; //(int) mapf(zonadiv[i][j][0],0.,1.,-zonasdimx,zonasdimx);				//rand()%x->numters;
		x->zonas[idx].z_box[1] =(float)	*(zonamapa +idx+1);//zonamap[i][j][1]; //(int) mapf(zonadiv[i][j][0],0.,1.,-zonasdimx,zonasdimx);				//rand()%x->numters;
		x->zonas[idx].z_box[2] =(float)	*(zonamapa +idx+2);//zonamap[i][j][2]; //(int) mapf(zonadiv[i][j][0],0.,1.,-zonasdimx,zonasdimx);				//rand()%x->numters;
		x->zonas[idx].z_box[3] =(float)	*(zonamapa +idx+3);//zonamap[i][j][3]; //(int) mapf(zonadiv[i][j][0],0.,1.,-zonasdimx,zonasdimx);				//rand()%x->numters;
		
		x->zonas[idx].z_center[0] =	((*(zonamapa +idx+0))+(*(zonamapa +idx+2)))*0.5*spacing;//((zonamap[i][j][0]+zonamap[i][j][2])*0.5)*spacing;//*goneover;	//rand()%x->numters;
		x->zonas[idx].z_center[1] =	0;//fix//((zonamap[i][j][0]+zonamap[i][j][3])*0.5)*goneover;	//rand()%x->numters;
		x->zonas[idx].z_center[2] =	((*(zonamapa +idx+1))+(*(zonamapa +idx+3)))*0.5*spacing;//((zonamap[i][j][1]+zonamap[i][j][3])*0.5)*spacing;	//rand()%x->numters;

//		post("zona %ld com ter %ld %ld com zbox %ld %ld %ld %ld e centro em %f %f %f",idx,index,x->zonas[idx].z_ter,zonamap[i][j][0],zonamap[i][j][1],zonamap[i][j][2],zonamap[i][j][3],x->zonas[idx].z_center[0],x->zonas[idx].z_center[1],x->zonas[idx].z_center[2]);
		post("zona %ld com ter %ld %ld com zbox %f %f %f %f e centro em %f %f %f",idx,index,x->zonas[idx].z_ter,*(zonamapa +idx+0),*(zonamapa +idx+1),*(zonamapa +idx+2),*(zonamapa +idx+3),x->zonas[idx].z_center[0],x->zonas[idx].z_center[1],x->zonas[idx].z_center[2]);
		}
		}
		


//delete pointers
jit_disposeptr((void *)zonamapa);
jit_disposeptr((void *)zonadivX);
jit_disposeptr((void *)zonadivZ);

}


void jit_gl_terreno_build_zonas(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv&&argc>1&&argc<3) {
		int numx = (int)jit_atom_getlong(argv);
		int numz = (int)jit_atom_getlong(argv+1);
		 ZONAS_make_zonas(x,numx,numz);
		 x->recalc=1;
	}

}
void jit_gl_terreno_build_zona(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){

}
void jit_gl_terreno_build_ters(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
		int num = (int)jit_atom_getlong(argv);
		 ZONAS_make_ters(x,num);
	}

}

*/

void jit_gl_terreno_build_ter(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv/*&&argc==4*/) {
	int i,j;
	//	int t = jit_atom_getlong(argv);
		int rx = jit_atom_getlong(argv+0);
		int rz = jit_atom_getlong(argv+1);

		float of7 = jit_atom_getfloat(argv+2);
		float dev = jit_atom_getfloat(argv+3);
		
		// make rnd ter
		
		
		x->ter->resx = rx;
		x->ter->resz = rz;
		//posit
		for(i=0;i<rx;i++)
			for(j=0;j<rz;j++) {
				int v = rand()%65535;
				x->ter->posit[i][j] = mapf(v,0,65535,of7-dev,of7+dev);
			}
			
		//norms
		TERRENO_norm(x);//ter_norm(x,t);
		x->recalc=1;
	}
	
}

/*
void jit_gl_terreno_zonasdim(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
		x->zonasdim[0] = jit_atom_getfloat(argv);
		x->zonasdim[1] = jit_atom_getfloat(argv+1);
		// ZONAS_make_ters(x,num);
	}

}

void jit_gl_terreno_zonacenter(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
		x->zonacenter[0] = (int)jit_atom_getfloat(argv); //scan first arg only and register that zones center
//		x->zonasdim[1] = jit_atom_getfloat(argv+1);
		// ZONAS_make_ters(x,num);
		int z = CLIP(x->zonacenter[0],0,x->numzonas);
		
		x->zonacenter[1] = x->zonas[z].z_center[0];
		x->zonacenter[2] = x->zonas[z].z_center[1];
		x->zonacenter[3] = x->zonas[z].z_center[2];
		
	}

}

*/

void jit_gl_terreno_Ymax(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
	//	Terreno *ter;
		x->Ymax[0] = jit_atom_getfloat(argv);
		x->Ymax[1] = jit_atom_getfloat(argv+1);
		x->Ymax[2] = jit_atom_getfloat(argv+2);

		// calc max height Ymax is at world coords, transform to memory space
		float  posy;
		int posx, posz;
		float gonespacing=x->goneoverspacing;
		
		posx = (int)(x->Ymax[0] * gonespacing);
		posz = (int)(x->Ymax[2] * gonespacing);

			//find this zone & ter
//		int activezone = find_zone_indexes(x,posx,posz);

			//ter = &x->ter[activezone];//wrong
		 Terreno *ter = &x->ter;//[x->zonas[activezone].z_ter_id];
		int	trx = ter->resx;
		int	trz = ter->resz;
		int	ti = posx;
		int	tj = posz;
		int ti1 = posx+1;
		int tj1 = posz+1;
			
			ti = ((ti%=trx)<0) ? ti+= trx : ti;
			tj = ((tj%=trz)<0) ? tj+= trz : tj;

			ti1 = ((ti1%=trx)<0) ? ti1+= trx : ti1;
			tj1 = ((tj1%=trz)<0) ? tj1+= trz : tj1;

			//vertex
		//	*p++ = ter->posit[ti][tj];// posit[i][j]; // z == posit[i][j]

		float val[4];
		val[0] = ter->posit[ti][tj];
		val[1] = ter->posit[ti1][tj];
		val[2] = ter->posit[ti1][tj1];
		val[3] = ter->posit[ti][tj1];
		
		posy = MAX(val[0],val[1]);
		posy = MAX(posy,val[2]);
		posy = MAX(posy,val[3]);
		
		x->Ymax[1] = posy;
		
//	CLAMP
		
	}

}


/*

void jit_gl_terreno_zonater(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
	Zona *z;
	int zid,zt;
		zid = x->zonater[0] = jit_atom_getlong(argv);//CLIP(jit_atom_getlong(argv),0,x->numzonas);
		zt = x->zonater[1] = jit_atom_getlong(argv+1);//CLIP(jit_atom_getlong(argv+1),0,x->numters);
		
		z = &x->zonas[zid];
		z->z_ter = &x->ter[zt];
		z->z_ter_id = zt;
//this is so wrong should point at ter!!		x->zonas[zid].z_ter_id = zt;
//		x->zonas[zid].z_ter = &x->ter[zt];
		x->recalc=1;

	}

}

*/

void jit_gl_terreno_spacing(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
		x->spacing = jit_atom_getfloat(argv);
		x->goneoverspacing = 1.0f/x->spacing;//jit_atom_getfloat(argv);
		// ZONAS_make_ters(x,num);
		//rebuild terr norms
//		terrs_norm(x);
		x->recalc=1;
		
	}

}

/*
void jit_gl_terreno_postzonas(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
		x->postzonas = jit_atom_getlong(argv);
		if(x->postzonas==7)
			postzonas(x);
		// ZONAS_make_ters(x,num);
	}

}

void jit_gl_terreno_postters(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv){
	if (argc&&argv) {
		x->postters = jit_atom_getlong(argv);
		if(x->postters==7)
			postters(x);
		// ZONAS_make_ters(x,num);
	}

}

/////////
/////////

void postzonas(t_jit_gl_terreno *x) {
	int i,j;
	int numx = x->numZx;
	int numz = x->numZz;
	int index,idx;
	//float zonasdimx=x->zonasdim[0],zonasdimz=x->zonasdim[1];

	post("postzonas init");
	for(i=0;i<numx;i++)
		for(j=0;j<numz;j++){
			
			idx=i*numz+j;
			
			index = x->zonas[idx].z_ter_id;//x->ter[i].id; //= i;

			post("zona %ld com ter %ld %ld com zbox %f %f %f %f e centro em %f %f %f",idx,index,x->zonas[idx].z_ter,x->zonas[idx].z_box[0],x->zonas[idx].z_box[1],x->zonas[idx].z_box[2],x->zonas[idx].z_box[3],x->zonas[idx].z_center[0],x->zonas[idx].z_center[1],x->zonas[idx].z_center[2]);
	
		}
}
/////////
void postters(t_jit_gl_terreno *x) {
	int i,j,k;
	int numters = x->numters;
	int rx = 0, rz=0; //idx;
	Terreno * t=NULL;
	float h=0.0f;
//	int index,idx;
	//float zonasdimx=x->zonasdim[0],zonasdimz=x->zonasdim[1];

	

	post("postters init");


for(i=0;i<numters;i++) {
	t = &x->ter[i];
	rx = t->resx;
	rz = t->resz;
	post("ter %ld res %ld %ld",i,rx,rz);
}

	for(i=0;i<numters;i++) {
	t = &x->ter[i];
	rx = t->resx;
	rz = t->resz;
	post("ter %ld res %ld %ld",i,rx,rz);

		for(j=0;j<rx;j++){
		for(k=0;k<rz;k++){
			
			//idx=j*rz+k;
			h=t->posit[j][k];
			

	//		post("ter %ld casa %ld %ld posit %f ",i,j,k,/*idx,*/ //h);
/*	
		}
		}
	}
}
*/
/////////


void jit_gl_terreno_user(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->user[0] = jit_atom_getfloat(argv);//CLIP(jit_atom_getlong(argv),0,x->numzonas);
		x->user[1] = jit_atom_getfloat(argv+1);//CLIP(jit_atom_getlong(argv+1),0,x->numters);
		x->user[2] = jit_atom_getfloat(argv+2);
		
		x->recalc=1;//important to make geom again, but switched
	}


}


void jit_gl_terreno_terrenodim(t_jit_gl_terreno *x,void *attr, long argc, t_atom *argv)
{

	if (argc&&argv) {
		t_atom a[2];
		t_atom *ap = a;
		x->terrenodim[0] = jit_atom_getlong(argv);//CLIP(jit_atom_getlong(argv),0,x->numzonas);
		x->terrenodim[1] = jit_atom_getlong(argv+1);//CLIP(jit_atom_getlong(argv+1),0,x->numters);
		//SETSYM(a+0,ps_dim);
		//add one to dim!
//		SETLONG(a+0,2*x->gridrender[0]+1);
//		SETLONG(a+1,2*x->gridrender[1]+1);
		SETLONG(a+0,2*x->terrenodim[0]);
		SETLONG(a+1,2*x->terrenodim[1]);
		jit_gl_terreno_dim(x,attr,2,ap);
		
		x->recalc=1;
		
	}

}



t_jit_err jit_gl_terreno_shape(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->shape = jit_atom_getsym(argv);
		x->recalc = 1;		
	}
	
	return JIT_ERR_NONE;
}


t_jit_err jit_gl_terreno_dim(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv&&x->chunk&&x->chunk->m_vertex) {
		t_jit_matrix_info info;

		jit_object_method(x->chunk->m_vertex,_jit_sym_dim, argc, argv);
		jit_object_method(x->chunk->m_vertex,_jit_sym_getinfo,&info);
		x->dim[0] = info.dim[0];
		x->dim[1] = info.dim[1];
		x->recalc = 1;	

		//important to		
		//	calc_getTexCoords(x);
		// when dimensions change	
	}
	
	return JIT_ERR_NONE;
}



t_jit_err jit_gl_terreno_displaylist(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv)
{
	if (argc&&argv) {
		x->displaylist = jit_atom_getlong(argv);
		x->recalc = 1;		
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_gl_terreno_recalc(t_jit_gl_terreno *x)
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


t_jit_err jit_gl_terreno_dest_closing(t_jit_gl_terreno *x)
{
	if (x->dlref) {
		glDeleteLists(x->dlref,1);
		x->dlref=NULL;
		x->recalc=1;
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_gl_terreno_draw(t_jit_gl_terreno *x)
{
	t_jit_err result = JIT_ERR_NONE;
	GLenum prim;
	
	prim = (x->gridmode) ? GL_TRIANGLE_STRIP : GL_QUAD_STRIP;
	
	if (x->recalc) {
		jit_gl_terreno_recalc(x);
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
		if (!jit_attr_getlong(x,ps_matrixoutput)) {//gensym("matrixoutput"))) {
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

t_jit_err jit_gl_terreno_dest_changed(t_jit_gl_terreno *x)
{
	if (x->dlref) x->recalc=1;
	return JIT_ERR_NONE;
}

void jit_gl_terreno_get_color(t_jit_gl_terreno *x, float *red, float *green, float *blue, float *alpha)
{
	long ac=0;
	t_atom *av=NULL;
	t_symbol *s = ps_getcolor;
	
	jit_object_method(x,s,&ac,&av);//gensym("getcolor"),&ac,&av);
	if ((ac==4)&&av) {
		*red 	= jit_atom_getfloat(av);
		*green 	= jit_atom_getfloat(av+1);
		*blue 	= jit_atom_getfloat(av+2);
		*alpha 	= jit_atom_getfloat(av+3);
		jit_freebytes(av,ac*sizeof(t_atom));
	}
}

void jit_gl_terreno_get_position(t_jit_gl_terreno *x, float * posx, float *posy, float *posz)
{
	long ac=0;
	t_atom *av=NULL;
	t_symbol *s = ps_getposition;
	
	jit_object_method(x,s,&ac,&av);
//	jit_object_method(x,gensym("getposition"),&ac,&av);
	if ((ac==3)&&av) {
		*posx 	= jit_atom_getfloat(av);
		*posy 	= jit_atom_getfloat(av+1);
		*posz 	= jit_atom_getfloat(av+2);
		jit_freebytes(av,ac*sizeof(t_atom));
	}
}

void jit_gl_terreno_set_position(t_jit_gl_terreno *x)
{
	long ac=3;
//	t_atom *av=NULL;
	t_symbol *s = ps_setposition;
	t_atom *newpos = x->atomics;

	//t_jit_err jit_atom_setfloat(t_atom *a, double b);
	//jit_atom_setfloat(newpos, 3);

	jit_object_method(x,s,&ac,&newpos);


}



void jit_gl_terreno_get_scale(t_jit_gl_terreno *x, float * scalx, float *scaly, float *scalz)
{
	long ac=0;
	t_atom *av=NULL;
	t_symbol *s = ps_getscale;
	
	jit_object_method(x,s,&ac,&av);
//	jit_object_method(x,gensym("getscale"),&ac,&av);

	if ((ac==3)&&av) {
		*scalx 	= jit_atom_getfloat(av);
		*scaly 	= jit_atom_getfloat(av+1);
		*scalz 	= jit_atom_getfloat(av+2);
		jit_freebytes(av,ac*sizeof(t_atom));
	}
}


void color_surface(t_jit_gl_terreno *x)
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
	
	jit_gl_terreno_get_color(x,&red,&green,&blue,&alpha);			
	
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



t_jit_err jit_gl_terreno_head_xyz(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv)
{
	
//	float scale[3], position[3];
	float stridex,stridey;
//	void *o;
//	long i, j;
//
//	float	pt0,pt1,pt2,pt3;
	long	cx,cz;
	float	dx,dz,val;
//	float	*positt;


	if (argc&&argv) {
	
//		positt = x->positt; // when in dyn mem
//		positt = posit;
		
		x->head_xyz[2] = (float)jit_atom_getfloat(argv);
		x->head_xyz[1] = (float)jit_atom_getfloat(argv+1);
		x->head_xyz[0] = (float)jit_atom_getfloat(argv+2);


//		jit_object_method(x, gensym("getscale"), &scale);


//
//		CLAMP(x->head_xyz[0],0.,1.);
//		CLAMP(x->head_xyz[2],0.,1.);
//		
//		(x->head_xyz[0]<0.)?0.:(x->head_xyz[0]>1.)?1.:x->head_xyz[0];
//		(x->head_xyz[2]<0.)?0.:(x->head_xyz[2]>1.)?1.:x->head_xyz[2];


		stridex=1.0/(float)x->dim[0];
		stridey=1.0/(float)x->dim[1];
		
		dx = stridex - ((long)stridex);
		dz = stridey - ((long)stridey);


		//this floors
		cx = x->head_xyz[0] * (float)x->dim[0];
		cz = x->head_xyz[2] * (float)x->dim[1];



//		val = posit[cx][cz];



		x->head_xyz[1] = val;

// should interpolate

//		dx = x->head_xyz[0] - 

//			post("positt coords %ld %ld & val %f",cx,cz,val);
//			post("dxdz %f %f",dx,dz);
//
//			post("head_xyz %f %f %f",x->head_xyz[0], x->head_xyz[1], x->head_xyz[2]);
//
//		jit_gl_terreno_get_scale(x,&scale[0],&scale[1],&scale[2]);			
//		jit_gl_terreno_get_position(x,&position[0],&position[1],&position[2]);			

		//methods not working when the filter for this method not set right (clipping 0.-1. now)
		// working very well by now


//			post("scale %f %f %f",scale[0],scale[1],scale[2]);
//			post("position %f %f %f",position[0],position[1],position[2]);

//
//	for(i=0;i<x->dim[0];i++)
//		for(j=0;j<x->dim[1];j++) {
//		
//			post("posit %ld %ld = %f", i, j, posit[i][j]);
//		
//		}

//		x->recalc = 1;		
	}
	return JIT_ERR_NONE;
}

/*
t_jit_err jit_gl_terreno_head_xyz2(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv)
{
	
	float scale[3], position[3];
	float stridex,stridey;
//	void *o;
	long ac,i;

	float	pt0,pt1,pt2,pt3;
	long	cx,cz;
	float	dx,dz,val;
	float	*positt;


	if (argc&&argv) {
	
//		positt = x->positt;
		positt = posit;
		
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

//		//clamp again ad nauseum
//		(cx<0)?0:(cx>x->dim[0])?x->dim[0]:cx; 
//		(cz<0)?0:(cz>x->dim[1])?x->dim[1]:cz; 

	
		
		//&c[0][0][0]+[j]*[k]*i+[k]*j+k)
		val = (float *)(positt + x->dim[1]*cx + cz);
		post("val from *p %f", val);

		val = posit[cx][cz];

		post("val from table %f", val);
		
		x->head_xyz[1] = val;

//		dx = x->head_xyz[0] - 

			post("positt coords %ld %ld & val %f",cx,cz,val);
			post("dxdz %f %f",dx,dz);

			post("head_xyz %f %f %f",x->head_xyz[0], x->head_xyz[1], x->head_xyz[2]);

	jit_gl_terreno_get_scale(x,&scale[0],&scale[1],&scale[2]);			
	jit_gl_terreno_get_position(x,&position[0],&position[1],&position[2]);			

			post("scale %f %f %f",scale[0],scale[1],scale[2]);
			post("position %f %f %f",position[0],position[1],position[2]);




//		x->recalc = 1;		


	}
	
	return JIT_ERR_NONE;




}
*/

t_jit_err jit_gl_terreno_object_xyz(t_jit_gl_terreno *x, void *attr, long argc, t_atom *argv)
{
	// get objects pos and set signed distance to z inside one input array element
	float scale[3], position[3];
//	float stridex,stridey;
//	void *o;
	long i;

//	float	pt0,pt1,pt2,pt3;
	long	cx,cz,fx,fz, cx1, cz1;
	float	dx,dz, interpx, interpz;
	float	val,ival,ival1,ival2;
	float	pt0,pt1,pt2,pt3;
//	float	*positt;
	float	boxmin[3], boxmax[3];
	float	toplayer[3], player[3];
	short	inside;


	if (argc&&argv) {
	
//		positt = x->positt; // when in dyn mem
//		positt = posit;
		
		player[0] = x->object_xyz[0] = (float)jit_atom_getfloat(argv);
		player[1] = x->object_xyz[1] = (float)jit_atom_getfloat(argv+1);
		player[2] = x->object_xyz[2] = (float)jit_atom_getfloat(argv+2);


		jit_gl_terreno_get_scale(x,&scale[0],&scale[1],&scale[2]);			
		jit_gl_terreno_get_position(x,&position[0],&position[1],&position[2]);			

		for(i=0;i<3;i++) {
			boxmin[i] = position[i]-scale[i];
			boxmax[i] = position[i]+scale[i];
			toplayer[i] = player[i]-position[i];
		}

		//ensure calc inside terrain
//		for(i=0;i<3;i++) {
//			boxmin[i] = position[i]-scale[i];
//			boxmax[i] = position[i]+scale[i];
//			toplayer[i] = x->object_xyz[i]-position[i];
//		}

//		post("object_xyz boxmin_xyz %f %f %f",boxmin[0], boxmin[1], boxmin[2]);
//		post("object_xyz boxmax_xyz %f %f %f",boxmax[0], boxmax[1], boxmax[2]);


		inside = (player[0]>boxmin[0]&&player[0]<boxmax[0]&&
//					player[1]>boxmin[1]&&player[1]<boxmax[1]&&		//dont care for y!! thats what we want
					player[2]>boxmin[2]&&player[2]<boxmax[2]);
		
		if(inside) { //safe to acess memory?
						
//			cx = player[0] * (float)x->dim[0];
//			cz = x->object_xyz[2] * (float)x->dim[1];
			
			// x is flipped
			// z is flipped too
			
			dx = my_map(player[0], boxmin[0], boxmax[0], 0, x->dim[1]); //look, changed dim
			dz = my_map(player[2], boxmax[2], boxmin[2], 0, x->dim[0]);

			cx = (long)(dx+0.5);		//actual rounded center
			cz = (long)(dz+0.5);
			fx = (long)(dx);			//low rounded center
			fz = (long)(dz);
			cx1 = fx+1;
			cz1 = fz+1;
			if(cx1>x->dim[1])
				cx1 = x->dim[1];
			if(cz1>x->dim[0])
				cz1 = x->dim[0];

			interpx = dx - fx;
			interpz = dz - fz;

//			val = posit[cz][cx];
			
//			pt0 = posit[cz][cx];
//			pt1 = posit[fz][cx1];
//			pt2 = posit[cz1][cx1];
//			pt3 = posit[cz1][fx];

/*			pt0 = posit[fz][fx];
			pt1 = posit[fz][cx1];
			pt2 = posit[cz1][cx1];
			pt3 = posit[cz1][fx];
*/			
			if(interpx<=0.5)  {								//left side of the thigie

				ival1 = my_map(interpx,0.,1.,pt0,pt1);

				ival2 = my_map(interpz,0.,1.,pt0,pt3);		
			
			
			} else if(interpx>0.5)  { //left side of the thigie

				ival1 = my_map(interpx,0.,1.,pt0,pt1);

				ival2 = my_map(interpz,0.,1.,pt1,pt2); //? who to interp 2d?

			}
			
			ival = ival1+ival2*0.5;
			
//			post("cx cz & val %ld %ld %f %f %f %f -- dx dz %f %f %f %f %f %f",cx, cz, val,pt1,pt2,pt3,dx,dz,interpx,interpz,ival1,ival2);

			x->object_xyz[1] = player[1]-val;

			if(interpx<0.5){ cx=fx; } else {cx=cx1; }
			if(interpz<0.5){ cz=fz; } else {cz=cz1; }
//			val = posit[cz][cx];

//			x->object_xyz[0] = player[1]-val;


//			cx++; if(cx>=x->dim[0]) { cx = x->dim[0]; }
//			val = posit[cz][cx];

//			val = MIN(pt0, pt1);
//			val = MIN(pt2, val);
//			val = MIN(pt3, val);

			x->object_xyz[2] = player[1]-ival;
			
			// store on [2] the one ahead


		} else if(!inside) {
//			error("not inside!");
		}

	}
	return JIT_ERR_NONE;
}




float my_map (float in, float min, float max, float imin, float imax)
{
//	float result = ((in-min) * ((float)imax-imin)/(max-min)+imin);
	float result = ((in-min) * (imax-imin)/(max-min)+imin);
	if(result<imin)
		result=imin;
	if(result>imax)
		result=imax;
	
	return (result);
}

/*
int find_zone_indexes(t_jit_gl_terreno *x,int i, int j){

int k;
Zona *z=NULL; 
int activezone = -1;

	for(k=0;k<x->numzonas;k++) {

			z=&x->zonas[k];
			if( i>=(int)z->z_box[0] && i < (int)z->z_box[2] && j>=(int)z->z_box[1] && j < (int)z->z_box[3]) {
		
//			if(i>=x->zonas[k].z_box[0] && i < x->zonas[k].z_box[2] 
//			  && j>=x->zonas[k].z_box[1] && j < x->zonas[k].z_box[3]) {

  //      println("zone IS "+k+" for");
		//			post("zone %ld %ld %ld",ki,i,j);
					activezone = k;
					goto done;
       
				}
	}

done:
	
		if(activezone==-1)
			activezone= x->defaultzone;
	return (activezone);

}

int find_zone_pos(t_jit_gl_terreno *x,float pos[3]){

}
*/
void calc_terrain(t_jit_gl_terreno *x)
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
	int i,j,rowstride;
	t_jit_matrix_info info;
	float red,green,blue,alpha;
	Terreno *ter;
	int trx,trz;
	int ti,tj;
	int spacing=x->spacing;
	int ii,jj;//,ki;activezone;
	
	int usersquare[2];
	int userrender[4];
//	int pzone=0;
		
	if (!x->chunk)
		return;

	if (!(m=x->chunk->m_vertex))
		return;
	
	jit_object_method(m,_jit_sym_getinfo,&info);
	jit_object_method(m,_jit_sym_getdata,&bp);
	
	if (!bp)
		return;
	
	rowstride = info.dimstride[1];//info.dimstride[1];
	
	jit_gl_terreno_get_color(x,&red,&green,&blue,&alpha);
	
	
	//find grid pos
	usersquare[0] = (int) (x->user[0] * x->goneoverspacing);
	usersquare[1] = (int) (x->user[2] * x->goneoverspacing);
	
	// min min max max
	userrender[0] =  usersquare[0] - x->terrenodim[0];
	userrender[1] =  usersquare[1] - x->terrenodim[1];
	userrender[2] =  usersquare[0] + x->terrenodim[0];
	userrender[3] =  usersquare[1] + x->terrenodim[1];
	
	int dimx = userrender[2]-userrender[0];
	int dimz = userrender[3]-userrender[1];
//	int numz = x->numzonas;
		
	float 	dimx1 = 1.0f/(float)dimx;
	float 	dimz1 = 1.0f/(float)dimz;
		
		//changed x to inner loop 
		
			ter = x->ter;//&x->ter;//[x->zonas[activezone].z_ter_id];
			trx = ter->resx;
			trz = ter->resz;
		
//	for (i=0;i<=nummajor;i++) {
	for (j=userrender[1],jj=0;j<userrender[3];j++,jj++) {
	//for (i=userrender[0],ii=0;i<userrender[2];i++,ii++) {
		float xx,xy;//was double
	//	xx = i*spacing; // i*majorstep-1.;
	  xy = j*spacing;
	
		p = (float *)(bp + jj*rowstride);//make sure access pointer from 00!! to its dim
//		p = (float *)(bp + ii*rowstride);//make sure access pointer from 00!! to its dim
//		p = (float *)(bp + ii*dimz);//make sure access pointer from 00!! to its dim
				
	//	for (j=userrender[1],jj=0;j<userrender[3];j++,jj++) {
		for (i=userrender[0],ii=0;i<userrender[2];i++,ii++) {
//			xy = j*spacing;
			xx = i*spacing;
		//	xx = 1.-j*minorstep;
		//	xx*= spacing;
			
			//find this zone & ter
//			int activezone = find_zone_indexes(x,i,j);

//			ter = &x->ter[x->zonas[activezone].z_ter_id];
//			trx = ter->resx;
//			trz = ter->resz;
			ti = i;
			tj = j;
			
			ti = ((ti%=trx)<0) ? ti+= trx : ti;
			tj = ((tj%=trz)<0) ? tj+= trz : tj;

			//vertex
			*p++ = xx; 
			*p++ = ter->posit[ti][tj];// posit[i][j]; // z == posit[i][j]
			*p++ = xy;

/*			//vertex
			*p++ = xx; 
			*p++ = posit[i][j]; // z == posit[i][j]
			*p++ = xy;
*/
			//texture
//			*p++ = j*numminor_inv; 
//			*p++ = i*nummajor_inv;
			*p++ = jj*dimz1;//texCoords[i][j][0]; 
			*p++ = ii*dimx1;//texCoords[i][j][1];

			//normals; 
			*p++ = ter->vertNorms[ti][tj][0];//vertNorms[i][j][0]; 
			*p++ =  ter->vertNorms[ti][tj][1]; 
			*p++ =  ter->vertNorms[ti][tj][2]; 
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
	int i,j,rowstride,width,height,planecount;
	t_jit_matrix_info info;

	if (!matrix) return;

	jit_object_method(matrix,_jit_sym_getinfo,&info);
	jit_object_method(matrix,_jit_sym_getdata,&bp);

	if (!bp) return;

	planecount	= info.planecount;
	rowstride 	= info.dimstride[1];
	height 		= info.dim[1]-1;//changed!!
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

//				glVertex3f(p2[_I_X],p2[_I_Y],p2[_I_Z]);
				glVertex3f(p2[_I_X],p2[_I_Y],p2[_I_Z]); //previous height
								
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

void pdraw_grid(t_jit_object *matrix, GLenum mode)
{
	float *p,*p2;
	char *bp=NULL;
	int i,j,rowstride,width,height,planecount;
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

float mapf (float x, float a, float b, float c, float d) 
{	
	return((x - a) * (d-c)/(b-a) + c);
}

// math -- VECTOR3 ops LIB
void copy(float vec0[3], float vec1[3])
{
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];
}


void mult(float vec0[3], float vec1[3])
{
    vec0[0] *= vec1[0];
    vec0[1] *= vec1[1];
    vec0[2] *= vec1[2];
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
	float c1 = (c==0.) ? 0. : 1.0/c;
    vec[0] *= c1; vec[1] *= c1; vec[2] *= c1;
}

void multf(float vec[3], float c1)
{
//	float c1 = (c==0.) ? 0. : 1.0/c;
    vec[0] *= c1; vec[1] *= c1; vec[2] *= c1;
}

void cross(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    vec0[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    vec0[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

void norm(float vec[3]) //IMPORTANT: CHANGED from sqrt call to  jit_math_sqrt
{
//    float c = jit_math_sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
     float c = jit_math_sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
   
	  scalDiv(vec, c); 
}

void set(float vec[3], float x, float y, float z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}


/*----------------------------------------------------------
	EOF
----------------------------------------------------------*/




