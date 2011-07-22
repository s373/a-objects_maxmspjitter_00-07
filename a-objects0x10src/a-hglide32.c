/*ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡAhglide - © ASier 2002				Linear List Expansion;		Receives lists and expands linearly,		horizontally to n arguments.		Floats, Ints get how many breakpoints.thanks to jasch and trond for making their src code available.ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ*/#include "ext.h"#define MAXSIZE 256typedef struct {	t_object 			x_ob;	t_atom			x_tsilist[MAXSIZE];	//store output list inside	t_atom			x_inlist[MAXSIZE];	//store input list inside	short				x_tsillen; 			//the lenght - max 256	short				x_break, x_step; 	//num breakpoints: 4 int, float methods	Boolean			x_fire;			//bang on calc?	void 			       *x_outlet;											} ahglide;void *ahglide_new 		(long lenght);void  ahglide_assist		(ahglide *x, void *b, long m, long a, char *s);			void  ahglide_bang		(ahglide *x);												void  ahglide_int			(ahglide *x, long n);											void  ahglide_in1		(ahglide *x, long n);											void  ahglide_float		(ahglide *x, float n);											void  ahglide_list		(ahglide *x,Symbol *s, short argc, Atom *argv);											void  ahglide_fire		(ahglide *x, long n);											void  ahglide_break		(ahglide *x, long n);											void  ahglide_free		(ahglide *x);	//not required?												void  ahglide_post		(ahglide *x);	//not required?												void *ahglide_class		; 					void  ahglide_free(ahglide *x){ notify_free((t_object *)x);}												void  ahglide_fire(ahglide *x, long n){ x->x_fire = n;}												void  ahglide_break(ahglide *x, long n){ x->x_break = n;}												void  ahglide_in1 (ahglide *x, long n){	if(!n || n < 0) x->x_tsillen = 100;//register outlistsize	else x->x_tsillen = n;}void  ahglide_list(ahglide *x,Symbol *s, short argc, Atom *argv){	double	poslisratio=0.0, incremento=0.0, 			a=0.0, b=0.0, 			indice=0.0;	short int	i=0, indi=0, 			len = x->x_tsillen;	t_atom     *output, *input;		output = x->x_tsilist;	input  = x->x_inlist;	for (i=0; i < argc; i++) {		if (argv[i].a_type==A_LONG) {			x->x_inlist[i].a_w.w_float = (float)argv[i].a_w.w_long;		}		else if (argv[i].a_type==A_FLOAT) {			x->x_inlist[i].a_w.w_float = argv[i].a_w.w_float;		}		else if (argv[i].a_type==A_SYM) {			error("wrong argument type for list gliding");		//	i--;		}	 }	poslisratio = (double)len / argc;	x->x_tsilist[0].a_w.w_float = x->x_inlist[0].a_w.w_float; //1st arg is same		for(i=1; i<len-1; i++) {			//from second to one but last arg		indice = (double)(i/poslisratio);	//unrounded index for ratio		indi = (int)(indice);			//rounded index for calling			a = (double)x->x_inlist[indi].a_w.w_float;		b = (double)x->x_inlist[indi+1].a_w.w_float;				incremento = (double) (((-b+a)/poslisratio)*((indi)-indice));		//Γ linda!!		x->x_tsilist[i].a_w.w_float = x->x_inlist[indi].a_w.w_float + incremento;	 }	x->x_tsilist[len-1].a_w.w_float = x->x_inlist[argc-1].a_w.w_float; //last arg is same	if(x->x_fire) ahglide_bang(x);	return;}void  ahglide_float (ahglide *x, float n){	double	poslisratio=0.0, incremento=0.0, 			a=0.0, b=0.0, 			indice=0.0;	short int	i=0, indi=0, 			len = x->x_tsillen;	t_atom     *output, *input;		output = x->x_tsilist;	input  = x->x_inlist;	x->x_inlist[x->x_step].a_w.w_float = n;	poslisratio = (double)len / x->x_break;	x->x_tsilist[0].a_w.w_float = x->x_inlist[0].a_w.w_float; //1st arg is same		for(i=1; i<len-1; i++){		indice = (double)(i/poslisratio);		indi = (int)(indice);				a = (double)x->x_inlist[indi].a_w.w_float;		b = (double)x->x_inlist[indi+1].a_w.w_float;				incremento = (double) (((-b+a)/poslisratio)*((indi)-indice));		//Γ linda!!		x->x_tsilist[i].a_w.w_float = x->x_inlist[indi].a_w.w_float + incremento;	 }	x->x_tsilist[len-1].a_w.w_float = x->x_inlist[x->x_break-1].a_w.w_float; //last arg is same	x->x_step +=1;	if(x->x_step>(x->x_break-1))x->x_step =0;	if(x->x_fire) ahglide_bang(x);}void  ahglide_int (ahglide *x, long n){	double	poslisratio=0.0, incremento=0.0, 			a=0.0, b=0.0, 			indice=0.0;	short int	i=0, indi=0, 			len = x->x_tsillen;	t_atom     *output, *input;		output = x->x_tsilist;	input  = x->x_inlist;	x->x_inlist[x->x_step].a_w.w_float = (float)n;	poslisratio = (double)len / x->x_break;	x->x_tsilist[0].a_w.w_float = x->x_inlist[0].a_w.w_float; //1st arg is same		for(i=1; i<len-1; i++){		indice = (double)(i/poslisratio);		indi = (int)(indice);				a = (double)x->x_inlist[indi].a_w.w_float;		b = (double)x->x_inlist[indi+1].a_w.w_float;				incremento = (double) ((-b+a)/poslisratio)*((indi)-indice);		x->x_tsilist[i].a_w.w_float = x->x_inlist[indi].a_w.w_float + incremento;	 }	x->x_tsilist[len-1].a_w.w_float = x->x_inlist[x->x_break-1].a_w.w_float; //last arg is same	x->x_step +=1;	if(x->x_step>(x->x_break-1))x->x_step =0;	if(x->x_fire) ahglide_bang(x);}void  ahglide_bang (ahglide *x){	t_atom		*output;	short 		i;		output = x->x_tsilist;		for(i=0; i<x->x_tsillen; i++){		output[i].a_w.w_float = x->x_tsilist[i].a_w.w_float ;		SETFLOAT(output+i, output[i].a_w.w_float);	}		outlet_list(x->x_outlet,0L,x->x_tsillen, output);	}void ahglide_assist(ahglide *x, void *b, long m, long a, char *s)		{ if ((m==1)&&(a==0)) 		sprintf(s,"(list), (int), (float), bang"); else if ((m==1)&&(a==1)) 	sprintf(s,"(int) list lenght"); else if ((m==2)&&(a==0)) 	sprintf(s,"(list) hglide out");}void *ahglide_new (long lenght){	short i;	ahglide *x;	x = (ahglide *)newobject(ahglide_class);	intin(x,1); 	if(!lenght || lenght < 0) x->x_tsillen = 100;//register outlistsize	else x->x_tsillen = lenght;	//zero floats on the atom.float struct	for (i=0;i<MAXSIZE;i++) {		x->x_inlist[i].a_w.w_float=0.000f;		x->x_tsilist[i].a_w.w_float=0.00f;				}	x->x_outlet = listout(x);		x->x_fire = 1;	x->x_break = 10, x->x_step=0 ; //default numbreakpoints, start increment point	return (x);} void main(void){	long int tick = gettime();			setup((t_messlist **)&ahglide_class, (method)ahglide_new,\	(method)ahglide_free, (short)sizeof(ahglide), 0L, A_DEFLONG, 0); //initial arg default len	addmess((method)ahglide_list,"list",A_GIMME,0);	addfloat((method)ahglide_float);	addint((method)ahglide_int);	addinx((method)ahglide_in1, 1);	addbang((method)ahglide_bang);	addmess((method)ahglide_in1,"size", A_LONG, 0);	addmess((method)ahglide_break,"numpoints", A_LONG, 0);	addmess((method)ahglide_fire,"fire", A_LONG, 0);	addmess((method)ahglide_post,"post", 0);	addmess((method)ahglide_assist,"assist",A_CANT,0);	post(__DATE__" \t \a \t  %ld"__TIME__, tick,0);}void ahglide_post(ahglide *x){	post("\"Thou   rollest   up   into   the	 horizon,   thou   hast   set   light",0);	post("(Keb)   hath   seen   his   splendours,   he   hath",0);	post("advocate   of   her   brother.    She	sought   him",0);	post("all   the   fish,   and   all   the	creeping   things,   and   all",0);	post("and   speech,   the   favoured   of	valour,   who   overthrew   the",0);	post("animals   therof.   The   desert   is	the   lawful   possession   of   the",0);	post("are   content   to   crown   thee   upon	the   the   House   of   Keb.   The",0);	post("beloved   of   thy   mother   Nut,   the	mighty   one   feathered   fowl,   and",0);	post("companion   of   the   stars,   and	the   guide   His   sister   [Isis]",0);	post("Company   of   the   Gods   rejoiced,	rejoiced,   at   the   the   Two   Lands",0);	post("Company   of   the   Gods,   and   the	beloved   of   the   Little   Prince",0);	post("darkness,   thou   sendest   forth	air   from   thy   plumes,   and   thou",0);	post("death   fiends,   and   turned   aside	calamities   (of   evil).   She",0);	post("essence,   she   made   an   heir,	height   of   heaven,   thou   art   the",0);	post("feathers,   she   created   air   with	her   wings,   and   she   uttered   the",0);	post("floodest   decreed   for   him   the	guidance   of   the   world   by   thy",0);	post("grew   in   strength   and   stature,	and   his   hand   was   mighty   in",0);	post("hand   as   long   as   times   endure.	Thou   hast   made   this   earth   with",0);	post("hath   protected   him,   and   hath	repulsed   the   known,   and   he",0);	post("Her   tongue   was   perfect,   and   it	woman   of   magical   spells,   the",0);	post("light   with   her   are   set   firm.	Thou   art   the   heir   of   Keb   and",0);	post("like   the   Disk   at   daybreak.   Thy	crown   penetrateth   the   the   wild",0);	post("ahglide:: 	hortizontal  list  glide  ΕΖΕ   copyright ASier 2002",0);	post("members   of   whose   with   the	magical   power   of   her   mouth.",0);	post("mountains.   Thy   heart   is   fixed,	thy   legs   coming   of   Horus,   the",0);	post("never   halted   at   a   word.	Beneficent   in   command   and   word",0);	post("of   the   Company   of   the   Gods,	the   stablisher   of   Right   and",0);	post("of   the   sovereignty   of   the	bring   the   boundaries   of   the",0);	post("over   the   and   she   alighted   not	without   finding   him.   She   made",0);	post("Sebau-fiend.   Thou   didst   stand	up   and   one   of   the   Great",0);	post("son   of   Isis,   the   heir   of	Osiris.\"   Two   Lands   (Egypt).   He",0);	post("son   of   Osiris,   whose   heart   was	firm,   the   Company   of   the   Gods.",0);	post("Son   who   was   set   on   the   great	throne   of   his   triumphant,   the",0);	post("sorrow,   wail   for   her   brother.	She   raised   up   the   inactive",0);	post("the   throne   of   thy   father,   like	Ra.   throughout   the   World,   the",0);	post("the   winds,   and   the   vegetation,	and   all   the   cattle,   and   all",0);	post("thine   adversary.   Thou   dost   son	of   Nut.   The   Two   Lands   (Egypt)",0);	post("thy   hand,   and   the   waters,   and	father   Keb.   Thou   art   the",0);	post("Truth   she   reared   the   child   in	loneliness,   and   the   place",0);	post("untiringly,   she   wandered   round	and   round   about   this   earth   in",0);	post("uttered   the   spell   heart   was	still,   she   drew   from   him   his",0);	post("was   Isis,   the   of   every   god.	Thou   art   beneficent   in   decree",0);	post("where   he   was   not   smite   thine	enemy,   and   set   thy   fear   in",0);}