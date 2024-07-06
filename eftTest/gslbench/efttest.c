#include <math.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_sf.h>
#include <float.h>
#include <stdlib.h>
#include <gsl/gsl_ieee_utils.h>

extern double GLOBAL_ERROR;
//extern double sum_error;
//extern int sum_count;

typedef struct Node {
  double bits_error;
  struct Node* next;
} Node;

typedef struct {
  Node* top;
} Stack;

void initStack(Stack* s){
  s->top = NULL;
}

int isEmpty(Stack *s){
  if (s->top == NULL){
    return 0;
  }
  return 1;
}

void push(Stack *s, int bits_error){
  Node* newNode = (Node*)malloc(sizeof(Node));
  if (newNode == NULL){
    printf("Memory allocation failed\n");
    return;
  }
  newNode->bits_error = bits_error;
  newNode->next = s->top;
  s->top = newNode;
}

int pop(Stack *s){
  if (isEmpty(s)){
    printf("Stack is empyt!\n");
    return -1;
  }
  Node* temp = s->top;
  int popData = temp->bits_error;
  s->top = temp->next;
  free(temp);
  return popData;
}

void clearStack(Stack *s){
  while(!isEmpty(s)){
    pop(s);
  }
}
char* GSLFunArray[] = {
  "gsl_sf_legendre_P2",
  "gsl_sf_legendre_P3",
  "gsl_sf_legendre_Q1",
  "gsl_sf_psi",
  "gsl_sf_psi_1",
  "gsl_sf_sin",
  "gsl_sf_cos",
  "gsl_sf_sinc",
  "gsl_sf_lnsinh",
  "gsl_sf_zeta",
  "gsl_sf_zetam1",
  "gsl_sf_eta",
  "gsl_sf_airy_Ai",
  "gsl_sf_airy_Bi",
  "gsl_sf_airy_Ai_scaled",
  "gsl_sf_airy_Bi_scaled",
  "gsl_sf_airy_Ai_deriv",
  "gsl_sf_airy_Bi_deriv",
  "gsl_sf_airy_Ai_deriv_scaled",
  "gsl_sf_airy_Bi_deriv_scaled",
  "gsl_sf_bessel_J0",
  "gsl_sf_bessel_J1",
  "gsl_sf_bessel_Y0",
  "gsl_sf_bessel_Y1",
  "gsl_sf_bessel_j1",
  "gsl_sf_bessel_j2",
  "gsl_sf_bessel_y0",
  "gsl_sf_bessel_y1",
  "gsl_sf_bessel_y2",
  "gsl_sf_clausen",
  "gsl_sf_dilog",
  "gsl_sf_expint_E1",
  "gsl_sf_expint_E2",
  "gsl_sf_expint_E1_scaled",
  "gsl_sf_expint_E2_scaled",
  "gsl_sf_expint_Ei",
  "gsl_sf_expint_Ei_scaled",
  "gsl_sf_Chi",
  "gsl_sf_Ci",
  "gsl_sf_lngamma",
  "gsl_sf_lambert_W0",
  "gsl_sf_lambert_Wm1",
};

double GSLInputs[42] = {
  -5.773502691896254e-01,
  7.745966692414830e-01,
  8.335565596009644e-01,
  -6.678418213073426e+00,
  -4.799999999999998e+01,
  -5.037566598712291e+17,
  -1.511080519199221e+17,
  3.050995817918706e+15,
  8.813735870195427e-01,
  -9.999999999999984e+00,
  -1.699999999999999e+02,
  -9.999999999999989e+00,
  -4.042852549222488e+11,
  -7.237129918123468e+11,
  -3.073966210399579e+11,
  -8.002750158072251e+11,
  -1.018792971647468e+00,
  -2.294439682614124e+00,
  -1.018792971647467e+00,
  -2.294439682614120e+00,
  2.404825557695774e+00,
  3.831705970207514e+00,
  3.957678419314854e+00,
  2.197141326031017e+00,
  -7.725251836937709e+00,
  9.095011330476359e+00,
  2.585919463588284e+17,
  9.361876298934626e+16,
  1.586407411088372e+17,
  1.252935780352301e+14,
  1.259517036984501e+01,
  -3.725074107813663e-01,
  -1.347155251069168e+00,
  -3.725074107813663e-01,
  -2.709975303391678e+228,
  3.725074107813668e-01,
  3.725074107813666e-01,
  5.238225713898647e-01,
  2.311778262696607e+17,
  -2.457024738220797e+00,
  1.666385643189201e-41,
  1.287978304826439e-121
};

double foo(double x,int index) {
  double val = 0.0;
  if(index == 0){
    val = gsl_sf_legendre_P2(x);
  } else if (index==1){
    val = gsl_sf_legendre_P3(x);
  } else if (index==2){
    val = gsl_sf_legendre_Q1(x);
  } else if (index==3){
    val = gsl_sf_psi(x);
  } else if (index==4){
    val = gsl_sf_psi_1(x);
  } else if (index==5){
    val = gsl_sf_sin(x);
  } else if (index==6){
    val = gsl_sf_cos(x);
  } else if (index==7){
    val = gsl_sf_sinc(x);
  } else if (index==8){
    val = gsl_sf_lnsinh(x);
  } else if (index==9){
    val = gsl_sf_zeta(x);
  } else if (index==10){
    val = gsl_sf_zetam1(x);
  } else if (index==11){
    val = gsl_sf_eta(x);
  } else if (index==12){
    val = gsl_sf_airy_Ai(x,GSL_PREC_DOUBLE);
  } else if (index==13){
    val = gsl_sf_airy_Bi(x,GSL_PREC_DOUBLE);
  } else if (index==14){
    val = gsl_sf_airy_Ai_scaled(x,GSL_PREC_DOUBLE);
  } else if (index==15){
    val = gsl_sf_airy_Bi_scaled(x,GSL_PREC_DOUBLE);
  } else if (index==16){
    val = gsl_sf_airy_Ai_deriv(x,GSL_PREC_DOUBLE);
  } else if (index==17){
    val = gsl_sf_airy_Bi_deriv(x,GSL_PREC_DOUBLE);
  } else if (index==18){
    val = gsl_sf_airy_Ai_deriv_scaled(x,GSL_PREC_DOUBLE);
  } else if (index==19){
    val = gsl_sf_airy_Bi_deriv_scaled(x,GSL_PREC_DOUBLE);
  } else if (index==20){
    val = gsl_sf_bessel_J0(x);
  } else if (index==21){
    val = gsl_sf_bessel_J1(x);
  } else if (index==22){
    val = gsl_sf_bessel_Y0(x);
  } else if (index==23){
    val = gsl_sf_bessel_Y1(x);
  } else if (index==24){
    val = gsl_sf_bessel_j1(x);
  } else if (index==25){
    val = gsl_sf_bessel_j2(x);
  } else if (index==26){
    val = gsl_sf_bessel_y0(x);
  } else if (index==27){
    val = gsl_sf_bessel_y1(x);
  } else if (index==28){
    val = gsl_sf_bessel_y2(x);
  } else if (index==29){
    val = gsl_sf_clausen(x);
  } else if (index==30){
    val = gsl_sf_dilog(x);
  } else if (index==31){
    val = gsl_sf_expint_E1(x);
  } else if (index==32){
    val = gsl_sf_expint_E2(x);
  } else if (index==33){
    val = gsl_sf_expint_E1_scaled(x);
  } else if (index==34){
    val = gsl_sf_expint_E2_scaled(x);
  } else if (index==35){
    val = gsl_sf_expint_Ei(x);
  } else if (index==36){
    val = gsl_sf_expint_Ei_scaled(x);
  } else if (index==37){
    val = gsl_sf_Chi(x);
  } else if (index==38){
    val = gsl_sf_Ci(x);
  } else if (index==39){
    val = gsl_sf_lngamma(x);
  } else if (index==40){
    val = gsl_sf_lambert_W0(x);
  } else if (index==41){
    val = gsl_sf_lambert_Wm1(x);
  }
  return val;
}

int main(int argc, char *argv[]) {
  double val = 0.0;
  //int index = atoi(argv[1]);
  //const char* output_file = argv[2];
  //FILE *fp = fopen(output_file, "w");
  //if (fp == NULL) {
  //  perror("Unable to open output file");
  //  return 1;
  //}
  for(int i=0;i<42;i++){
	  int index = i;
	  double input = GSLInputs[index];
	  val = foo(input,index);
	  double relerr = fabs(GLOBAL_ERROR/(val+GLOBAL_ERROR));

	  printf("GSLFunc: %s output is %.20e relative error is %.20e\n",GSLFunArray[index],val, relerr);
  }
  return 0;
}
