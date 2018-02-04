
#ifndef D3d_matrix_stuff
#define D3d_matrix_stuff

#include <stdio.h>
#include <math.h>
#include <stdlib.h>




/*

 ( x')          (x)
 ( y')  =   M * (y)
 ( z')          (z)  
 ( 1 )          (1)

instead of (x',y',z',1) = (x,y,z,1) * M  

*/



#define SX 0
#define SY 1
#define SZ 2

#define RX 3
#define RY 4
#define RZ 5

#define TX 6
#define TY 7
#define TZ 8

#define NX 9
#define NY 10
#define NZ 11





int D3d_print_mat (double a[4][4]) ;

int D3d_copy_mat (double a[4][4], double b[4][4]) ;
// a = b

int D3d_mat_mult (double res[4][4], double a[4][4], double b[4][4]) ;
// res = a * b
// this is SAFE, i.e. the user can make a call such as 
// D3d_mat_mult(p,  p,q) or D3d_mat_mult(p,  q,p) or  D3d_mat_mult(p, p,p)

int D3d_make_identity (double a[4][4]) ;
// a = I



int D3d_translate (double a[4][4], double b[4][4], 
                   double dx, double dy, double dz) ;
// a = translation*a  
// b = b*translation_inverse  



int D3d_scale (double a[4][4], double b[4][4],
               double sx, double sy, double sz) ;
// a = scale*a  
// b = b*scale_inverse  



int D3d_rotate_x (double a[4][4], double b[4][4], double radians) ;
// a = rotate*a  
// b = b*rotate_inverse  



int D3d_rotate_y (double a[4][4], double b[4][4], double radians) ;
// a = rotate*a  
// b = b*rotate_inverse  



int D3d_rotate_z (double a[4][4], double b[4][4], double radians) ;
// a = rotate*a  
// b = b*rotate_inverse  



int D3d_cs_rotate_x (double a[4][4], double b[4][4], double cs, double sn) ;
// a = rotate*a  
// b = b*rotate_inverse  


int D3d_cs_rotate_y (double a[4][4], double b[4][4], double cs, double sn) ;
// a = rotate*a  
// b = b*rotate_inverse  


int D3d_cs_rotate_z (double a[4][4], double b[4][4], double cs, double sn) ;
// a = rotate*a  
// b = b*rotate_inverse  


int D3d_negate_x (double a[4][4], double b[4][4]) ;
// negate the x....reflects in the yz-plane
// a = reflect*a 
// b = b*reflect_inverse  


int D3d_negate_y (double a[4][4], double b[4][4]) ;
// negate the y....reflects in the xz-plane
// a = reflect*a 
// b = b*reflect_inverse  


int D3d_negate_z (double a[4][4], double b[4][4]) ;
// negate the z....reflects in the xy-plane
// a = reflect*a 
// b = b*reflect_inverse  




int D3d_mat_mult_points (double *X, double *Y, double *Z,
                         double m[4][4],
                         double *x, double *y, double *z, int numpoints) ;
// |X0 X1 X2 ...|       |x0 x1 x2 ...|
// |Y0 Y1 Y2 ...| = m * |y0 y1 y2 ...|
// |Z0 Z1 Z2 ...|       |z0 z1 z2 ...|
// | 1  1  1 ...|       | 1  1  1 ...|

// SAFE, user may make a call like D3d_mat_mult_points (x,y,z, m, x,y,z, n) ;



int D3d_mat_mult_pt (double P[3],
                     double m[4][4],
                     double Q[3]) ;
// multiplies a SINGLE point by a matrix
// | P[0] |       | Q[0] |
// | P[1] | = m * | Q[1] |
// | P[2] |       | Q[2] |
// |  1   |       |  1   |

// SAFE, user may make a call like 
// D3d_mat_mult_pt (q,  m,q) ;



int D3d_x_product (double res[3], double a[3], double b[3]) ;
// res = a x b  , cross product of two vectors
// SAFE: it is ok to make a call such as
// D3d_x_product (a,  a,b) or
// D3d_x_product (b,  a,b) or
// D3d_x_product (a,  a,a) 






int D3d_make_movement_sequence_matrix (
                              double mat[4][4],
                              double inv[4][4],
                              int num_movements,
                              int *movement_type_list,
                              double *parameter_list ) ;
// create a matrix (mat) and its inverse (inv)
// that specify a sequence of movements....
// movement_type_list[k] is an integer that
// specifies the type of matrix to be used in the
// the k-th movement.  the parameter that each
// matrix needs is supplied in parameter_list[k].

// return 1 if successful, 0 if error

// the codes for movement_type_list are :
// 0 - scale x
// 1 - scale y
// 2 - scale z

// 3 - rotate x
// 4 - rotate y
// 5 - rotate z
   
// 6 - translate x
// 7 - translate y
// 8 - translate z

// 9  - negate x...reflect in the yz plane
// 10 - negate y...relfect in the xz plane
// 11 - negate z...reflect in the xy plane





int D3d_view (double view[4][4],  double view_inverse[4][4],
              double eye[3], double coi[3], double up[3]) ;
// Construct the view matrix and its inverse given the location
// of the eye, the center of interest, and an up point.
// return 1 if successful, 0 otherwise.



#endif
