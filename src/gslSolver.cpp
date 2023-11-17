#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cfloat>
#include <cstring>
#include <unordered_map>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <functional>
#include <chrono>
#include <memory>
#include "fpUtil.h"
#include "fpInterface.h"
#include "direct.h"
using namespace std;

class InputIntervalsPair {
public:
    int idx;
    double db;
    double ub;
    double max_inp;
    double max_out;
    double max_fit;
    int usinsts;

    InputIntervalsPair() {
        idx = -1;
        db = 0.0;
        ub = 0.0;
        max_fit = 1.0;
        max_inp = 0.0;
    }
    InputIntervalsPair(int k, double dbound, double ubound, double fit_val,double inp) {
        idx = k;
        db = dbound;
        ub = ubound;
        max_fit = fit_val;
        max_inp = inp;
    }
    bool operator < (const InputIntervalsPair &rhs) const {
        return max_fit < rhs.max_fit;
    }
    bool operator > (const InputIntervalsPair &rhs) const {
        return max_fit > rhs.max_fit;
    }
};
class InputIntervalsPairGreater {
public:
    bool operator () (const InputIntervalsPair& lhs, const InputIntervalsPair& rhs) const {
        return lhs.max_fit > rhs.max_fit;
    }
}IntpairGreater;
class InputIntervalsPairGreaterUsinst {
public:
    bool operator () (const InputIntervalsPair& lhs, const InputIntervalsPair& rhs) const {
        return lhs.usinsts > rhs.usinsts;
    }
}usIntpairGreater;
class InputIntervalsPairLess {
public:
    bool operator () (const InputIntervalsPair& lhs, const InputIntervalsPair& rhs) const {
        return lhs.max_fit < rhs.max_fit;
    }
}IntpairLess;

class InputFitnessPair {
public:
    double input;
    double fitness;

    InputFitnessPair() {
        input = 0;
        fitness = -std::numeric_limits<double>::max();
    }
    InputFitnessPair(double in, double fit) {
        input = in;
        fitness = fit;
    }
    bool operator < (const InputFitnessPair &rhs) const {
        return fitness < rhs.fitness;
    }
    bool operator > (const InputFitnessPair &rhs) const {
        return fitness > rhs.fitness;
    }
};
class InputFitnessPairGreater {
public:
    bool operator () (const InputFitnessPair& lhs, const InputFitnessPair& rhs) const {
        return lhs.fitness > rhs.fitness;
    }
}pairGreater;
class InputFitnessPairLess {
public:
    bool operator () (const InputFitnessPair& lhs, const InputFitnessPair& rhs) const {
        return lhs.fitness < rhs.fitness;
    }
}pairLess;

class InstructionInfo {
private:
    uint64_t instID;
    uint64_t opcode;
    uint32_t recordSize = 100;
    std::vector<InputFitnessPair> topInputsRandom;
    std::vector<InputFitnessPair> topInputsEvolution;
    // For recording prioritization results.
    uint32_t topInputCountToEnd;
    double   topInputConditionToEnd;

private:
    void pushHeap(double input, double fitness) {
        topInputsRandom.push_back(InputFitnessPair(input, fitness));
        std::push_heap(topInputsRandom.begin(), topInputsRandom.end(), pairGreater);
    }
    void popHeap() {
        std::pop_heap(topInputsRandom.begin(), topInputsRandom.end(), pairGreater);
        topInputsRandom.pop_back();
    }
    InputFitnessPair topHeap() {
        return topInputsRandom[0];
    }
public:
    InstructionInfo() {
        instID = 0;
        opcode = 0;
    }
    InstructionInfo(uint64_t id, uint64_t op) {
        instID = id;
        opcode = op;
    }
    uint64_t getInstID() const { return instID; }
    uint64_t getOpCode() const { return opcode; }
    uint32_t getRecordSize() const { return recordSize; }
    void setRecordSize(uint32_t k) { recordSize = k; }

    std::vector<InputFitnessPair> getInputsRandom() const { return topInputsRandom; }
    std::vector<InputFitnessPair> getInputsEvolution() const { return topInputsEvolution; }
    int getInputsRandomSize() const { return topInputsRandom.size(); }
    int getInputsEvolutionSize() const { return topInputsEvolution.size(); }
    void setInputsEvolution(std::vector<InputFitnessPair> l) { topInputsEvolution = l; }
    InputFitnessPair getInputsEvolutionTop() const { return topInputsEvolution[0]; }

    void setTopInputCountToEnd(uint32_t c) { topInputCountToEnd = c; }
    void setTopInputConditionToEnd(double d) { topInputConditionToEnd = d; }
    uint32_t getTopInputCountToEnd() const { return topInputCountToEnd; }
    double getTopInputConditionToEnd() const { return topInputConditionToEnd; }


    void printRandomInfo(std::unique_ptr<FloatingPointFunction> & funcPtr) {
        std::sort(topInputsRandom.begin(), topInputsRandom.end(), pairGreater);

        std::cout << "************************************************\n";
        std::cout << "Instruction ID: " << instID << '\n';
        std::cout << "Instruction OP: " << opcode << '\n';
        std::cout.precision(5);
        uint32_t inputSize = topInputsRandom.size();
        std::cout << "Valid inputs in our records: " << topInputsRandom.size() << '\n';
        std::cout << "Largest fitness after random: " << std::scientific << topInputsRandom[0].fitness << '\n';
        std::cout << "50th    fitness after random: " << std::scientific << topInputsRandom[inputSize/2].fitness << '\n';
        std::cout.precision(16);
        std::cout << "    Largest's input:  " << std::setw(25) << std::scientific << topInputsRandom[0].input << '\n';
        std::cout << "    Largest's output: " << std::setw(25) << std::scientific << funcPtr->callAndGetResult(topInputsRandom[0].input) << '\n';

        std::make_heap(topInputsRandom.begin(), topInputsRandom.end(), pairGreater);
    }
    void printEvolutionInfo(std::unique_ptr<FloatingPointFunction> & funcPtr) {
        std::cout << "------------------\n";
        std::cout << "Instruction ID: " << instID << '\n';
        std::cout << "Instruction OP: " << opcode << '\n';
        std::cout.precision(5);
        uint32_t inputSize = topInputsEvolution.size();
        std::cout << "Valid inputs in our records: " << topInputsEvolution.size() << '\n';
        std::cout << "Largest fitness after evo: " << std::scientific << topInputsEvolution[0].fitness << '\n';
        std::cout << "50th    fitness after evo: " << std::scientific << topInputsEvolution[inputSize/2].fitness << '\n';
        std::cout.precision(16);
        std::cout << "    Largest's input:  " << std::setw(25) << std::scientific << topInputsEvolution[0].input << '\n';
        std::cout << "    Largest's output: " << std::setw(25) << std::scientific << funcPtr->callAndGetResult(topInputsEvolution[0].input) << '\n';
    }

    void printBriefInfo(int funcIndex) {
        std::cout << "2. Searching on Operations.\nFunction Index: " << funcIndex << ", ";
        std::cout << "Analyzing Operation: " << instID << ", OPCode: " << opcode << '\n';
        std::cout << "  Largest's input: " << std::setw(15) << std::scientific << topInputsEvolution[0].input << ", ";
        std::cout << "  Fitness: " << std::scientific << topInputsEvolution[0].fitness << '\n';
    }
};
struct errorMapInfo
{
    uint64_t    opcode = -1;
    double      op_val = 0;
    double      s1val = 0;
    double      s2val = 0;
    double      fitness = 0;
    uint64_t    s1ID = 0;
    uint64_t    s2ID = 0;
    char*       funcName;
    char*       modName;
    int32_t     debugLine;
    int32_t     colLine;
    double      input;
    double      output;
};


class CCSolver {
private:
    std::unique_ptr<FloatingPointFunction> funcUnderTest;
    std::vector<InputFitnessPair> inputList;
    std::vector<uint64_t> tracedSID;
    std::vector<InputIntervalsPair> inputIntervals;
    std::map<uint64_t, InstructionInfo> instMap;
    std::map<uint64_t, std::vector<errorMapInfo>> opMap;
    std::vector<errorMapInfo> errorResInfo;
    uint32_t unstableInstCount = 0;
    int32_t GSLFuncIndex;
    // For record execution time.
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point finishTime;
    std::chrono::duration<double> elapsedTime;

    std::chrono::high_resolution_clock::time_point startTime2;
    std::chrono::high_resolution_clock::time_point finishTime2;
    std::function <double(int, const double *, int *, void*)> binf;
    std::chrono::duration<double> elapsedTime2;

    std::mt19937 mtGenerator;
    std::uniform_real_distribution<double> uni01;

    std::string outPath = "tempOutput.out";
    std::string errorPath = "errorTrace.out";
    static double wrapper(int n, const double* x, int* undefined_flag, void* data) {
        // This wrapper calls the binf member function
        CCSolver* self = static_cast<CCSolver*>(data);
        return self->binf(n, x, undefined_flag, data);
    }

public:
    CCSolver() :
        //mtGenerator(0xdeadbeef),
        mtGenerator(time(0)),
        uni01(0.0, 1.0)
    {
        // Clean File.
        std::ofstream myfile;
        myfile.open(outPath, std::ofstream::out);
        myfile.close();
        binf = std::bind(&CCSolver::test_func_direct,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);
    }

    void run(std::unique_ptr<FloatingPointFunction> & funcPtr,double input, int index) {
        startTime = std::chrono::high_resolution_clock::now();
        _init(funcPtr, index);
        _1DirectSearch();
        _2LineSearch();
        finishTime = std::chrono::high_resolution_clock::now();
        elapsedTime = finishTime - startTime;
        _3PrintInputsList();

        _writeToFile();
        _printInfo();
        // _saveGraphPoints();
        // if(input!=0){
        //     _inputTrace(input);
        // }
        if(input==1.0){
            _errorTrace(input);
        }
        inputList.clear();
        inputIntervals.clear();
    }


private:
    double generateRandomdouble(double min, double max) 
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(min, max);
        return dist(mt);
    }
    double fit_functions_disp(double x,double y){
        // funcUnderTest->call(x);
        std::vector<InstInfo> infoList = funcUnderTest->getInstInfoList();
        Communicator &comm = Communicator::getInstance();
        std::map<uint64_t, InputFitnessPair> opMap;
        double op3fit = 0.0;
        uint64_t op1ID = 0;
        uint64_t op2ID = 0;
        uint64_t op3ID = 0;
        double op1fit = 0.0;
        double op2fit = 0.0;
        double rawfit = 0.0;
        // opMap[fpUtil::doubleToI64(x)] = InputFitnessPair(x,0.0);
        for (const auto &info : infoList){
            op1ID = fpUtil::doubleToI64(info.op1);
            op2ID = fpUtil::doubleToI64(info.op2);
            op3ID = fpUtil::doubleToI64(info.op3);
            if (opMap.count(op1ID)==0){
               opMap[op1ID] = InputFitnessPair(info.op1,0.0);
            }
            if (opMap.count(op2ID)==0){
               opMap[op2ID] = InputFitnessPair(info.op2,0.0);
            }
            if(info.op1ID==0){
                op1fit = 0.0;
            }else{
                op1fit = opMap[op1ID].fitness;
            }
            if(info.op2ID==0){
                op2fit = 0.0;
            }else{
                op2fit = opMap[op2ID].fitness;
            }
            if((op1fit==0.0)&&(op2fit==0.0)){
                if(info.opcode>16){
                    if((info.op1==0)||(info.op2==0)){
                        op3fit = 0.0;
                    }else{
                        op3fit = 1.0;
                    }
                }
                else
                {
                    op3fit=1.0;
                }
            }
            else{
                if(comm.isUnstable(info.opcode)){
                    op3fit = fpUtil::disCalculation(info.opcode,info.op1,info.op2,info.op3,op1fit,op2fit);
                }else{
                    if(op1ID==op2ID){
                        op3fit = op1fit;
                    }else{
                        op3fit = op1fit+op2fit;
                    }
                }
            }
            rawfit = fpUtil::rawCondition(info.opcode, info.op1, info.op2);
            opMap[op3ID] = InputFitnessPair(info.op3,op3fit);
            std::cout<<std::setprecision(20) << "input " << "opcode " << "op1 " << " op2 " << " op3 " << " op1 fit " << " op2 fit " << " op3 fit " << "rawfit" << "modName" <<"Debug Line"<< "\t"<<std::endl; 
            // std::cout<<std::setprecision(5) << x << " " << info.opcode << " " << info.op1 << " "  << info.op2 << " "  << info.op3 << " "<< op1fit << " " << op2fit<< " " << op3fit << " " <<rawfit<<" "<<info.op2ID<<" "<<info.modName<<" "<<info.debugLine<< "\t"<<std::endl;
            std::cout<<std::setprecision(5) << x << " " << info.opcode << " "  << "\t"<<std::endl;
            std::cout<<std::setprecision(5) <<  info.op1 << " "  << info.op2 << " "  << info.op3 << "\t"<<std::endl;
            std::cout<<std::setprecision(5) << op1fit << " " << op2fit<< " " << op3fit << " " <<rawfit<<" "<<info.modName<<" "<<info.debugLine<< "\t"<<std::endl;
        }
        //op3fit = opMap[fpUtil::doubleToI64(y)].fitness;
        if(y!=op3fit){
            op3fit = opMap[fpUtil::doubleToI64(y)].fitness;
        }
        std::map<uint64_t, InputFitnessPair> emptyopMap;
        std::swap(emptyopMap, opMap);
        if((std::isnan(op3fit))||(std::isinf(op3fit))){
            return 1.0;
        }else{
            //return res;
            return op3fit;
        }
    }
    void writeToMapInfo(errorMapInfo *empinfo,uint64_t opcode,double op_val,double s1val,double s2val,double fitness,uint64_t s1ID,uint64_t s2ID,char* funcName,char* modName,int32_t debugLine,int32_t colLine, double input, double output){
        empinfo->opcode = opcode;
        empinfo->op_val = op_val;
        empinfo->s1val = s1val;
        empinfo->s2val = s2val;
        empinfo->fitness = fitness;
        empinfo->s1ID = s1ID;
        empinfo->s2ID = s2ID;
        empinfo->funcName = funcName;
        empinfo->modName = modName;
        empinfo->debugLine = debugLine;
        empinfo->colLine = colLine;
        empinfo->input = input;
        empinfo->output = output;

    }
    int print_map_errs(uint64_t resID, int flag){
        uint64_t opcode = opMap[resID][0].opcode;
        double res = opMap[resID][0].op_val;
        double resfit = opMap[resID][0].fitness;
        double op1fit = opMap[resID][0].s1val;
        double op2fit = opMap[resID][0].s2val;
        uint64_t s1ID = opMap[resID][0].s1ID;
        uint64_t s2ID = opMap[resID][0].s2ID;
        char* funcName = opMap[resID][0].funcName;
        char* modName = opMap[resID][0].modName;
        int32_t debugLine = opMap[resID][0].debugLine;
        int32_t colLine = opMap[resID][0].colLine;
        if(flag >= 1){
            opcode = opMap[resID][flag].opcode;
            res = opMap[resID][flag].op_val;
            resfit = opMap[resID][flag].fitness;
            op1fit = opMap[resID][flag].s1val;
            op2fit = opMap[resID][flag].s2val;
            s1ID = opMap[resID][flag].s1ID;
            s2ID = opMap[resID][flag].s2ID;
            funcName = opMap[resID][flag].funcName;
            modName = opMap[resID][flag].modName;
            debugLine = opMap[resID][flag].debugLine;
            colLine = opMap[resID][flag].colLine;
        }
        auto iter = std::find(tracedSID.begin(), tracedSID.end(), resID);
        if(iter != tracedSID.end()){
            return 0;
        }
        if(resfit>=1.0){
            if((resID == s1ID)&&(resID == s2ID)){
                return 0;
            }
            if(resfit>=1e4){
                    if((resfit>100*op1fit)&&(resfit>100*op2fit)){
                            tracedSID.push_back(resID);
                            errorResInfo.push_back(opMap[resID][flag]);
                    }

            }
            if(s1ID==s2ID){
                print_map_errs(s1ID,0);
            }else{
                print_map_errs(s1ID,0);
                print_map_errs(s2ID,0);
            }
            return 0;
            
        }else{
            return 0;
        }

    }
    void error_trace_map(double x, double y){
        std::vector<InstInfo> infoList = funcUnderTest->getInstInfoList();
        Communicator &comm = Communicator::getInstance();
        // std::map<uint64_t, errorMapInfo> emptyopMap;
        // std::swap(emptyopMap, opMap);
        std::map<uint64_t, std::vector<errorMapInfo>> emptyopMap;
        std::swap(emptyopMap, opMap);
        double op3fit = 0.0;
        uint64_t op1ID = 0;
        uint64_t op2ID = 0;
        uint64_t op3ID = 0;
        double op1fit = 0.0;
        double op2fit = 0.0;
        errorMapInfo empinfo;
        // forward error record
        for (const auto &info : infoList){
            op1ID = fpUtil::doubleToI64(info.op1);
            op2ID = fpUtil::doubleToI64(info.op2);
            op3ID = fpUtil::doubleToI64(info.op3);
            if (opMap.count(op1ID)==0){
               writeToMapInfo(&empinfo,0,info.op1,0,0,0.0,op1ID,op1ID,info.funcName,info.modName,info.debugLine,info.colLine,x,y); 
               opMap[op1ID].push_back(empinfo); 
            }
            if (opMap.count(op2ID)==0){
               writeToMapInfo(&empinfo,0,info.op2,0,0,0.0,op2ID,op2ID,info.funcName,info.modName,info.debugLine,info.colLine,x,y); 
               opMap[op2ID].push_back(empinfo); 
            }
            if(info.op1ID==0){
                op1fit = 0.0;
            }else{
                op1fit = opMap[op1ID].back().fitness;
            }
            if(info.op2ID==0){
                op2fit = 0.0;
            }else{
                op2fit = opMap[op2ID].back().fitness;
            }
            if(info.op1==x){
                op1fit = 0.0;
            }
            if(info.op2==x){
                op2fit = 0.0;
            }
            if((op1fit==0.0)&&(op2fit==0.0)){
                if((op1fit==0.0)&&(op2fit==0.0)){
                    if(info.opcode>16){
                        if((info.op1==0)||(info.op2==0)){
                            op3fit = 0.0;
                        }else{
                            op3fit = 1.0;
                        }
                    }
                    else
                    {
                        op3fit=1.0;
                    }
                }
            }else{
                if(comm.isUnstable(info.opcode)){
                    op3fit = fpUtil::disCalculation(info.opcode,info.op1,info.op2,info.op3,op1fit,op2fit);
                }else{
                    if(op1ID==op2ID){
                        op3fit = op1fit;
                    }else{
                        op3fit = op1fit+op2fit;
                    }
                }

            }
            writeToMapInfo(&empinfo,info.opcode,info.op3,op1fit,op2fit,op3fit,op1ID,op2ID,info.funcName,info.modName,info.debugLine,info.colLine,x,y); 
            opMap[op3ID].push_back(empinfo);
        }
        uint64_t resID = fpUtil::doubleToI64(y);
        std::cout << "Backward error tracing..."<<"\n";
        // std::cout<<std::setprecision(16) << "out is:" << y << "\n"<<std::endl;
        // std::cout<<std::setprecision(16) << "res ID is:" << resID << "\n"<<std::endl;
        // std::cout << "fitness is:" << opMap[resID].back().fitness << "\n"<<std::endl;
        // std::cout << "out is:" << opMap[resID].back().op_val << "\n"<<std::endl;
        // std::cout << "out is:" << opMap[resID].back().s1ID << "\n"<<std::endl;
        // std::cout << "out is:" << opMap[resID].back().s2ID << "\n"<<std::endl;
        int k = opMap[resID].size()-1;
        while(tracedSID.size()==0)
        {
            print_map_errs(resID,k);
            if(k==0){
                break;
            }
            k = k-1;
        }
        std::swap(emptyopMap, opMap);
        tracedSID.clear();
    }

    double fit_functions_dis(double x,double y){
        std::vector<InstInfo> infoList = funcUnderTest->getInstInfoList();
        Communicator &comm = Communicator::getInstance();
        std::map<uint64_t, InputFitnessPair> opMap;
        std::map<uint64_t, InputFitnessPair> emptyopMap;
        std::swap(emptyopMap, opMap);
        double op3fit = 0.0;
        uint64_t op1ID = 0;
        uint64_t op2ID = 0;
        uint64_t op3ID = 0;
        double op1fit = 0.0;
        double op2fit = 0.0;
        for (const auto &info : infoList){
            op1ID = fpUtil::doubleToI64(info.op1);
            op2ID = fpUtil::doubleToI64(info.op2);
            op3ID = fpUtil::doubleToI64(info.op3);
            if (opMap.count(op1ID)==0){
               opMap[op1ID] = InputFitnessPair(info.op1,0.0);
            }
            if (opMap.count(op2ID)==0){
               opMap[op2ID] = InputFitnessPair(info.op2,0.0);
            }
            if(info.op1ID==0){
                op1fit = 0.0;
            }else{
                op1fit = opMap[op1ID].fitness;
            }
            if(info.op2ID==0){
                op2fit = 0.0;
            }else{
                op2fit = opMap[op2ID].fitness;
            }
            if(info.op1==x){
                op1fit = 0.0;
            }
            if(info.op2==x){
                op2fit = 0.0;
            }
            if((op1fit==0.0)&&(op2fit==0.0)){
                if((op1fit==0.0)&&(op2fit==0.0)){
                    if(info.opcode>16){
                        if((info.op1==0)||(info.op2==0)){
                            op3fit = 0.0;
                        }else{
                            op3fit = 1.0;
                        }
                    }
                    else
                    {
                        op3fit=1.0;
                    }
                }
            }else{
                if(comm.isUnstable(info.opcode)){
                    op3fit = fpUtil::disCalculation(info.opcode,info.op1,info.op2,info.op3,op1fit,op2fit);
                }else{
                    if(op1ID==op2ID){
                        op3fit = op1fit;
                    }else{
                        op3fit = op1fit+op2fit;
                    }
                }

            }
            opMap[op3ID] = InputFitnessPair(info.op3,op3fit);
        }
        if(y!=fpUtil::i64ToDouble(op3ID)){
            op3fit = opMap[fpUtil::doubleToI64(y)].fitness;
        }
        if(op3fit<=0){
        return 1.0;
        }
        if((std::isnan(op3fit))||(std::isinf(op3fit))){
            return 1.0;
        }else{
            return op3fit;
        }
    }
    void _init(std::unique_ptr<FloatingPointFunction> & funcPtr, int index) {
        GSLFuncIndex = index;
        funcUnderTest = std::move(funcPtr);

        // Clear member values
        unstableInstCount = 0;
        // Fast clear instMap.
        std::map<uint64_t, InstructionInfo> emptyMap;
        // std::vector<InputFitnessPair> emptyList;
        std::swap(emptyMap, instMap);
        // std::swap(emptyList, inputList);
        // Initial the input intervals
        inputIntervals.push_back(InputIntervalsPair(1,0.0,2.2250738585072014e-308,1.0,0.0));
        // inputIntervals.push_back(InputIntervalsPair(1,0.0,2.2250738585072014,-1.0,0.0));
        inputIntervals.push_back(InputIntervalsPair(2,-2.2250738585072014e-308,0.0,1.0,0.0));
        int k = 5;
        int cnt = 3;
        double tmp_bd = -2.2250738585072014e-308; 
        k = 30;
        for(int i=-1022;i<-22;i=i+k){
            if(i+k>-22){
                inputIntervals.push_back(InputIntervalsPair(cnt,tmp_bd,2.384185791015625e-07,1.0,tmp_bd));
                inputIntervals.push_back(InputIntervalsPair(cnt+1,-2.384185791015625e-07,-tmp_bd,1.0,-tmp_bd));
                cnt = cnt + 2;
            }else{
                double tmp_res= std::pow(2.0,i+k);
                inputIntervals.push_back(InputIntervalsPair(cnt,tmp_bd,tmp_res,1.0,tmp_bd));
                inputIntervals.push_back(InputIntervalsPair(cnt+1,-tmp_res,-tmp_bd,1.0,-tmp_bd));
                tmp_bd = tmp_res;
                cnt = cnt + 2;
            }
        }
        k = 1;
        for(int i=-22;i<88;i=i+k){
            if(i+k>88){
                inputIntervals.push_back(InputIntervalsPair(cnt,tmp_bd,5.316911983139664e+36,1.0,tmp_bd));
                inputIntervals.push_back(InputIntervalsPair(cnt+1,-5.316911983139664e+36,-tmp_bd,1.0,-tmp_bd));
                cnt = cnt + 2;
            }else{
                double tmp_res= std::pow(2.0,i+k);
                inputIntervals.push_back(InputIntervalsPair(cnt,tmp_bd,tmp_res,1.0,tmp_bd));
                inputIntervals.push_back(InputIntervalsPair(cnt+1,-tmp_res,-tmp_bd,1.0,-tmp_bd));
                tmp_bd = tmp_res;
                cnt = cnt + 2;
            }
        }
        k = 60;
        for(int i=88;i<1023;i=i+k){
            if(i+k>1023){
                inputIntervals.push_back(InputIntervalsPair(cnt,tmp_bd,1.7976931348623157e+308,1.0,tmp_bd));
                inputIntervals.push_back(InputIntervalsPair(cnt+1,-1.7976931348623157e+308,-tmp_bd,1.0,-tmp_bd));
                cnt = cnt + 2;
            }else{
                double tmp_res= std::pow(2.0,i+k);
                inputIntervals.push_back(InputIntervalsPair(cnt,tmp_bd,tmp_res,1.0,tmp_bd));
                inputIntervals.push_back(InputIntervalsPair(cnt+1,-tmp_res,-tmp_bd,1.0,-tmp_bd));
                tmp_bd = tmp_res;
                cnt = cnt + 2;
            }
        }
        
        std::cout << "Current Analyzing Function Index: " << index << std::endl;
    }

    double test_func_direct(int n, const double *xy, int *undefined_flag, void *unused){
        double x;
        x = xy[0];
        funcUnderTest->call(x);
        if (!funcUnderTest->isSuccess())
            return 1.0;
        double y = funcUnderTest->getResult();
        if (std::isnan(y)){
            return 1.0;
        }
        if (std::isinf(y)){
            return 1.0;
        }
        double fit_val = fit_functions_dis(x,y);
        if(fit_val <= 0.0){
            return 1.0;
        }
        return 1.0/fabs(fit_val); 
    }
    double simple_func_call(double x){
        funcUnderTest->call(x);
        if (!funcUnderTest->isSuccess())
            return 1.0;
        double y = funcUnderTest->getResult();
        if (std::isnan(y)){
            return 1.0;
        }
        if (std::isinf(y)){
            return 1.0;
        }
        double fit_val = fit_functions_dis(x,y);
        if(fit_val <= 0.0){
            return 1.0;
        }
        return 1.0/fabs(fit_val); 
        
    }

    double simple_lineSearch(double x,double *y,double lb,double ub){
        double ulpx = fabs(fpUtil::doubleULP(x));
        double step = 1024.0;
        double direct = 1;
        double newx = x + direct*step*ulpx;
        double fix_x = simple_func_call(x);
        double fix_nx = simple_func_call(newx);
        if(fix_nx>fix_x){
            direct = -1; 
        }
        // printf("input x %.20e nfx %.20e fx %.20e lb %.20e ub %.20e\n",x,fix_nx,fix_x,lb,ub);
        step = 1024.0;
        double tpx = x;
        double fitpx = fix_x;
        double ori_fit = fix_x;
        int count = 0;
        int flag = 0;
        for(int i=1;i<1000;i++){
            // printf("INTO i %d x %.20e tpx %.20e \n",i,tpx+direct*step*ulpx,tpx);
            tpx = tpx + direct*step*ulpx;
            fitpx = simple_func_call(tpx);
            // printf("INTO fitpx %.20e ori_fit %.20e step %f\n",fitpx,ori_fit,step);
            if(fitpx>ori_fit){
                // printf("large\n");
                if(ori_fit>1e-5){
                    if(count>10){
                        *y = ori_fit;
                        return tpx-direct*step*ulpx;
                    } 
                }
                if(count > 30){
                    *y = ori_fit;
                    return tpx-direct*step*ulpx;
                }
                if(step<=1.0){
                    *y = ori_fit;
                    return tpx-direct*step*ulpx;
                }else{
                    tpx = tpx - direct*step*ulpx;
                    step = step/2.0;
                    fitpx = simple_func_call(tpx+direct*step*ulpx);
                    if(fitpx>ori_fit){
                        tpx = tpx - direct*step*ulpx;
                        if(step>2048.0*16){
                            step = step/4.0;
                            flag = 1;
                        }else{step = step/2.0;}
                        count = count + 1;
                    }
                    ori_fit = simple_func_call(tpx);
                }
            }else{
                if(tpx>ub||tpx<lb){
                    *y = 1.0;
                    return x;
                }
                if(step>=1.0){
                    if(flag == 0){
                        step = step*2; 
                    }
                }else{
                    break;
                }
                ori_fit = fitpx;
            }
        }
        *y = fitpx;
        return tpx;
    }
    //-1.3471552510691680e+00
    // The results stored in instMap.
    void _1DirectSearch() {
        double x, y;
        // Direct search over input intervals
        int info;
        int force_stop = 0;
        startTime2 = std::chrono::high_resolution_clock::now();
        for(auto &inpIt: inputIntervals){
            double l[1],u[1];
            l[0] = inpIt.db;
            u[0] = inpIt.ub;
            if(GSLFuncIndex==90){
                if(l[0]<-1e4){
                    continue;
                }
            }
            info = direct_optimize(wrapper,this,1,l,u,&x,&y,24,8, inpIt.db, 0, 0, 0,
                          0, 0,
                          &force_stop,
                          DIRECT_UNKNOWN_FGLOBAL, 0,
                          nullptr, DIRECT_GABLONSKY);
            inpIt.max_fit=y;
            inpIt.max_inp=x;
            // std::cout<<std::setprecision(16) << "input is" << x << "\n"<<std::endl;
            // std::cout<<std::setprecision(16) << "output is" << y << "\n"<<std::endl;
            inpIt.max_out=funcUnderTest->callAndGetResult(x);
        }
        std::cout<<std::setprecision(16) << "input intervals size is:" << inputIntervals.size() << "\n"<<std::endl;
        std::sort(inputIntervals.begin(), inputIntervals.end(), IntpairLess);
        finishTime2 = std::chrono::high_resolution_clock::now();
        elapsedTime2 = finishTime2 - startTime2;
        std::cout << "\n1. First Direct Search Done.\n";
        std::cout<<std::setprecision(16)<<"First Direct Time is" << elapsedTime2.count()<<"\n"<<std::endl;
    }

    // The results stored in instMap.
    void _2LineSearch() {
        double x,y;
        int limit_n = 10;
        double dbmul = 0.0;
        double l[1],u[1];
        int force_stop = 0;
        int info =0;
        double tempMaxFit = 0;
        if(inputIntervals.size()<limit_n){
            limit_n = inputIntervals.size();
        }
        for(auto &inpIt: inputIntervals){
            x = inpIt.max_inp;
            if((GSLFuncIndex==90)||(GSLFuncIndex==88)){
                if(inpIt.db<-1e4){
                    continue;
                }
            }
            if(limit_n>0){
                limit_n -=1;
            }else{
                if(inpIt.max_fit<1e-14){
                    if(inpIt.max_fit!=0.0){
                        if(tempMaxFit*inpIt.max_fit<1e10){
                            if((GSLFuncIndex==90)||(GSLFuncIndex==88)){
                                if(fmod(x,1.0)==0.0){
                                    x = x + fpUtil::doubleULP(x)*1.0;
                                    funcUnderTest->call(x);
                                    y = funcUnderTest->getResult();
                                    inputList.push_back(InputFitnessPair(x,fit_functions_dis(x,y)));
                                }
                            }else{
                                inputList.push_back(InputFitnessPair(inpIt.max_inp,1.0/inpIt.max_fit));
                            }
                        }
                    }
                }
                continue;
            }
            x = simple_lineSearch(x,&y,inpIt.db,inpIt.ub);
            double fit = 1.0/y;
            if(y==1.0){
                fit = 1.0/simple_func_call(x);
            }
            if((GSLFuncIndex==90)||(GSLFuncIndex==88)){
                if(fmod(x,1.0)==0.0){
                    x = x + fpUtil::doubleULP(x)*1.0;
                    x = x + fpUtil::doubleULP(x)*1.0;
                    funcUnderTest->call(x);
                    y = funcUnderTest->getResult();
                    fit = fit_functions_dis(x,y);
                }
            }
            if(tempMaxFit<fit){
                tempMaxFit = fit;
            }
            if(fit>1e14){
                inputList.push_back(InputFitnessPair(x,fit));
            }
        }
        std::sort(inputList.begin(), inputList.end(), pairGreater);
    }

    // The results stored in prioritizedInput.
    void _3PrintInputsList() {

        double x, y;
        int status;
        gsl_sf_result res;

        int count = 0;
        for (auto &ip: inputList){
                funcUnderTest->call(ip.input);
                y = funcUnderTest->getResult();
                std::cout << "***********Results after Prioritize***********\n";
                std::cout<<std::setprecision(26) << "Most suspicious input first: \n";
                std::cout<<std::setprecision(36)  << "input: " << ip.input << '\n';
                std::cout<<std::setprecision(26)  << "fitness: " << 1.0/ip.fitness << '\n';
                std::cout<<std::setprecision(26)  << "Output: " << y << '\n';
            count = count + 1;
        }

        std::cout << "End of suspicious input list.\n";
    }
    void _inputTrace(double input){
        std::cout << "************************************************\n";
        std::cout<<std::setprecision(16) << "input is:" << input<< "\n"<<std::endl;
        std::cout<<std::setprecision(16) << "function index" << GSLFuncIndex<< "\n"<<std::endl;
        std::cout << "************************************************\n";

        funcUnderTest->call(input);
        double y = funcUnderTest->getResult();
        double fit = fit_functions_disp(input,y);
        std::cout<<std::setprecision(16) << "fitness value" << fit<< "\n"<<std::endl;
        std::cout<<std::setprecision(16) << "output value" << y<< "\n"<<std::endl;
        if(inputList.size()>0){
            input = inputList[0].input;
            std::cout << "************************************************\n";
            std::cout<<std::setprecision(16) << "input is:" << input<< "\n"<<std::endl;
            std::cout<<std::setprecision(16) << "function index" << GSLFuncIndex<< "\n"<<std::endl;
            std::cout << "************************************************\n";

            funcUnderTest->call(input);
            y = funcUnderTest->getResult();
            fit = fit_functions_disp(input,y);
            std::cout<<std::setprecision(16) << "fitness value" << fit<< "\n"<<std::endl;
            std::cout<<std::setprecision(16) << "output value" << y<< "\n"<<std::endl;
        }

    }
    void _saveGraphPoints(){
        double input = inputList[0].input;
        std::ofstream myfile;
        std::string graphPath = "graphCC/graphOut" + std::to_string(GSLFuncIndex)+ ".out";
        myfile.open(graphPath, std::ofstream::out);
        double ulpi = fpUtil::doubleULP(input);
        double y,fit;
        double oriInpt=input;
        for(int i = -300000;i<300000;i=i+100){
            input = oriInpt + i*ulpi;
            funcUnderTest->call(input);
            y = funcUnderTest->getResult();
            fit = fit_functions_dis(input,y);
            myfile<<std::setprecision(20)<<input<<"," << fit <<","<<y << "\n";
        }
    }
        

    void _errorTrace(double input){
        int limit_n = 0;
        if(inputList.size()!=0){
            if(inputList.size()>10){
                limit_n = 10;
            }else{
                limit_n = inputList.size();
            }
        }
        double y;
        double x= inputList[0].input;
        limit_n = inputList.size();
        std::ofstream errorfile;
        errorfile.open(errorPath, std::ofstream::out | std::ofstream::app);
        errorfile << "************************************************\n";
        errorfile<<std::setprecision(16) << "function index " << GSLFuncIndex<< "\n";
        errorfile<<std::setprecision(16) << "InputList size is " << inputList.size()<< "\n";
        for(auto &inp:inputList){
            if(limit_n>0){
                std::cout << "************************************************\n";
                std::cout<<std::setprecision(16) << "input is:" << inp.input<< "\n"<<std::endl;
                std::cout<<std::setprecision(16) << "function index" << GSLFuncIndex<< "\n"<<std::endl;
                std::cout << "************************************************\n";

                funcUnderTest->call(inp.input);
                y = funcUnderTest->getResult();
                error_trace_map(inp.input,y);
                limit_n = limit_n - 1;

            }
        }
        for(auto &prtInfo: errorResInfo){
            uint64_t opcode = prtInfo.opcode;
            double res = prtInfo.op_val;
            double resfit = prtInfo.fitness;
            double op1fit = prtInfo.s1val;
            double op2fit = prtInfo.s2val;
            uint64_t s1ID = prtInfo.s1ID;
            uint64_t s2ID = prtInfo.s2ID;
            char* funcName = prtInfo.funcName;
            char* modName = prtInfo.modName;
            double input = prtInfo.input;
            double output = prtInfo.output;
            int32_t debugLine = prtInfo.debugLine;
            int32_t colLine = prtInfo.colLine;
            errorfile<<std::setprecision(20) <<input <<" " << output<<" "<< res <<" "<< resfit <<" "<< opcode <<" "<< debugLine <<" " << colLine <<" "<< modName <<" "<< funcName <<" "<<op1fit<<" "<<op2fit<<" "<<fpUtil::i64ToDouble(s1ID)<<" "<<fpUtil::i64ToDouble(s2ID)<<"\t\n"; 
        }
        errorResInfo.clear();
        errorfile.close();
        errorResInfo.clear();
        //}
    }

    // The evolution search info.
    void _printInfo() {
        std::cout << "************************************************\n";
        std::cout << "Execution Time: " << elapsedTime.count() << " sec.\n";
        std::cout << "************************************************\n";
    }

    void _writeToFile() {
        std::ofstream myfile;
        myfile.open(outPath, std::ofstream::out | std::ofstream::app);
        // Write Timestamp.
        std::time_t stamp = time(NULL);
        myfile << "---------------------------------------\n";
        myfile << "Time: " << std::ctime(&stamp) << '\n';
        // Write Function Index.
        myfile << "Function Index: " << GSLFuncIndex << '\n';
        myfile << "Execution Time: " << elapsedTime.count() << " sec.\n";
        myfile << "Format: InstID, OpCode, MaxAtomCond, Input, Output, CountToEnd, ConditionToEnd\n";
        // Write Computation Results.
        if(inputList.size()!=0){
        for (const auto &ip : inputList) {
            myfile << "Data: ";
            myfile << 1000 << ", ";
            myfile << 1000 << ", ";
            myfile << std::setprecision(5) << std::scientific << ip.fitness << ", ";
            myfile << std::setprecision(16) << std::scientific << ip.input << ", ";
            myfile << std::setprecision(16) << std::scientific << funcUnderTest->callAndGetResult(ip.input) << ", ";
            myfile << 10 << ", ";
            myfile << 1.3<< "\n";
        }
        }
        myfile << "***************************************\n";
        myfile.close();
    }
};



int main(int argc, char *argv[]) {
    // // Init communicator.
    // Communicator &comm = Communicator::getInstance();

    CCSolver es;
    std::unique_ptr<FloatingPointFunction> funcPtr;
    double input;

    if (argc == 1 || (argc > 1 && strcmp(argv[1], "example") == 0)) {
        int index = 0;
	if (argc > 2)
	    index = atoi(argv[2]);
	    input = atof(argv[3]);
	if (index >= simpleFuncList.size()) {
	    std::cout << "Invalid index in simpleFuncList\n";
	    return 0;
	}
        funcPtr.reset(new SimpleFunction(index));
        es.run(funcPtr,input, index);
    }
    else if (argc > 2 && strcmp(argv[1], "gsl") == 0) {
	    input = atof(argv[3]);
	if (!strcmp(argv[2],"all")) {
	    for (int i = 0; i < GSLFuncList.size(); i++) {
                funcPtr.reset(new GSLFunction(i));
		        es.run(funcPtr, input,i);
	    }
	}
	else {
	    input = atof(argv[3]);
	    int index = atoi(argv[2]);
	    if (index >= GSLFuncList.size()) {
		std::cout << "Invalid index in GSLFuncList.\n";
		return 0;
	    }
        funcPtr.reset(new GSLFunction(index));
        es.run(funcPtr,input, index);
	}
    }
    else {
        std::cout << "Invalid argument." << std::endl;
	    std::cout << "Valid Example: \n\tbin/gslSolver.out gsl 73\n\tor\n\tbin/gslSolver.out example\n";
    }

    return 0;
}



