# Detecting Floating-Point Errors via Chain Conditions

## Introduction

FPCC is a tool to support detecting significant floating-point errors in numerical functions. Floating point is notorious for the rounding errors, which could propagate and accumulate to unacceptable results. Detecting inputs that can trigger large floating-point errors is crucial for improving the reliability of numerical programs. Existing error-inducing input generation approaches either heavily depend on the expensive shadow executions with high precision computations or suffer from false positives. This paper introduces chain conditions to capture the propagation and accumulation of floating-point errors and utilize them to guide the search for error-inducing inputs. We conduction experiments on 88 GSL functions and 21 multiple-inputs numerical programs from FPGen's Benchmarks (https://github.com/ucd-plse/FPGen). The experimental results demonstrate the effectiveness and efficiency of FPCC:

 - FPCC achieves 100\% accuracy in detecting significant errors for the reported rank-1 inputs, while 72.69\% rank-1 inputs of the state-of-the-art tool ATOMU can trigger significant errors. Overall, 99.64\% (1049/1053) of the inputs reported by FPCC can trigger significant errors, whereas only 19.45\% (141/723) of the inputs reported by ATOMU can trigger significant errors; 
 
 - FPCC exhibits a 2.17x speedup over ATOMU in detecting significant errors; 
 
 - FPCC also excels in supporting multiple inputs functions, outperforming FPGen (https://github.com/ucd-plse/FPGen).



## Hardware Dependencies

We conduct experiments on a desktop running Ubuntu 18.04 LTS with an Intel Core i9-13900 @5.20 GHz CPU and 32GB RAM. 

## Setup

Install the [docker](https://www.docker.com/). Here are some guides for install [docker for Ubuntu](https://docs.docker.com/install/linux/docker-ce/ubuntu/) and [docker for MacOS](https://docs.docker.com/docker-for-mac/install/)

Clone this repo to your local workspace:

```
$ git clone https://github.com/DataReportRe/FPCC.git
```

The `docker` folder contains a Dockerfile that can be used to build our implementation. If an error about permission denied occurs, try to run docker in root `sudo docker ...`.

```
$ cd FPCC/docker
$ docker build -t fpcc .
```

It may takes a few minutes for installing necessary packages and compiling the whole GSL Library, note that both FPCC and ATOMU will be installed after build the docker.

You can also just pull the image from docker (**recommond**):
```
$ docker pull fpccdocker/fpcc:v1.0
```


## Usage

Run this docker's container with interactive mode, the working directory is at `/fpcc`.

```
$ docker run -it fpcc /bin/bash
```

### Play with the Motivation Example

The small example `foo` in `src/targetExample.c` computes the motivation example in our paper: `p1 = 0.375 + (a-x*(0.25+b)) and p2=0.375 * (a-x*(0.25+b))`

Run on the small example `foo,foo2`, defined in `src/targetExample.c`:

```
( In docker's container: /fpcc )
$ make
$ bin/gslSolver.out example 0 0
$ bin/gslSolver.out example 1 0
```

If you want to play with your own function, just modify the `foo` function in `src/targetExample.c`, then

```
$ make
$ bin/gslSolver.out example
```

### Run on GSL Functions

```
$ make
$ bin/gslSolver.out gsl <function_index> <error_trace_flag>
```

#### Compute Relative Error

Using the oracle from `mpmath` to compute the relative error:

```
$ make
$ bin/gslSolver.out gsl <function_index> <error_trace_flag> && python3 script/oracleMpmath.py
```

Example: GSL function `gsl_sf_lngamma` has the index 73,

```
$ bin/gslSolver.out gsl 73 0 && python3 script/oracleMpmath.py
...
Function Index: 73
Max Relative Error:
  Input:  -2.457024738220797e+00
  Output: 3.552713678800501e-15
  Oracle: 4.767633887335327e-15
        Relative Error: 2.54827e-01
```

If you want trace error, set the <error_trace_flag> to 1:
```
$ bin/gslSolver.out gsl 73 1
```
The trace results will be stored in errorTrace.out

### Reproduce the Results in Paper

Please note that it will take approximately 1.5 hours to obtain all the results mentioned in the paper.

#### Getting the results of fpcc for GSL functions

You can just run one script to get all results of fpcc for GSL functions in the paper:

```
$ python3 fpcc_test.py
```

All results will be stored in /fpcc/result_analysis/outs_fpcc (it will take 20 minutes)

#### Getting the results of fpcc for FPGen's Benchmarks

You can just run one script to get all results of fpcc for 21 multiple-inputs numerical programs from FPGen's Benchmarks in the paper, it will take 35 minutes:

```
$ cd /fpcc/fpgenBench/bench
$ ./run.sh
```

Results of Table 3 for fpcc will be stored in output2.csv
(To obtain results for FPGen, please refer to the Readme in https://github.com/ucd-plse/FPGen. Please note that the process may take up to 63 hours and is not recommended. Alternatively, you can verify FPGen results by  FPGen's paper titled "Efficient Generation of Error-Inducing Floating-Point Inputs via Symbolic Execution." )

#### Getting the results of atom

Similarly, you can just run one script to get all results of atom in paper:

```
$ cd /atom
$ python3 atomic_test.py
```

All results will be stored in /fpcc/result_analysis/outs_atom (it will take 40 minutes)

#### Getting the tables and figures for GSL fuctions in paper

After get results of FPCC and ATOMU, you can just run the following script to get tables and figures in paper:

```
$ cd /fpcc/result_analysis
$ python3 results_analyasis.py
```

Table 1 will be stored in /fpcc/result_analysis/res_table_FPCC.xls

Table 2 will be stored in /fpcc/result_analysis/res_table_FPCC_NoErr.xls

Figure 7 will be stored in /fpcc/result_analysis/graph/compareTimes.pdf

Figure 8 will be stored in /fpcc/result_analysis/graph/err_num_stable.pdf

Figure 9 will be stored in /fpcc/result_analysis/graph/top1_err_stable.pdf


#### Getting the results for EFTSanitier in paper


```
$ cd /fpcc/eftTest/gslbench
$ ./run.sh 
```

The results of Table 5 for eftsanitizer will direct output.


#### Getting the tracing results in section 5.4 and appendix

Run traceErrors42.py to get the trace results:

```
$ python3 traceErrors42.py
```

Results will be stored in /fpcc/result_analysis/errorTrace.out. And run script in /fpcc/result_analysis

```
$ python3 errorTraceAna.py
```

Results for each function will be stored in /fpcc/result_analysis/localErr.xls (Original data for Table 4)

## GSL Function List and mpmath Support

| GSL Function Name           | Index | mpmath  support? |
| --------------------------- | ----- | ---------------- |
| gsl_sf_airy_Ai              | 0     | ✔️             |
| gsl_sf_airy_Bi              | 1     | ✔️             |
| gsl_sf_airy_Ai_scaled       | 2     | ✔️             |
| gsl_sf_airy_Bi_scaled       | 3     | ✔️             |
| gsl_sf_airy_Ai_deriv        | 4     | ✔️             |
| gsl_sf_airy_Bi_deriv        | 5     | ✔️             |
| gsl_sf_airy_Ai_deriv_scaled | 6     | ✔️             |
| gsl_sf_airy_Bi_deriv_scaled | 7     | ✔️             |
| gsl_sf_bessel_J0            | 8     | ✔️             |
| gsl_sf_bessel_J1            | 9     | ✔️             |
| gsl_sf_bessel_Y0            | 10    | ✔️             |
| gsl_sf_bessel_Y1            | 11    | ✔️             |
| gsl_sf_bessel_I0            | 12    | ✔️             |
| gsl_sf_bessel_I1            | 13    | ✔️             |
| gsl_sf_bessel_I0_scaled     | 14    | ✔️             |
| gsl_sf_bessel_I1_scaled     | 15    | ✔️             |
| gsl_sf_bessel_K0            | 16    | ✔️             |
| gsl_sf_bessel_K1            | 17    | ✔️             |
| gsl_sf_bessel_K0_scaled     | 18    | ✔️             |
| gsl_sf_bessel_K1_scaled     | 19    | ✔️             |
| gsl_sf_bessel_j0            | 20    | ✔️             |
| gsl_sf_bessel_j1            | 21    | ✔️             |
| gsl_sf_bessel_j2            | 22    | ✔️             |
| gsl_sf_bessel_y0            | 23    | ✔️             |
| gsl_sf_bessel_y1            | 24    | ✔️             |
| gsl_sf_bessel_y2            | 25    | ✔️             |
| gsl_sf_bessel_i0_scaled     | 26    | ✔️             |
| gsl_sf_bessel_i1_scaled     | 27    | ✔️             |
| gsl_sf_bessel_i2_scaled     | 28    | ✔️             |
| gsl_sf_bessel_k0_scaled     | 29    | ✔️             |
| gsl_sf_bessel_k1_scaled     | 30    | ✔️             |
| gsl_sf_bessel_k2_scaled     | 31    | ✔️             |
| gsl_sf_clausen              | 32    | ✔️             |
| gsl_sf_dawson               | 33    |                  |
| gsl_sf_debye_1              | 34    |                  |
| gsl_sf_debye_2              | 35    |                  |
| gsl_sf_debye_3              | 36    |                  |
| gsl_sf_debye_4              | 37    |                  |
| gsl_sf_debye_5              | 38    |                  |
| gsl_sf_debye_6              | 39    |                  |
| gsl_sf_dilog                | 40    | ✔️             |
| gsl_sf_ellint_Kcomp         | 41    | ✔️             |
| gsl_sf_ellint_Ecomp         | 42    | ✔️             |
| gsl_sf_ellint_Dcomp         | 43    |                  |
| gsl_sf_erfc                 | 44    | ✔️             |
| gsl_sf_log_erfc             | 45    | ✔️             |
| gsl_sf_erf                  | 46    | ✔️             |
| gsl_sf_erf_Z                | 47    | ✔️             |
| gsl_sf_erf_Q                | 48    | ✔️             |
| gsl_sf_hazard               | 49    | ✔️             |
| gsl_sf_exp                  | 50    | ✔️             |
| gsl_sf_expm1                | 51    | ✔️             |
| gsl_sf_exprel               | 52    | ✔️             |
| gsl_sf_exprel_2             | 53    | ✔️             |
| gsl_sf_expint_E1            | 54    | ✔️             |
| gsl_sf_expint_E2            | 55    | ✔️             |
| gsl_sf_expint_E1_scaled     | 56    | ✔️             |
| gsl_sf_expint_E2_scaled     | 57    | ✔️             |
| gsl_sf_expint_Ei            | 58    | ✔️             |
| gsl_sf_expint_Ei_scaled     | 59    | ✔️             |
| gsl_sf_Shi                  | 60    | ✔️             |
| gsl_sf_Chi                  | 61    | ✔️             |
| gsl_sf_expint_3             | 62    |                  |
| gsl_sf_Si                   | 63    | ✔️             |
| gsl_sf_Ci                   | 64    | ✔️             |
| gsl_sf_atanint              | 65    |                  |
| gsl_sf_fermi_dirac_m1       | 66    | ✔️             |
| gsl_sf_fermi_dirac_0        | 67    | ✔️             |
| gsl_sf_fermi_dirac_1        | 68    | ✔️             |
| gsl_sf_fermi_dirac_2        | 69    | ✔️             |
| gsl_sf_fermi_dirac_mhalf    | 70    | ✔️             |
| gsl_sf_fermi_dirac_half     | 71    | ✔️             |
| gsl_sf_fermi_dirac_3half    | 72    | ✔️             |
| gsl_sf_lngamma              | 73    | ✔️             |
| gsl_sf_gamma                | 74    | ✔️             |
| gsl_sf_gammastar            | 75    |                  |
| gsl_sf_gammainv             | 76    | ✔️             |
| gsl_sf_lambert_W0           | 77    | ✔️             |
| gsl_sf_lambert_Wm1          | 78    | ✔️             |
| gsl_sf_legendre_P1          | 79    | ✔️             |
| gsl_sf_legendre_P2          | 80    | ✔️             |
| gsl_sf_legendre_P3          | 81    | ✔️             |
| gsl_sf_legendre_Q0          | 82    | ✔️             |
| gsl_sf_legendre_Q1          | 83    | ✔️             |
| gsl_sf_log                  | 84    | ✔️             |
| gsl_sf_log_abs              | 85    | ✔️             |
| gsl_sf_log_1plusx           | 86    | ✔️             |
| gsl_sf_log_1plusx_mx        | 87    | ✔️             |
| gsl_sf_psi                  | 88    | ✔️             |
| gsl_sf_psi_1piy             | 89    |                  |
| gsl_sf_psi_1                | 90    | ✔️             |
| gsl_sf_sin_pi               | 91    |                  |
| gsl_sf_cos_pi               | 92    |                  |
| gsl_sf_synchrotron_1        | 93    |                  |
| gsl_sf_synchrotron_2        | 94    | ✔️             |
| gsl_sf_transport_2          | 95    |                  |
| gsl_sf_transport_3          | 96    |                  |
| gsl_sf_transport_4          | 97    |                  |
| gsl_sf_transport_5          | 98    |                  |
| gsl_sf_sin                  | 99    | ✔️             |
| gsl_sf_cos                  | 100   | ✔️             |
| gsl_sf_sinc                 | 101   | ✔️             |
| gsl_sf_lnsinh               | 102   | ✔️             |
| gsl_sf_lncosh               | 103   | ✔️             |
| gsl_sf_zeta                 | 104   | ✔️             |
| gsl_sf_zetam1               | 105   | ✔️             |
| gsl_sf_eta                  | 106   | ✔️             |
