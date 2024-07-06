#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "direct.h"
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t flag = 0;

void timerHandler(int signum)
{
    flag = 1;
}

struct fpdata 
{
  int fidx;
  double x[32]; 
  int rdAarry[32];
  int fpi;
};
struct resdata 
{
  int fpi;
  double x[32]; /* data */
  double maxerr;
  double fval;
  double rval;
  double cval;
};
// use mpfr to avoid 

// 辅助函数：将 long double 数组转换为 mpfr_t 数组
// void convert_to_mpfr(mpfr_t *mpfr_array, long double *ld_array, int size, mpfr_prec_t prec) {
//     for (int i = 0; i < size; i++) {
//         mpfr_init2(mpfr_array[i], prec);
//         mpfr_set_ld(mpfr_array[i], ld_array[i], MPFR_RNDN);
//     }
// }

// // 辅助函数：将 mpfr_t 数组的第一个元素转换回 long double
// long double convert_from_mpfr(mpfr_t *mpfr_array) {
//     long double result = mpfr_get_ld(mpfr_array[0], MPFR_RNDN);
//     // 清理资源
//     for (int i = 0; i < 32; i++) { 
//         mpfr_clear(mpfr_array[i]);
//     }
//     return result;
// }

// long double recursive_summationLD(long double *L) {
//     mpfr_t mpfr_L[32];
//     convert_to_mpfr(mpfr_L, L, 32, 128); // 转换为128位精度的MPFR数组

//     for (int i = 31; i > 0; i--) {
//         mpfr_add(mpfr_L[i-1], mpfr_L[i-1], mpfr_L[i], MPFR_RNDN); // 使用MPFR进行加法
//     }

//     return convert_from_mpfr(mpfr_L); // 获取结果并清理资源
// }

// long double pairwise_summationLD(long double *L){
//     unsigned int curra = 0, copya = 1, alen = 32, index[2];
//     mpfr_t arr[2][32];

//     // 初始化MPFR变量并从输入数组复制
//     for(int i=0; i<32; i++) {
//         mpfr_init2(arr[curra][i], 128);
//         mpfr_init2(arr[1][i], 128);
//         mpfr_set_ld(arr[curra][i], L[i], MPFR_RNDN);
//     }

//     while (alen > 1) {
//         index[0] = index[1] = 0;
//         for (index[curra] = 0; index[curra] < alen; index[curra] += 2) {
//             if (index[curra] + 2 > alen) {
//                 mpfr_set(arr[copya][index[copya]], arr[curra][index[curra]], MPFR_RNDN);
//                 index[copya]++;
//             } else {
//                 mpfr_add(arr[copya][index[copya]], arr[curra][index[curra]], arr[curra][index[curra]+1], MPFR_RNDN);
//                 index[copya]++;
//             }
//         }

//         alen = (alen % 2 == 1) ? (alen / 2 + 1) : (alen / 2);
//         curra = (curra + 1) % 2;
//         copya = (copya + 1) % 2;
//     }

//     long double result = mpfr_get_ld(arr[curra][0], MPFR_RNDN);

//     // 清理MPFR变量
//     for(int i = 0; i < 32; i++) {
//         mpfr_clear(arr[0][i]);
//         mpfr_clear(arr[1][i]);
//     }

//     return result;
// }

// long double compensated_summationLD(long double *L){
//     mpfr_t mpfr_sum, mpfr_value, mpfr_e, mpfr_L[32];
//     convert_to_mpfr(mpfr_L, L, 32, 128); // 转换为128位精度的MPFR数组

//     mpfr_init2(mpfr_sum, 128);
//     mpfr_init2(mpfr_value, 128);
//     mpfr_init2(mpfr_e, 128);
//     mpfr_set_d(mpfr_e, 0.0, MPFR_RNDN); // e = 0.0

//     for (int i = 31; i > 0; i--){
//         mpfr_set(mpfr_sum, mpfr_L[i], MPFR_RNDN); // sum = L[i]
//         mpfr_add(mpfr_value, mpfr_L[i-1], mpfr_e, MPFR_RNDN); // value = L[i-1] + e
//         mpfr_add(mpfr_L[i-1], mpfr_sum, mpfr_value, MPFR_RNDN); // L[i-1] = sum + value
//         mpfr_sub(mpfr_e, mpfr_sum, mpfr_L[i-1], MPFR_RNDN); // e = (sum - L[i-1])
//         mpfr_add(mpfr_e, mpfr_e, mpfr_value, MPFR_RNDN); // e += value
//     }

//     long double result = mpfr_get_ld(mpfr_L[0], MPFR_RNDN);

//     // 清理MPFR变量
//     for (int i = 0; i < 32; i++) { 
//         mpfr_clear(mpfr_L[i]);
//     }
//     mpfr_clear(mpfr_sum);
//     mpfr_clear(mpfr_value);
//     mpfr_clear(mpfr_e);

//     return result;
// }
long double recursive_summationLD(long double *L) {
  int i;
  int N = 32;
  for (i = N-1 ; i > 0 ; i--){
      L[i-1] += L[i];
  }
  return L[0];
}

long double pairwise_summationLD(long double *L){
  unsigned int curra = 0;
  unsigned int copya = 1;
  unsigned alen = 32;
  unsigned index[2];
  long double arr[2][32];
  for(int i=0;i<32;i++){
    arr[curra][i] = L[i];
  }
  while (alen > 1) {
      index[0] = index[1] = 0;
      for (index[curra] = 0 ; index[curra] < alen ; index[curra]=index[curra] + 2) {
        if (index[curra] + 2 > alen) {
              arr[copya][index[copya]] = arr[curra][index[curra]];
              index[copya]++;
        }
        else {
              arr[copya][index[copya]] = arr[curra][index[curra]] + arr[curra][index[curra]+1];
              index[copya]++;
        }
      }

      alen = (alen % 2 == 1) ? (alen / 2 + 1) : (alen / 2);
      curra = (curra + 1) % 2;
      copya = (copya + 1) % 2;
    }
    return arr[curra][0];

}
long double compensated_summationLD(long double *L){
  int i = 0;
  int N = 32;
  long double sum, value, e=0;
    for (i = N-1 ; i > 0 ; i--){
      sum = L[i];
      value = L[i-1] + e;
      L[i-1] = sum + value;
      e = (sum - L[i-1]) + value;
    }
    return L[0];
}

//matrix bench
// long double meschach_sumLD(long double *L){
//     mpfr_t sum_mpfr, tmp;
//     long double sum = 0.0;
//     int K = 4;
    
//     // 初始化MPFR变量
//     mpfr_init2(sum_mpfr, 300);  // 使用128位精度
//     mpfr_init2(tmp, 300);
//     mpfr_set_ld(sum_mpfr, 0.0, MPFR_RNDN);  // sum_mpfr = 0.0
    
//     for(int i = 0; i < K; i++) {
//         printf("L[%d] %.20e\n",i,(double)L[i]);
//         mpfr_set_ld(tmp,L[i], MPFR_RNDN); // 将long double的值设置给tmp
//         mpfr_add(sum_mpfr, sum_mpfr, tmp, MPFR_RNDN); // sum_mpfr += tmp
//         sum = mpfr_get_ld(tmp, MPFR_RNDN);
//         printf("sum[%d] %.20e\n",i,(double)sum);
//     }
//     mpfr_out_str(stdout, 10, 0, sum_mpfr, MPFR_RNDN);
    
//     // 将最终结果从mpfr_t转换回long double
//     sum = mpfr_get_ld(sum_mpfr, MPFR_RNDN);
    
//     // 清理MPFR变量占用的资源
//     mpfr_clear(sum_mpfr);
//     mpfr_clear(tmp);
//     return sum;
// }
long double meschach_sumLD(long double *L){
    long double sum = 0.0;
    for(int i=0;i<4;i++){
      sum+=L[i];
    }
    return sum;
}

long double meschach_2normLD(long double *L){
  long double norm = 0.0;
  int K = 4;
  for(int i=0;i<K;i++)
    norm += L[i]*L[i];
  norm = sqrt(norm);
  return norm;
}
long double meschach_1normLD(long double *L){
  long double norm = 0.0;
  int K = 4;
  for(int i=0;i<K;i++)
    norm += fabsl(L[i]);
  return norm;
}

long double meschach_dotLD(long double *L,long double *L2){
  int K = 4;
  long double sum = 0.0;
  for(int i=0;i<K;i++)
    sum += L[i]*L2[i];
  return sum;
  
}
void mltaddLD(long double *dp1, long double *dp2, long double s, int len)
{
    int	i;
    for ( i = 0; i < len; i++ )
      dp1[i]+= s*dp2[i];
}
long double meschach_convLD(long double *L1,long double *L2){
  int K = 4;
  long double Z[2*K-1];
  for(int i=0;i<2*K-1;i++){
    Z[i]= 0.0;
  }
  for(int i=0;i<K;i++)
    mltaddLD(&(Z[i]),L2,L1[i],4);
  return Z[3];
  
}

long double meschach_mvLD(long double *A,long double *X){
  int K = 4;
  long double Z[K];
  for(int i=0;i<K;i++){
    Z[i] = 0.0;
  }
  for(int i=0;i<K;i++)
    for(int j=0;j<K;j++)
        Z[i] += A[i*K+j]*X[j];
  return Z[0];
}
long double meschach_mmLD(long double *A,long double *B){
  int K = 4;
  long double Z[K*K];
  for(int i=0;i<K*K;i++){
    Z[i] = 0.0;
  }
  for(int i=0;i<K;i++){
    for(int j=0;j<K;j++)
        for(int k=0;k<K;k++)
          Z[i+j*K]+=A[i+k*K]*B[j*K+k];
  }
  return Z[0];
}


long double meschach_LULD(long double *A){
  int pivot[4];
	int	i, j, k, k_max, m, n;
	int	i_max;
	long double	A_v[4][4], *A_piv, *A_row;
	long double	max1, temp, tiny;
	double scale[4];

	m = 4;	n = 4;
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
	    A_v[i][j] = A[i*4+j];
    }
  }

	tiny = 10.0/HUGE_VAL;

	/* initialise pivot with identity permutation */
	for ( i=0; i<m; i++ )
		  pivot[i] = i;

	/* set scale parameters */
	for ( i=0; i<m; i++ )
	{
		max1 = 0.0;
		for ( j=0; j<n; j++ )
		{
			temp = fabsl(A_v[i][j]);
			max1 = (max1>temp)?max1:temp;
		}
		scale[i] = max1;
	}

	/* main loop */
	k_max = 3;
	for ( k=0; k<k_max; k++ )
	{
	    /* find best pivot row */
	    max1 = 0.0;	i_max = -1;
	    for ( i=k; i<m; i++ )
		if ( fabsl(scale[i]) >= tiny*fabsl(A_v[i][k]) )
		{
		    temp = fabsl(A_v[i][k])/scale[i];
		    if ( temp > max1 )
		    { max1 = temp;	i_max = i;	}
		}
	    
	    /* if no pivot then ignore column k... */
	    if ( i_max == -1 )
	    {
		/* set pivot entry A[k][k] exactly to zero,
		   rather than just "small" */
		A_v[k][k] = 0.0;
		continue;
	    }
	    
	    /* do we pivot ? */
	    if ( i_max != k )	/* yes we do... */
	    {
        temp = pivot[i_max];
        pivot[i_max] = pivot[k];
        pivot[k] = temp;
		for ( j=0; j<n; j++ )
		{
		    temp = A_v[i_max][j];
		    A_v[i_max][j] = A_v[k][j];
		    A_v[k][j] = temp;
		}
	    }
	    
	    /* row operations */
	    for ( i=k+1; i<m; i++ )	/* for each row do... */
	    {	/* Note: divide by zero should never happen */
        temp = A_v[i][k];
        A_v[i][k] = A_v[i][k]/A_v[k][k];
        A_piv = &(A_v[k][k+1]);
        A_row = &(A_v[i][k+1]);
        if ( k+1 < n )
              mltaddLD(A_row,A_piv,-temp,(int)(n-(k+1)));
	    }
	    
	}
	return A_v[3][3];
}


void hhvecLD(long double *vec,int i0,long double *beta,long double *out,long double *newval)
{
	long double	norm;

  for(int i=i0;i<4;i++){
    out[i]=vec[i];
  }
  long double sum = 0.0;
  for(int i=i0;i<4;i++)
    sum += out[i]*out[i];
	norm = sqrt(sum);
	if ( norm <= 0.0 )
	{
		*beta = 0.0;
    return;
	}
	*beta = 1.0/(norm * (norm+fabsl(out[i0])));
	if ( out[i0] > 0.0 )
		*newval = -norm;
	else
		*newval = norm;
	out[i0] -= *newval;
}

void hhtrcolsLD(long double (*M)[4],int i0,int j0,long double *hh,long double beta)
{
	/* Real	ip, scale; */
	int	i /*, k */;
  long double w[4];
	if ( beta == 0.0 )	return;
  for(i=0;i<4;i++){
    w[i]=0.0;
  }

	for ( i = i0; i < 4; i++ )
		mltaddLD(&(w[j0]),&(M[i][j0]),hh[i],(int)(4-j0));
	for ( i = i0; i < 4; i++ )
		mltaddLD(&(M[i][j0]),&(w[j0]),-beta*hh[i],(int)(4-j0));
	return;
}

long double meschach_QRLD(long double *L){
    int	k,limit;
    long double	beta;
    
    limit = 4;
    long double tmp1[4];
    long double A[4][4];
    long double diag[4];
    int K=4;
    for(int i=0;i<K;i++)
    {
      for(int j=0;j<K;j++)
      {
          A[i][j] = L[i*K+j];
      }

    }
    
    for ( k=0; k<limit; k++ )
    {
          for(int i=0;i<limit;i++){
            tmp1[i] = A[i][k];
          }
          hhvecLD(tmp1,k,&beta,tmp1,&A[k][k]);
          diag[k] = tmp1[k];

          hhtrcolsLD(A,k,k+1,tmp1,beta);
    }
    return A[3][3];

}

// 9 program from GSL
long double compute_wmeanLD(long double *w,int wstride, long double *data,int stride,int size){
  long double wmean = 0;
  long double W = 0;
  int i; 
  // for (i = 0; i < 4; i++){
  //   if (i<2) w[i] = 1; else w[i] = 2*w[i-1];
  // }


  for (i = 0; i < size; i++)
    {
      long double wi = w[i * wstride];

      if (wi > 0)
        {
          W += wi;
          wmean += (data[i * stride] - wmean) * (wi / W);
        }
    }

  return wmean;
}

long double test_gsl_wmeanLD(long double *w,long double *data){
  int K = 4;
  int size = 4;
  // long double w[K]; 
  // random inputs
  int i; 
  // for (i = 0; i < K; i++){
  //   if (i<2) w[i] = 1; else w[i] = 2*w[i-1];
  // }
  long double wmean=0.0;
  long double W = 0;
  int wstride = 1;
  int stride = 1;

  for (i = 0; i < size; i++)
    {
      long double wi = w[i * wstride];

      if (wi > 0)
        {
          W += wi;
          wmean += (data[i * stride] - wmean) * (wi / W);
        }
    }

  return wmean;
}
long double compute_wvarianceLD(long double *w, int wstride, long double *data, int stride, int n, long double wmean){
  long double wvariance = 0 ;
  long double W = 0;

  int i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      long double wi = w[i * wstride];

      if (wi > 0) {
        const long double delta = (data[i * stride] - wmean);
        W += wi ;
        wvariance += (delta * delta - wvariance) * (wi / W);
      }
    }

  return wvariance ;

}
long double compute_factorLD(long double *w, int wstride, int n){
   long double a = 0 ;
  long double b = 0;
  long double factor;

  int i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      long double wi = w[i * wstride];

      if (wi > 0)
        {
          a += wi ;
          b += wi * wi ;
        }
    }

  factor = (a*a) / ((a*a) - b);

  return factor ;

}
long double compute_wvariance_mLD(long double *w, int wstride,long double *data, int stride, int n, long double wmean){
  const long double variance = compute_wvarianceLD(w, wstride, data, stride, n, wmean);
  const long double scale = compute_factorLD(w, wstride, n);
  return scale * variance;
}

long double test_gsl_wvariance_mLD(long double *w,long double *data){
  int K = 4;
  long double wmean = compute_wmeanLD(w,1,data,1,K);
  long double wvariance = compute_wvariance_mLD(w, 1, data, 1, K, wmean);
  return wvariance;
}
long double test_gsl_wvariance_wLD(long double *w,long double *data){
  const long double wmean = compute_wmeanLD(w, 1, data, 1, 4);
  long double val = compute_wvarianceLD(w, 1, data, 1, 4, wmean);
  return val;
}
long double test_gsl_wvarianceLD(long double *w,long double *data){
  const long double wmean = compute_wmeanLD(w, 1, data, 1, 4);
  long double val = compute_wvariance_mLD(w, 1, data, 1, 4, wmean);
  return val;
}

long double compute_wsd_mLD(long double *w, int wstride,long double *data, int stride, int n, long double wmean){
  const long double variance = compute_wvarianceLD(w, wstride, data, stride, n, wmean);
  const long double scale = compute_factorLD(w, wstride, n);
  const long double wsd = sqrt(scale * variance) ;
  return wsd;
}


long double test_gsl_wsd_mLD(long double *w, long double *data){
    int K=4;
    long double wmean = compute_wmeanLD(w,1,data,1,K);
    long double wsd = compute_wsd_mLD(w,1,data,1,K,wmean);
    return wsd;
    // return wmean;
}

long double test_gsl_wsdLD(long double *w, long double *data){
    int K=4;
    long double wmean = compute_wmeanLD(w,1,data,1,K);
    long double wsd = compute_wsd_mLD(w,1,data,1,K,wmean);
    return wsd;
}

long double test_gsl_wsd_wLD(long double *w, long double *data){
    int K=4;
    long double wmean = compute_wmeanLD(w,1,data,1,K);
    long double variance = compute_wvarianceLD(w, 1, data, 1, K, wmean);
    long double wsd = sqrt(variance);
    return wsd;
}

long double compute_wtssLD(long double *w, int wstride,long double *data, int stride, int n, long double wmean){
long double wtss = 0 ;
  int i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      long double wi = w[i * wstride];

      if (wi > 0) {
        const long double delta = (data[i * stride] - wmean);
        wtss += wi * delta * delta;
      }
    }

  return wtss ;
}

long double test_gsl_wtss_mLD(long double *w, long double *data){
    int K = 4;
    long double wmean = compute_wmeanLD(w,1,data,1,K);
    long double wtss = compute_wtssLD(w, 1, data, 1, K, wmean);
    return wtss;
    // return wmean;
}

long double compute_wabsdev_mLD(long double *w, int wstride,long double *data, int stride, int n, long double wmean){
  long double wabsdev = 0;
  long double W = 0;

  int i;

  /* find the sum of the absolute deviations */
  for (i = 0; i < n; i++)
    {
      long double wi = w[i * wstride];
      
      if (wi > 0) {
        const long double delta = fabsl(data[i * stride] - wmean);
        W += wi ;
        wabsdev += (delta - wabsdev) * (wi / W);
      }
    }

  return wabsdev;
}

long double test_gsl_wabsdev_mLD(long double *w, long double *data){
    int K = 4;
    long double wabsdev, wmean;
    wmean       = compute_wmeanLD(w, 1, data, 1, K);
    wabsdev     = compute_wabsdev_mLD(w, 1, data, 1, K, wmean);
    return wabsdev;
}

long double compute_wkurtosis_m_sdLD(long double *w, int wstride,long double *data, int stride, int n, long double wmean,long double wsd){
  long double wavg = 0, kurtosis;
  long double W = 0;
  int i;

  /* find the fourth moment the deviations, normalized by the sd */

  /* we use a recurrence relation to stably update a running value so
     there aren't any large sums that can overflow */
 for (i = 0; i < n; i++)
    {
      long double wi = w[i * wstride];
      
      if (wi > 0) {
        const long double x = (data[i * stride] - wmean) / wsd;
        W += wi ;
        wavg += (x * x * x * x - wavg) * (wi / W);
      }
    }

  kurtosis = wavg - 3.0;  /* makes kurtosis zero for a Gaussian */

  return kurtosis;
}

long double test_gsl_wkurtosis_mLD(long double *w, long double *data){
    int K = 4;
    long double wkurtosis, wmean, wsd;
    wmean         = compute_wmeanLD(w, 1, data, 1, K);
    wsd = compute_wsd_mLD(w,1,data,1,K,wmean);
    wkurtosis     = compute_wkurtosis_m_sdLD(w, 1, data, 1, K, wmean, wsd);
    return wkurtosis;
}

long double compute_wskew_m_sdLD(long double *w, int wstride,long double *data, int stride, int n, long double wmean,long double wsd){
  /* Compute the weighted skewness of a dataset */

  long double wskew = 0;
  long double W = 0;

  int i;

  for (i = 0; i < n; i++)
    {
      long double wi = w[i * wstride];
      
      if (wi > 0) {
        const long double x = (data[i * stride] - wmean) / wsd;
        W += wi ;
        wskew += (x * x * x - wskew) * (wi / W);
      }
    }

  return wskew;
}

long double test_gsl_wkew_mLD(long double *w, long double *data){
    int K = 4;
    long double wmean, wsd, wskew;
    wmean         = compute_wmeanLD(w, 1, data, 1, K);
    wsd = compute_wsd_mLD(w,1,data,1,K,wmean);
    wskew     = compute_wskew_m_sdLD(w, 1, data, 1, K, wmean, wsd);
    return wskew;
}




extern double GLOBAL_ERROR;
double recursive_summation(double *L) {
  int i;
  int N = 32;
  for (i = N-1 ; i > 0 ; i--){
      L[i-1] += L[i];
  }
  return L[0];
}


double pairwise_summation(double *L){
  unsigned int curra = 0;
  unsigned int copya = 1;
  unsigned alen = 32;
  unsigned index[2];
  double arr[2][32];
  for(int i=0;i<32;i++){
    arr[curra][i] = L[i];
  }
  while (alen > 1) {
      index[0] = index[1] = 0;
      for (index[curra] = 0 ; index[curra] < alen ; index[curra]=index[curra] + 2) {
        if (index[curra] + 2 > alen) {
              arr[copya][index[copya]] = arr[curra][index[curra]];
              index[copya]++;
        }
        else {
              arr[copya][index[copya]] = arr[curra][index[curra]] + arr[curra][index[curra]+1];
              index[copya]++;
        }
      }

      alen = (alen % 2 == 1) ? (alen / 2 + 1) : (alen / 2);
      curra = (curra + 1) % 2;
      copya = (copya + 1) % 2;
    }
    return arr[curra][0];

}
double compensated_summation(double *L){
  int i = 0;
  int N = 32;
  double sum, value, e=0;
    for (i = N-1 ; i > 0 ; i--){
      sum = L[i];
      value = L[i-1] + e;
      L[i-1] = sum + value;
      e = (sum - L[i-1]) + value;
    }
    return L[0];
}

//matrix bench
double meschach_sum(double *L){
  double sum=0.0;
  int K = 4;
  for(int i=0;i<K;i++)
    sum +=L[i];
  return sum;
}

double meschach_2norm(double *L){
  double norm = 0.0;
  int K = 4;
  for(int i=0;i<K;i++)
    norm += L[i]*L[i];
  norm = sqrt(norm);
  return norm;
}
double meschach_1norm(double *L){
  double norm = 0.0;
  int K = 4;
  for(int i=0;i<K;i++)
    norm += fabs(L[i]);
  return norm;
}

double meschach_dot(double *L,double *L2){
  int K = 4;
  double sum = 0.0;
  for(int i=0;i<K;i++)
    sum += L[i]*L2[i];
  return sum;
  
}
void	mltadd(double	*dp1, double *dp2, double s, int len)
{
    int	i;
    
    for ( i = 0; i < len; i++ )
	    dp1[i] += s*dp2[i];
}
double meschach_conv(double *L1,double *L2){
  int K = 4;
  double Z[2*K-1];
  for(int i=0;i<2*K-1;i++){
    Z[i]= 0.0;
  }
  for(int i=0;i<K;i++)
    mltadd(&(Z[i]),L2,L1[i],4);
  return Z[3];
  
}
double meschach_mv(double *A,double *X){
  int K = 4;
  double Z[K];
  for(int i=0;i<K;i++){
    Z[i] = 0.0;
  }
  for(int i=0;i<K;i++)
    for(int j=0;j<K;j++)
        Z[i] += A[i*K+j]*X[j];
  // double sum=0.0;
  // for(int i=0;i<K;i++)
  //   sum += Z[i];
  return Z[0];
}
double meschach_mm(double *A,double *B){
  int K = 4;
  double Z[K*K];
  double tmp = 0.0;
  for(int i=0;i<K*K;i++){
    Z[i] = 0.0;
  }
  for(int i=0;i<K;i++){
    for(int j=0;j<K;j++)
        for(int k=0;k<K;k++)
          Z[i+j*K]+=A[i+k*K]*B[j*K+k];
  }
  return Z[0];
}


double meschach_LU(double *A){
  int pivot[4];
	int	i, j, k, k_max, m, n;
	int	i_max;
	double	A_v[4][4], *A_piv, *A_row;
	double	max1, temp, tiny;
	double	scale[4];

	m = 4;	n = 4;
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
	    A_v[i][j] = A[i*4+j];
    }
  }

	tiny = 10.0/HUGE_VAL;

	/* initialise pivot with identity permutation */
	for ( i=0; i<m; i++ )
		  pivot[i] = i;

	/* set scale parameters */
	for ( i=0; i<m; i++ )
	{
		max1 = 0.0;
		for ( j=0; j<n; j++ )
		{
			temp = fabs(A_v[i][j]);
			max1 = (max1>temp)?max1:temp;
		}
		scale[i] = max1;
	}

	/* main loop */
	k_max = 3;
	for ( k=0; k<k_max; k++ )
	{
	    /* find best pivot row */
	    max1 = 0.0;	i_max = -1;
	    for ( i=k; i<m; i++ )
		if ( fabs(scale[i]) >= tiny*fabs(A_v[i][k]) )
		{
		    temp = fabs(A_v[i][k])/scale[i];
		    if ( temp > max1 )
		    { max1 = temp;	i_max = i;	}
		}
	    
	    /* if no pivot then ignore column k... */
	    if ( i_max == -1 )
	    {
		/* set pivot entry A[k][k] exactly to zero,
		   rather than just "small" */
		A_v[k][k] = 0.0;
		continue;
	    }
	    
	    /* do we pivot ? */
	    if ( i_max != k )	/* yes we do... */
	    {
        temp = pivot[i_max];
        pivot[i_max] = pivot[k];
        pivot[k] = temp;
		for ( j=0; j<n; j++ )
		{
		    temp = A_v[i_max][j];
		    A_v[i_max][j] = A_v[k][j];
		    A_v[k][j] = temp;
		}
	    }
	    
	    /* row operations */
	    for ( i=k+1; i<m; i++ )	/* for each row do... */
	    {	/* Note: divide by zero should never happen */
        //temp = A_v[i][k] = A_v[i][k]/A_v[k][k];
        temp = A_v[i][k];
        A_v[i][k] = A_v[i][k]/A_v[k][k];
        A_piv = &(A_v[k][k+1]);
        A_row = &(A_v[i][k+1]);
        if ( k+1 < n )
              mltadd(A_row,A_piv,-temp,(int)(n-(k+1)));
		/*********************************************
		  for ( j=k+1; j<n; j++ )
		  A_v[i][j] -= temp*A_v[k][j];
		  (*A_row++) -= temp*(*A_piv++);
		  *********************************************/
	    }
	    
	}
	return A_v[3][3];
}


void hhvec(double *vec,int i0,double *beta,double *out,double *newval)
{
	double	norm;

  for(int i=i0;i<4;i++){
    out[i]=vec[i];
  }
  double sum = 0.0;
  for(int i=i0;i<4;i++)
    sum += out[i]*out[i];
	norm = sqrt(sum);
	if ( norm <= 0.0 )
	{
		*beta = 0.0;
    return;
	}
	*beta = 1.0/(norm * (norm+fabs(out[i0])));
	if ( out[i0] > 0.0 )
		*newval = -norm;
	else
		*newval = norm;
	out[i0] -= *newval;
}

void hhtrcols(double (*M)[4],int i0,int j0,double *hh,double beta)
{
	/* Real	ip, scale; */
	int	i ;
  double w[4];
	if ( beta == 0.0 )	return;
  for(i=0;i<4;i++){
    w[i]=0.0;
  }
	for ( i = i0; i < 4; i++ )
    mltadd(&(w[j0]),&(M[i][j0]),hh[i],
            (int)(4-j0));
	for ( i = i0; i < 4; i++ )
		mltadd(&(M[i][j0]),&(w[j0]),-beta*hh[i],
							(int)(4-j0));
	return;
}

double meschach_QR(double *L){
    int	k,limit;
    double	beta;
    limit = 4;
    double tmp1[4];
    double A[4][4];
    double diag[4];
    int K=4;
    for(int i=0;i<K;i++)
    {
      for(int j=0;j<K;j++)
      {
          A[i][j] = L[i*K+j];
      }

    }
    
    for ( k=0; k<limit; k++ )
    {
          for(int i=0;i<limit;i++){
            tmp1[i] = A[i][k];
          // printf("A %.20e\n",A[i][k]);
          }
          hhvec(tmp1,k,&beta,tmp1,&A[k][k]);
          diag[k] = tmp1[k];

          // printf("A into %.20e\n",A[k][k+1]);
          hhtrcols(A,k,k+1,tmp1,beta);
    }
    return A[3][3];

}

// 9 program from GSL
double compute_wmean(double *w,int wstride, double *data,int stride,int size){
  double wmean = 0;
  double W = 0;
  int i; 
  // for (i = 0; i < 4; i++){
  //   if (i<2) w[i] = 1; else w[i] = 2*w[i-1];
  // }


  for (i = 0; i < size; i++)
    {
      double wi = w[i * wstride];

      if (wi > 0)
        {
          W += wi;
          wmean += (data[i * stride] - wmean) * (wi / W);
        }
    }

  return wmean;
}

double test_gsl_wmean(double *w,double *data){
  int K = 4;
  int size = 4;
  int i; 
  double wmean=0.0;
  double W = 0;
  int wstride = 1;
  int stride = 1;

  for (i = 0; i < size; i++)
    {
      double wi = w[i * wstride];

      if (wi > 0)
        {
          W += wi;
          wmean += (data[i * stride] - wmean) * (wi / W);
        }
    }

  return wmean;
}
double compute_wvariance(double *w, int wstride, double *data, int stride, int n, double wmean){
  double wvariance = 0 ;
  double W = 0;
  int i; 


  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      double wi = w[i * wstride];

      if (wi > 0) {
        const double delta = (data[i * stride] - wmean);
        W += wi ;
        wvariance += (delta * delta - wvariance) * (wi / W);
      }
    }

  return wvariance ;
}
double compute_factor(double *w, int wstride, int n){
  double a = 0 ;
  double b = 0;
  double factor;

  int i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      double wi = w[i * wstride];

      if (wi > 0)
        {
          a += wi ;
          b += wi * wi ;
        }
    }

  factor = (a*a) / ((a*a) - b);

  return factor ;

}
double compute_wvariance_m(double *w, int wstride,double *data, int stride, int n, double wmean){
  const double variance = compute_wvariance(w, wstride, data, stride, n, wmean);
  const double scale = compute_factor(w, wstride, n);
  return scale * variance;
}

double test_gsl_wvariance_m(double *w,double *data){
  int K = 4;
  double wmean = compute_wmean(w,1,data,1,K);
  double wvariance = compute_wvariance_m(w, 1, data, 1, K, wmean);
  return wvariance;
}

double test_gsl_wvariance_w(double *w,double *data){
  const double wmean = compute_wmean(w, 1, data, 1, 4);
  double val = compute_wvariance(w, 1, data, 1, 4, wmean);
  return val;
}
double test_gsl_wvariance(double *w,double *data){
  const double wmean = compute_wmean(w, 1, data, 1, 4);
  double val = compute_wvariance_m(w, 1, data, 1, 4, wmean);
  return val;
}

double compute_wsd_m(double *w, int wstride,double *data, int stride, int n, double wmean){
  const double variance = compute_wvariance(w, wstride, data, stride, n, wmean);
  const double scale = compute_factor(w, wstride, n);
  const double wsd = sqrt(scale * variance);
  return wsd;
}


double test_gsl_wsd_m(double *w, double *data){
    int K = 4;
    double wmean = compute_wmean(w,1,data,1,K);
    double wsd = compute_wsd_m(w,1,data,1,K,wmean);
    return wsd;
}

double test_gsl_wsd(double *w, double *data){
    int K = 4;
    double wmean = test_gsl_wmean(w,data);
    double wsd = compute_wsd_m(w,1,data,1,K,wmean);
    return wsd;
}

double test_gsl_wsd_w(double *w, double *data){
    int K = 4;
    double wmean = compute_wmean(w,1,data,1,K);
    double variance = compute_wvariance(w, 1, data, 1, K, wmean);
    double wsd = sqrt(variance);
    return wsd;
}

double compute_wtss(double *w, int wstride,double *data, int stride, int n, double wmean){
 double wtss = 0 ;
  int i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
    {
      double wi = w[i * wstride];

      if (wi > 0) {
        const double delta = (data[i * stride] - wmean);
        wtss += wi * delta * delta;
      }
    }

  return wtss ;
}

double test_gsl_wtss_m(double *w, double *data){
    int K = 4;
    double wmean = compute_wmean(w,1,data,1,K);
    double wtss = compute_wtss(w, 1, data, 1, K, wmean);
    return wtss;
    // return wmean;
}

double compute_wabsdev_m(double *w, int wstride,double *data, int stride, int n, double wmean){
  double wabsdev = 0;
  double W = 0;

  int i;

  /* find the sum of the absolute deviations */
  for (i = 0; i < n; i++)
    {
      double wi = w[i * wstride];
      
      if (wi > 0) {
        const double delta = fabs(data[i * stride] - wmean);
        W += wi ;
        wabsdev += (delta - wabsdev) * (wi / W);
      }
    }

  return wabsdev;
}

double test_gsl_wabsdev_m(double *w, double *data){
    int K = 4;
    double wabsdev, wmean;
    wmean       = compute_wmean(w, 1, data, 1, K);
    wabsdev     = compute_wabsdev_m(w, 1, data, 1, K, wmean);
    return wabsdev;
}

double compute_wkurtosis_m_sd(double *w, int wstride,double *data, int stride, int n, double wmean,double wsd){
  double wavg = 0, kurtosis;
  double W = 0;
  int i;

  /* find the fourth moment the deviations, normalized by the sd */

  /* we use a recurrence relation to stably update a running value so
     there aren't any large sums that can overflow */

  for (i = 0; i < n; i++)
    {
      double wi = w[i * wstride];
      
      if (wi > 0) {
        const double x = (data[i * stride] - wmean) / wsd;
        W += wi ;
        wavg += (x * x * x * x - wavg) * (wi / W);
      }
    }

  kurtosis = wavg - 3.0;  /* makes kurtosis zero for a Gaussian */

  return kurtosis;
}

double test_gsl_wkurtosis_m(double *w, double *data){
    int K = 4;
    double wkurtosis, wmean, wsd;
    wmean         = compute_wmean(w, 1, data, 1, K);
    wsd = compute_wsd_m(w,1,data,1,K,wmean);
    wkurtosis     = compute_wkurtosis_m_sd(w, 1, data, 1, K, wmean, wsd);
    return wkurtosis;
}

double compute_wskew_m_sd(double *w, int wstride,double *data, int stride, int n, double wmean,double wsd){
  /* Compute the weighted skewness of a dataset */

  double wskew = 0;
  double W = 0;

  int i;

  for (i = 0; i < n; i++)
    {
      double wi = w[i * wstride];
      
      if (wi > 0) {
        const double x = (data[i * stride] - wmean) / wsd;
        W += wi ;
        wskew += (x * x * x - wskew) * (wi / W);
      }
    }

  return wskew;
}

double test_gsl_wkew_m(double *w, double *data){
    int K = 4;
    double wmean, wsd, wskew;
    wmean         = compute_wmean(w, 1, data, 1, K);
    wsd = compute_wsd_m(w,1,data,1,K,wmean);
    wskew     = compute_wskew_m_sd(w, 1, data, 1, K, wmean, wsd);
    return wskew;
}



double generate_random_float() {
    return ((double)rand() / RAND_MAX) * 200 - 100; // 生成-100到100之间的随机浮点数
}
double fooLD(const double*LL,int fidx){
  double result = 0.0;
  int option = fidx;
  long double L[32];
  for(int i = 0 ; i < 32 ; i++){
      L[i] =(long double) LL[i];
      //  printf("LL %.20e\n",LL[i]);
      //  printf("L %.20e\n",(double)L[i]);
  }
    switch(option) {
        case 0:
            result =(double) recursive_summationLD(L);
            break;
        case 1:
            result =(double) pairwise_summationLD(L);
            break;
        case 2:
            result =(double) compensated_summationLD(L);
            break;
        case 3:
            result =(double) meschach_sumLD(L);
            break;
        case 4:
            result =(double) meschach_2normLD(L);
            break;
        case 5:
            result =(double) meschach_1normLD(L);
            break;
        case 6:
            result =(double) meschach_dotLD(&L[0], &L[4]);
            break;
        case 7:
            result =(double) meschach_convLD(L, &L[4]);
            break;
        case 8:
            result =(double) meschach_mvLD(L, &L[16]);
            break;
        case 9:
            result =(double) meschach_mmLD(L, &L[16]);
            break;
        case 10:
            result =(double) meschach_LULD(L);
            break;
        case 11:
            result =(double) meschach_QRLD(L);
            break;
        case 12:
            result =(double) test_gsl_wmeanLD(L,&L[4]);
            break;
        case 13:
            result =(double) test_gsl_wvariance_mLD(L, &L[4]);
            break;
        case 14:
            result =(double) test_gsl_wvariance_wLD(L, &L[4]);
            break;
        case 15:
            result =(double) test_gsl_wsd_mLD(L, &L[4]);
            break;
        case 16:
            result =(double) test_gsl_wsd_wLD(L, &L[4]);
            break;
        case 17:
            result =(double) test_gsl_wtss_mLD(L, &L[4]);
            break;
        case 18:
            result =(double) test_gsl_wabsdev_mLD(L, &L[4]);
            break;
        case 19:
            result =(double) test_gsl_wkurtosis_mLD(L, &L[4]);
            break;
        case 20:
            result =(double) test_gsl_wkew_mLD(L, &L[4]);
            break;
        default:
            printf("Invalid option\n");
            break;
    }
  //return LL[0];
  return result; 
}
double foo(const double*LL,int fidx){
  double result = 0.0;
  int option = fidx;
  // printf("fidx %d\n",option);
  double L[32];
  for(int i = 0 ; i < 32 ; i++){
      L[i] = LL[i];
  }
    switch(option) {
        case 0:
            result = recursive_summation(L);
            break;
        case 1:
            result = pairwise_summation(L);
            break;
        case 2:
            result = compensated_summation(L);
            break;
        case 3:
            result = meschach_sum(L);
            break;
        case 4:
            result = meschach_2norm(L);
            break;
        case 5:
            result = meschach_1norm(L);
            break;
        case 6:
            result = meschach_dot(&L[0], &L[4]);
            break;
        case 7:
            result = meschach_conv(L, &L[4]);
            break;
        case 8:
            result = meschach_mv(L, &L[16]);
            break;
        case 9:
            result = meschach_mm(L, &L[16]);
            break;
        case 10:
            result = meschach_LU(L);
            break;
        case 11:
            result = meschach_QR(L);
            break;
        case 12:
            result = test_gsl_wmean(L,&L[4]);
            break;
        case 13:
            result = test_gsl_wvariance_m(L, &L[4]);
            break;
        case 14:
            result = test_gsl_wvariance_w(L, &L[4]);
            break;
        case 15:
            result = test_gsl_wsd_m(L, &L[4]);
            break;
        case 16:
            result = test_gsl_wsd_w(L, &L[4]);
            break;
        case 17:
            result = test_gsl_wtss_m(L, &L[4]);
            break;
        case 18:
            result = test_gsl_wabsdev_m(L, &L[4]);
            break;
        case 19:
            result = test_gsl_wkurtosis_m(L, &L[4]);
            break;
        case 20:
            result = test_gsl_wkew_m(L, &L[4]);
            break;
        default:
            printf("Invalid option\n");
            break;
    }
  //return LL[0];
  return result; 
}
double my_objective_function(int n, const double *x,
                             int *undfinedflag,
                             void *dataPtr) {
    if(flag==1){
      return 1.0;
    }
    struct fpdata *tmp_data = (struct fpdata *)dataPtr;
    int fidx = tmp_data->fidx;
    int fpi = tmp_data->fpi;
    for(int i=0;i<fpi;i++){
      // printf("rand %d\n",tmp_data->rdAarry[i]);
      tmp_data->x[tmp_data->rdAarry[i]]=x[i];
    }
    // printf("out\n");
    double val = foo(tmp_data->x,tmp_data->fidx);
    if(isnan(val)||isinf(val)){
      return 1.0;
    }
    if(GLOBAL_ERROR<=0.0){
      return 1.0;
    }else{
      return 1.0/GLOBAL_ERROR;
    }
}
const double *fpgenErr[] = {


};

const char *functions[] = {
        "recursive_summation",
        "pairwise_summation",
        "compensated_summation",
        "sum",
        "2norm",
        "1norm",
        "dot",
        "conv",
        "mv",
        "mm",
        "LU",
        "QR",
        "wmean",
        "wvariance_m",
        "wvariance_w",
        "wsd_m",
        "wsd_w",
        "wtss_m",
        "wabsdev_m",
        "wkurtosis_m",
        "wkew_m"
};
int contains(int *arr, int size, int num)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == num)
        {
            return 1;
        }
    }
    return 0;
}
void getRandAarryInt(int *randomArray, int fpi,int fparam){
    int index = 0;
    while (index < fpi)
    {
        int randomNum = rand() % fparam; // 生成0到32之间的随机整数

        // 检查随机整数是否已经在数组中
        if (!contains(randomArray, index, randomNum))
        {
            randomArray[index] = randomNum; // 将随机整数存入数组
            index++;
        }
    }

}
void longRunningTask(int fidx,int fparam, struct resdata *res,int timelimit){
  double x[fparam];
  double max_err = 0.0;
  struct fpdata data;
  // struct resdata res;
  void *dataPtr = (void *)&data;
  data.fidx = fidx;
  int sample_num = 4000;
  int force_stop = 0;
  double val,val2;
  struct itimerval timer;
  timer.it_value.tv_sec = timelimit; 
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 0; 
  timer.it_interval.tv_usec = 0;
  int randomArray[fparam];
  double testX[fparam];
  double lb[fparam],ub[fparam];
  double relErr = 0.0;
  double y;
  int random_int;
  double cval;

  signal(SIGALRM, timerHandler);

  setitimer(ITIMER_REAL, &timer, NULL);

  while (1)
  {
    srand(time(NULL));
    for(int i=0;i<fparam;i++){
        x[i] = generate_random_float();
        data.x[i] = x[i];
    }
    for(int fpi=1;fpi<=fparam;fpi++){
      // printf("fpi %d flag %d\n",fpi,flag);
      for(int i=0;i<fpi;i++){
        lb[i] = -100;
        ub[i] = 100;
        testX[i]=x[i];
      }
      random_int = sample_num * fpi;
      getRandAarryInt(randomArray,fpi,fparam);
      for(int tpf=0;tpf<fpi;tpf++){
        // printf("%d,",randomArray[tpf]);
        data.rdAarry[tpf] = randomArray[tpf];
      }
      // printf("\n");
      data.fpi = fpi;
      direct_optimize(my_objective_function,dataPtr, fpi, lb,ub,testX,&y,random_int,(int)(random_int/40), 1.0, 0, 0, 0,
                            0, 0,
                            &force_stop,
                            DIRECT_UNKNOWN_FGLOBAL, 0,
                            NULL, DIRECT_GABLONSKY);
      for(int i=0;i<fpi;i++){
        data.x[randomArray[i]]=testX[i];
      }
      val = foo(data.x,fidx);
      cval = GLOBAL_ERROR;
      val2 = fooLD(data.x,fidx);
      if(val2==0.0){
      if(val==0.0){
        relErr=0.0;
      }else{
        relErr=1.0;
      }
      }else{
        relErr = fabs((val-val2)/val2);
      }
      if(relErr>res->maxerr){
        res->fpi = fpi;
        res->maxerr = relErr;
        for(int i=0;i<fparam;i++){
          res->x[i]=data.x[i];
        }
        res->cval=cval;
        res->fval=val;
        res->rval=val2;
      }
      if (flag == 1){
        break;
      }
    }
    if (flag == 1){
      break;
    }
  }
}


void run_test(int fidx,int fparam,int timelimit){
  struct timeval start_time, end_time;
  double elapsed_time;

    // 获取开始时间
  gettimeofday(&start_time, NULL);
  struct resdata res;
  for(int i=0;i<fparam;i++){
    res.x[i]=0.0;
  }
  FILE *fp;
  fp = fopen("output2.csv", "a");
  printf("function id is %d \n",fidx);
  printf("Searching for maximum error\n");
  longRunningTask(fidx,fparam,&res,timelimit);

  gettimeofday(&end_time, NULL);

  elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

   printf("Total time is: %f seconds\n", elapsed_time);
   printf("Function id: %d, search input num: %d, name %s,maximum error:%.20e, function output: %.20e, real output:%.20e,condition number: %.20e, time: %.20e\n",fidx,res.fpi,functions[fidx],res.maxerr,res.fval,res.rval,res.cval,elapsed_time);
   fprintf(fp,"####################################################################################################\n");
   fprintf(fp,"Function id: %d, search input num: %d, name %s,maximum error:%.20e, function output: %.20e, real output:%.20e,condition number: %.20e, time: %.20e\n",fidx,res.fpi,functions[fidx],res.maxerr,res.fval,res.rval,res.cval,elapsed_time);
   fprintf(fp,"input list is:");
   for(int i=0;i<fparam;i++){
    fprintf(fp,"%a,",res.x[i]);
   }
   fprintf(fp,"\n\n\n\n\n");
   fclose(fp);
}

int main(int argc, char *argv[]) {
  int input_params[] = {32,32,32,4,4,4,8,8,20,32,16,16,8,8,8,8,8,8,8,8,8};
  int fidx = atoi(argv[1]);
  int timelimit = atoi(argv[2]);
  int fparam = input_params[fidx];
  printf("######################\n");
  printf("%s\n",functions[fidx]);
  run_test(fidx,fparam,timelimit);

  return 0;
}
