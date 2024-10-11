#include <SDL2/SDL.h>
#include <d3d_matrix.h>
#include <math.h>
#include <parametric_equations.h>
#include <stdio.h>
#include <stdlib.h>

// General window vars
#define WIDTH 800
#define HEIGHT 800

// 3D vars
double half_angle = 30 * M_PI / 180;
double eye[3];
double center_of_interest[3];
double up[3];
double view[4][4];
double view_inv[4][4];

// Lighting vars
double light_source[3];
double ambient = 0.3;
double diffuse = 0.6;
double specularity = 50;
double spec_max;

int fnum;

// Z-buffer stuff
double zbuffer[WIDTH][HEIGHT];

// Sets all values in zbuffer to some number
int init_zbuffer() {
  int i, j;
  for (i = 0; i < WIDTH; i++) {
    for (j = 0; j < HEIGHT; j++) {
      zbuffer[i][j] = pow(100, 100);
    }
  }
  return 1;
}

// Initializes arrays and sets view matrix
int init() {
  // zbuffer
  init_zbuffer();

  // Eye
  eye[0] = 0;
  eye[1] = 15;
  eye[2] = -15;

  // Center of Interest
  center_of_interest[0] = 0;
  center_of_interest[1] = 0;
  center_of_interest[2] = 0;

  // Up is just the Eye[1] + 1
  up[0] = eye[0];
  up[1] = eye[1] + 1;
  up[2] = eye[2];

  // Light source
  light_source[0] = eye[0];
  light_source[1] = eye[1] - 5;
  light_source[2] = eye[2] - 5;

  // View Matrix
  D3d_make_identity(view);
  D3d_make_identity(view_inv);
  D3d_view(view, view_inv, eye, center_of_interest, up);

  // Run light source through view matrix in order to pull it into world space
  D3d_mat_mult_pt(light_source, view, light_source);
  // Spec max
  spec_max = 1 - ambient - diffuse;
  return 1;
}

int shade(double *rgb, double ambientplusdiffuse, double intensity) {
  double new_rgb[3];
  double ratio;
  double t;
  if (intensity <= ambientplusdiffuse) {
    ratio = intensity / ambientplusdiffuse;
    new_rgb[0] = ratio * rgb[0];
    new_rgb[1] = ratio * rgb[1];
    new_rgb[2] = ratio * rgb[2];
  } else {
    ratio = ((intensity - ambientplusdiffuse) / (1.0 - ambientplusdiffuse));
    t = 1.0 - ratio;
    new_rgb[0] = t * rgb[0] + ratio;
    new_rgb[1] = t * rgb[1] + ratio;
    new_rgb[2] = t * rgb[2] + ratio;
  }

  rgb[0] = new_rgb[0];
  rgb[1] = new_rgb[1];
  rgb[2] = new_rgb[2];

  return 1;
}

int light_model(double *vector1, double *vector2, double *xyz, double *rgb) {
  // Vector from position to light source
  double lxyz[3] = {light_source[0] - xyz[0], light_source[1] - xyz[1],
                    light_source[2] - xyz[2]};
  D3d_normalize(lxyz);

  // Normal vector from two input vectors
  double normal_vector[3];
  D3d_x_product(normal_vector, vector1, vector2);
  D3d_normalize(normal_vector);

  // Vector from position
  double exyz[3] = {0 - xyz[0], 0 - xyz[1], 0 - xyz[2]};
  D3d_normalize(exyz);

  double nl = D3d_dot_product(normal_vector, lxyz);
  double ne = D3d_dot_product(normal_vector, eye);

  if ((nl < 0) && (ne < 0)) {
    normal_vector[0] = 0 - normal_vector[0];
    normal_vector[1] = 0 - normal_vector[1];
    normal_vector[2] = 0 - normal_vector[2];

    nl *= (-1.0);
    ne *= (-1.0);
  }
  // Reflection vector
  double u = 2 * D3d_dot_product(normal_vector, lxyz);
  double r[3] = {((u * normal_vector[0]) - lxyz[0]),
                 ((u * normal_vector[1]) - lxyz[1]),
                 ((u * normal_vector[2]) - lxyz[2])};
  D3d_normalize(r);

  double er = D3d_dot_product(exyz, r);

  if ((nl * ne) <= 0) {
    goto LLL;
  }
  if (er <= 0) {
    er = 0;
  }
  if (nl <= 0) {
    nl = 0;
  }

LLL:;
  double intensity =
      ambient + (diffuse * nl) + (spec_max * pow(er, specularity));
  shade(rgb, ambient + diffuse, intensity);
  return 1;
}

int plot_3d(SDL_Renderer *renderer,
            int (*func)(double u1, double v1, double points[3]),
            double mat[4][4], double rgb[3]) {
  // instance stuff
  int p[2];
  double xyz[3] = {0, 0, 0};
  double temp_rgb[3];
  double ulo, uhi, vlo, vhi;

  int CHECK_FLAG = 0;
  int a = 0;
  int b;
  double CHECK_RGB[3] = {1.0, 1.0, 1.0};

  // Choosing which object to plot
  if (fnum == 8) {
    ulo = 0.0, uhi = 2 * M_PI;
    vlo = -(M_PI / 2), vhi = M_PI / 2;
  } else if (fnum == 9) {
    ulo = -1;
    uhi = 1;
    vlo = -M_PI;
    vhi = M_PI;
  }

  double u, v;
  // Maps every point to matrix w/ translations
  for (v = vlo; v <= vhi; v += 0.01) {
    if ((v - (a * M_PI / 32.0)) > -1) {
      a = a + 1;
      CHECK_FLAG = (CHECK_FLAG + 1) % 2;
    }

    b = 0;

    for (u = ulo; u <= uhi; u += 0.0015) {
      if (b * M_PI / 32.0 < u) {
        b = b + 1;
        CHECK_FLAG = (CHECK_FLAG + 1) % 2;
      }

      // reinit temp_rgb
      temp_rgb[0] = rgb[0];
      temp_rgb[1] = rgb[1];
      temp_rgb[2] = rgb[2];

      // calls function, saves point into xyz, then makes necessary moves based
      // on identity matrix and view matrix
      func(u, v, xyz);
      D3d_mat_mult_pt(xyz, mat, xyz);
      D3d_mat_mult_pt(xyz, view, xyz);

      // Vectors for light_model
      double t1[3], t2[3];

      func(u + 0.1, v, t1);
      D3d_mat_mult_pt(t1, mat, t1);
      D3d_mat_mult_pt(t1, view, t1);
      t1[0] = t1[0] - xyz[0];
      t1[1] = t1[1] - xyz[1];
      t1[2] = t1[2] - xyz[2];

      func(u, v + 0.1, t2);
      D3d_mat_mult_pt(t2, mat, t2);
      D3d_mat_mult_pt(t2, view, t2);
      t2[0] = t2[0] - xyz[0];
      t2[1] = t2[1] - xyz[1];
      t2[2] = t2[2] - xyz[2];

      // change rgb w/ light
      if (fnum == 8) {
        if (CHECK_FLAG == 1) {
          light_model(t2, t1, xyz, CHECK_RGB);
        } else {
          light_model(t2, t1, xyz, temp_rgb);
        }
      } else if (fnum == 9) {
        light_model(t1, t2, xyz, temp_rgb);
      }

      // translates xyz to xy plane
      p[0] = (400 / tan(half_angle)) * (xyz[0] / xyz[2]) + (WIDTH / 2);
      p[1] = (400 / tan(half_angle)) * (xyz[1] / xyz[2]) + (HEIGHT / 2);

      // saves zvalue of xyz into zbuffer[translatedx][translatedy]
      if ((p[0] < WIDTH && p[0] > -1) && (p[1] < HEIGHT && p[1] > -1)) {
        if (xyz[2] < zbuffer[p[0]][p[1]]) {
          zbuffer[p[0]][p[1]] = xyz[2];
          SDL_SetRenderDrawColor(renderer, (int)(temp_rgb[0] * 255),
                                 (int)(temp_rgb[1] * 255),
                                 (int)(temp_rgb[2] * 255), 255);
          SDL_RenderDrawPoint(renderer, p[0], p[1]);
        }
      }
    }
  }

  return 1;
}

int main() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow("3D Renderer", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                                        SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create renderer
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // vars
  int filenum;
  filenum = 0;
  int Tn, Ttypelist[100];
  double Tvlist[100];
  double mat[4][4], mat_inv[4][4];
  char *file = "output/sphere", filename[25];

  init();
  D3d_make_identity(mat_inv);
  double sphere_rgb[3] = {.5, 0, .5};
  double hyperbeloid_rgb[3] = {1, 0, 0};

  double u;
  for (u = 0; u < 2 * M_PI; u += M_PI / 180) {
    // Reinitialize some stuff
    init_zbuffer(); // Needs a clear zbuffer for every frame

    // VIEW matrix stuffs, we only move the view around
    // Set values for eye
    eye[0] =
        10 * cos(u); // We use u as we're moving in a circle around the object
    eye[1] = (3 * cos(4 * u)) + 4;
    eye[2] = 10 * sin(u);

    // Because we're changing eye we need to reinitialize up
    up[0] = eye[0];
    up[1] = eye[1] + 1;
    up[2] = eye[2];

    center_of_interest[0] = 0;
    center_of_interest[1] = 2;
    center_of_interest[2] = 0;
    D3d_make_identity(
        view); // Remake view matrix every time because init only runs once
    D3d_view(view, view_inv, eye, center_of_interest, up);

    // BEGINNING OF SPHERES
    // First Sphere
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);

    Tn = 0;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = 4;
    Tn++;

    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 8;
    plot_3d(renderer, f8, mat, sphere_rgb);

    // Second Sphere
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);

    Tn = 0;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = -2;
    Tn++;

    Ttypelist[Tn] = TZ;
    Tvlist[Tn] = 2 * sqrt(3);
    Tn++;

    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 8;
    plot_3d(renderer, f8, mat, sphere_rgb);

    // Third Sphere
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);

    Tn = 0;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = -2;
    Tn++;

    Ttypelist[Tn] = TZ;
    Tvlist[Tn] = -2 * sqrt(3);
    Tn++;

    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 8;
    plot_3d(renderer, f8, mat, sphere_rgb);

    // Fourth Sphere
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);

    Tn = 0;
    Ttypelist[Tn] = TY;
    Tvlist[Tn] = 4 * sqrt(2);
    Tn++;

    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 8;
    plot_3d(renderer, f8, mat, sphere_rgb);

    //// END OF SPHERES
    //// BEGINNING OF HYPERBELOIDS

    // Hyperbeloid 1
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);

    Tn = 0;
    Ttypelist[Tn] = SX;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SZ;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 5.5;
    Tn++;
    Ttypelist[Tn] = RX;
    Tvlist[Tn] = 90;
    Tn++;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = -2;
    Tn++;

    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 9;
    plot_3d(renderer, f9, mat, hyperbeloid_rgb);

    // Hyperbeloid 2
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);

    Tn = 0;
    Ttypelist[Tn] = SX;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SZ;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 5.5;
    Tn++;
    Ttypelist[Tn] = RX;
    Tvlist[Tn] = 90;
    Tn++;
    Ttypelist[Tn] = RY;
    Tvlist[Tn] = -60;
    Tn++;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = 1;
    Tn++;
    Ttypelist[Tn] = TZ;
    Tvlist[Tn] = -((-2 * sqrt(3)) + sqrt(3));
    Tn++;
    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 9;
    plot_3d(renderer, f9, mat, hyperbeloid_rgb);

    // Hyperbeloid 3
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);
    Tn = 0;
    Ttypelist[Tn] = SX;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SZ;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 5.5;
    Tn++;
    Ttypelist[Tn] = RZ;
    Tvlist[Tn] = 90;
    Tn++;
    Ttypelist[Tn] = RY;
    Tvlist[Tn] = -30;
    Tn++;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = 1;
    Tn++;
    Ttypelist[Tn] = TZ;
    Tvlist[Tn] = (-2 * sqrt(3)) + sqrt(3);
    Tn++;

    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 9;
    plot_3d(renderer, f9, mat, hyperbeloid_rgb);

    // Hyperbeloid 4
    D3d_make_identity(mat);
    D3d_make_identity(mat_inv);
    Tn = 0;
    Ttypelist[Tn] = SX;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SZ;
    Tvlist[Tn] = 0.5;
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = 5.5;
    Tn++;
    Ttypelist[Tn] = RY;
    Tvlist[Tn] = 90;
    Tn++;
    Ttypelist[Tn] = RZ;
    Tvlist[Tn] = 37.5;
    Tn++;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = 2;
    Tn++;
    Ttypelist[Tn] = TY;
    Tvlist[Tn] = 2 * sqrt(2);
    Tn++;
    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 9;
    plot_3d(renderer, f9, mat, hyperbeloid_rgb);

    // Hyperbeloid 5
    Ttypelist[Tn] = RY;
    Tvlist[Tn] = 120;
    Tn++;
    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 9;
    plot_3d(renderer, f9, mat, hyperbeloid_rgb);

    // Hyperbeloid 6
    Ttypelist[Tn] = RY;
    Tvlist[Tn] = 120;
    Tn++;
    D3d_make_movement_sequence_matrix(mat, mat_inv, Tn, Ttypelist, Tvlist);
    fnum = 9;
    plot_3d(renderer, f9, mat, hyperbeloid_rgb);

    sprintf(filename, "%s%04d.bmp", file, filenum);
    filenum++;

    // Save the frame as a BMP file
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, WIDTH, HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32,
                         surface->pixels, surface->pitch);
    SDL_SaveBMP(surface, filename);
    SDL_FreeSurface(surface);

    printf("\nRendered frame #%d", filenum);

    // Clear the renderer for the next frame
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
  }

  // Clean up
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
