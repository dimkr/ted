/************************************************************************/
/*									*/
/*  Cluster line segments.						*/
/*									*/
/************************************************************************/

# include <appDebugon.h>

# if 0

================================================================
Find the line that minimises the sum of squared distances of the
end points of two segments to itself.
================================================================

We have two line segments:

    R: (r_x1,r_y1) -> (r_x2,r_y2)
    S: (s_x1,s_y1) -> (s_x2,s_y2)

Distance to a line with normal formula sin( a )*x+ cos( a )*y+ c= 0 of
each of the four end points. Where q is the angle of the line with the 
x axis.

    d= sin( q )* r_x1+ cos( q )* r_y1+ c. Etc

Squares of the distances are:

    ( sin( q )* r_x1+ cos( q )* r_y1+ c )^2. Etc

Sum of the squared distances is: ...

    T_a_a= r_x1*r_x1+ r_x2*r_x2+ s_x1*s_x1+ s_x2*s_x2;
    T_a_b= r_x1*r_y1+ r_x2*r_y2+ s_x1*s_y1+ s_x2*s_y2;
    T_a_c= r_x1     + r_x2     + s_x1     + s_x2;
    T_b_b= r_y1*r_y1+ r_y2*r_y2+ s_y1*s_y1+ s_y2*s_y2;
    T_b_c= r_y1     + r_y2     + s_y1     + s_y2;
    T_c_c= 4;

    DS=	T_a_a*		sin( q )* sin( q )+
	2* T_a_b*	sin( q )* cos( q )+
	2* T_a_c*	sin( q )* c+
	T_b_b*		cos( q )* cos( q )+
	2* T_b_c*	cos( q )* c+
	T_c_c*		c* c;

    Should be minimal. Partial derivatives to q,c respectively.

    to q:	T_a_a*		[  sin( q )*  cos( q )+  cos( q )* sin( q ) ]+
		2* T_a_b* 	[  sin( q )* -sin( q )+  cos( q )* cos( q ) ]+
		2* T_a_c*	[  cos( q )* c ]+
		T_b_b*		[  cos( q )* -sin( q )+ -sin( q )* cos( q ) ]+
		2* T_b_c*	[ -sin( q )* c ]+
		T_c_c*		0.
		= 0

    to c:	cos( q )- sin( q )+ 2* c.
		= 0;

    or		T_a_a* [ sin( q )* cos( q ) ]+
		T_a_b* [ cos^2( q )- sin^2( q ) ]+
		T_a_c* [ c* cos( q ) ]+
		T_b_b* [ cos( q )* -sin( q ) ]+
		T_b_c* [ c* -sin( q ) ]
		= 0

===

To avoid gonio use the line a* x+ b* y+ c= 0 with a+ b= 1. In stead of
a*a+ b*b= 1.

    d= a* r_x1+ ( 1- a )* r_y1+ c. Etc
    d= a* r_x1+ r_y1- a* r_y1+ c. Etc
    d= ( r_x1- r_y1 )* a+ r_y1+ c. Etc

Squares:

    d*d=	( r_x1- r_y1 )^2* a*a+
		2*( r_x1- r_y1 )* r_y1* a+
		2*( r_x1- r_y1 )* a* c+
		r_y1* r_y1= 
		2* r_y1* c+
		c* c;

    Be		V_a_a= sum( ( r_x1- r_y1 )^2 )
		V_a= sum( ( r_x1- r_y1 )* r_y1 );
		V_a_c= sum( ( r_x1- r_y1 ) );
		V_c= r_y1;
		V_c_c= 4;

Sum of the squared distances is: ...

    DS=		V_a_a*		a*a+
		2* V_a*		a+
		2* V_a_c*	a*c+
		2* V_c*		c+
		V_c_c*		c* c;

    Should be minimal. Partial derivatives to a,c respectively.

    to a:	2* V_a_a* a+ 2* V_a+ 2* V_a_c* c,
    to c:	2* V_a_c* a+ 2* V_c+ 2* V_c_c* c,

    Divide by 2, should be 0.

    (a.1)	V_a_a* a+ V_a_c* c+ V_a= 0.
    (c.1)	V_a_c* a+ V_c_c* c+ V_c= 0.

    (a.2)	V_a_c* a+
		( V_a_c^2 )/V_a_a* c+
		( V_a* V_a_c )/V_a_a= 0

    (c.2)	( V_a_c^2 )/V_c_c* a+
		V_a_c* c+ 
		( V_a_c* V_c )/ V_c_c= 0;

    (a.2-c.1)	[ ( V_a_c^2 )/V_a_a- V_c_c ]* c+
		[ ( V_a* V_a_c )/V_a_a- V_c ]= 0.

    (c.2-a.1)	[ ( V_a_c^2 )/V_c_c- V_a_a ]* a+
		[ ( V_a_c* V_c )/V_c_c- V_a ]= 0.

    a= [ V_a- ( V_a_c* V_c )/V_c_c ]/ [ ( V_a_c^2 )/V_c_c- V_a_a ].
    c= [ V_c- ( V_a* V_a_c )/V_a_a ]/ [ ( V_a_c^2 )/V_a_a- V_c_c ].

# endif
