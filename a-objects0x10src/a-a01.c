/*����������������������������������������������������������������a - � andr� sier 2002				retorna o n�mero de ouro, 		numero irracional dado pela d�zima infinita n�o peri�dica			L= (1+�5)/2����������������������������������������������������������������*/#include "ext.h"#include "ext_common.h"#include "math.h"typedef struct	{	t_object 	        c_ob;				void 		        *c_out;		// value		double L; //gold	double in;} ouro;void *ouro_new (Symbol *msg, short argc, Atom *argv);void ouro_bang (ouro *x);void ouro_calc (ouro *x);void ouro_echo (ouro *x, double echo);void ouro_int(ouro *x, int n);void ouro_float(ouro *x, double n);void ouro_assist(ouro *x, void *b, long m, long a, char *s);void *ouro_class;void *ouro_new (Symbol *msg, short argc, Atom *argv) //input the args {	 ouro *x;	 int i;	 	 x=(ouro *)newobject(ouro_class);	 x->c_out=floatout(x);	 	if (argc) {	} else   {		x->in = 1.000f;	}		 return(x);	}void  ouro_calc (ouro *x){	double L;		L = (1+sqrt(5))/2.000000f;//ouro		if (x->in != 1.0)		L = x->in * L;			x->L = L;}void ouro_echo (ouro *x, double echo){ 	x->in = echo; 	ouro_bang(x);}void ouro_int(ouro *x, int n)     { ouro_echo(x, n);}void ouro_float(ouro *x, double n) { ouro_echo(x, n);}void ouro_bang (ouro *x){	double temp;		temp = x->L; //previous    ouro_calc(x);   //next        if (temp!=x->L)outlet_float(x->c_out, x->L); //output only changed values}void main(void){ long int tick = gettime(); setup((t_messlist**)&ouro_class,(method)ouro_new,0L,(short)sizeof(ouro),0L, A_GIMME,0); addbang((method)ouro_bang);  addint((method)ouro_int); addfloat((method)ouro_float); addmess((method)ouro_assist,"assist", A_CANT, 0);// post("Media, e Caos",0); post("	 \t \a \t  %08xd	  			a-a � A-Sier 2003"__TIME__,tick,0);}void ouro_assist(ouro *x, void *b, long m, long a, char *s){    if (m==1) { sprintf(s,"echo e   c  h   o"); }       else if (m==2&&a==0) { sprintf(s,"(double)     e         c            h              o"); }    else if (m==2&&a==1) { sprintf(s,"(lisy)       e             c                h                   o"); }}