#include "d3d_matrix.h"
#include "SDL2/SDL.h"

/*

 ( x')          (x)
 ( y')  =   M * (y)
 ( z')          (z)
 ( 1 )          (1)

instead of (x',y',z',1) = (x,y,z,1) * M
*/

int D3d_print_mat(double a[4][4]) {
  int r, c;
  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++) {
      printf(" %12.4lf ", a[r][c]);
    }
    printf("\n");
  }

  return 1;
}

int D3d_copy_mat(double a[4][4], double b[4][4])
// a = b
{
  int r, c;
  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++) {
      a[r][c] = b[r][c];
    }
  }

  return 1;
}

int D3d_mat_mult(double res[4][4], double a[4][4], double b[4][4])
// res = a * b
// this is SAFE, i.e. the user can make a call such as
// D3d_mat_mult(p,  p,q) or D3d_mat_mult(p,  q,p) or  D3d_mat_mult(p, p,p)
{
  double sum;
  int k;
  int r, c;
  double tmp[4][4];

  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++) {
      sum = 0.0;
      for (k = 0; k < 4; k++) {
        sum = sum + a[r][k] * b[k][c];
      }
      tmp[r][c] = sum;
    }
  }

  D3d_copy_mat(res, tmp);

  return 1;
}

int D3d_make_identity(double a[4][4])
// a = I
{
  int r, c;
  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++) {
      if (r == c)
        a[r][c] = 1.0;
      else
        a[r][c] = 0.0;
    }
  }

  return 1;
}

int D3d_mat_mult_pt(double P[3], double m[4][4], double Q[3]) {
    Q[0] = m[0][0] * P[0] + m[0][1] * P[1] + m[0][2] * P[2] + m[0][3];
    Q[1] = m[1][0] * P[0] + m[1][1] * P[1] + m[1][2] * P[2] + m[1][3];
    Q[2] = m[2][0] * P[0] + m[2][1] * P[1] + m[2][2] * P[2] + m[2][3];
    return 1;
}

int D3d_translate(double a[4][4], double b[4][4], double dx, double dy, double dz) {
    D3d_make_identity(a);
    a[0][3] = dx;
    a[1][3] = dy;
    a[2][3] = dz;
    return 1;
}

int D3d_view(double view[4][4], double view_inverse[4][4], double eye[3], double coi[3], double up[3]) {
    // Implementation of view matrix setup
    // This is a simplified version; you may need to adjust based on your requirements
    double f[3], s[3], u[3];
    // Calculate forward vector f
    f[0] = coi[0] - eye[0];
    f[1] = coi[1] - eye[1];
    f[2] = coi[2] - eye[2];
    // Normalize f
    D3d_normalize(f);
    
    // Calculate side vector s
    D3d_x_product(s, f, up);
    D3d_normalize(s);
    
    // Recalculate u as the cross product of s and f
    D3d_x_product(u, s, f);
    
    // Set up the view matrix
    view[0][0] = s[0];
    view[1][0] = s[1];
    view[2][0] = s[2];
    view[0][1] = u[0];
    view[1][1] = u[1];
    view[2][1] = u[2];
    view[0][2] = -f[0];
    view[1][2] = -f[1];
    view[2][2] = -f[2];
    view[3][3] = 1.0;
    
    // Set translation
    view[0][3] = -D3d_dot_product(s, eye);
    view[1][3] = -D3d_dot_product(u, eye);
    view[2][3] = D3d_dot_product(f, eye);
    
    return 1;
}

double D3d_dot_product(double vector1[3], double vector2[3]) {
    return vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2];
}

int D3d_normalize(double *vector) {
    double length = sqrt(D3d_dot_product(vector, vector));
    if (length == 0) return 0; // Cannot normalize a zero vector
    vector[0] /= length;
    vector[1] /= length;
    vector[2] /= length;
    return 1;
}

int D3d_x_product(double res[3], double a[3], double b[3]) {
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
    return 1;
}

// New plot_3d function implementation
int plot_3d(SDL_Renderer *renderer,
            int (*func)(double u1, double v1, double points[3]),
            double mat[4][4], double rgb[3], double half_angle, int WIDTH, int HEIGHT) {
  int p[2];
  double xyz[3];
  double u, v;
  double ulo, uhi, vlo, vhi;

  // Define the ranges for u and v based on the function type
  ulo = 0.0, uhi = 2 * M_PI;
  vlo = -(M_PI / 2), vhi = M_PI / 2;

  // Iterate through the u and v ranges
  for (v = vlo; v <= vhi; v += 0.01) {
    for (u = ulo; u <= uhi; u += 0.01) {
      // Call the parametric function to get the 3D point
      func(u, v, xyz);
      // Apply the transformation matrix
      D3d_mat_mult_pt(xyz, mat, xyz);

      // Project the 3D point onto the 2D screen
      p[0] = (400 / tan(half_angle)) * (xyz[0] / xyz[2]) + (WIDTH / 2);
      p[1] = (400 / tan(half_angle)) * (xyz[1] / xyz[2]) + (HEIGHT / 2);

      // Set the color and draw the point
      SDL_SetRenderDrawColor(renderer, (int)(rgb[0] * 255),
                             (int)(rgb[1] * 255),
                             (int)(rgb[2] * 255), 255);
      SDL_RenderDrawPoint(renderer, p[0], p[1]);
    }
  }

  return 1;
}