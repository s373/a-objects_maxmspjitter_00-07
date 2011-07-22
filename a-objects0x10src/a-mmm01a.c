/*

————————————————————————————————————————————————————————————————
mmm - © andré sier 2002
	min, max, mean of input	
		
————————————————————————————————————————————————————————————————
*/

#include "ext.h"
#include "ext_common.h"
#include <math.h>




typedef struct	
{
	t_object 	        c_ob;			
	void 		        *c_out4;  //dump output			
	void 		        *c_out3;		// value
	void 		        *c_out2;  //dvalue			
	void 		        *c_out;		// value

//	double	mean, max, min;
	double  echo, m[3], coef, pre[3];
	
	double	sum, quantity, avg; //to calc the avg
	
	Boolean silence, exe[3];
	char	mode;
	Atom	listo[5];

} mmm;


void *mmm_new (Symbol *msg, short argc, Atom *argv);
void mmm_bang (mmm *x);
void mmm_7min (mmm *x, double p);
void mmm_7max (mmm *x, double v);
void mmm_7mean (mmm *x, double mass);
void mmm_7coef (mmm *x, double mass);

void mmm_echo (mmm *x, double echo);							// store inside
void  mmm_calc (mmm *x);									// calculat

void mmm_silence (mmm *x, int sil);							// exe on arrival
void mmm_mode (mmm *x, int sil);
void mmm_reset(mmm *x);
void mmm_mmm (mmm *x, Symbol *msg, short argc, Atom *argv); // set min, max, mean
void mmm_exe (mmm *x, Symbol *msg, short argc, Atom *argv); // toggle internal cals
void mmm_getmmm (mmm *x);
void mmm_getmode (mmm *x);


void mmm_int(mmm *x, int n);
void mmm_float(mmm *x, double n);

void mmm_assist(mmm *x, void *b, long m, long a, char *s);
void *mmm_class;

t_symbol *mod;
double mean[1000];


// 						-

void *mmm_new (Symbol *msg, short argc, Atom *argv) //input the args 
{
	 mmm *x;
	 int i;
	 
	 x=(mmm *)newobject(mmm_class);

	 x->c_out4=listout(x);
	 x->c_out3=floatout(x);
	 x->c_out2=floatout(x);
	 x->c_out=floatout(x);
	 
	 mmm_reset(x);

	if (argc) {

		for (i=0; i<argc/2; i++){ //jitter style setup
		
		if(argv->a_w.w_sym == gensym("@exe")){
			argv++;
			x->exe[0] = argv->a_w.w_long; argv++;
			x->exe[1] = argv->a_w.w_long;	argv++;
			x->exe[2] = argv->a_w.w_long;	argv++;
			}
		if(argv->a_w.w_sym == gensym("@mmm")){
			argv++;
			x->m[0] = argv->a_w.w_float; argv++;
			x->m[1] = argv->a_w.w_float;	argv++;
			x->m[2] = argv->a_w.w_float;	argv++;
			}
		if(argv->a_w.w_sym == gensym("@mode")){
			argv++;
			x->mode = argv->a_w.w_long; argv++;
			}

		if(argv->a_w.w_sym == gensym("@silence")){
			argv++;
			x->silence = argv->a_w.w_long; argv++;
			}

		} 

	} else   { //set up the basic stuff
		x->m[0] = 2147483647.;
		x->m[1] = -2147483647.;	
		x->m[2] = 0.;	
		x->exe[0] = 1;
		x->exe[1] = 1;	
		x->exe[2] = 1;	
	}	



		x->pre[0] = 0.;
		x->pre[1] = 0.;	
		x->pre[2] = 0.;	


	for(i=0; i<1000; i++)
		mean[i] = 0.0f;
		
	 return(x);	
}

void mmm_reset(mmm *x)
{
		x->m[0] = 2147483647.;
		x->m[1] = -2147483647.;	
		x->m[2] = 0.;	

	x->echo = 0.0f;
	x->silence = 0;
	x->mode = 0;
	
	x->coef = 0.999;
	
	x->sum = 0.;
	x->quantity = 0.;
	x->avg = 0.;

}

void  mmm_calc (mmm *x)
{
	double tem;
	int i;
	double sum = x->sum, qua = x->quantity, avg = x->avg;
	
	
	//calc min
	if (x->exe[0]){
		tem = x->m[0];
		if (x->echo<tem) x->m[0]=x->echo;
	}
	//calc max
	if (x->exe[1]){
		tem = x->m[1];
		if (x->echo>tem) x->m[1]=x->echo;
	}
	//calc mean
	if (x->exe[2]){

		sum += x->echo; 	//sum the sample with the rest of the amount
		qua += 1; 			//increment the samples number
		x->m[2] = sum / qua;//register average

		x->sum = sum;
		x->quantity = qua;
		x->avg = x->m[2];
	}
	
}



void mmm_mmm (mmm *x, Symbol *msg, short argc, Atom *argv)
{
	x->m[0] =  argv->a_w.w_float; argv++;
	x->m[1] =  argv->a_w.w_float; argv++;
	x->m[2] =  argv->a_w.w_float; argv++;

}

void mmm_exe (mmm *x, Symbol *msg, short argc, Atom *argv)
{
	x->exe[0] =  argv->a_w.w_long; argv++;
	x->exe[1] =  argv->a_w.w_long; argv++;
	x->exe[2] =  argv->a_w.w_long; argv++;

}

void mmm_getmmm (mmm *x)
{

	SETSYM  (x->listo+0, gensym("mmm"));
	SETFLOAT(x->listo+1, x->m[0]);
	SETFLOAT(x->listo+2, x->m[1]);
	SETFLOAT(x->listo+3, x->m[2]);
	SETLONG (x->listo+4, x->mode);
	outlet_list(x->c_out4, 0L, 5, x->listo);
}

void mmm_getmode (mmm *x)
{

	SETSYM  (x->listo+0, gensym("mode"));
	SETLONG(x->listo+1, x->mode);
	SETSYM(x->listo+2, mod);
	outlet_list(x->c_out4, 0L, 3, x->listo);
}

void mmm_getsamples (mmm *x)
{

	SETSYM  (x->listo+0, gensym("samples"));
	SETLONG(x->listo+1, x->quantity);
//	SETSYM(x->listo+2, mod);
	outlet_list(x->c_out4, 0L, 2, x->listo);
}
void mmm_getsum (mmm *x)
{

	SETSYM  (x->listo+0, gensym("sum"));
	SETLONG(x->listo+1, x->sum);
//	SETSYM(x->listo+2, mod);
	outlet_list(x->c_out4, 0L, 2, x->listo);
}


void mmm_7min (mmm *x, double p)
{
 	x->m[0] = p;
}
void mmm_7max (mmm *x, double v)
{
 	x->m[1] = v;
}
void mmm_7mean (mmm *x, double mass)
{
	x->m[2] = mass;
}
void mmm_7coef (mmm *x, double mass)
{
	x->coef = mass;
}

void mmm_echo (mmm *x, double echo)
{
 	x->echo = echo;
 	if (!x->silence) mmm_bang(x);
}

//goto echo
void mmm_int(mmm *x, int n)     { mmm_echo(x, n);}
void mmm_float(mmm *x, double n) { mmm_echo(x, n);}



void mmm_silence (mmm *x, int sil)
{
 	if (sil>0)
 		x->silence = 1;
 	else 
 	    x->silence = 0;
}

void mmm_mode (mmm *x, int sil)
{
	if (sil<=0)
 		x->mode = 0;
 	else if (sil > 1)
 		x->mode = 1;
 	else
 		x->mode = sil;
 		

	if (x->mode == 0) mod = gensym("@mean");
	else if (x->mode == 1) mod = gensym("@power");
//	else if (x->mode == 2) mod = gensym("wrap");
//	else if (x->mode == 3) mod = gensym("fold");
	

}


void mmm_bang (mmm *x)
{
	double temp[3];
	
	temp[0] = x->m[0]; //previous
	temp[1] = x->m[1]; //previous
	temp[2] = x->m[2]; //previous

    mmm_calc(x);   //next
    
    if(temp[0] != x->m[0]) outlet_float(x->c_out , x->m[0]); //output only changed values
    if(temp[1] != x->m[1]) outlet_float(x->c_out2, x->m[1]); //output only changed values
    if(temp[2] != x->m[2]) outlet_float(x->c_out3, x->m[2]); //output only changed values
}



void main(void)
{
 long int tick = gettime();
 setup((t_messlist**)&mmm_class,(method)mmm_new,0L,(short)sizeof(mmm),0L,
 A_GIMME,0);

 addbang((method)mmm_bang);
 addmess((method)mmm_7min,"setmin", A_DEFFLOAT, 0);
 addmess((method)mmm_7max,"setmax", A_DEFFLOAT, 0);
 addmess((method)mmm_7mean,"setmean", A_DEFFLOAT, 0);
 addmess((method)mmm_7coef,"coef", A_DEFFLOAT, 0);
 addmess((method)mmm_echo,"echo", A_DEFFLOAT, 0);
 addmess((method)mmm_silence,"silence", A_DEFLONG, 0);
 addmess((method)mmm_mode,"mode", A_DEFLONG, 0);
 addmess((method)mmm_mmm,"mmm", A_GIMME, 0);
 addmess((method)mmm_exe,"exe", A_GIMME, 0);
 addmess((method)mmm_reset,"reset", 0);
 addmess((method)mmm_getmmm,"getmmm", 0);
 addmess((method)mmm_getmode,"getmode", 0);
 addmess((method)mmm_getsamples,"getsamples", 0);
 addmess((method)mmm_getsum,"getsum", 0);
 
 addint((method)mmm_int);
 addfloat((method)mmm_float);
 addmess((method)mmm_assist,"assist", A_CANT, 0);
 post("	 \t \a \t  %08xd	  			a-mmm © A-Sier 2003"__TIME__,tick,0);
}


void mmm_assist(mmm *x, void *b, long m, long a, char *s)
{
    if (m==1) { sprintf(s,"echo e   c  h   o"); }   
    else if (m==2&&a==0) { sprintf(s,"(double) m i n    e         c            h              o"); }
    else if (m==2&&a==1) { sprintf(s,"(double)  m a x     e             c                h                   o"); }
    else if (m==2&&a==2) { sprintf(s,"(double)  m e a n     e             c                h                   o"); }
    else if (m==2&&a==3) { sprintf(s,"(dump)  d u m p     e             c                h                   o"); }
}