#include <FPT.h>
#include <D3d_matrix.h>

double sgn(double v)
{
  if (v > 0) return  1.0  ;
  if (v < 0) return -1.0 ;
  return 0.0 ;
}

// circle :   x^2 + y^2 = 1
int f1 (double u, double xy[2])
{
  xy[0] = cos(u) ;  
  xy[1] = sin(u) ;
}


// sum4 :   x^4 + y^4 = 1
int f2 (double u, double xy[2])
{
  double c,s ;
  c = cos(u) ; s = sin(u) ;
  xy[0] = sgn(c)*sqrt(fabs(c)) ;
  xy[1] = sgn(s)*sqrt(fabs(s)) ;
}


// square :   |x| + |y| = 1
int f3 (double u, double xy[2])
{
  double c,s ;
  c = cos(u) ; s = sin(u) ;
  xy[0] = sgn(c)*c*c ;
  xy[1] = sgn(s)*s*s ;
}


// astroid :   sqrt(|x|) + sqrt(|y|) = 1
int f4 (double u, double xy[2])
{
  double c,s ;
  c = cos(u) ; s = sin(u) ;
  xy[0] = sgn(c)*c*c*c*c ;
  xy[1] = sgn(s)*s*s*s*s ;
}


// hyperbola :   x^2 - y^2 = 1 
int f5 (double u, double xy[2])
// right branch :
{
  xy[0] = cosh(u) ;
  xy[1] = sinh(u) ;
}


// parabola :   y = x^2
int f6 (double u, double xy[2])
{
  xy[0] = u ;
  xy[1] = u*u ;
}


// lemon :   x^2 - (1 - y^2)^3 = 0
int f7 (double u, double xy[2])
{
  double c ;
  c = cos(u) ;
  xy[0] = c*c*c ;
  xy[1] = sin(u) ;
}

//sphere : x^2 + y^2 + z^2 =0
//u = longitude && v = latitiude
int f8(double u, double v, double xyz[3]){
  xyz[0] = cos(u) * cos(v);
  xyz[1] = sin(v);
  xyz[1] = cos(v) * sin(u);
}

int plot (double ulo, double uhi,
          int (*func)(double u, double xy[2]),
          double mat[4][4]
	 ) 
{
  double u,p[3] ;
  double twovals[2] ;

  for (u = ulo ; u <= uhi ; u += 0.01) {
    func(u, twovals) ;
    p[0] = twovals[0] ; p[1] = twovals[1] ; p[2] = 0 ;
    D3d_mat_mult_pt(p,mat,p) ;
    G_point(p[0],p[1]) ;
  }
}

int plot_3d(double ulo, double uhi, int(*func)(double u , double xyz[3], double mat [4][4])){
    double u, v;
    double p[2];
    double HALF_ANGLE = M_PI/4;

    for (u = ulo; u<= uhi; u+= 0.01){
      for (v = ulo; v <= uhi; v += 0.01){
	p[0] = ((400/tan(HALF_ANGLE)) * (xyz[0]/xyz[2]) + 400);
	p[1] = ((400/tan(HALF_ANGLE)) * (xyz[1]/xyz[2]) + 400);
	func(u,v,p);
	D3d_mat_mult_pt(y,mat,y);
	G_point(p[0],p[1]);
      }
    }					 
}

// n = (b-a) x (c-a)
int plot_normals(double ulo, double uhi,
          int (*func)(double u, double xy[2]),
          double mat[4][4]
	 ) 
{
  double u,p[3] ;
  double twovals[2] ;
  double twovalsb[2];

  double a,b;
  double xx,yy;

  
  for (u = ulo ; u <= uhi ; u += 0.1) {
    func(u, twovals) ;
    func((u-0.1), twovalsb);
    D3d_mat_mult_pt(twovals,mat,twovals);
    D3d_mat_mult_pt(twovalsb,mat,twovalsb);

    
    a = twovalsb[0] - twovals[0];
    b = twovalsb[1] - twovals[1];

    xx = twovals[0] - b;
    yy = twovals[1] + a;
    
    G_line(twovals[0],twovals[1],xx,yy) ;
  }
}





int main()
{
  int i, Tn, Ttypelist[100] ;
  double Tvlist[100] ;
  double mat[4][4],imat[4][4] ;

  G_init_graphics(800,800) ;
  G_rgb(0,0,0) ;
  G_clear() ;


  Tn = 0 ; // number of transformations
  Ttypelist[Tn] = SX ; Tvlist[Tn] =  150.0 ; Tn++ ;
  Ttypelist[Tn] = SY ; Tvlist[Tn] =  150.0 ; Tn++ ;
  Ttypelist[Tn] = RZ ; Tvlist[Tn] =   60.0 ; Tn++ ;
  Ttypelist[Tn] = TX ; Tvlist[Tn] =  600.0 ; Tn++ ;
  Ttypelist[Tn] = TY ; Tvlist[Tn] =  150.0 ; Tn++ ;
  D3d_make_movement_sequence_matrix (mat,imat,
                                     Tn,
                                     Ttypelist,
                                     Tvlist) ;
  G_rgb(1,1,1) ;
  plot_3d(0, 2*M_PI,  f8, mat) ;
  //plot_normals(0, 2*M_PI,  f8, mat) ;
  G_wait_key() ;


}



