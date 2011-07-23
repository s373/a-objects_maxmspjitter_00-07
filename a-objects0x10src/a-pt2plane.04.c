/*����������������������������������������������������������������pt2plane - � andr� sier 2003			fazer o output da coordenada z de um planerado 3d consoante a xy de entradahttp://answers.google.com/answers/threadview?id=18979Hello,Lets define some terminology first:  P1, P2, P3 and P4 are thevertices defining the rectangle in clockwise fashion.  x1, y1 and z1are the x, y and z coordinates of P1 and so on. Let V1 = P2 - P1 andV2 = P4 - P1 be 2 adjacent vectors of the rectangle.  Let R0 = (x0,y0, z0) be the origin point of the ray and let Rd = (xd, yd, zd) bethe ray vector.There are several ways to find out if a ray intersects with arectangle or not.The majority of the methods first find out where the ray intersectsthe plane that the rectangle lies on.  (I'm assuming we're talkingabout Euclidean spaces)To do this we first derive the plane equation from the points of therectangle.The general equation for a plane is Ax + By + Cz + D = 0 where A, B, Cand D are constants.  The values of A, B and C are the x, y and zcomponents of the vector normal to the rectangle.  The value of D isthe distance of the rectangle to the origin.To find the normal vector you take the cross product of adjacentvectors of the rectangle (for example V1xV2 [I'll skip the explanationof cross product since you mentioned you can do that]).Finding the distance from a point to the plane is a little long(including slow functions like square root) so we'll sidestep thatplugging x4, y4 and z4, along with the x, y and z components of thenormal for A, B and C, into the general equation and solve for D.That was the general method of getting the plane equation.  It worksbut here's a faster more direct way of deriving it:Manipulate the general plane equation into this form:(A/D)x + (B/D)y + (C/D)z = -1(hopefully, you know what I did to get there).  Then plug in P1, P2,and P3 into that equation to get 3 different equations.  ApplyCramer's Rule to solve for A/D, B/D and C/D.You should get the following results:A = y1(z2-z3) + y2(z3-z1) + y3(z1-z2)B = z1(x2-x3) + z2(x3-x1) + z3(x1-x2)C = x1(y2-y3) + x2(y3-y1) + x3(y1-y2)D = -x1(y2*z3 - y3*z2) - x2(y3*z1 - y1*z3) - x3(y1*z2 - y2*z1)Now we have the plane equation.  Lets find the intersection of the raywith the plane.We define the ray parametrically as R(t) = R0 + Rd*t.  Then we plug in R(t) into the plane equation we derived earlier:A(x0 + xd*t) + B(y0 + yd*t) + (z0 + zd*t) + D = 0then solve for t:t = -(A*x0 + B*y0 + C*z0 + D) / (A*xd + B*yd + C*zd)After plugging t back into R(t), we finally have the point at whichthe ray intersects with the plane.R(t) = (x0 + xd*t, y0 + yd*t, z0 + zd*t).  Be sure to check if t isnot negative.  If t is negative, then the line that coincides with theray intersects with the plane but not the ray itself.Now, all we have to do is a couple of checks to make sure that thepoint is inside the rectangle.Since the shape in question is a rectangle, I'll streamline the methodused for general convex polygons.Let V3 = P4 - P3 be the vector opposite from V1.  Let V4 be the vectorfrom P1 to the point of intersection, and let V5 be the vector from P3to the point of intersection.  Normalize vectors V1, V3, V4 and V5. Then find the dot products V1 dot V4 and V3 dot V5. If both arenon-negative, then the point is in the rectangle.  This works becausewe use the fact that if the angle between V1 and V4 and the anglebetween V3 and V5 are between 0 and 90 inclusive, the point is in therectangle.Hope that helps����������������������������������������������������������������*/#include "ext.h"#include "ext_common.h"#include <math.h>#ifndef M_PI#define M_PI	3.14159265358979323846#endiftypedef struct	{	t_object 	        c_ob;				void 		        *c_out;		// outlet value			void				*c_out2;	float			a[3], b[3], c[3], d[3]; // as coordenadas de cada ponto	float			in[3]; // as coordenadas de entrada	float			z;	// a coordenada de sa�da} pt2plane;void *pt2plane_new (Symbol *msg, short argc, Atom *argv);void pt2plane_bang (pt2plane *x);void pt2plane_calc (pt2plane *x);void pt2plane_a (pt2plane *x, float echo, float ech, float ec);void pt2plane_b (pt2plane *x, float echo, float ech, float ec);void pt2plane_c (pt2plane *x, float echo, float ech, float ec);void pt2plane_d (pt2plane *x, float echo, float ech, float ec);void pt2plane_plane (pt2plane *x, Symbol *msg, short argc, Atom *argv);void pt2plane_list (pt2plane *x, Symbol *msg, short argc, Atom *argv);void pt2plane_post (pt2plane *x);void pt2plane_assist(pt2plane *x, void *b, long m, long a, char *s);void *pt2plane_class;//3d simple prototypesvoid copy3(float vec0[3], float vec1[3]);void sub3(float vec0[3], float vec1[3], float vec2[3]);void add3(float vec0[3], float vec1[3], float vec2[3]);void scalDiv3(float vec[3], float c);void cross3(float vec0[3], float vec1[3], float vec2[3]);void norm3(float vec[3]) ;void set3(float vec[3], float x, float y, float z);void dot3(float c, float vec1[3], float vec2[3]);void *pt2plane_new (Symbol *msg, short argc, Atom *argv) //input the args {	 pt2plane *x;	 int i;	 	 x=(pt2plane *)newobject(pt2plane_class);	 x->c_out2=bangout(x);// output bang at end	 x->c_out=listout(x);// xy pt2plane coords	 		// (0,0,1)		x->a[0] = 0.0;		x->a[1] = 0.0;		x->a[2] = 1.0;		// (0,1,0)		x->b[0] = 0.0;		x->b[1] = 1.0;		x->b[2] = 0.0;		// (1,1,1)		x->c[0] = 1.0;		x->c[1] = 1.0;		x->c[2] = 1.0;		// (1,0,0)		x->d[0] = 1.0;		x->d[1] = 0.0;		x->d[2] = 0.0;		x->in[0] = 0.;		x->in[1] = 0.;		x->in[2] = 0.;		x->z = 0.;			if (argc) { // 	 for(i=0;i<argc/2;i++){		if(argv->a_w.w_sym == gensym("@plane")){			argv++;			if(argv->a_type == A_FLOAT) {				x->a[2] = argv->a_w.w_float;	argv++;				x->b[2] = argv->a_w.w_float;	argv++;				x->c[2] = argv->a_w.w_float;	argv++;				x->d[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->a[2] = (float)argv->a_w.w_long;	argv++;				x->b[2] = (float)argv->a_w.w_long;	argv++;							x->c[2] = (float)argv->a_w.w_long;	argv++;							x->d[2] = (float)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@4")){			argv++;			if(argv->a_type == A_FLOAT) {				x->a[2] = argv->a_w.w_float;	argv++;				x->b[2] = argv->a_w.w_float;	argv++;				x->c[2] = argv->a_w.w_float;	argv++;				x->d[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->a[2] = (float)argv->a_w.w_long;	argv++;				x->b[2] = (float)argv->a_w.w_long;	argv++;							x->c[2] = (float)argv->a_w.w_long;	argv++;							x->d[2] = (float)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@a")){			argv++;			if(argv->a_type == A_FLOAT) {				x->a[0] = argv->a_w.w_float;	argv++;				x->a[1] = argv->a_w.w_float;	argv++;				x->a[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->a[0] = (float)argv->a_w.w_long;	argv++;				x->a[1] = (float)argv->a_w.w_long;	argv++;							x->a[2] = (float)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@b")){			argv++;			if(argv->a_type == A_FLOAT) {				x->b[0] = argv->a_w.w_float;	argv++;				x->b[1] = argv->a_w.w_float;	argv++;				x->b[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->b[0] = (float)argv->a_w.w_long;	argv++;				x->b[1] = (float)argv->a_w.w_long;	argv++;							x->b[2] = (float)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@c")){			argv++;			if(argv->a_type == A_FLOAT) {				x->c[0] = argv->a_w.w_float;	argv++;				x->c[1] = argv->a_w.w_float;	argv++;				x->c[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->c[0] = (float)argv->a_w.w_long;	argv++;				x->c[1] = (float)argv->a_w.w_long;	argv++;							x->c[2] = (float)argv->a_w.w_long;	argv++;						}		}		if(argv->a_w.w_sym == gensym("@d")){			argv++;			if(argv->a_type == A_FLOAT) {				x->d[0] = argv->a_w.w_float;	argv++;				x->d[1] = argv->a_w.w_float;	argv++;				x->d[2] = argv->a_w.w_float;	argv++;			} else if(argv->a_type == A_LONG) {				x->d[0] = (float)argv->a_w.w_long;	argv++;				x->d[1] = (float)argv->a_w.w_long;	argv++;							x->d[2] = (float)argv->a_w.w_long;	argv++;						}		}	 } // end for	} // end argc	 return(x);	}void pt2plane_a (pt2plane *x, float echo, float ech, float ec){	x->a[0]= echo;	x->a[1]= ech;	x->a[2]= ec;}void pt2plane_b (pt2plane *x, float echo, float ech, float ec){	x->b[0]= echo;	x->b[1]= ech;	x->b[2]= ec;}void pt2plane_c (pt2plane *x, float echo, float ech, float ec){	x->c[0]= echo;	x->c[1]= ech;	x->c[2]= ec;}void pt2plane_d (pt2plane *x, float echo, float ech, float ec){	x->d[0]= echo;	x->d[1]= ech;	x->d[2]= ec;}void pt2plane_plane (pt2plane *x, Symbol *msg, short argc, Atom *argv){	short i;	float t[4];		 	for (i=0; i<4; i++) {				if (argv->a_type == A_FLOAT) {			t[i] = argv->a_w.w_float;			argv++;		} 		else if (argv->a_type == A_LONG) {			t[i] = (float)argv->a_w.w_long;			argv++;		} 			}		// (0,0,1)		x->a[0] = 0.0;		x->a[1] = 0.0;		x->a[2] = t[0];		// (0,1,0)		x->b[0] = 0.0;		x->b[1] = 1.0;		x->b[2] = t[1];		// (1,1,1)		x->c[0] = 1.0;		x->c[1] = 1.0;		x->c[2] = t[2];		// (1,0,0)		x->d[0] = 1.0;		x->d[1] = 0.0;		x->d[2] = t[3];}void pt2plane_list (pt2plane *x, Symbol *msg, short argc, Atom *argv){	short i;		 	for (i=0; i<3; i++) {				if (argv->a_type == A_FLOAT) {			x->in[i] = argv->a_w.w_float;			argv++;		} 		else if (argv->a_type == A_LONG) {			x->in[i] = (float)argv->a_w.w_long;			argv++;		} 			}}void  pt2plane_calc (pt2plane *x){	float 	a[3], b[3], c[3], d[3], in[3];	float   normalvec[3], tvec[3], tvec2[3], k=0., tempdist=0., origin[3];	a[0]= x->a[0];	a[1]= x->a[1];	a[2]= x->a[2];	b[0]= x->b[0];	b[1]= x->b[1];	b[2]= x->b[2];	c[0]= x->c[0];	c[1]= x->c[1];	c[2]= x->c[2];	d[0]= x->d[0];	d[1]= x->d[1];	d[2]= x->d[2];	in[0] = x->in[0];	in[1] = x->in[1];	in[2] = x->in[2];// 1. Calculating a Plane from 3 Points// normal = (p1-p2) x (p3-p2)  ;// cross produtc vec1=p1-p2// k = normal * p1		sub3(tvec,a,b);	sub3(tvec2,c,b);	cross3(normalvec, tvec, tvec2);	norm3(normalvec);	//The Dot Product between the normalized normalvec and a point	k = normalvec[0]*a[0] + normalvec[1]*a[1] + normalvec[2]*a[2];	//	dot3(k,normalvec,a);	// j� temos a equa��o do plano:	// Ax + By + Cz + k = 0;	set3 (origin, in[0], in[1], in[2]); // a origem!	//	dot3 (tempdist, normalvec, origin); // aplicar a equa��o com os valores do ponto	tempdist = normalvec[0]*origin[0] + normalvec[1]*origin[1] + normalvec[2]*origin[2];	//tempdist = normal x ponto_arbitrario_no_plano		//dist = tempdist + D	//	x->z = -k + tempdist;	x->z = tempdist + k;}void pt2plane_bang (pt2plane *x){	float	z;		pt2plane_calc(x);	z	= x->z;	outlet_float(x->c_out, z); //xy pt2planeular coords}void main(void){ long int tick = gettime(); setup((t_messlist**)&pt2plane_class,(method)pt2plane_new,0L,(short)sizeof(pt2plane),0L, A_GIMME,0); addbang((method)pt2plane_bang); addmess((method)pt2plane_plane,"plane",A_GIMME, 0);/* addmess((method)pt2plane_a,"a",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0); addmess((method)pt2plane_b,"b",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0); addmess((method)pt2plane_c,"c",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0); addmess((method)pt2plane_d,"d",A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT, 0);*/ addmess((method)pt2plane_list,"list",A_GIMME,0); addmess((method)pt2plane_assist,"assist", A_CANT, 0); post(__DATE__" \t \a \t  "__TIME__"\t \a \t %08xd                                                     a-pt2plane   �   a n d r � s i e r   2 0 0 4   all rights reserved",tick, 0);}void pt2plane_assist(pt2plane *x, void *b, long m, long a, char *s){    if (m==1) { sprintf(s,"echo e   c  h   o"); }       else if (m==2&&a==0) { sprintf(s,"(float) pt2plane     e         c            h              o"); }    else if (m==2&&a==1) { sprintf(s,"(ping) pt2plane end ramp    e         c            h              o"); }}// 3d vecs simple libvoid copy3(float vec0[3], float vec1[3]){    vec0[0] = vec1[0];    vec0[1] = vec1[1];    vec0[2] = vec1[2];}void sub3(float vec0[3], float vec1[3], float vec2[3]){    vec0[0] = vec1[0] - vec2[0];    vec0[1] = vec1[1] - vec2[1];    vec0[2] = vec1[2] - vec2[2];}void add3(float vec0[3], float vec1[3], float vec2[3]){    vec0[0] = vec1[0] + vec2[0];    vec0[1] = vec1[1] + vec2[1];    vec0[2] = vec1[2] + vec2[2];}void scalDiv3(float vec[3], float c){    vec[0] /= c; vec[1] /= c; vec[2] /= c;}void cross3(float vec0[3], float vec1[3], float vec2[3]){    vec0[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];    vec0[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];    vec0[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];}void norm3(float vec[3]) {//    float c = jit_math_sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);    float c = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);    scalDiv3(vec, c); }void set3(float vec[3], float x, float y, float z){    vec[0] = x;    vec[1] = y;    vec[2] = z;}void dot3(float c, float vec1[3], float vec2[3]) //sier{	c = vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];}