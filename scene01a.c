#include <FPT.h>
#include <D3d_matrix.h>

int main(){
  //Graphics init
  G_init_graphics(800,800);
  G_rgb(0,0,0); G_clear();
  
  int tlist[100];
  double plist[100];
  int list_size;

  //Movement matricies and related vars
  double m[4][4], minv[4][4];

  list_size = 0;

  tlist[list_size] = SX; plist[list_size] = 50.0; list_size ++;
  tlist[list_size] = SY; plist[list_size] = 100.0; list_size ++;
  tlist[list_size] = TX; plist[list_size] = 300.0; list_size ++;
  tlist[list_size] = TY; plist[list_size] = 500; list_size ++;
  D3d_make_movement_sequence_matrix(m, minv, list_size, tlist, plist);

  double u;
  double circlex = 0; double circley = 0; double circlez = 0;
  double P[3] = {circlex, circley, circlez};
  G_rgb(0,0,1);

  for (u = 0.25*M_PI; u<= 1.50*M_PI; u+= 0.01){
    circlex = cos(u);
    circley = sin(u);
    P[0] = circlex;
    P[1] = circley;
    D3d_mat_mult_pt(P, m, P);
    G_point(P[0],P[1]);
  }

  G_wait_key();

  


  
  G_wait_key();
}
