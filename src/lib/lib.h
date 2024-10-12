#ifndef LIB_H
#define LIB_H

// d3d_matrix.c function declarations
int D3d_print_mat(double a[4][4]);
int D3d_copy_mat(double a[4][4], double b[4][4]);
int D3d_mat_mult(double res[4][4], double a[4][4], double b[4][4]);
int D3d_make_identity(double a[4][4]);
int D3d_translate(double a[4][4], double b[4][4], double dx, double dy, double dz);
int D3d_scale(double a[4][4], double b[4][4], double sx, double sy, double sz);
int D3d_rotate_x(double a[4][4], double b[4][4], double radians);
int D3d_rotate_y(double a[4][4], double b[4][4], double radians);
int D3d_rotate_z(double a[4][4], double b[4][4], double radians);
int D3d_cs_rotate_x(double a[4][4], double b[4][4], double cs, double sn);
int D3d_cs_rotate_y(double a[4][4], double b[4][4], double cs, double sn);
int D3d_cs_rotate_z(double a[4][4], double b[4][4], double cs, double sn);
int D3d_negate_x(double a[4][4], double b[4][4]);
int D3d_negate_y(double a[4][4], double b[4][4]);
int D3d_negate_z(double a[4][4], double b[4][4]);
int D3d_mat_mult_points(double *X, double *Y, double *Z, double m[4][4], double *x, double *y, double *z, int numpoints);
int D3d_mat_mult_pt(double P[3], double m[4][4], double Q[3]);
int D3d_x_product(double res[3], double a[3], double b[3]);
double D3d_dot_product(double vector1[3], double vector2[3]);
int D3d_normalize(double *vector);
int D3d_make_movement_sequence_matrix(double mat[4][4], double inv[4][4], int num_movements, int *movement_type_list, double *parameter_list);
int D3d_view(double view[4][4], double view_inverse[4][4], double eye[3], double coi[3], double up[3]);

// parametric_equations.c function declarations
double sgn(double v);
int f1(double u, double xy[2]);
int f2(double u, double xy[2]);
int f3(double u, double xy[2]);
int f4(double u, double xy[2]);
int f5(double u, double xy[2]);
int f6(double u, double xy[2]);
int f7(double u, double xy[2]);
int f8(double u, double v, double xyz[3]);
int f9(double u, double v, double xyz[3]);

// plot_3d function declaration
int plot_3d(SDL_Renderer *renderer, int (*func)(double u1, double v1, double points[3]), double mat[4][4], double rgb[3]);

#endif // LIB_H
