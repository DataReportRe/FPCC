import pickle
import math
from mpmath import sign
import xlrd
from xlutils.copy import copy
import xlwt
import mpmath
import numpy as np
import struct
import matplotlib.pyplot as plt
from scipy.stats import ranksums
def floatToRawLongBits(value):
	return struct.unpack('Q', struct.pack('d', value))[0]

def longBitsToFloat(bits):
	return struct.unpack('d', struct.pack('Q', bits))[0]
#ATOM functions List

GSLFuncList = [
    "gsl_sf_airy_Ai_e", 
    "gsl_sf_airy_Bi_e", 
    "gsl_sf_airy_Ai_scaled_e",
    "gsl_sf_airy_Bi_scaled_e",
    "gsl_sf_airy_Ai_deriv_e", 
    "gsl_sf_airy_Bi_deriv_e", 
    "gsl_sf_airy_Ai_deriv_scaled_e",
    "gsl_sf_airy_Bi_deriv_scaled_e",
    "gsl_sf_bessel_J0_e",
    "gsl_sf_bessel_J1_e",
    "gsl_sf_bessel_Y0_e",
    "gsl_sf_bessel_Y1_e",
    "gsl_sf_bessel_I0_e",
    "gsl_sf_bessel_I1_e",
    "gsl_sf_bessel_I0_scaled_e",
    "gsl_sf_bessel_I1_scaled_e",
    "gsl_sf_bessel_K0_e",
    "gsl_sf_bessel_K1_e",
    "gsl_sf_bessel_K0_scaled_e",
    "gsl_sf_bessel_K1_scaled_e",
    "gsl_sf_bessel_j0_e",
    "gsl_sf_bessel_j1_e",
    "gsl_sf_bessel_j2_e",
    "gsl_sf_bessel_y0_e",
    "gsl_sf_bessel_y1_e",
    "gsl_sf_bessel_y2_e",
    "gsl_sf_bessel_i0_scaled_e",
    "gsl_sf_bessel_i1_scaled_e",
    "gsl_sf_bessel_i2_scaled_e",
    "gsl_sf_bessel_k0_scaled_e",
    "gsl_sf_bessel_k1_scaled_e",
    "gsl_sf_bessel_k2_scaled_e",
    "gsl_sf_clausen_e",
    "gsl_sf_dawson_e",
    "gsl_sf_debye_1_e",
    "gsl_sf_debye_2_e",
    "gsl_sf_debye_3_e",
    "gsl_sf_debye_4_e",
    "gsl_sf_debye_5_e",
    "gsl_sf_debye_6_e",
    "gsl_sf_dilog_e",
    "gsl_sf_ellint_Kcomp_e", 
    "gsl_sf_ellint_Ecomp_e", 
    "gsl_sf_ellint_Dcomp_e", 
    "gsl_sf_erfc_e",
    "gsl_sf_log_erfc_e",
    "gsl_sf_erf_e",
    "gsl_sf_erf_Z_e",
    "gsl_sf_erf_Q_e",
    "gsl_sf_hazard_e",
    "gsl_sf_exp_e",
    "gsl_sf_expm1_e",
    "gsl_sf_exprel_e",
    "gsl_sf_exprel_2_e",
    "gsl_sf_expint_E1_e",
    "gsl_sf_expint_E2_e",
    "gsl_sf_expint_E1_scaled_e",
    "gsl_sf_expint_E2_scaled_e",
    "gsl_sf_expint_Ei_e",
    "gsl_sf_expint_Ei_scaled_e",
    "gsl_sf_Shi_e",
    "gsl_sf_Chi_e",
    "gsl_sf_expint_3_e",
    "gsl_sf_Si_e",
    "gsl_sf_Ci_e",
    "gsl_sf_atanint_e",
    "gsl_sf_fermi_dirac_m1_e",
    "gsl_sf_fermi_dirac_0_e",
    "gsl_sf_fermi_dirac_1_e",
    "gsl_sf_fermi_dirac_2_e",
    "gsl_sf_fermi_dirac_mhalf_e",
    "gsl_sf_fermi_dirac_half_e",
    "gsl_sf_fermi_dirac_3half_e",
    "gsl_sf_lngamma_e",
    "gsl_sf_gamma_e",
    "gsl_sf_gammastar_e",
    "gsl_sf_gammainv_e",
    "gsl_sf_lambert_W0_e",
    "gsl_sf_lambert_Wm1_e",
    "gsl_sf_legendre_P1_e",
    "gsl_sf_legendre_P2_e",
    "gsl_sf_legendre_P3_e",
    "gsl_sf_legendre_Q0_e",
    "gsl_sf_legendre_Q1_e",
    "gsl_sf_log_e",
    "gsl_sf_log_abs_e",
    "gsl_sf_log_1plusx_e",
    "gsl_sf_log_1plusx_mx_e",
    "gsl_sf_psi_e",
    "gsl_sf_psi_1piy_e",
    "gsl_sf_psi_1_e",
    "gsl_sf_sin_pi_e",
    "gsl_sf_cos_pi_e",
    "gsl_sf_synchrotron_1_e",
    "gsl_sf_synchrotron_2_e",
    "gsl_sf_transport_2_e",
    "gsl_sf_transport_3_e",
    "gsl_sf_transport_4_e",
    "gsl_sf_transport_5_e",
    "gsl_sf_sin_e",
    "gsl_sf_cos_e",
    "gsl_sf_sinc_e",
    "gsl_sf_lnsinh_e",
    "gsl_sf_lncosh_e",
    "gsl_sf_zeta_e",
    "gsl_sf_zetam1_e",
    "gsl_sf_eta_e"
]

def analysis_dfeg_results_numPersec():
    str_lst = []
    file_num = file_nums("dfeg_res")
    # print(file_num)
    for i in range(0,file_num):
      str_lst.append("dfeg_res/dfeg_res_"+str(i)+".pkl")
    datas = []
    for i in str_lst:
      tmp_res = load_pickle(i)
      datas.append(exceptionNumsDivTime(tmp_res))
    mix_datas=[]
    for i in range(0,len(name_list)):
      mix_datas.append([])
    for i in datas:
      for j in range(0,len(name_list)):
        mix_datas[j].append(i[j])
    cv_lst = []
    print("Calculate CV")
    for i in mix_datas:
        mean = int(np.mean(i))
        std = np.std(i,ddof = 0)
        if mean != 0.0:
            cv_lst.append(std/mean)
        else:
            cv_lst.append(0)
    print("CV lst for each function")
    # print(len(cv_lst))
    print(cv_lst)
    print("Mean value of CV")
    print(np.mean(cv_lst))
    plot_many_boxes(mix_datas,"graph/plot_numsBox.pdf","Log2 of floating-point exceptions per second")


def dump_list(fnm,res):
  file_name = fnm
  with open(file_name,"wb") as fp:
    pickle.dump(res,fp)

def load_pickle(file_name):
    fp = open(file_name,'rb')
    res = pickle.load(fp)
    fp.close()
    return res


atomic_idx_list = [0, 5, 6, 7, 8, 9, 12, 13, 15, 16, 17, 18, 21, 22, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 60, 61, 68, 69, 70, 71, 72, 76, 77, 78, 79, 80, 82, 83, 84, 85, 91, 94, 98, 101, 105, 107, 108, 109, 116, 118, 131, 133, 140, 142, 143, 146, 147, 148, 149, 152, 153, 154, 155, 156, 157]
false_posit_idx = [0,1,2,3,25,90,104,105,106]
# demc_idx_list = [0, 5, 6, 8, 12, 13, 15, 17, 21, 22, 39, 40, 42, 43, 46, 48, 50, 52, 68, 69, 70, 71, 72, 76, 77, 78, 79, 80, 82, 83, 84, 85, 94, 98, 101, 105, 107, 116, 118, 133, 140, 142, 146, 147, 148, 149, 152, 154, 156]
# demc_idx_list2 = [48, 52, 46, 50, 12, 13, 21, 22, 0, 156, 69, 70, 72, 118, 116, 146, 147, 83, 142, 140]
def plot_bar_compare(y1,y2,new_name_lst):
    fig = plt.figure(figsize=(19, 8))
    ax = plt.subplot(111)
    index = np.arange(0,len(y1),1)
    # index = range(0,n_groups,2)
    # print(index)
    bar_width = 0.35
    opacity = 0.8

    # plt.boxplot(rpt1_lst)
    # plt.boxplot(rpt2_lst)
    plt.xlim(-0.8, len(y1)) 

    ax.bar(index, y1, bar_width,edgecolor='purple', color='None',hatch="/",label='ATOMU')

    ax.bar(index + bar_width, y2, bar_width, label='FPCC')
    #add_value_labels(ax)
    # plt.xlabel('Functions',fontsize=20)
    # plt.ylabel('Repair time ratios',fontsize=20)
    plt.ylabel('Log2(N) of error-triggering inputs per second',fontsize=16)
    # plt.title('Scores by person')
    # plt.yticks(index+0.5*bar_width, id_lst,rotation=0)
    plt.xticks(index+0.5*bar_width, new_name_lst,rotation=90,fontsize=16)
    # plt.xticks(index, new_name_lst,rotation=45,fontsize=16)
    # plt.xticks(range(0,int(np.max(rpt1_lst))+1,2), range(0,int(np.max(rpt1_lst))*10+10,20),rotation=30)
    #plt.yticks(list(range(0,int(np.max(y1))+10,20)), list(range(0,int(np.max(rpt1_lst))*10+10,20)),fontsize=16)
    #plt.yticks(x,fontsize=16)
    plt.legend(prop={'size': 19.5})
    plt.tight_layout()
    plt.grid(zorder=1)
    plt.savefig("graph/compareTimes.pdf", format="pdf")
    plt.close()


def plot_inputs_per_seconds(res1,res2,name_list):
    xscope_exps_per =[]
    k = 0
    for i in res1[0]:
        xscope_exps_per.append(np.sum(i))
        k = k + 1
    dfeg_exps_per =[]
    k = 0
    for i in res2[0]:
        dfeg_exps_per.append(np.sum(i))
        k = k + 1
    print("Sum of inputs")
    print("Inputs detected by ATOMU")
    print(np.sum(xscope_exps_per))
    print("Inputs detected by FPCC")
    print(np.sum(dfeg_exps_per))
    print("Times: FPCC/ATOMU")
    print(np.sum(dfeg_exps_per)/np.sum(xscope_exps_per))
    xscope_exps_per =[]
    k = 0
    for i in res1[0]:
        xscope_exps_per.append(np.sum(i)/res1[1][k])
        k = k + 1
    dfeg_exps_per =[]
    k = 0
    for i in res2[0]:
        dfeg_exps_per.append(np.sum(i)/res2[1][k])
        k = k + 1
    print("ATOMU: Inputs per second")
    print(np.mean(xscope_exps_per))
    print("FPCC: Inputs per second")
    print(np.mean(dfeg_exps_per))
    print("Times: FPCC/ATOMU")
    print(np.mean(dfeg_exps_per)/np.mean(xscope_exps_per))
    log2_timesXs = []
    log2_timesDe = []
    for x in dfeg_exps_per:
      # log2_timesDe.append(x)
      if x!=0:
          if x == 1.0:
              log2_timesDe.append(np.log2(x)+1.0)
          else:
              log2_timesDe.append(np.log2(x))
      else:
          log2_timesDe.append(0)
    for x in xscope_exps_per:
      # log2_timesXs.append(x)
      if x!=0:
          if x == 1.0:
              log2_timesXs.append(np.log2(x)+1.0)
          else:
              log2_timesXs.append(np.log2(x))
      else:
          log2_timesXs.append(0)
    plot_bar_compare(log2_timesXs,log2_timesDe,name_list)
def analysiAtomPKL_Max(res_dir):
    # max_err for each functions
    # [time,max_err,input,input_rank]
    # resOnefunc = {"time":[],"max_err":[],"inputs":[],"rank":[]}
    sum_res = []
    for i in range(0,107):
      sum_res.append([[],[],[],[]])
    for i in range(0,100):
      fnm1 = res_dir+"/ATOM" + str(i)+".pkl"
      res1 = load_pickle(fnm1)
      cnt = 0
      for ri in res1:
        if ri[2]!=[]:
          sum_res[cnt][0].append(ri[1]-ri[4])
          err_lst = [float(x) for x in ri[2]]
          tmp_cne = 0
          max_index = 0
          tmp_max_err = 0
          #for er in err_lst:
          #    if er > 1e-3:
          #        tmp_max_err = er 
          #        max_index = tmp_cne 
          #        break
          #    tmp_cne = tmp_cne + 1
          tmp_max_err = max(err_lst)
          max_index = err_lst.index(tmp_max_err)
          sum_res[cnt][1].append(tmp_max_err)
          sum_res[cnt][2].append(ri[3][max_index])
          sum_res[cnt][3].append(max_index)
        else:
          sum_res[cnt][0].append(ri[1]-ri[4])
          sum_res[cnt][1].append(0.0)
          sum_res[cnt][2].append(0.0)
          sum_res[cnt][3].append(-1)
        cnt = cnt + 1
    dump_list(res_dir+"/sum_res_max.pkl",sum_res)
def getMidDistance(a,b):
    ia = floatToRawLongBits(np.abs(a))
    ib = floatToRawLongBits(np.abs(b))
    zo = floatToRawLongBits(0)
    if sign(a)!=sign(b):
        res = abs(ib-zo)+abs(ia-zo)
    else:
        res = abs(ib-ia)
    if (sign(a)==sign(b))&(ib == ia):
        return 1.0
    return int(res)+1

def analysiFPCCPKL(res_dir):
    # max_err for each functions
    # [time,max_err,input,input_rank]
    # resOnefunc = {"time":[],"max_err":[],"inputs":[],"rank":[]}
    sum_res = []
    for i in range(0,107):
      sum_res.append([[],[],[],[],[],[],[],[],[],[],[]])
    for i in range(0,100):
      fnm1 = res_dir+"/FPCC" + str(i)+".pkl"
      res1 = load_pickle(fnm1)
      cnt = 0
      # new_res = res1[0:106]+[res1[107]]
      # print(res1[0])
      for ri in res1:
      # for ri in new_res:
        if ri[2]!=[]:
          # print(cnt)
          sum_res[cnt][0].append(ri[1]-ri[4])
          err_lst = [float(x) for x in ri[2]]
          tmp_cne = 0
          fst_index = 0
          fst_err = 0
          for er in err_lst:
              if er > 1e-3:
                  fst_err = er 
                  fst_index = tmp_cne 
                  break
              tmp_cne = tmp_cne + 1
          tmp_max_err = max(err_lst)
          max_index = err_lst.index(tmp_max_err)
          ulp_errs = []
          for r,o in zip(ri[6],ri[7]):
            ulp_errs.append(math.log2(getMidDistance(r,o)))
            # if tmp_max_err == 0:
            # if getMidDistance(r,o) > 1000:
              # print("ulp_errs")
              # print(r)
              # print(o)
          sum_res[cnt][1].append(tmp_max_err)
          sum_res[cnt][2].append(ri[3][max_index])
          sum_res[cnt][3].append(max_index)
          num_err = 0
          for er in err_lst:
              if er > 1e-3:
                num_err +=1
          sum_res[cnt][4].append(len(ri[3]))
          sum_res[cnt][5].append(num_err)
          sum_res[cnt][6].append(fst_err)
          sum_res[cnt][7].append(fst_index)
          sum_res[cnt][8].append(err_lst[0])
          #sum_res[cnt][9].append(np.mean(err_lst))
          sum_res[cnt][9].append(np.max(ulp_errs))
          sum_res[cnt][10].append(ulp_errs[0])
        else:
          sum_res[cnt][0].append(ri[1]-ri[4])
          sum_res[cnt][1].append(0.0)
          sum_res[cnt][2].append(0.0)
          sum_res[cnt][3].append(-1)
          sum_res[cnt][4].append(len(ri[3]))
          sum_res[cnt][5].append(0)
          sum_res[cnt][6].append(0)
          sum_res[cnt][7].append(-1)
          sum_res[cnt][8].append(0)
          sum_res[cnt][9].append(0)
          sum_res[cnt][10].append(0)
        cnt = cnt + 1
    dump_list(res_dir+"/sum_res.pkl",sum_res)
def analysiAtomPKL(res_dir):
    # max_err for each functions
    # [time,max_err,input,input_rank]
    # resOnefunc = {"time":[],"max_err":[],"inputs":[],"rank":[]}
    sum_res = []
    for i in range(0,107):
      sum_res.append([[],[],[],[],[],[],[],[],[],[],[]])
    flag = 10
    for i in range(0,100):
      fnm1 = res_dir+"/ATOM" + str(i)+".pkl"
      res1 = load_pickle(fnm1)
      cnt = 0
      for ri in res1:
        # if flag >= 0:
          # if cnt == 10:
          #   print(GSLFuncList[cnt])
          #   print("inputs")
          #   print(ri[3])
          #   print("outs")
          #   print(ri[7])
          #   print("orl outs")
          #   print(ri[6])
          #   print("errs")
          #   print(ri[2])
          #   flag = flag - 1
      # for ri in new_res:
        if ri[2]!=[]:
          # print(cnt)
          sum_res[cnt][0].append(ri[1]-ri[4])
          err_lst = [float(x) for x in ri[2]]
          tmp_cne = 0
          fst_index = 0
          fst_err = 0
          for er in err_lst:
              if er > 1e-3:
                  fst_err = er 
                  fst_index = tmp_cne 
                  break
              tmp_cne = tmp_cne + 1
          tmp_max_err = max(err_lst)
          max_index = err_lst.index(tmp_max_err)
          ulp_errs = []
          for r,o in zip(ri[6],ri[7]):
            ulp_errs.append(math.log2(getMidDistance(r,o)))
            # if tmp_max_err == 0:
            # if getMidDistance(r,o) > 1000:
              # print("ulp_errs")
              # print(r)
              # print(o)
          sum_res[cnt][1].append(tmp_max_err)
          sum_res[cnt][2].append(ri[3][max_index])
          sum_res[cnt][3].append(max_index)
          num_err = 0
          for er in err_lst:
              if er > 1e-3:
                num_err +=1
          sum_res[cnt][4].append(len(ri[3]))
          sum_res[cnt][5].append(num_err)
          sum_res[cnt][6].append(fst_err)
          sum_res[cnt][7].append(fst_index)
          sum_res[cnt][8].append(err_lst[0])
          #sum_res[cnt][9].append(np.mean(err_lst))
          sum_res[cnt][9].append(np.max(ulp_errs))
          sum_res[cnt][10].append(ulp_errs[0])
        else:
          sum_res[cnt][0].append(ri[1]-ri[4])
          sum_res[cnt][1].append(0.0)
          sum_res[cnt][2].append(0.0)
          sum_res[cnt][3].append(-1)
          sum_res[cnt][4].append(len(ri[3]))
          sum_res[cnt][5].append(0)
          sum_res[cnt][6].append(0)
          sum_res[cnt][7].append(-1)
          sum_res[cnt][8].append(0)
          sum_res[cnt][9].append(0)
          sum_res[cnt][10].append(0)
        cnt = cnt + 1
    # print('&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&')
    # for i in sum_res[55]:
    #   print(i)
    dump_list(res_dir+"/sum_res.pkl",sum_res)
          

        # for ri in res:
        #   # print(ri)
        #   print(len(ri[2]))
        #   print(len(ri[3]))
def calCV(i):
  tmp_i = []
  #print(i)
  for ti in i:
      if np.isinf(ti):
          tmp_i.append(1.0)
      else:
          if ti == 0:
              tmp_i.append(0.0)
          else:
              tmp_i.append(np.log2(ti))
  if np.isinf(i[0]):
    i = [1.0]*len(i)
  #print(tmp_i)
  mean = int(np.mean(tmp_i))
  std = np.std(tmp_i,ddof = 0)
  if mean != 0.0:
    return std/mean
  else:
    return 0.0
  
def processRes(res):
  retRes = []
  avg_percent = 0
  for i in range(0,107):
    avg_time = np.mean(res[i][0])
    max_err = np.mean(res[i][1])
    max_idx = 1 
    avg_rank = np.mean(res[i][3])
    tmp_cv = calCV(res[i][1]) 
    tmp_inp = res[i][2][max_idx]
    tmp_ulp = np.mean(res[i][9])
    top1ErrNum = 0
    for tp1 in res[i][8]:
      if tp1 > 1e-3:
        top1ErrNum = top1ErrNum + 1
    avg_percent += top1ErrNum
    retRes.append(dict({"fun_idx":i,"top1ErrNum": top1ErrNum,"time":avg_time,"max_err":max_err,"cv":tmp_cv,"rank":avg_rank,"max_rank":res[i][3][max_idx],"input":tmp_inp,"numInps":np.mean(res[i][4]),"numErrs":np.mean(res[i][5]),"fst_err":np.mean(res[i][6]),"fst_idx":np.mean(res[i][7]),"top1err":float(np.mean(res[i][8])),"time":np.mean(res[i][0]),"ulpErr":tmp_ulp}))
  print("************************")
  print("Average percent of rank-1 inputs is " + str(avg_percent/42))
  return retRes
  
def extract_res2PKL():
  analysiAtomPKL("outs_atom")
  analysiFPCCPKL("outs_fpcc")
def compare_res():
  res1 = load_pickle("outs/sum_res.pkl")
  res2 = load_pickle("outs_tmp/sum_res.pkl")
  retRes1 = processRes(res1)
  retRes2 = processRes(res2)
  cnt = 0
  err_less_cnt = 0
  atom_cnt = 0
  new_cnt = 0
  rank_cnt = 0
  rank_cnt2 = 0
  for i,j in zip(retRes1,retRes2):
    if (i['max_err']>1e-3):
      atom_cnt +=1
      if (i['rank']<=1):
        rank_cnt +=1
    if (j['max_err']>1e-3):
      new_cnt +=1
      if (j['rank']<1):
        rank_cnt2 +=1
      #else:
      #  print(cnt)
      #  print(GSLFuncList[cnt])
      #  print(j)
    #if (i['max_err']>1e-3)&(j['max_err']<1e-3):
    if ((j['max_err']>1e-3)|(i['max_err']>1e-3)):
       if i['max_err']>j['max_err']:
       #if i['time']<j['time']:
        print(cnt)
        print(GSLFuncList[cnt])
        print(i)
        print(j)
        err_less_cnt +=1
    cnt = cnt + 1
  print(err_less_cnt)
  print(atom_cnt)
  print(new_cnt)
  print(rank_cnt)
  print(rank_cnt2)
def compare_res():
  res1 = load_pickle("outs/sum_res.pkl")
  res2 = load_pickle("outs_tmp/sum_res.pkl")
  retRes1 = processRes(res1)
  retRes2 = processRes(res2)
  cnt = 0
  err_less_cnt = 0
  atom_cnt = 0
  new_cnt = 0
  rank_cnt = 0
  rank_cnt2 = 0
  for i,j in zip(retRes1,retRes2):
    if (i['max_err']>1e-3):
      atom_cnt +=1
      if (i['rank']<=1):
        rank_cnt +=1
    if (j['max_err']>1e-3):
      new_cnt +=1
      if (j['rank']<1):
        rank_cnt2 +=1
      #else:
      #  print(cnt)
      #  print(GSLFuncList[cnt])
      #  print(j)
    #if (i['max_err']>1e-3)&(j['max_err']<1e-3):
    if ((j['max_err']>1e-3)|(i['max_err']>1e-3)):
       if i['max_err']>j['max_err']:
       #if i['time']<j['time']:
        print(cnt)
        print(GSLFuncList[cnt])
        print(i)
        print(j)
        err_less_cnt +=1
    cnt = cnt + 1
  print(err_less_cnt)
  print(atom_cnt)
  print(new_cnt)
  print(rank_cnt)
  print(rank_cnt2)
  
def plot_many_boxes(data,name,yname,name_list):
    # 创建一些随机数据
    #data = [np.random.normal(0, std, 100) for std in range(1,70)]

    # 创建一个箱线图
    #fig, ax = plt.subplots()
    fig = plt.figure(figsize=(19, 8))
    ax = plt.subplot(111)
    ax.boxplot(data)

    # 设置图表标题和轴标签
    # ax.set_title('Multiple Boxplots')
    # ax.set_xlabel('')
    ax.set_ylabel(yname,fontsize=20)

    # 设置x轴刻度标签
    #xticklabels = [f'Group {i}' for i in range(1, len(data)+1)]
    xticklabels = name_list 
    ax.set_xticklabels(xticklabels, rotation=90,fontsize=16)
    #plt.legend(prop={'size': 23})
    plt.tight_layout()
    plt.grid(zorder=1)
    plt.savefig(name, format="pdf")
    plt.close()
def plot_Compare_boxs(name,A,B,name_list,xname):
  # Step 2: Create data for sets A and B
  # A = [np.random.normal(0, 1, size=100) for _ in range(42)]
  # B = [np.random.normal(1, 1, size=100) for _ in range(42)]

  # Step 4: Set up figure and axes
  fig, ax = plt.subplots(figsize=(18,7))
  # fig, ax = plt.subplots()
  # print(A[0])
  # print(B[0])
  # print(len(A))
  # print(len(B))

  # Step 5: Plot boxplots for each set in A and B, with different colors
  box_A = ax.boxplot(A, positions=np.arange(len(A))*2-0.4, widths=0.4, patch_artist=True,medianprops={'color': 'gray'},whiskerprops={'color': 'gray'},flierprops={'markeredgecolor': 'gray', 'markerfacecolor': 'gray'})
  box_B = ax.boxplot(B, positions=np.arange(len(B))*2+0.4, widths=0.4, patch_artist=True,medianprops={'color': 'black'})

  # Set different colors for A and B
  color_A = 'lightgray'
  color_B = 'black'
  # color_B = 'red'
    # Add vertical lines to separate each compared set
  for i in range(len(A)):
      ax.axvline(x=i*2+1, color='gray', linestyle='--')

  # Customize the boxplot colors
  for box in box_A['boxes']:
      box.set(color='lightgray', linewidth=1)
      box.set(facecolor=color_A)
  for box in box_B['boxes']:
      box.set(color='black', linewidth=1)
      box.set(facecolor=color_B)
  for i, patch in enumerate(box_A['boxes']):
      patch.set(hatch='//', edgecolor='black',facecolor='white')
  # Step 6: Customize the plot
  ax.set_xticks(np.arange(len(A))*2)
  ax.set_xticklabels(name_list,rotation=90,fontsize=12)
  ax.set_xlim(-1,84)
  # ax.set_xlabel('Func_name')
  # ax.set_ylabel('Value of error-triggering inputs')
  ax.set_ylabel(xname,fontsize=16)
  # ax.set_title('Comparison of Sets A and B')

  # Add legend
  legend_labels = ['ATOMU', 'FPCC']
  legend_handles = [plt.Rectangle((-1,0),1,1,facecolor='white',edgecolor='black',hatch='//'), plt.Rectangle((-1,0),1,1, color=color_B)]
  # legend_handles = [plt.Rectangle(color=color_A), plt.Rectangle(color=color_B)]
  ax.legend(legend_handles,legend_labels,fontsize=16)
  # ax.legend(legend_labels,fontsize=16)
  plt.tight_layout()


  plt.savefig(name, format="pdf")
  plt.close()
  # Step 7: Display the plot
  
def ini_xls_file(exname):
  new_excel = xlwt.Workbook()
  sheet = new_excel.add_sheet("ATOMvsFPCC")
  sheet.write(0,0,"benchmark")
  sheet.write_merge(0,0,1,2, "Top1Err")
  sheet.write(1, 1, "FPCC")
  sheet.write(1, 2, "ATOMU")
  sheet.write_merge(0,0,3,4, "Error Inputs")
  sheet.write(1, 3, "FPCC")
  sheet.write(1, 4, "ATOMU")
  sheet.write_merge(0,0,5,6, "Max RelErr")
  sheet.write(1, 5, "FPCC")
  sheet.write(1, 6, "ATOMU")
  # sheet.write_merge(0,0,7,8, "Max UlpErr")
  # sheet.write(1, 7, "FPCC")
  # sheet.write(1, 8, "ATOMU")
  sheet.write_merge(0,0,7,8, "Mean RelErr")
  sheet.write(1, 7, "FPCC")
  sheet.write(1, 8, "ATOMU")
  sheet.write_merge(0,0,9,10, "Time")
  sheet.write(1, 9, "FPCC")
  sheet.write(1, 10, "ATOMU")
  sheet.write_merge(0,0,11,12, "Top1Pec")
  sheet.write(1, 11, "FPCC")
  sheet.write(1, 12, "ATOMU")
  new_excel.save(exname)

def generate_table_NOERR(exname,res1,res2):
    old_excel = xlrd.open_workbook(exname, formatting_info=True)
    new_excel = copy(old_excel)
    sheet = new_excel.get_sheet(0)
    bold_font = xlwt.easyxf('font: bold on')
    cnt = 0
    k = 2
    avg_time_fpcc=[]
    avg_time_atom=[]
    sum_inps_fpcc = 0
    sum_errs_fpcc = 0
    sum_inps_atom = 0
    sum_errs_atom = 0
    for i,j in zip(res1,res2):
      if (((j['max_err']<1e-3)&(i['max_err']<1e-3)))&(j["time"]!=0.0):
        func_name = GSLFuncList[cnt][7:-2] 
        sheet.write(k,0,func_name)
        if i["top1err"]<j["top1err"]:
          sheet.write(k,1,j["top1err"],bold_font)
          sheet.write(k,2,i["top1err"])
        elif i["top1err"]==j["top1err"]:
          sheet.write(k,1,j["top1err"],bold_font)
          sheet.write(k,2,i["top1err"],bold_font)
        else:
          sheet.write(k,1,j["top1err"])
          sheet.write(k,2,i["top1err"],bold_font)
        if i["numErrs"]<j["numErrs"]:
          sheet.write(k,3,str(round(j["numErrs"]))+"/"+str(round(j["numInps"])),bold_font)
          sheet.write(k,4,str(round(i["numErrs"]))+"/"+str(round(i["numInps"])))
        else:
          if i["numErrs"]==j["numErrs"]:
            sheet.write(k,3,str(round(j["numErrs"]))+"/"+str(round(j["numInps"])),bold_font)
            sheet.write(k,4,str(round(i["numErrs"]))+"/"+str(round(i["numInps"])),bold_font)
          else:
            sheet.write(k,3,str(round(j["numErrs"]))+"/"+str(round(j["numInps"])))
            sheet.write(k,4,str(round(i["numErrs"]))+"/"+str(round(i["numInps"])),bold_font)
        sum_inps_fpcc+= j["numInps"]
        sum_errs_fpcc+= j["numErrs"]
        sum_inps_atom+= i["numInps"]
        sum_errs_atom+= i["numErrs"]
        if i["max_err"]<j["max_err"]:
          sheet.write(k,5,j["max_err"],bold_font)
          sheet.write(k,6,i["max_err"])
        else:
          if i["max_err"]==j["max_err"]:
            sheet.write(k,5,j["max_err"],bold_font)
            sheet.write(k,6,i["max_err"],bold_font)
          else:
            sheet.write(k,5,j["max_err"])
            sheet.write(k,6,i["max_err"],bold_font)
        if i["time"]>j["time"]:
          sheet.write(k,9,j["time"],bold_font)
          sheet.write(k,10,i["time"])
        else:
          if i["time"]==j["time"]:
            sheet.write(k,9,j["time"],bold_font)
            sheet.write(k,10,i["time"],bold_font)
          else:
            sheet.write(k,9,j["time"])
            sheet.write(k,10,i["time"],bold_font)
        avg_time_fpcc.append(j["time"])
        avg_time_atom.append(i["time"])
        if i["ulpErr"]<j["ulpErr"]:
          sheet.write(k,7,j["ulpErr"],bold_font)
          sheet.write(k,8,i["ulpErr"])
        else:
          if i["ulpErr"]==j["ulpErr"]:
            sheet.write(k,7,j["ulpErr"],bold_font)
            sheet.write(k,8,i["ulpErr"],bold_font)
          else:
            sheet.write(k,7,j["ulpErr"])
            sheet.write(k,8,i["ulpErr"],bold_font)
        if i["top1ErrNum"]<j["top1ErrNum"]:
          sheet.write(k,11,str(j["top1ErrNum"])+"%",bold_font)
          sheet.write(k,12,str(i["top1ErrNum"])+"%")
        else:
          if i["top1ErrNum"]==j["top1ErrNum"]:
            sheet.write(k,11,str(j["top1ErrNum"])+"%",bold_font)
            sheet.write(k,12,str(i["top1ErrNum"])+"%",bold_font)
          else:
            sheet.write(k,11,str(j["top1ErrNum"])+"%")
            sheet.write(k,12,str(i["top1ErrNum"])+"%",bold_font)
        k = k+1
      cnt = cnt + 1
    # sheet.write(k,3,str(sum_errs_fpcc)+"/"+str(sum_inps_fpcc))
    # sheet.write(k,4,str(sum_errs_atom)+"/"+str(sum_inps_atom))
    sheet.write(k,3,str(int(sum_errs_fpcc))+"/"+str(int(sum_inps_fpcc)))
    sheet.write(k,4,str(int(sum_errs_atom))+"/"+str(int(sum_inps_atom)))
    # Perform the Wilcoxon rank-sum test
    statistic, p_value = ranksums(avg_time_atom, avg_time_fpcc)

    # Output the results
    print("Speedup on 46 functions without significant errors: FPCC vs ATOMU")
    # sheet.write(k+1,3,str(sum_errs_fpcc/sum_inps_fpcc))
    # sheet.write(k+1,4,str(sum_errs_atom/sum_inps_atom))
    sheet.write(k,9,str(np.mean(avg_time_fpcc)))
    sheet.write(k,10,str(np.mean(avg_time_atom)))
    speedlst = []
    for fc,at in zip(avg_time_fpcc,avg_time_atom):
      speedlst.append(at/fc)
    print("Max speedup")
    print(max(speedlst))
    print("Min speedup")
    print(min(speedlst))
    print("Wilcoxon Rank-Sum Test:")
    print("Statistic:", statistic)
    print("p-value:", p_value)
    new_excel.save(exname)

def generate_table_NOHE(exname,res1,res2):
    old_excel = xlrd.open_workbook(exname, formatting_info=True)
    new_excel = copy(old_excel)
    sheet = new_excel.get_sheet(0)
    bold_font = xlwt.easyxf('font: bold on')
    cnt = 0
    k = 2
    avg_time_fpcc=[]
    avg_time_atom=[]
    sum_inps_fpcc = 0
    sum_errs_fpcc = 0
    sum_inps_atom = 0
    sum_errs_atom = 0
    temp_res1_num = []
    temp_res1_time = []
    temp_res2_num = []
    temp_res2_time = []
    name_list = []
    tmp_cnt = 0
    err_idx_lst = []
    for i,j in zip(res1,res2):
      if ((j['max_err']>1e-3)|(i['max_err']>1e-3)):
        err_idx_lst.append(cnt)
        func_name = GSLFuncList[cnt][7:-2] 
        name_list.append(func_name)
        sheet.write(k,0,func_name)
        if i["top1err"]<1e-3:
          tmp_cnt = tmp_cnt + 1
        if i["top1err"]<j["top1err"]:
          sheet.write(k,1,j["top1err"],bold_font)
          sheet.write(k,2,i["top1err"])
        elif i["top1err"]==j["top1err"]:
          sheet.write(k,1,j["top1err"],bold_font)
          sheet.write(k,2,i["top1err"],bold_font)
        else:
          sheet.write(k,1,j["top1err"])
          sheet.write(k,2,i["top1err"],bold_font)
        if i["numErrs"]<j["numErrs"]:
          sheet.write(k,3,str(round(j["numErrs"]))+"/"+str(round(j["numInps"])),bold_font)
          sheet.write(k,4,str(round(i["numErrs"]))+"/"+str(round(i["numInps"])))
        else:
          if i["numErrs"]==j["numErrs"]:
            sheet.write(k,3,str(round(j["numErrs"]))+"/"+str(round(j["numInps"])),bold_font)
            sheet.write(k,4,str(round(i["numErrs"]))+"/"+str(round(i["numInps"])),bold_font)
          else:
            sheet.write(k,3,str(round(j["numErrs"]))+"/"+str(round(j["numInps"])))
            sheet.write(k,4,str(round(i["numErrs"]))+"/"+str(round(i["numInps"])),bold_font)
        temp_res1_num.append(i["numErrs"])
        temp_res2_num.append(j["numErrs"])
        sum_inps_fpcc+= j["numInps"]
        sum_errs_fpcc+= j["numErrs"]
        sum_inps_atom+= i["numInps"]
        sum_errs_atom+= i["numErrs"]
        if i["max_err"]<j["max_err"]:
          sheet.write(k,5,j["max_err"],bold_font)
          sheet.write(k,6,i["max_err"])
        else:
          if i["max_err"]==j["max_err"]:
            sheet.write(k,5,j["max_err"],bold_font)
            sheet.write(k,6,i["max_err"],bold_font)
          else:
            sheet.write(k,5,j["max_err"])
            sheet.write(k,6,i["max_err"],bold_font)
        if i["time"]>j["time"]:
          sheet.write(k,9,j["time"],bold_font)
          sheet.write(k,10,i["time"])
        else:
          if i["time"]==j["time"]:
            sheet.write(k,9,j["time"],bold_font)
            sheet.write(k,10,i["time"],bold_font)
          else:
            sheet.write(k,9,j["time"])
            sheet.write(k,10,i["time"],bold_font)
        temp_res1_time.append(i["time"])
        temp_res2_time.append(j["time"])
        avg_time_fpcc.append(j["time"])
        avg_time_atom.append(i["time"])
        if i["ulpErr"]<j["ulpErr"]:
          sheet.write(k,7,j["ulpErr"],bold_font)
          sheet.write(k,8,i["ulpErr"])
        else:
          if i["ulpErr"]==j["ulpErr"]:
            sheet.write(k,7,j["ulpErr"],bold_font)
            sheet.write(k,8,i["ulpErr"],bold_font)
          else:
            sheet.write(k,7,j["ulpErr"])
            sheet.write(k,8,i["ulpErr"],bold_font)
        if i["top1ErrNum"]<j["top1ErrNum"]:
          sheet.write(k,11,str(j["top1ErrNum"])+"%",bold_font)
          sheet.write(k,12,str(i["top1ErrNum"])+"%")
        else:
          if i["top1ErrNum"]==j["top1ErrNum"]:
            sheet.write(k,11,str(j["top1ErrNum"])+"%",bold_font)
            sheet.write(k,12,str(i["top1ErrNum"])+"%",bold_font)
          else:
            sheet.write(k,11,str(j["top1ErrNum"])+"%")
            sheet.write(k,12,str(i["top1ErrNum"])+"%",bold_font)
        k = k+1
      cnt = cnt + 1
    sheet.write(k,3,str(round(sum_errs_fpcc))+"/"+str(round(sum_inps_fpcc)))
    sheet.write(k,4,str(round(sum_errs_atom))+"/"+str(round(sum_inps_atom)))
    sheet.write(k+1,3,str(sum_errs_fpcc/sum_inps_fpcc))
    sheet.write(k+1,4,str(sum_errs_atom/sum_inps_atom))
    sheet.write(k,9,str(np.mean(avg_time_fpcc)))
    sheet.write(k,10,str(np.mean(avg_time_atom)))
    new_excel.save(exname)
    statistic, p_value = ranksums(avg_time_atom, avg_time_fpcc)
    print("*****************************************")
    print("Speedup on 42 functions with significant errors: FPCC vs ATOMU")
    speedlst = []
    for fc,at in zip(avg_time_fpcc,avg_time_atom):
      speedlst.append(at/fc)
    print("Max speedup")
    print(max(speedlst))
    print("Min speedup")
    print(min(speedlst))
    # Output the results
    print("Wilcoxon Rank-Sum Test:")
    print("Statistic:", statistic)
    print("p-value:", p_value)
    print(tmp_cnt)
    
    print("*****************************************")
    print("Output Fig. 7  for the comparison of the Log2(N) number of error-triggering inputs per second detected by ATOMU and FPCC")
    plot_inputs_per_seconds([temp_res1_num,temp_res1_time],[temp_res2_num,temp_res2_time],name_list)

    
def CV_calc(mix_datas):
  print("Calculate CV")
  cv_lst = []
  for i in mix_datas:
      mean = int(np.mean(i))
      std = np.std(i,ddof = 0)
      if mean != 0.0:
          cv_lst.append(np.fabs(std/mean))
      else:
          cv_lst.append(0)
  print("CV lst for each function")
  # print(len(cv_lst))
  # print(cv_lst)
  print("Mean value of CV")
  print(np.mean(cv_lst))
  print("MAX value of CV")
  print(np.max(cv_lst))
  print("MIN value of CV")
  print(np.min(cv_lst))

def stable_compare_plot(res1,res2):
  atom_num_errs = []
  fpcc_num_errs = []
  name_list = []
  for i in range(0,107):
    if (np.max(res1[i][1])>1e-3)|(np.max(res2[i][1])>1e-3):
      name_list.append(GSLFuncList[i][7:-2])
      atom_num_errs.append([np.log2(x+1) for x in res1[i][1]])
      fpcc_num_errs.append([np.log2(x+1) for x in res2[i][1]])
      # atom_num_errs.append([x for x in res1[i][5]])
      # fpcc_num_errs.append([x for x in res2[i][5]])
      # if i == 90:
      #   print(res2[i][5])
  print("#########################################")
  print("Max Error")
  print("CV ATOM")
  CV_calc(atom_num_errs)
  print("CV FPCC")
  
  CV_calc(fpcc_num_errs)
  xname = "Log2 value of maximum error"
  plot_Compare_boxs("graph/max_err_stable.pdf",atom_num_errs,fpcc_num_errs,name_list,xname)
  atom_num_errs = []
  fpcc_num_errs = []
  name_list = []
  for i in range(0,107):
    if (np.max(res1[i][1])>1e-3)|(np.max(res2[i][1])>1e-3):
      name_list.append(GSLFuncList[i][7:-2])
      # atom_num_errs.append([np.log2(x) for x in res1[i][1]])
      # fpcc_num_errs.append([np.log2(x) for x in res2[i][1]])
      atom_num_errs.append([np.log2(x+1) for x in res1[i][5]])
      fpcc_num_errs.append([np.log2(x+1) for x in res2[i][5]])
      # if i == 90:
      #   print(res2[i][5])
  print("#########################################")
  print("Fig. 8: Error Triggering Inputs, store in graph/top1_err_stable.pdf")
  print("CV ATOM")
  CV_calc(atom_num_errs)
  print("CV FPCC")
  CV_calc(fpcc_num_errs)
  # print(fpcc_num_errs)
  xname = "Log2(N) of error-triggering inputs"
  plot_Compare_boxs("graph/err_num_stable.pdf",atom_num_errs,fpcc_num_errs,name_list,xname)
  atom_num_errs = []
  fpcc_num_errs = []
  name_list = []
  for i in range(0,107):
    if (np.max(res1[i][1])>1e-3)|(np.max(res2[i][1])>1e-3):
      name_list.append(GSLFuncList[i][7:-2])
      # atom_num_errs.append([np.log2(x+1.0e-16) for x in res1[i][8]])
      # fpcc_num_errs.append([np.log2(x+1.0e-16) for x in res2[i][8]])
      # atom_num_errs.append([x for x in res1[i][5]])
      atom_num_errs.append([x for x in res1[i][10]])
      fpcc_num_errs.append([x for x in res2[i][10]])
      # fpcc_num_errs.append([x for x in res2[i][5]])
      # if i == 90:
      #   print(res2[i][5])
  print("#########################################")
  print("Fig.9 Top1 ULP errors, store in graph/top1_err_stable.pdf")
  print("CV ATOM")
  CV_calc(atom_num_errs)
  print("CV FPCC")
  CV_calc(fpcc_num_errs)
  xname = "ULP errors of rank-1 inputs"
  plot_Compare_boxs("graph/top1_err_stable.pdf",atom_num_errs,fpcc_num_errs,name_list,xname)
if __name__ == "__main__":
  extract_res2PKL()
  res1 = load_pickle("outs_atom/sum_res.pkl")
  res2 = load_pickle("outs_fpcc/sum_res.pkl")
  print("#########################################")
  print("Process ATOMU datas ...")
  retRes1 = processRes(res1)
  print("#########################################")
  print("Process FPCC datas ...")
  retRes2 = processRes(res2)
  print("#########################################")
  # compare_res()
  exname = "res_table_FPCC.xls"
  exname2 = "res_table_FPCC_NoErr.xls"
  ini_xls_file(exname)
  ini_xls_file(exname2)
  # Data on 42 functions with significant errors
  print("Generate Table 1 in file res_table_FPCC.xls for 42 functions with significant errors")
  generate_table_NOHE(exname,retRes1,retRes2)
  # Data on 46 functions with significant errors
  print("Generate Table 2 in file res_table_FPCC_NoErr.xls for 46 functions without significant errors")
  generate_table_NOERR(exname2,retRes1,retRes2)
  # stability analysis of inputs that can trigger significant errors
  stable_compare_plot(res1,res2)
  #
