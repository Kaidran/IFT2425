//------------------------------------------------------
// module  : Tp-IFT2425-II.3.c
// author  : Andre Lalonde - Elizabeth Michel
// date    : 17/02/2019
// version : 1.0
// language: C
// note    :
//------------------------------------------------------
//  

//------------------------------------------------
// FICHIERS INCLUS -------------------------------
//------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <new>
#include <time.h>

//------------------------------------------------
// DEFINITIONS -----------------------------------
//------------------------------------------------
#define CARRE(X) ((X)*(X))
#define CUBE(X)  ((X)*(X)*(X))

const double EPSILON = 1E-5;
const double EPSILON_f = 1E-6;

//------------------------------------------------
// PROTOTYPE DE FONCTIONS  -----------------------   
//------------------------------------------------
double fcmv(double, double*, int);
double dfcmv(double, double*, int);
double f_1_eps(double, double*, int);
double f_2_eps(double, double*, int);
double f_true_eps(double, double*, int);
double newton(double, double*, int, int);
void mandelbrot(int, int, float**);
void mandelbrot_2(int, int, float**);
void mandelbrot_3(int, int, float**);


//-------------------------
//--- Windows -------------
//-------------------------
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

Display   *display;
int	  screen_num;
int 	  depth;
Window	  root;
Visual*	  visual;
GC	  gc;

/************************************************************************/
/* OPEN_DISPLAY()							*/
/************************************************************************/
int open_display()
{
  if ((display=XOpenDisplay(NULL))==NULL)
  { 
    printf("Connection impossible\n");
    return(-1); 
  }

  else
  { 
    screen_num=DefaultScreen(display);
    visual=DefaultVisual(display,screen_num);
    depth=DefaultDepth(display,screen_num);
    root=RootWindow(display,screen_num);
    return 0;
  }
}

/************************************************************************/
/* FABRIQUE_WINDOW()							*/
/* Cette fonction crée une fenetre X et l'affiche à l'écran.	        */
/************************************************************************/
Window fabrique_window(char *nom_fen,int x,int y,int width,int height,int zoom)
{
  Window                 win;
  XSizeHints      size_hints;
  XWMHints          wm_hints;
  XClassHint     class_hints;
  XTextProperty  windowName, iconName;

  char *name=nom_fen;

  if(zoom<0) { width/=-zoom; height/=-zoom; }
  if(zoom>0) { width*=zoom;  height*=zoom;  }

  win=XCreateSimpleWindow(display,root,x,y,width,height,1,0,255);

  size_hints.flags=PPosition|PSize|PMinSize;
  size_hints.min_width=width;
  size_hints.min_height=height;

  XStringListToTextProperty(&name,1,&windowName);
  XStringListToTextProperty(&name,1,&iconName);
  wm_hints.initial_state=NormalState;
  wm_hints.input=True;
  wm_hints.flags=StateHint|InputHint;
  class_hints.res_name=nom_fen;
  class_hints.res_class=nom_fen;

  XSetWMProperties(display,win,&windowName,&iconName,
                   NULL,0,&size_hints,&wm_hints,&class_hints);

  gc=XCreateGC(display,win,0,NULL);

  XSelectInput(display,win,ExposureMask|KeyPressMask|ButtonPressMask| 
               ButtonReleaseMask|ButtonMotionMask|PointerMotionHintMask| 
               StructureNotifyMask);

  XMapWindow(display,win);
  return(win);
}

/****************************************************************************/
/* CREE_XIMAGE()							    */
/* Crée une XImage à partir d'un tableau de float                           */
/* L'image peut subir un zoom.						    */
/****************************************************************************/
XImage* cree_Ximage(float** mat,int z,int length,int width)
{
  int lgth,wdth,lig,col,zoom_col,zoom_lig;
  float somme;
  unsigned char	 pix;
  unsigned char* dat;
  XImage* imageX;

  /*Zoom positiv*/
  /*------------*/
  if (z>0)
  {
    lgth=length*z;
    wdth=width*z;

    dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
    if (dat==NULL)
      { 
        printf("Impossible d'allouer de la memoire.");
        exit(-1);
      }

    for(lig=0;lig<lgth;lig=lig+z) for(col=0;col<wdth;col=col+z)
    { 
      pix=(unsigned char)mat[lig/z][col/z];
      for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
      { 
        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+0)]=pix;
        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+1)]=pix;
        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+2)]=pix;
        dat[((lig+zoom_lig)*wdth*4)+((4*(col+zoom_col))+3)]=pix; 
      }
    }
  } /*--------------------------------------------------------*/

  /*Zoom negatifv*/
  /*------------*/
  else
  {
   z=-z;
   lgth=(length/z);
   wdth=(width/z);

   dat=(unsigned char*)malloc(lgth*(wdth*4)*sizeof(unsigned char));
   if (dat==NULL)
      { printf("Impossible d'allouer de la memoire.");
        exit(-1); }

  for(lig=0;lig<(lgth*z);lig=lig+z) for(col=0;col<(wdth*z);col=col+z)
   {  
    somme=0.0;
    for(zoom_lig=0;zoom_lig<z;zoom_lig++) for(zoom_col=0;zoom_col<z;zoom_col++)
     somme+=mat[lig+zoom_lig][col+zoom_col];
           
     somme/=(z*z);    
     dat[((lig/z)*wdth*4)+((4*(col/z))+0)]=(unsigned char)somme;
     dat[((lig/z)*wdth*4)+((4*(col/z))+1)]=(unsigned char)somme;
     dat[((lig/z)*wdth*4)+((4*(col/z))+2)]=(unsigned char)somme;
     dat[((lig/z)*wdth*4)+((4*(col/z))+3)]=(unsigned char)somme; 
   }
  } /*--------------------------------------------------------*/

  imageX=XCreateImage(display,visual,depth,ZPixmap,0,(char*)dat,wdth,lgth,16,wdth*4);
  return (imageX);
}

//-------------------------//
//-- Matrice de Flottant --//
//-------------------------//
//---------------------------------------------------------
//  alloue de la memoire pour une matrice 1d de float
//----------------------------------------------------------
float* fmatrix_allocate_1d(int hsize)
 {
  float* matrix;
  matrix=new float[hsize]; return matrix; }

//----------------------------------------------------------
//  alloue de la memoire pour une matrice 2d de float
//----------------------------------------------------------
float** fmatrix_allocate_2d(int vsize,int hsize)
 {
  float** matrix;
  float *imptr;

  matrix=new float*[vsize];
  imptr=new  float[(hsize)*(vsize)];
  for(int i=0;i<vsize;i++,imptr+=hsize) matrix[i]=imptr;
  return matrix;
 }

//----------------------------------------------------------
// libere la memoire de la matrice 1d de float
//----------------------------------------------------------
void free_fmatrix_1d(float* pmat)
{ delete[] pmat; }

//----------------------------------------------------------
// libere la memoire de la matrice 2d de float
//----------------------------------------------------------
void free_fmatrix_2d(float** pmat)
{ delete[] (pmat[0]);
  delete[] pmat;}

//----------------------------------------------------------
// Sauvegarde de l'image de nom <name> au format pgm                        
//----------------------------------------------------------                
void SaveImagePgm(char* bruit,char* name,float** mat,int lgth,int wdth)
{
 int i,j;
 char buff[300];
 FILE* fic;

  //--extension--
  strcpy(buff,bruit);
  strcat(buff,name);
  strcat(buff,".pgm");

  //--ouverture fichier--
  fic=fopen(buff,"wb");
    if (fic==NULL) 
        { printf("Probleme dans la sauvegarde de %s",buff); 
          exit(-1); }
  printf("\n Sauvegarde de %s au format pgm\n",buff);

  //--sauvegarde de l'entete--
  fprintf(fic,"P5");
  fprintf(fic,"\n# IMG Module");
  fprintf(fic,"\n%d %d",wdth,lgth);
  fprintf(fic,"\n255\n");

  //--enregistrement--
  for(i=0;i<lgth;i++) for(j=0;j<wdth;j++) 
	fprintf(fic,"%c",(char)mat[i][j]);
   
  //--fermeture fichier--
   fclose(fic); 
}

//----------------------------------------------------------
// Recal                                                    *
//----------------------------------------------------------
void Recal(float** mat,int lgth,int wdth)
{
 int i,j;
 float max,min,tmp;;


 //Recherche du min
  min=mat[0][0];
  for(i=0;i<lgth;i++) for(j=0;j<wdth;j++)
    if (mat[i][j]<min) min=mat[i][j];

  //plus min
  for(i=0;i<lgth;i++) for(j=0;j<wdth;j++) mat[i][j]-=min;
 
  //Recherche du max
  max=mat[0][0];
  for(i=0;i<lgth;i++) for(j=0;j<wdth;j++) 
    if (mat[i][j]>max) max=mat[i][j];

  //Recalibre la matrice
 for(i=0;i<lgth;i++) for(j=0;j<wdth;j++)
   mat[i][j]*=(255/max);
}

//----------------------------------------------------------
//  Egalisation Histogramme         
//----------------------------------------------------------
void Egalise(float** img,int lgth,int wdth,int thresh)
{
  int i,j;
  float tmp;
  float nb;
  float HistoNg[256];
  float FnctRept[256];

  //Calcul Histogramme Ng
  for(i=0;i<256;i++) HistoNg[i]=0.0;

  nb=0;
  for(i=0;i<lgth;i++) for(j=0;j<wdth;j++)
  { tmp=img[i][j];
    if (tmp>thresh) { HistoNg[(int)(tmp)]++; nb++; } } 
 
  for(i=0;i<256;i++)  HistoNg[i]/=(float)(nb);

  //Calcul Fnct Repartition
  for(i=0;i<256;i++) FnctRept[i]=0.0;

  for(i=0;i<256;i++)
  { if (i>0)  FnctRept[i]=FnctRept[i-1]+HistoNg[i];
    else      FnctRept[i]=FnctRept[i]; }

  for(i=0;i<256;i++) FnctRept[i]=(int)((FnctRept[i]*255)+0.5);

 //Egalise
  for(i=0;i<lgth;i++) for(j=0;j<wdth;j++)
    img[i][j]=FnctRept[(int)(img[i][j])];
}


//----------------------------------------------------------
//----------------------------------------------------------
// PROGRAMME PRINCIPAL -------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
int main(int argc,char** argv)
{
  int i,j,k;
  int flag_graph;
  int zoom;

  //Pour Xwindow
  //------------
  XEvent ev;
  Window win_ppicture;
  XImage *x_ppicture; 
  char   nomfen_ppicture[100]; 
  int    length,width;

  length=width=512;
  float** Graph2D=fmatrix_allocate_2d(length,width); 
  flag_graph=1;
  zoom=1;

  //Init
  for(i=0;i<length;i++) for(j=0;j<width;j++) Graph2D[i][j]=0.0;
 
//--------------------------------------------------------------------------------
// PROGRAMME ---------------------------------------------------------------------
//--------------------------------------------------------------------------------

 //Affichage dégradé de niveaux de gris dans Graph2D
  for(int i=0;i<length;i++) for(int j=0;j<width;j++) Graph2D[i][j]=j/2.0;

  
  //---------------------------
  //Algorithme NEWTON
  //---------------------------
  double c0 = 0.25;
  double y[] = {0.11, 0.24, 0.27, 0.52, 1.13, 1.54, 1.71, 1.84, 1.92, 2.01};
  int N = sizeof(y) / sizeof(double*);
  int func = 1; // Decides on which approximation to use
  double c_mv = newton(c0, y, N, func); 
  printf("c_mv = %f\n", c_mv);
  
  //mandelbrot(512, 512, Graph2D);
  mandelbrot_2(512, 512, Graph2D);
  //mandelbrot_3(512, 512, Graph2D);


//--------------------------------------------------------------------------------
//---------------- visu sous XWINDOW ---------------------------------------------
//--------------------------------------------------------------------------------
  
  //Recalage-Egalise le graph
  Recal(Graph2D,length,width);
  Egalise(Graph2D,length,width,0.0);

  if (flag_graph)
  {
   //ouverture session graphique
    if (open_display()<0) printf(" Impossible d'ouvrir une session graphique");
      sprintf(nomfen_ppicture,"Graphe : ");
      win_ppicture=fabrique_window(nomfen_ppicture,10,10,width,length,zoom);
      x_ppicture=cree_Ximage(Graph2D,zoom,length,width);

    //Sauvegarde
    SaveImagePgm((char*)"",(char*)"FractalMandelbrot_QII.3",Graph2D,length,width);
    printf("\n\n Pour quitter,appuyer sur la barre d'espace");
    fflush(stdout);

    //boucle d'evenements
    for(;;)
    {
      XNextEvent(display,&ev);
      switch(ev.type)
      {
        case Expose:   
          XPutImage(display,win_ppicture,gc,x_ppicture,0,0,0,0,x_ppicture->width,x_ppicture->height);  
          break;

        case KeyPress: 
          XDestroyImage(x_ppicture);

          XFreeGC(display,gc);
          XCloseDisplay(display);
          flag_graph=0;
          break;
      }
      if (!flag_graph) break;
    }
  }
  
}


//---------------------------
//Algorithme NEWTON Helper functions
//---------------------------

/* Newton main function. Store old value to check difference.
   TODO:: Optimize */
double newton(double c_0, double* y, int N, int call_f) {
  double c_1 = c_0;
  double c_f;
  double df = 0.0;
  do {
    c_f = fcmv(c_1, y, N);
    c_0 = c_1;
    
    if (call_f == 1) {
      df = f_1_eps(c_0, y, N);
    } else if ( call_f == 2) {
      df = f_2_eps(c_0, y, N);
    } else {
      df = dfcmv(c_0, y, N);
    }
    
    c_1 = c_0 - c_f/df;

  } while (fabs(c_1) >= EPSILON_f && fabs(c_1 - c_0) >= EPSILON_f);

  return c_1;
}

/*
  Calculate the function approximation
    f(c_mv;y_i) =
    Sum(y_i^(c_mv) * ln(y_i)) / sum(y_i^(c_mv) -
    1 / c_mv - 1/N * sum(ln(y_i)
*/
double fcmv(double cmv, double* y, int N) {
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;

  for (int i=0; i < N; i++) {
    double pow_y = pow(y[i], cmv);
    double log_y = log(y[i]);
    sum1 = sum1 + pow_y * log_y;
    sum2 = sum2 + pow_y;
    sum3 = sum3 + log_y;
  }
  return sum1 / sum2 - 1 / cmv - 1/N * sum3;
}

/*  Calculate the analytical derivative of fcmv 
    Sum(y_i^(c_mv) * ln(y_i)^2) / 
    Sum(y_i^(c_mv) * ln(y_i)) 
    - 1/c_mv^2
    */
double dfcmv(double cmv, double* y, int N) {
  double sum1 = 0.0;
  double sum2 = 0.0;
  for (int i=0; i < N; i++) {
    double pow_y = pow(y[i], cmv);
    double log_y = log(y[i]);
    sum1 = sum1 + pow_y * CARRE(log_y);
    sum2 = sum2 + pow_y * log_y;
  }
  return sum1 / sum2 + 1/CARRE(cmv);
}

/*  Modify cmv value according to f'(x) = f(x+e) - f(x)/e */
double f_1_eps(double cmv, double* y, int N) {
  double f1 = fcmv(cmv+EPSILON, y, N);
  double f2 = fcmv(cmv, y, N);
  return (f1 - f2)/EPSILON;
}

/* Modify cmv to f'(x) = [-f(x+2e) + 8f(x+e) - 8f(x-e) + f(x-2e)]/12e */
double f_2_eps(double cmv, double* y, int N) {
  double f1 = fcmv(cmv+2*EPSILON, y, N);
  double f2 = fcmv(cmv+EPSILON, y, N);
  double f3 = fcmv(cmv-EPSILON, y, N);
  double f4 = fcmv(cmv-2*EPSILON, y, N);
  return (-f1 + 8*f2 - 8*f3 + f4) / (12*EPSILON);
}

//---------------------------
//Mandlebrot
//---------------------------

void mandelbrot(int width, int length, float** Graph2D) {
  int NbIterMax = 200;
  int z_diverge = 2;
  double c_x;
  double c_y;
  for (int k=0; k < width; k++) {
    for (int l=0; l < length; l++) {
      c_x = 2.0 * (k - width/1.35)/(width - 1);
      c_y = 2.0 * (l - length/2.0)/(length - 1);

      // z_0 = 0
      double z_x0 = 0, z_y0 = 0, z_xk = 0, z_yl = 0, z_n = 0;
      int color = 0;
      for (int i=0; i < NbIterMax; i++) {
        z_xk = CARRE(z_x0) - CARRE(z_y0) + c_x;
        z_yl = 2 * z_x0 * z_y0 + c_y;
        z_n = CARRE(z_xk) + CARRE(z_yl);
        double z_n_sqrt = sqrt(z_n);
        if (z_n_sqrt > z_diverge) {
          color = 255;
        }
        z_x0 = z_xk;
        z_y0 = z_yl;
      }
      Graph2D[k][l] = color;
    }
  }
}

void mandelbrot_3(int width, int length, float** Graph2D) {
  int NbIterMax = 200;
  int z_diverge = 2;
  double x_k, y_l;
  int chemin_x[NbIterMax], chemin_y[NbIterMax], color = 0;

  for (double k=0; k < width; k += 0.1) {
    for (double l=0; l < length; l += 0.1) {
      x_k = 2.0 * (k - width/1.35)/(width - 1);
      y_l = 2.0 * (l - length/2.0)/(length - 1);

      // z_0 = 0
      double z_x0 = 0, z_y0 = 0, z_xk = 0, z_yl = 0, z_n = 0;
      int i = 0;
      bool diverge = false;
 
      while(i < NbIterMax && !diverge) {
        z_xk = (pow(z_x0, 2.0) - pow(z_y0, 2.0) + x_k);
        z_yl = (2 * z_x0 * z_y0 + y_l);
        //z_n = sqrt(pow(z_xk, 2.0) + pow(z_yl,2.0));
        
        //sauver le chemin tant que ne diverge pas
        chemin_x[i] = (z_xk * (width - 1) / 2.0) + width/1.35;
        chemin_y[i] = (z_yl * (width - 1) / 2.0) + width/2.0;
 
        z_x0 = z_xk;
        z_y0 = z_yl;
        i++;

        diverge = sqrt(pow(z_xk, 2.0) + pow(z_yl,2.0)) > z_diverge;
        }

        if(i > 1 && !diverge){ 
          int j = 0;
          while(chemin_x[j] > 0){
            int k = chemin_x[j];
            int l = chemin_y[j];
            if(k < 512 && l < 512){
               Graph2D[k][l] += 1;
            }
            j++;
          }
        }
        
      }
      int chemin_x[NbIterMax];
      int chemin_y[NbIterMax];
    }
  }

void mandelbrot_2(int width, int length, float** Graph2D) {
  int NbIterMax = 200;
  int z_diverge = 2;
  double x_k, y_l;
  int chemin_x[NbIterMax], chemin_y[NbIterMax], color = 0;

  for (double k=0; k < width; k += 0.1) {
    for (double l=0; l < length; l += 0.1) {
      x_k = 2.0 * (k - width/1.35)/(width - 1);
      y_l = 2.0 * (l - length/2.0)/(length - 1);

      // z_0 = 0
      double z_x0 = 0, z_y0 = 0, z_xk = 0, z_yl = 0, z_n = 0;
      int i = 0;
      bool diverge = false;
 
      while(i < NbIterMax && !diverge) {
        z_xk = CARRE(z_x0) - CARRE(z_y0) + x_k;
        z_yl = (2 * z_x0 * z_y0 + y_l);
        //z_n = sqrt(pow(z_xk, 2.0) + pow(z_yl,2.0));
        
        //sauver le chemin tant que ne diverge pas
        chemin_x[i] = (z_xk * (width - 1) / 2.0) + width/1.35;
        chemin_y[i] = (z_yl * (width - 1) / 2.0) + width/2.0;
 
        z_x0 = z_xk;
        z_y0 = z_yl;
        i++;

        diverge = sqrt(CARRE(z_xk) + CARRE(z_yl)) > z_diverge;
        }

        if(i > 1 && diverge){ 
          int j = 0;
          while(chemin_x[j] > 0){
            int k = chemin_x[j];
            int l = chemin_y[j];
            if(k < 512 && k > 0 && l < 512 && l > 0){
               Graph2D[k][l] += 1;
            }
            j++;
          }
        }
        
      }
      int chemin_x[NbIterMax];
      int chemin_y[NbIterMax];
    }
}