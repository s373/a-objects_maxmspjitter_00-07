/*

ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
polys - © andrŽ sier 2003
		
		objecto que recebe lista de valores validos de output 
		de coordenadas, double entre 0. e 1. itera entre esses
		valores...
		
		um tipo de multiplas seleccoes, polys(election)
			
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
*/

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ
#include "ext.h"
#include "ext_common.h"
//#include "math.h"
//
//#ifndef M_PI
//#define M_PI	3.14159265358979323846
//#endif

//#define pi2		2 * M_PI

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ
typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out[2];		// outlet value	+ zone outlet value

	double			value;			// o valor de saida registado
	long			val;			// o valor int, no caso de programado
	t_atom			list[256];		// a lista dos elementos de seleccao
	short			num;			// o numero de elementos de intervalo de
									// seleccao. algo como argc / 2									
	short			curr_zone;
	Boolean			om;
	double			input[2];
} polys;

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ

void *polys_new 		(Symbol *msg, short argc, Atom *argv);
void polys_bang 		(polys *x);

void polys_echo 		(polys *x, double echo);
void polys_int			(polys *x, int n);
void polys_float		(polys *x, double n);
void polys_list 		(polys *x, Symbol *msg, short argc, Atom *argv);

void polys_input 		(polys *x, double echo, double ech);
void polys_om 			(polys *x, long echo);
void polys_post 		(polys *x);
void polys_assist		(polys *x, void *b, long m, long a, char *s);
void *polys_class;

//double mapvalue 			(double x, double a, double b, double c, double d) ; // the double mapping version

// ÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐÐ

void *polys_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 polys *x;

	 t_atom *list;
	 int i;
	 
	 
	 
	 x=(polys *)newobject(polys_class); // primeira ordem. importante.

	 list = x->list; // store list in the list vector

	x->c_out[1]=intout(x);		// outlet da zona actual
	x->c_out[0]=floatout(x);	// outlet do valor

	if (argc) { // se houver argumentos
/*		switch (argv->a_type) {
			case A_LONG:  x->c_out=intout(x); x->om = 0; break;
			case A_FLOAT:  x->c_out=floatout(x); x->om = 1; break;
		}
*/
		x->input[0] = 0.f;
		x->input[1] = 0.f;

		x->num = argc / 2; // guardar o numero de seleccoes q vamos ter
		x->curr_zone = 0;	
		x->om = 0;
		for (i=0; i<argc; i++) {
			if (argv[i].a_type==A_LONG) {
			  x->list[i].a_w.w_float = (double)argv[i].a_w.w_long; //copiar a lista da entrada 
			}													  //para o interior do objecto
			else if (argv[i].a_type==A_FLOAT) {
			  x->list[i].a_w.w_float = argv[i].a_w.w_float; //copiar a lista da entrada 
			}													  //para o interior do objecto
			else if (argv[i].a_type==A_SYM) {
				error("%s wrong argument type for polys", argv[i].a_w.w_sym,0);
			}
		}			
	} else if (!argc)  {
//		 x->c_out[1]=intout(x);
//		 x->c_out[0]=floatout(x);
		 
		  x->om = 1;
		 x->num = 1; //two default args
		 x->list[0].a_w.w_float = 0.;
		 x->list[1].a_w.w_float = 1.;
   		 x->curr_zone = 0;	
		x->input[0] = 0.f;
		x->input[1] = 0.f;
	}	

	 return(x);	
}




void polys_post (polys *x)
{
	short i;
post(" 	");
post(" 	");
post("  a-polys 004 20031128");
post(" 	");
post(" value    = %f", x->value);
post(" num_intervals   = %ld", (short)x->num*2);

	for (i=0; i<(short)x->num*2; i++)
		post(" interval_point[%ld]    = %f", i, x->list[i].a_w.w_float);

post(" om          = %ld", x->om);
post(" zonas          = %ld", x->num);
post(" zona          = %ld", x->curr_zone);
post(" 		");
post(" 	");
}


void polys_echo (polys *x, double echo) //compute directly from number, no iter
{
	
	short zonas = x->num;
	double ratio = 1. / zonas; // ratio de floats por intervalo na zona
	double zona  = 0.0;
	double zona_max = 1.0;
	double	dstmin = 0.0;
	double	dstmax = 0.0f;
	double	temp = 0.0f;	
	double	srcmin, srcmax;
	
	srcmin = x->input[0];
	srcmax = x->input[1];

//	post ("      echo_called with %f",echo,0);

	zona = CLIP (echo / ratio, 0., (double)zonas); // de certeza q a zona nao sai do intervalo permitido
	x->curr_zone = (short)zona;

//	post ("ratio %f zona %f curr_zone %ld", ratio, zona, x->curr_zone,0);
	
	zona_max = zona + ratio; // o proximo intervalor comecara ali/aqui
	dstmin = x->list[x->curr_zone*2].a_w.w_float;	// fix zona 2 list posit
	dstmax = x->list[x->curr_zone*2+1].a_w.w_float;	
	
	srcmin = (double)ratio*x->curr_zone;
	srcmax = (double)ratio*(x->curr_zone+1);
	
//	post ("mapa entre %f %f para %f %f", srcmin, srcmax, dstmin, dstmax,0);

//	temp = echo;	
//	mapvalue (temp, zona, zona_max, dstmin, dstmax);		
//	mapvalue (temp, srcmin, srcmax, dstmin, dstmax);		

	if (echo == 1.0f) {
		temp = dstmax;
	} else {
		temp = ((echo - srcmin) * (dstmax-dstmin)/(srcmax-srcmin) + dstmin); //calculating temp
	}
	x->value = temp;
//	post ("valor %f temp %f", x->value,temp, 0);

	polys_bang(x);
}

void polys_int(polys *x, int n)     { polys_echo(x,n*0.01);}
void polys_float(polys *x, double n) { polys_echo(x, n);}


void polys_list (polys *x, Symbol *msg, short argc, Atom *argv)
{
	short i;

	if (argc) { 
		x->num = argc / 2; 
		x->curr_zone = 0;	
		x->om = 0;
		for (i=0; i<argc; i++) {
			if (argv[i].a_type==A_LONG) {
			  x->list[i].a_w.w_float = (double)argv[i].a_w.w_long; //copiar a lista da entrada 
			}													  //para o interior do objecto
			else if (argv[i].a_type==A_FLOAT) {
			  x->list[i].a_w.w_float = argv[i].a_w.w_float; //copiar a lista da entrada 
			}													  //para o interior do objecto
			else if (argv[i].a_type==A_SYM) {
				error("%s wrong argument type for polys", argv[i].a_w.w_sym,0);
			}
		}			
	}	
	
}

void polys_input 		(polys *x, double echo, double ech)
{
	x->input[0] = echo;
	x->input[1] = ech;
}


void polys_om (polys *x, long echo)
{
	x->om = echo;
}


void polys_bang (polys *x)
{
	
	outlet_int	(x->c_out[1], x->curr_zone); // mais eficaz fazer output apenas se mudou!...
	outlet_float(x->c_out[0], x->value); 
}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&polys_class,(method)polys_new,0L,(short)sizeof(polys),0L,
 A_GIMME,0);

 addbang((method)polys_bang);
 addfloat((method)polys_float);
 addint((method)polys_int);

 addmess((method)polys_list,"list",A_GIMME,0);
 addmess((method)polys_input,"input",A_DEFFLOAT,A_DEFFLOAT, 0);
 addmess((method)polys_om,"om",A_DEFLONG,0);
 addmess((method)polys_post,"post",0);
 addmess((method)polys_assist,"assist", A_CANT, 0);

 post("	 \t \a \t  %08xd	  			a-polys © andrŽ sier 2003-7   "__TIME__,tick,0);
}


void polys_assist(polys *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"e c ho    e   c  h   o"); }   
    else if (m==2&&a==0)  { sprintf(s,"(double) polys   e         c            h              o"); }
	else if (m==2&&a==1)  {sprintf(s,"(int) polys zone  e         c            h             o"); }
}


