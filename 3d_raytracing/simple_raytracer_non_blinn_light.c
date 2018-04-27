
#include <FPT.h>
#include <D3d_matrix.h>


double light_in_eye_space[3] ;
double AMBIENT      = 0.2 ;
double MAX_DIFFUSE  = 0.5 ;
double SPECPOW      = 80 ;
double tan_half;

double obmat[100][4][4] ;
double obinv[100][4][4] ;
double color[100][3] ;
int    num_objects ;
int    q ;





int Light_Model (double irgb[3],
                 double s[3],
                 double p[3],
                 double n[3],
                 double argb[3])
// s,p,n in eyespace

// irgb == inherent color of object (input to this function)
// s = location of start of ray (probably the eye)
// p = point on object (input to this function)
// n = normal to the object at p (input to this function)
// argb == actual color of object (output of this function)
// globals : AMBIENT, MAX_DIFFUSE, SPECPOW, light_in_eye_space[3]

// return 1 if successful, 0 if error
{

  double len ;
  double N[3] ;
  len = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]) ;
  if (len == 0) return 0 ;
  N[0] = n[0]/len ;  N[1] = n[1]/len ;  N[2] = n[2]/len ;

  double E[3] ;
  E[0] = s[0] - p[0] ;
  E[1] = s[1] - p[1] ;
  E[2] = s[2] - p[2] ;
  len = sqrt(E[0]*E[0] + E[1]*E[1] + E[2]*E[2]) ;
  if (len == 0) return 0 ;
  E[0] /= len ;  E[1] /= len ;  E[2] /= len ;
  double NdotE = N[0]*E[0] + N[1]*E[1] + N[2]*E[2] ;

  double L[3] ;
  L[0] = light_in_eye_space[0] - p[0] ;
  L[1] = light_in_eye_space[1] - p[1] ;
  L[2] = light_in_eye_space[2] - p[2] ;
  len = sqrt(L[0]*L[0] + L[1]*L[1] + L[2]*L[2]) ;
  if (len == 0) return 0 ;
  L[0] /= len ;  L[1] /= len ;  L[2] /= len ;
  double NdotL = N[0]*L[0] + N[1]*L[1] + N[2]*L[2] ;





  double max_ambient_and_diffuse = AMBIENT + MAX_DIFFUSE ;
     // this needs to occur BEFORE you possibly jump to LLL below




  double intensity ;
  if (NdotL*NdotE < 0) {
    // eye and light are on opposite sides of polygon
    intensity = AMBIENT ;
    goto LLL ;
  } else if ((NdotL < 0) && (NdotE < 0)) {
    // eye and light on same side but normal pointing "wrong" way
    N[0] *= (-1.0) ;    N[1] *= (-1.0) ;    N[2] *= (-1.0) ;
    NdotL *= (-1.0) ;
    NdotE *= (-1.0) ;   // don't use NdotE below, probably should eliminate this
  }


  // ignore Blinn's variant
  double R[3] ; // Reflection vector of incoming light
  R[0] = 2*NdotL*N[0] - L[0] ;
  R[1] = 2*NdotL*N[1] - L[1] ;
  R[2] = 2*NdotL*N[2] - L[2] ;

  double EdotR = E[0]*R[0] + E[1]*R[1] + E[2]*R[2] ;

  double diffuse ;
  if (NdotL <= 0.0) { diffuse = 0.0 ; }
  else { diffuse = MAX_DIFFUSE*NdotL ; }

  double specular ;
  if (EdotR <= 0.0) { specular = 0.0 ; }
  else { specular = (1.0 - max_ambient_and_diffuse)*pow(EdotR,SPECPOW) ;}

  // printf("%lf %lf\n",diffuse,specular) ;
  intensity = AMBIENT + diffuse + specular ;



 LLL : ;

  double f,g ;
  if (intensity <= max_ambient_and_diffuse) {
    f = intensity / max_ambient_and_diffuse ;
    argb[0] = f * irgb[0] ;
    argb[1] = f * irgb[1] ;
    argb[2] = f * irgb[2] ;
  } else {
    f = (intensity - max_ambient_and_diffuse) /
                           (1.0 - max_ambient_and_diffuse) ;
    g = 1.0 - f ;
    argb[0] = g * irgb[0] + f ;
    argb[1] = g * irgb[1] + f ;
    argb[2] = g * irgb[2] + f ;
  }

  return 1 ;
}


int quad(double a, double b, double c, double result[2])
// return number of solutions
{
  int n ;
  double d,ta ;

  d = b*b - 4*a*c ;


  if (d < 0) {
     n = 0 ;
  } else if (d == 0) {
     ta = 2*a ;
     result[0] = -b / ta ;
     n = 1 ;
  } else {
     d = sqrt(d) ;
     ta = 2*a ;
     result[0] = (-b + d) / ta ;
     result[1] = (-b - d) / ta ;
     n = 2 ;
  }

  return n ;
}









int main()
{
  int s,e;
  int frame_number ;
  double eangle ;

  double degrees_of_half_angle ;
  double eye[3],coi[3],up[3] ;
  double light_in_world_space[3] ;

  double vm[4][4], vi[4][4];

  double Tvlist[100];
  int Tn, Ttypelist[100];



  int x_pix, y_pix;
  double screen_pt[3];
  double origin[3];

  double m[4][4], mi[4][4];


  double a[3],b[3],c[3] ;
  double A, B, C ;
  double qres[2];
  double t,tmin,x,y,z ;
  int objnum,objmin,i,n ;

  double osp[3],osn[3],esp[3],esn[3],argb[3] ;
  char fname[100] ;

  G_init_graphics(801,801);


  s = 0 ; e = 72 ;

  for(frame_number = s; frame_number <= e; frame_number++){

    G_rgb(0,0,0); G_clear();

    printf("frame = %d\n", frame_number);
    degrees_of_half_angle = 15 ;
    tan_half = tan(degrees_of_half_angle*M_PI/180);

    eangle = 5*frame_number*M_PI/180 ;
    eye[0] = 10*cos(eangle)   ;
    eye[1] =  8*cos(2*eangle) ;
    eye[2] =  6*sin(eangle)   ;

    coi[0] = 0 ;
    coi[1] = 0 ;
    coi[2] = 0 ;

    up[0] = eye[0] ;
    up[1] = eye[1] + 1 ;
    up[2] = eye[2] ;


    light_in_world_space[0] =  20 ;
    light_in_world_space[1] = 100 ;
    light_in_world_space[2] = 100 ;


    D3d_view(vm,vi,  eye,coi,up) ;

    D3d_mat_mult_pt(light_in_eye_space, vm, light_in_world_space) ;


    num_objects = 0 ;



    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // Sphere A
    color[num_objects][0] = 1 ;
    color[num_objects][1] = 0 ;
    color[num_objects][2] = 0 ;

    Tn = 0 ;
    Ttypelist[Tn] = SX ; Tvlist[Tn] =  0.5   ; Tn++ ;
    Ttypelist[Tn] = SY ; Tvlist[Tn] =  0.5   ; Tn++ ;
    Ttypelist[Tn] = SZ ; Tvlist[Tn] =  0.5   ; Tn++ ;

    Ttypelist[Tn] = TX ; Tvlist[Tn] =   0    ; Tn++ ;
    Ttypelist[Tn] = TY ; Tvlist[Tn] =   0    ; Tn++ ;
    Ttypelist[Tn] = TZ ; Tvlist[Tn] = -0.5   ; Tn++ ;

    D3d_make_movement_sequence_matrix(m, mi, Tn, Ttypelist, Tvlist);
    D3d_mat_mult(obmat[num_objects], vm, m) ;
    D3d_mat_mult(obinv[num_objects], mi, vi) ;

    num_objects++ ; // don't forget to do this


    //////////////////////////////////////////////////////////////
    // Sphere B
    color[num_objects][0] = 0 ;
    color[num_objects][1] = 1 ;
    color[num_objects][2] = 0 ;

    Tn = 0 ;
    Ttypelist[Tn] = SX ; Tvlist[Tn] =  0.5   ; Tn++ ;
    Ttypelist[Tn] = SY ; Tvlist[Tn] =  0.5   ; Tn++ ;
    Ttypelist[Tn] = SZ ; Tvlist[Tn] =  0.5   ; Tn++ ;

    Ttypelist[Tn] = TX ; Tvlist[Tn] =   0    ; Tn++ ;
    Ttypelist[Tn] = TY ; Tvlist[Tn] =   0    ; Tn++ ;
    Ttypelist[Tn] = TZ ; Tvlist[Tn] =  0.5   ; Tn++ ;

    D3d_make_movement_sequence_matrix(m, mi, Tn, Ttypelist, Tvlist);
    D3d_mat_mult(obmat[num_objects], vm, m) ;
    D3d_mat_mult(obinv[num_objects], mi, vi) ;

    num_objects++ ; // don't forget to do this


    //////////////////////////////////////////////////////////////
    // Sphere C
    color[num_objects][0] = 0 ;
    color[num_objects][1] = 0 ;
    color[num_objects][2] = 1 ;

    Tn = 0 ;
    Ttypelist[Tn] = SX ; Tvlist[Tn] =  1.5   ; Tn++ ;
    Ttypelist[Tn] = SY ; Tvlist[Tn] =  0.2   ; Tn++ ;
    Ttypelist[Tn] = SZ ; Tvlist[Tn] =  0.7  ; Tn++ ;

    Ttypelist[Tn] = TX ; Tvlist[Tn] =   0    ; Tn++ ;
    Ttypelist[Tn] = TY ; Tvlist[Tn] =   0    ; Tn++ ;
    Ttypelist[Tn] = TZ ; Tvlist[Tn] =   0    ; Tn++ ;

    D3d_make_movement_sequence_matrix(m, mi, Tn, Ttypelist, Tvlist);
    D3d_mat_mult(obmat[num_objects], vm, m) ;
    D3d_mat_mult(obinv[num_objects], mi, vi) ;

    num_objects++ ; // don't forget to do this


    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    for(x_pix = 0; x_pix <= 800; x_pix++){
      for(y_pix = 0; y_pix <= 800; y_pix++){


	screen_pt[0] = x_pix - 400;
	screen_pt[1] = y_pix - 400;
	screen_pt[2] = 400 / tan_half;


	tmin = 1e50 ; objmin = -1 ;
        for (objnum = 0 ; objnum < num_objects ; objnum++) {

	  D3d_mat_mult_pt(a, obinv[objnum], origin) ;
	  D3d_mat_mult_pt(b, obinv[objnum], screen_pt) ;
	  c[0] = b[0] - a[0] ;
	  c[1] = b[1] - a[1] ;
	  c[2] = b[2] - a[2] ;

          A = c[0]*c[0] + c[1]*c[1] + c[2]*c[2] ;
          B = 2*(c[0]*a[0] + c[1]*a[1] + c[2]*a[2]) ;
          C = a[0]*a[0] + a[1]*a[1] + a[2]*a[2] - 1.0 ;

          n = quad(A,B,C,qres) ;
          for (i = 0 ; i < n ; i++) {
            t = qres[i] ;
	    if ((t > 0) && (t < tmin)) {
	      tmin = t ;
              x = a[0] + c[0]*t ;
              y = a[1] + c[1]*t ;
              z = a[2] + c[2]*t ;
              objmin = objnum ;
            }
	  }

	}  // end for objnum



	// you now know tmin and objmin and associated (x,y,z)
        if (objmin == -1) { continue ; }

	// object space pt
        osp[0] = x ; osp[1] = y ; osp[2] = z ;
	// object space normal
        osn[0] = 2*x ; osn[1] = 2*y ; osn[2] = 2*z ;
	// eye space pt
        D3d_mat_mult_pt(esp, obmat[objmin], osp) ;
	// eye space normal
        esn[0] = obinv[objmin][0][0]*osn[0] + obinv[objmin][1][0]*osn[1] + obinv[objmin][2][0]*osn[2] ;
        esn[1] = obinv[objmin][0][1]*osn[0] + obinv[objmin][1][1]*osn[1] + obinv[objmin][2][1]*osn[2] ;
        esn[2] = obinv[objmin][0][2]*osn[0] + obinv[objmin][1][2]*osn[1] + obinv[objmin][2][2]*osn[2] ;


        Light_Model (color[objmin], origin, esp, esn, argb) ;
        G_rgb(argb[0], argb[1], argb[2]) ;
        G_point(x_pix, y_pix) ;

      } // end for y_pix

    } // end for x_pix




    q = G_wait_key();
    sprintf(fname, "pic%04d.xwd",frame_number) ;
    G_save_image_to_file(fname) ;
  } // end for frame_number



  while (q != 'q') { q = G_wait_key();  }


}
