#include <FPT.h>


int main ()
{
  int width,height ;
  char prefix[100], sequence_name[100] ;
  int s,e,i ;
  double f ;
  int q ;

  printf("movie player ... q to quit, any other key to keep playing movie\n") ;

  printf("enter window width and height ") ;
  scanf("%d %d",&width,&height) ;

  printf("enter prefix name ") ;
  scanf("%s",prefix) ;

  printf("enter starting integer ") ;
  scanf("%d",&s) ;

  printf("enter ending integer ") ;
  scanf("%d",&e) ;

  /*
  printf("\n") ;
  for (i = s ; i <= e ; i++) {
    sprintf(sequence_name, "%s%04d.xwd", prefix, i) ;
    printf("%s\n",sequence_name) ;
  }
  */


  G_init_graphics(width,height) ;

  while (0 == 0) {

    // run the sequence forward
    for (i = s ; i <= e ; i++) {
      sprintf(sequence_name, "%s%04d.xwd", prefix, i) ;
      G_get_image_from_file(sequence_name,0,0) ;
      q = G_wait_key() ;
      if (q == 'q') goto L ;
    }

    // run the sequence backward
    for (i = e ; i >= s ; i--) {
      sprintf(sequence_name, "%s%04d.xwd", prefix, i) ;
      G_get_image_from_file(sequence_name,0,0) ;
      q = G_wait_key() ;
      if (q == 'q') goto L ;
    }

  }


 L:
  G_close() ;

}
