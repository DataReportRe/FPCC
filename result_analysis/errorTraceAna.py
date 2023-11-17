import xlwt
import xlrd
from xlutils.copy import copy
from base import GSLFuncList

# reslst.append[0inp,1oup,2res,3fit,4opcode,5locLine,6colLine,7fileName,8funcName,9fit1,10fit2,11s1val,12s2val]

def getMergeRes(reslst): 
    targetDict = {}
    for i in reslst:
        keyVal = str(i[5])+str(i[6])+str(i[7])
        if keyVal in targetDict:
            # print(targetDict[keyVal])
            targetDict[keyVal][-1]+=1
        else:
            targetDict[keyVal]=i+[0]
    res = []
    for val in targetDict.values():
        res.append(val)
    return res

def analysisRes(idx,reslst):
    # combine the same location
    funcName = GSLFuncList[idx]
    mergeRes = getMergeRes(reslst)
    # read source code according to the location 
    final_res = []
    code = ''
    for i in mergeRes:
        if "cheb" in i[8]:
            fileName = "../gsl-2.5/specfunc/"+i[8][:-2]+".c"
        else:
            fileName = "../gsl-2.5/specfunc/"+i[7][:-3]+".c"
        lineNum = i[5]
        with open(fileName,'r') as file:
            cnt = 1
            for line in file:
                if cnt == lineNum:
                    code = line
                    break
                cnt = cnt + 1
        final_res.append([i,fileName,code,funcName])
    return final_res

def ini_xls_file(exname):
  new_excel = xlwt.Workbook()
  sheet = new_excel.add_sheet("ErrorTrace")
  sheet.write(0,0,"GSL function")
  sheet.write(0,1,"Error Localization")
  sheet.write(0, 2, "Op1CC")
  sheet.write(0, 3, "Op2CC")
  sheet.write(0, 4, "Op1val")
  sheet.write(0, 5, "Op2val")
  sheet.write(0, 6, "Analysis")
  new_excel.save(exname)
    
def getTraceRes():
    data_file = 'errorTrace.out'
    scpath = '../gsl-2.5/specfunc/'
    flag = 0
    idx = 0
    tmp_idx = idx
    inpsize = 0
    allreslst = []
    with open(data_file, 'r') as file:
        reslst = []
        cnt = 0
        for line in file:
            values = line.strip().split(' ')
            if "index" in line:
                idx = int(values[2])
                if idx != 0:
                    tempRes = analysisRes(tmp_idx,reslst)
                    print("************************")
                    print(tempRes)
                    allreslst.append(tempRes)
                    reslst = []
                    cnt = cnt + 1
                    tmp_idx = idx
            elif "size" in line:
                inpsize = int(values[3])
            elif ".ll" in line:
                inp = float(values[0])
                oup = float(values[1])
                res = float(values[2])
                fit = float(values[3])
                opcode = int(values[4])
                locLine = int(values[5])
                colLine = int(values[6])
                fileName = values[7]
                funcName = values[8]
                fit1 = float(values[9])
                fit2 = float(values[10])
                s1val = float(values[11])
                s2val = float(values[12])
                reslst.append([inp,oup,res,fit,opcode,locLine,colLine,fileName,funcName,fit1,fit2,s1val,s2val])
    return allreslst
#     return final_res

# def ini_xls_file(exname):
#   new_excel = xlwt.Workbook()
#   sheet = new_excel.add_sheet("ErrorTrace")
#   sheet.write(0,0,"GSL function")
#   sheet.write_merge(0,1,"Error Localization")
#   sheet.write(0, 2, "Analysis")
#   sheet.write(0, 3, "Op1CC")
#   sheet.write(0, 4, "Op2CC")
#   sheet.write(0, 5, "Op1val")
#   sheet.write(0, 6, "Op2val")
# reslst.append[0inp,1oup,2res,3fit,4opcode,5locLine,6colLine,7fileName,8funcName,9fit1,10fit2,11s1val,12s2val]
#         final_res.append([i,fileName,code,funcName])

def write2excel(exname,allreslst):
    old_excel = xlrd.open_workbook(exname, formatting_info=True)
    new_excel = copy(old_excel)
    sheet = new_excel.get_sheet(0)
    k = 1
    resDict = {}
    for alr in allreslst:
        for j in alr:
            funcName = j[-1]
            srcCode = j[-2].strip()
            fileName = j[1][20:]
            locstr = str(j[0][5])+" "+str(j[0][6])
            combSrc = srcCode + "\n(" + fileName + " " + locstr+")"
            Op1CC = str(j[0][9])
            Op2CC = str(j[0][10])
            op1val = str(j[0][11])
            op2val = str(j[0][12])
            if combSrc not in resDict:
                resDict[combSrc] = [[funcName,Op1CC,Op2CC,op1val,op2val]] 
            else:
                resDict[combSrc].append([funcName,Op1CC,Op2CC,op1val,op2val]) 
            sheet.write(k,0,funcName)
            sheet.write(k,1,combSrc)
            sheet.write(k,2,Op1CC)
            sheet.write(k,3,Op2CC)
            sheet.write(k,4,op1val)
            sheet.write(k,5,op2val)
            k = k + 1
    for rd in resDict:
        funcName_lstStr = ''
        cnt = 0
        for ri in resDict[rd]:
            funcName_lstStr = funcName_lstStr + " " + ri[0] 
        Op1CC = str(resDict[rd][0][1])
        Op2CC = str(resDict[rd][0][2])
        Op1val = str(resDict[rd][0][3])
        Op2val = str(resDict[rd][0][4])
        sheet.write(k,0,rd)
        sheet.write(k,1,funcName_lstStr)
        sheet.write(k,2,Op1CC)
        sheet.write(k,3,Op2CC)
        sheet.write(k,4,Op1val)
        sheet.write(k,5,Op2val)
        k = k + 1
    new_excel.save(exname)


if __name__ == "__main__":
    allreslst = getTraceRes()
    exname = "localErr.xls"
    ini_xls_file(exname)
    write2excel(exname,allreslst)
         




