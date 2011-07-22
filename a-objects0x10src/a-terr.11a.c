/*
————————————————————————————————————————————————————————————————
————————————————————————————————————————————————————————————————

notas:

- alterada a ordem de cálculo de inserção de novos valores na lista:
1º por o valor, depois copyar paras as outras posições todas

- no draw0 terr_full alterei a ordem como se computa... i itera y's e j x's

————————————————————————————————————————————————————————————————
————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"
#include <math.h>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif
//lets be safe
#define MAXGRID	256	


typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out;		// outlet terr todo		
	void				*c_out2;	// outlet da posição z terr (dist z->xyz)
	void				*c_out3;	// outlet da grelha activa
	
	
		long				dim[2];  		/* dimensões <xy> da grelha */
//		double				pos[MAXGRID]; 	/* vector que alberga as posições a inserir na estrutura  */
		Boolean				shift;  		/* shifting do terr para a esquerda ou direita */

		double				coords[3]; 		/* posição do ponto para calcular distância  */
		long				wh[2]; 		/* "largura e comprimento a renderizar"  */
		short				om, draw;	/* optimizar o modo de saída e cálculo  */

		long				tail;	/* para o fifo rápido, fácil e indolor */
		short				dir;

	t_atom				out[MAXGRID];

		double	    posit[MAXGRID][MAXGRID];		/* vector que alberga as posições da estrutura toda. global?  */
		double		pos[MAXGRID][MAXGRID];

} terr;


//double	    posit[MAXGRID][MAXGRID];		/* vector que alberga as posições da estrutura toda. global?  */
//double		pos[MAXGRID][MAXGRID];



void *terr_new (Symbol *msg, short argc, Atom *argv);
void terr_bang (terr *x);

void terr_get_aktiv (terr *x); // main compute nomad interplay
void terr_get_full (terr *x); // main compute all interplay

void terr_list (terr *x, Symbol *msg, short argc, Atom *argv);
void terr_shift (terr *x, Symbol *msg, short argc, Atom *argv);
void terr_dim (terr *x, Symbol *msg, short argc, Atom *argv);
void terr_coords (terr *x, Symbol *msg, short argc, Atom *argv); // metodo distancia ponto a plano
void terr_coord (terr *x, Symbol *msg, short argc, Atom *argv); // metodo output z directo
void terr_core (terr *x, Symbol *msg, short argc, Atom *argv); // metodo output avg z 4 verts
void terr_wh (terr *x, Symbol *msg, short argc, Atom *argv);
void terr_om (terr *x, Symbol *msg, short argc, Atom *argv);
void terr_draw (terr *x, Symbol *msg, short argc, Atom *argv);
void terr_post (terr *x, Symbol *msg, short argc, Atom *argv);

void terr_dir (terr *x, Symbol *msg, short argc, Atom *argv);


void terr_assist(terr *x, void *b, long m, long a, char *s);
void *terr_class;

long wrap(long in, long min, long max);



//3d simple prototypes
void copy(double vec0[3], double vec1[3]);
void sub(double vec0[3], double vec1[3], double vec2[3]);
void add(double vec0[3], double vec1[3], double vec2[3]);
void scalDiv(double vec[3], double c);
void cross(double vec0[3], double vec1[3], double vec2[3]);
void norm(double vec[3]) ;
void set(double vec[3], double x, double y, double z);
void dot(double c, double vec1[3], double vec2[3]);




void *terr_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 terr *x;
	 int i,j;
	 
	 x=(terr *)newobject(terr_class);


	 x->c_out3=listout(x);// xy terr coords
	 x->c_out2=floatout(x);// output bang at end
	 x->c_out=listout(x);// xy terr coords
	 

	x->om = 0; x->draw = 0;
	x->dim[0] = x->dim[1] = 20;		
	x->coords[0] = x->coords[1] = x->coords[2] = 0.;
	x->dir = 0;		

	x->tail = MAXGRID; // important


		for (i=0; i<MAXGRID; i++) {
//			x->x->pos[i] = 0.0f;
			for (j=0; j<MAXGRID; j++) {
				x->pos[i][j] = x->posit[i][j] = 0.0f;
			}
		}





	if (argc) { // 
	 for(i=0;i<argc/2;i++){


		if(argv->a_w.w_sym == gensym("@om")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->om = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->om = (long)argv->a_w.w_float;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@dir")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->dir = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->dir = (long)argv->a_w.w_float;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@draw")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->draw = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->draw = (long)argv->a_w.w_float;	argv++;
			}
		}
		if(argv->a_w.w_sym == gensym("@dim")){
			argv++;
			if(argv->a_type == A_FLOAT) {
				x->dim[0] = (long)argv->a_w.w_float;	argv++;
				x->dim[1] = (long)argv->a_w.w_float;	argv++;
			} else if(argv->a_type == A_LONG) {
				x->dim[0] = (long)argv->a_w.w_float;	argv++;
				x->dim[1] = (long)argv->a_w.w_float;	argv++;
			}
		}


	 } // end for
	} // end argc
	 return(x);	
}



void terr_list (terr *x, Symbol *msg, short argc, Atom *argv)
{
	// list gets values inside geom
	long i, j, k, dim, tend;
	
	if (argc&&argv) { 

		// de certeza que não excede as dimensões
		dim = MIN(argc, x->dim[1]);

		// grandes alterações: thanks to jasch inspirational code

		// get list inside : write it always at the tail
		
		for (j=0; j < dim; j++) {
			if(argv->a_type == A_FLOAT) {
				x->pos[x->tail][j] = argv->a_w.w_float; argv++;
			} else if(argv->a_type == A_LONG) {
				x->pos[x->tail][j] = (double)argv->a_w.w_long; argv++;
			}
		}

		x->tail -= 1;													// moving backwards
		if (x->tail < 0){
				x->tail = MAXGRID; 										// ensure writepoint goes back to  MAXGRID
				}

	// este método já é para o output dos valores: eles já estão inseridos!
	// passá-los para dentro da estrutura que os vai referenciar sempre: array x->posit
		
		if (!x->dir){																	
			for (i = 0; i<x->dim[1]; i++){ 								
				for(j=(x->dim[0]-1); j>=0; --j){
					tend = wrap (i + x->tail + 1, (long) 0, (long)(MAXGRID));   // simply  wrap the coords across the valid N*num_len range
					x->posit[i][j] = x->pos[tend][j];
				}
			}
		// tá a crashar...	claro, i estava a incrementar
		} else if (x->dir){
			for (i = x->dim[1]-1, k=0; i>=0; i--, k++){ 								
				for(j=(x->dim[0]-1); j>=0; --j){
					tend = wrap (i + x->tail + 1, (long) 0, (long)(MAXGRID));   // simply  wrap the coords across the valid N*num_len range
					x->posit[k][j] = x->pos[tend][j];
				}
			}


		}



	}

}

void terr_shift (terr *x, Symbol *msg, short argc, Atom *argv)
{
	long i = 0;
	long j = 0;
	short val;

	if (argc&&argv) {
		for (i=0; i < argc; i++) 
			if(argv->a_type == A_FLOAT) {
				val = argv->a_w.w_long;
			} else if(argv->a_type == A_LONG) {
				val = argv->a_w.w_long;
			}

		//changed directions to fit
		if (!val) { //shifting destra
			for(i=x->dim[1]-1; i>=0; --i)
			   for(j=x->dim[0]-1; j>=0; --j)
				 x->posit[i+1][j] = x->posit[i][j+1];	//shift 1 destra			

		} else if (val) { //shifting sinistra
			for(i=x->dim[1]-1; i>=0; --i)
			   for(j=x->dim[0]-1; j>=0; --j)
	   			 x->posit[i+1][j+1] = x->posit[i][j];	//shift 1 sinistra			
		}


	}

}
void terr_dim (terr *x, Symbol *msg, short argc, Atom *argv)
{
	long i;
	
	if (argc&&argv) {
		for (i=0; i < 2; i++) {
			if(argv->a_type == A_FLOAT) {
				x->dim[i] = (long)argv->a_w.w_float; argv++;
			} else if(argv->a_type == A_LONG) {
				x->dim[i] = (long)argv->a_w.w_long; argv++;
			}
		
		}



	}

}
void terr_coords (terr *x, Symbol *msg, short argc, Atom *argv)
{
	long w, h, i, j;

    double vec0[3], vec1[3], vec2[3], norm0[3], norm1[3];
    double geom0[3], geom1[3], geom2[3], geom3[3];

	double d1, d2, td;
	double coords[3];
	
	
	
	if (argc&&argv) { // get input inside the object -- 3D!
		for (i=0; i < 3; i++) {
			if(argv->a_type == A_FLOAT) {
				x->coords[i] = (double)argv->a_w.w_float; argv++;
			} else if(argv->a_type == A_LONG) {
				x->coords[i] = (double)argv->a_w.w_long; argv++;
			}
		
		}


	// make sure coords within geom bounds
	// importante, tb troquei x, y
	if (x->coords[0]>x->dim[1])
		x->coords[0] = x->dim[1];
	if (x->coords[0]<0)
		x->coords[0] = 0;
	if (x->coords[1]>x->dim[0])
		x->coords[1] = x->dim[0];
	if (x->coords[1]<0)
		x->coords[1] = 0;


	set(coords, x->coords[0], x->coords[1],x->coords[2]);
	

	// passar para o ponto que vamos ver a normal
	i = (long) coords[0];
	j = (long) coords[1];

	// 1º normal de 3 pontos do plano, clockwise

	/* get vectors from geometry points */
	geom0[0] = i; geom0[1] = j; geom0[2] = x->posit[i][j];
	geom1[0] = i; geom1[1] = j+1; geom1[2] = x->posit[i][j+1];
	geom2[0] = i+1; geom2[1] = j; geom2[2] = x->posit[i+1][j]; //geom2 devia ser geom3, clockwise!
//	geom3[0] = i+1; geom3[1] = j+1; geom3[2] = x->posit[i+1][j+1];

    sub( vec0, geom1, geom0 );
    sub( vec1, geom1, geom2 );
//    sub( vec2, geom1, geom3 );

	cross(norm0, vec0, vec1);
	norm(norm0);
	
//	d1 = norm0[0]*
//	dot(d1, geom0, norm0);
	
	d1 = geom0[0]*norm0[0] + geom0[1]*norm0[1] + geom0[2]*norm0[2] ;
	
//	post("geom0 %f %f %f norm0 %f %f %f", geom0[0],geom0[1],geom0[2],norm0[0], norm0[1], norm0[2]);
//	post("d1 %f ", d1);
//	post("td %f ", td);
	
	
	// já temos a equação do plano em que nos encontramos
	// agora encontramos a diferença para 1 ponto arbitrário do plano
	
//	dot(d2, norm0, coords);
	d2 = coords[0]*norm0[0] + coords[1]*norm0[1] + coords[2]*norm0[2] ;

	d2 += d1; // adicionar D para encontrar a distância relativamente ao plano!
	
	outlet_float (x->c_out2, d2); // fazer o output do valor


	// do outro triangulo do plano
//	cross(norm1, vec1, vec2);
//	norm(norm1);



	}

}
void terr_coord (terr *x, Symbol *msg, short argc, Atom *argv) // simples output z
{
	long  i, j;

	double coords[3], d2;
	
	
	
	if (argc&&argv) { // get input inside the object -- 3D!
		for (i=0; i < 3; i++) {
			if(argv->a_type == A_FLOAT) {
				x->coords[i] = (double)argv->a_w.w_float; argv++;
			} else if(argv->a_type == A_LONG) {
				x->coords[i] = (double)argv->a_w.w_long; argv++;
			}
		
		}


	// make sure coords within geom bounds
	// importante, tb troquei x, y
	if (x->coords[0]>x->dim[1])
		x->coords[0] = x->dim[1];
	if (x->coords[0]<0)
		x->coords[0] = 0;
	if (x->coords[1]>x->dim[0])
		x->coords[1] = x->dim[0];
	if (x->coords[1]<0)
		x->coords[1] = 0;


	set(coords, x->coords[0], x->coords[1],x->coords[2]);
	

	// passar para o ponto que vamos ver a normal
	i = (long) (coords[0]+0.5);
	j = (long) (coords[1]+0.5);


	d2 = x->posit[i][j];

	
	outlet_float (x->c_out2, d2); // fazer o output do valor




	}

}
void terr_core (terr *x, Symbol *msg, short argc, Atom *argv) // output avg vertices z
{
	long  i, j;

	double coords[3], d[4], d2;
	
	
	
	if (argc&&argv) { // get input inside the object -- 3D!
		for (i=0; i < 3; i++) {
			if(argv->a_type == A_FLOAT) {
				x->coords[i] = (double)argv->a_w.w_float; argv++;
			} else if(argv->a_type == A_LONG) {
				x->coords[i] = (double)argv->a_w.w_long; argv++;
			}
		
		}


	// make sure coords within geom bounds
	// importante, tb troquei x, y
	if (x->coords[0]>x->dim[1])
		x->coords[0] = x->dim[1];
	if (x->coords[0]<0)
		x->coords[0] = 0;
	if (x->coords[1]>x->dim[0])
		x->coords[1] = x->dim[0];
	if (x->coords[1]<0)
		x->coords[1] = 0;


	set(coords, x->coords[0], x->coords[1],x->coords[2]);
	

	// passar para o ponto que vamos ver a normal
	i = (long) (coords[0]+0.5);
	j = (long) (coords[1]+0.5);


	d[0] = x->posit[i][j];
	d[1] = x->posit[i+1][j];
	d[2] = x->posit[i+1][j+1];
	d[3] = x->posit[i][j+1];

	d2 = (d[0]+d[1]+d[2]+d[3])/4.;
	
	outlet_float (x->c_out2, d2); // fazer o output do valor




	}

}
void terr_post (terr *x, Symbol *msg, short argc, Atom *argv)
{
	long i, j;
	
	for (i=0; i<x->dim[1]; i++)
		for (j=0; j<x->dim[0]; j++)
			post("%02ld %02ld __ %f", i, j, x->posit[i][j]);

}

void terr_wh (terr *x, Symbol *msg, short argc, Atom *argv)
{

}
void terr_om (terr *x, Symbol *msg, short argc, Atom *argv)
{
	if (argc&&argv) {
			if(argv->a_type == A_FLOAT) {
				x->om = (long)argv->a_w.w_float;
			} else if(argv->a_type == A_LONG) {
				x->om = (long)argv->a_w.w_long;
			}
	}
}
void terr_dir (terr *x, Symbol *msg, short argc, Atom *argv)
{
	if (argc&&argv) {
			if(argv->a_type == A_FLOAT) {
				x->dir = (long)argv->a_w.w_float;
			} else if(argv->a_type == A_LONG) {
				x->dir = (long)argv->a_w.w_long;
			}
	}
}
void terr_draw (terr *x, Symbol *msg, short argc, Atom *argv)
{
	if (argc&&argv) {
			if(argv->a_type == A_FLOAT) {
				x->draw = (long)argv->a_w.w_float;
			} else if(argv->a_type == A_LONG) {
				x->draw = (long)argv->a_w.w_long;
			}
	}

}

void terr_get_aktiv (terr *x)
{// main compute nomad interplay


} 
void terr_get_full (terr *x)
{ // main compute all interplay
	t_atom *o;
	long i, j;





	
	o = x->out;
	

	if (x->draw==0) {
		//for wireframe right // points
		for (i=0; i<x->dim[1]; i++) {
			for (j=0; j<x->dim[0]; j++) {
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				outlet_list(x->c_out, 0L, 3, o);
			}
		}
	} else if (x->draw==1) {
		//for GL_TRI_STRIP
		for (i=0; i<x->dim[1]; i++) {
			for (j=0; j<x->dim[0]; j++) {
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				outlet_list(x->c_out, 0L, 3, o);
				SETLONG (o+0, i+1);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i+1][j]);
				outlet_list(x->c_out, 0L, 3, o);
			}
		}
	} else if (x->draw==2) {
		//for _LINE_COMMANDS_x
		for (i=0; i<x->dim[1]-1; i++) { //-1!!
			for (j=0; j<x->dim[0]; j++) {
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				SETLONG (o+3, i+1);
				SETLONG (o+4, j);
				SETFLOAT (o+5, x->posit[i+1][j]);
				outlet_list(x->c_out, 0L, 6, o);
			}
		}
	} else if (x->draw==3) {
		//for _LINE_COMMANDS_y
		for (i=0; i<x->dim[1]; i++) {
			for (j=0; j<x->dim[0]-1; j++) { //-1!!
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				SETLONG (o+3, i);
				SETLONG (o+4, j+1);
				SETFLOAT (o+5, x->posit[i][j+1]);
				outlet_list(x->c_out, 0L, 6, o);
			}
		}
	} else if (x->draw==4) {
		//for _LINE_COMMANDS_x
		for (i=0; i<x->dim[1]-1; i++) { //-1!!
			for (j=0; j<x->dim[0]; j++) {
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				SETLONG (o+3, i+1);
				SETLONG (o+4, j);
				SETFLOAT (o+5, x->posit[i+1][j]);
				outlet_list(x->c_out, 0L, 6, o);
			}
		}
		//for _LINE_COMMANDS_y
		for (i=0; i<x->dim[1]; i++) {
			for (j=0; j<x->dim[0]-1; j++) { //-1!!
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				SETLONG (o+3, i);
				SETLONG (o+4, j+1);
				SETFLOAT (o+5, x->posit[i][j+1]);
				outlet_list(x->c_out, 0L, 6, o);
			}
		}


	} else if (x->draw==5) {
		//for wireframe left
		for (i=0; i<x->dim[0]; i++) {
			for (j=0; j<x->dim[1]; j++) {
				SETLONG (o+0, j);
				SETLONG (o+1, i);
				SETFLOAT (o+2, x->posit[j][i]);
				outlet_list(x->c_out, 0L, 3, o);
			}
		}
	} else if (x->draw==6) {
		//for wireframe crossed
		for (i=0; i<x->dim[0]; i++) {
			for (j=0; j<x->dim[1]; j++) {
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				outlet_list(x->c_out, 0L, 3, o);
			}
		}
		for (i=0; i<x->dim[0]; i++) {
			for (j=0; j<x->dim[1]; j++) {
				SETLONG (o+0, j);
				SETLONG (o+1, i);
				SETFLOAT (o+2, x->posit[j][i]);
				outlet_list(x->c_out, 0L, 3, o);
			}
		}
	} else if (x->draw==7) {
		//for GL_TRI_STRIP
		for (i=0; i<x->dim[0]; i++) {
			for (j=0; j<x->dim[1]; j++) {
				SETLONG (o+0, i);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i][j]);
				outlet_list(x->c_out, 0L, 3, o);
				SETLONG (o+0, i+1);
				SETLONG (o+1, j);
				SETFLOAT (o+2, x->posit[i+1][j]);
				outlet_list(x->c_out, 0L, 3, o);
			}
		}

	}
}


void terr_bang (terr *x)
{

	if (x->om>1) {	
		terr_get_aktiv(x);
		return;
	} 	else if (x->om>=0) {	
		terr_get_full(x);
		return;
	}

	
}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&terr_class,(method)terr_new,0L,(short)sizeof(terr),0L,
 A_GIMME,0);

 addbang((method)terr_bang);
 addmess((method)terr_list,"list",A_GIMME, 0);
 addmess((method)terr_shift,"shift",A_GIMME, 0);
 addmess((method)terr_dim,"dim",A_GIMME, 0);
 addmess((method)terr_coords,"coords",A_GIMME, 0);
 addmess((method)terr_coord,"coord",A_GIMME, 0);
 addmess((method)terr_core,"core",A_GIMME, 0);
 addmess((method)terr_wh,"wh",A_GIMME, 0);
 addmess((method)terr_om,"om",A_GIMME, 0);
 addmess((method)terr_draw,"draw",A_GIMME, 0);
 addmess((method)terr_dir,"dir",A_GIMME, 0);
 addmess((method)terr_post,"post",A_GIMME, 0);


 addmess((method)terr_assist,"assist", A_CANT, 0);

 post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-terr   ©   a n d r é s i e r   2 0 0 4   all rights reserved",tick, 0);

}


void terr_assist(terr *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(list) terr     e         c            h              o"); }
    else if (m==2&&a==1) { sprintf(s,"(double) terr z    e         c            h              o"); }
    else if (m==2&&a==2) { sprintf(s,"(list) terr aktiv    e         c            h              o"); }
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



// math -- VECTOR3 ops LIB  WITH DOT PRODUCT
void copy(double vec0[3], double vec1[3])
{
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];
}

void sub(double vec0[3], double vec1[3], double vec2[3])
{
    vec0[0] = vec1[0] - vec2[0];
    vec0[1] = vec1[1] - vec2[1];
    vec0[2] = vec1[2] - vec2[2];
}

void add(double vec0[3], double vec1[3], double vec2[3])
{
    vec0[0] = vec1[0] + vec2[0];
    vec0[1] = vec1[1] + vec2[1];
    vec0[2] = vec1[2] + vec2[2];
}

void scalDiv(double vec[3], double c)
{
    vec[0] /= c; vec[1] /= c; vec[2] /= c;
}

void cross(double vec0[3], double vec1[3], double vec2[3])
{
    vec0[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    vec0[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    vec0[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

void norm(double vec[3]) 
{
    double c = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    scalDiv(vec, c); 
}

void set(double vec[3], double x, double y, double z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}


// não funciona
void dot(double d, double vec1[3], double vec2[3]) 
{
    d = vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
}




/*----------------------------------------------------------
	EOF
----------------------------------------------------------*/




