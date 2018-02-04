
#include <FPT.h>
#include <D3d_matrix.h>

double x[13] = {175,225,225,300,225,225,250,200,150,175,175,100,175} ;
double y[13] = {300,300,250,225,225,200,100,175,100,200,225,225,250} ;
double z[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0} ;
       // z[] values unimportant but should NOT be left uninitialized
       // as nan values WILL propagate through
int n = 13 ;



int main() 
{
 double j ;
 char q ;

 double m[4][4], minv[4][4] ;

 G_init_graphics(601, 601) ;

 // the original design was for a 400x400
 // window and the object is centered on 200,200
 // so we recenter it and make it larger

 D3d_make_identity (m) ;
 D3d_make_identity (minv) ;
 D3d_translate (m,minv, -200, -200, 0) ;
 D3d_scale (m,minv, 2,2,1) ;
 D3d_translate (m,minv, 300, 300, 0) ;

 D3d_mat_mult_points (x,y,z,  m,  x,y,z,13) ;

 D3d_make_identity (m) ;
 D3d_make_identity (minv) ;
 D3d_translate (m,minv, -300, -300, 0) ;
 D3d_scale (m,minv, 0.95, 0.95, 1) ;
 D3d_rotate_z (m,minv, 4*M_PI/180) ;
 D3d_translate (m,minv, 300, 300, 0) ;


 j = 0 ;
 while (j < 100) {

   G_rgb(0,0,0) ;
   G_clear() ;

   G_rgb(0, 0.5, 1) ;
   G_fill_polygon(x,y,n) ;

   G_rgb(1, 1, 0) ;
   G_polygon(x,y,n) ;

   //   q = G_wait_key() ;
   G_display_image() ;
   usleep(25000) ;  // microseconds 

   D3d_mat_mult_points (x,y,z,  m,  x,y,z,13) ;   

   j=j+1 ;
 }

 G_rgb(1,0,0) ;
 G_fill_circle(300,300, 50) ;

 q = G_wait_key() ;

}

