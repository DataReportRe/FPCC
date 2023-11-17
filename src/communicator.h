#ifndef COMM_H
#define COMM_H

#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include "opcode.h"

struct InstInfo
{
    uint64_t    opcode = -1;
    double      op1 = 0;
    double      op2 = 0;
    double      op3 = 0;
    uint64_t    op1ID = 0;
    uint64_t    op2ID = 0;
    uint64_t    instID = 0;
    char*       funcName;
    char*       modName;
    int32_t     debugLine;
    int32_t     colLine;
    int         call_flag=0;
    uint64_t    mapID=0;
    double      fitness = 1.0;
};
struct InstMapInfo
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
    int         call_flag=0;
    uint64_t    mapID=0;
};
class Communicator {
public:
    static Communicator& getInstance() {
        static Communicator instance;
        return instance;
    }
private:
    Communicator() { }
public:
    Communicator(Communicator const &)  = delete;
    void operator=(Communicator const&) = delete;

private:
    std::vector<InstInfo> infoList;
    std::vector<uint64_t> callArgsList;
    std::set<uint64_t> instIDSet;
    std::map<uint64_t,InstMapInfo> opMap;
    uint64_t outID = 0;
    bool record = false;
    int unStable[OPSIZE] = {};

public:
    void initComm() {
        clearInfoList();
        record = true;
        // setup all unstable operations.
        unStable[OP_ADD] = 1;
        unStable[OP_SUB] = 1;
        unStable[OP_MUL] = 0;
        unStable[OP_DIV] = 0;

        unStable[OP_SIN] = 1;
        unStable[OP_COS] = 1;
        unStable[OP_TAN] = 1;
        unStable[OP_ASIN] = 1;
        unStable[OP_ACOS] = 1;
        unStable[OP_ATAN] = 0;
        unStable[OP_ATAN2] = 0;

        unStable[OP_SINH] = 1;
        unStable[OP_COSH] = 1;
        unStable[OP_TANH] = 0;

        unStable[OP_EXP] = 1;
        unStable[OP_LOG] = 1;
        unStable[OP_LOG10] = 1;

        unStable[OP_SQRT] = 0;
        unStable[OP_POW] = 1;
        unStable[OP_FLOOR] = 0;
        unStable[OP_FMOD] = 0;
        unStable[OP_FABS] = 0;
    }

    void clearInstIDSet() {
        std::set<uint64_t> emptySet;
        std::swap(instIDSet, emptySet);
    }

    void clear() {
        clearInfoList();
    }
 
    void clearInfoList() {
        std::vector<InstInfo> emptyList;
        std::swap(infoList, emptyList);
    }
    void clearCallArgsList() {
        std::vector<uint64_t> emptyList;
        std::swap(callArgsList, emptyList);
    }
    void writeOutputID(uint64_t opID) {
        outID = opID;
    }
    uint64_t getOutID() {
        return outID;
    }
    bool isUnstable(uint64_t opcode) {
        if (unStable[opcode] != 0) {
             return true;
        }else{
            return false;
        }
        
    }
    void writeMapInfo(uint64_t opID, uint64_t opcode, double op_val, double s1val, double s2val, double fitness, uint64_t s1ID, uint64_t s2ID,char* funcName, char* modName, int32_t debugLine,int call_flag,uint64_t mapID) {
        InstMapInfo info;
        info.opcode = opcode;
        info.op_val = op_val;
        info.s1val = s1val;
        info.s2val = s2val;
        info.fitness = fitness;
        info.s1ID = s1ID;
        info.s2ID = s2ID;
        info.funcName=funcName;
        info.modName = modName;
        info.debugLine = debugLine;
        info.debugLine = debugLine;
        info.call_flag = call_flag;
        info.mapID = mapID;
        opMap[opID]=info;
    }
    void loadToMap(uint64_t aID, uint64_t bID){
        opMap[aID] = opMap[bID];
    }

    void storeToMap(uint64_t aID, uint64_t bID){
        if(opMap[bID].call_flag==1){
            opMap[bID] = opMap[aID];
            opMap[opMap[bID].mapID] = opMap[aID];
        }
    }
    void writeInstInfo(uint64_t opcode, double op1, double op2, double op3, uint64_t op1ID, uint64_t op2ID, uint64_t instID,char* funcName, char* modName, int32_t debugLine,int32_t colLine) {
        if (record == false) {
            return;
        }
        // if (unStable[opcode] == 0) {
        //     return;
        // }
        // The below code works if record sets to true.
        InstInfo info;
        info.opcode = opcode;
        info.op1 = op1;
        info.op2 = op2;
        info.op3 = op3;
        info.op1ID = op1ID;
        info.op2ID = op2ID;
        info.instID = instID;
        info.funcName=funcName;
        info.modName = modName;
        info.debugLine = debugLine;
        info.colLine = colLine;
        infoList.push_back(info);
    }
    void writeCallInfo(uint64_t *op1ID,int size){
        if(callArgsList.size()==0){
            for(size_t i = 0;i<size;++i){
                callArgsList.push_back(op1ID[i]);
            }
        }
    }

    std::vector<InstInfo> getInstInfoList() {
        return infoList;
    }
    std::vector<uint64_t> getCallInfoList() {
        return callArgsList;
    }
    std::map<uint64_t,InstMapInfo> getMapInfoList() {
        return opMap;
    }
    bool isEmpty() {
        return infoList.empty();
    }
    uint64_t doubleToI64(double d) {
        return *(uint64_t*)(&d);
    }
    double i64ToDouble(uint64_t i) {
        return *(double*)(&i);
    }
    double doubleULP(double d) {
        uint64_t bits = doubleToI64(d);
        // make it positive.
        const uint64_t DBL_EXPOMASK = 0x7FF0000000000000uLL;
        const uint64_t DBL_FRACMASK = 0x000FFFFFFFFFFFFFuLL;
        bits = bits & (DBL_EXPOMASK | DBL_FRACMASK);
        d = i64ToDouble(bits);
        double d_plus = i64ToDouble(bits+1);
        return d_plus - d;
    }
    double disCalculation(uint64_t opcode, double lhs, double rhs, double ret, double lfit, double rfit) {
        double cond1, cond2, dzdist;
        double res;
        switch(opcode) {
            case OP_ADD:
                dzdist = fabs(ret);
                if(dzdist==0){
                    dzdist = doubleULP(lhs);
                    // dzdist = lfit*doubleULP(lhs) + rfit*doubleULP(rhs);
                    if(dzdist==0){
                        return 1.0;
                    }
                }
                cond1 = lfit * fabs(lhs) / dzdist;
                cond2 = rfit * fabs(rhs) / dzdist;
                return cond1 + cond2;
            case OP_SUB:
                dzdist = fabs(ret);
                if(dzdist==0){
                    dzdist = doubleULP(lhs);
                    if(dzdist==0){
                        return 1.0;
                    }
                }
                cond1 = lfit * (fabs(lhs) / dzdist);
                cond2 = rfit * (fabs(rhs) / dzdist);
                return cond1+cond2;
            case OP_SIN:
                if(ret == 0.0){
                    cond1 = lfit * fabs(lhs/tan(lhs+lfit*doubleULP(lhs)));
                    return cond1;
                }
                // cond1 = lfit * fabs(fabs(remainder(lhs, fpUtil::PI*2))/tan(lhs));
                // cond1 = lfit * fabs(fabs(asin(ret))/tan(lhs));
                cond1 = lfit * fabs(lhs/tan(lhs));
                return cond1;
            case OP_COS:
                if(ret==0){
                    cond1 = lfit * fabs(lhs*tan(lhs+lfit*doubleULP(lhs)));
                    return cond1;
                }
                cond1 = lfit * fabs(lhs*tan(lhs));
                // cond1 = lfit * fabs(fabs(acos(ret))*tan(lhs));
                // cond1 = lfit * fabs(fabs(remainder(lhs, fpUtil::PI*2))*tan(lhs));
                return cond1;
            case OP_TAN:
                cond1 = lfit * fabs(lhs / (sin(lhs) * cos(lhs)));
                if(std::isinf(cond1)){
                    cond1 = lfit * fabs(lhs / (sin(lhs+lfit*doubleULP(lhs)) * cos(lhs+lfit*doubleULP(rhs))));
                    return cond1;
                }
                return cond1;
            case OP_ASIN:
                if(ret==0){
                    return lfit*fabs(lhs/doubleULP(lhs));
                }
                cond1 = lfit * fabs(lhs / (sqrt(1-lhs*lhs) * asin(lhs)));
                return cond1;
            case OP_ACOS:
                if(ret==0){
                    return lfit*fabs(lhs/doubleULP(lhs));
                }
                cond1 = lfit * fabs(lhs / (sqrt(1-lhs*lhs) * acos(lhs)));
                return cond1;
            case OP_SINH:
                if(ret==0){
                    return lfit*fabs(lhs/doubleULP(lhs));
                }
                cond1 = lfit * fabs(lhs / tanh(lhs));
                return cond1;
            case OP_COSH:
                if(ret==0){
                    return lfit*fabs(lhs/doubleULP(lhs));
                }
                cond1 = lfit * fabs(lhs * tanh(lhs));
                return cond1;
            case OP_LOG:
                if(lhs==1.0){
                    if(lfit==0){
                        return 1.0;
                    }
                    cond1 = lfit * fabs(1 / log(lhs+lfit*doubleULP(lhs)));
                    return cond1;
                }
                cond1 =lfit * fabs(1 / log(lhs));
                return cond1;
            case OP_EXP:
                if(ret==0){
                    if(exp(lhs+lfit*doubleULP(lhs))==0){
                        return 1.0;
                    }else{
                        dzdist = lfit * fabs(lhs);
                        return dzdist;
                    }
                }
                dzdist = lfit * fabs(lhs);
                return dzdist;
            case OP_LOG10:
                if(lhs==1.0){
                    if(lfit==0){
                        return 1.0;
                    }
                    cond1 = lfit * fabs(1 / log(lhs+lfit*doubleULP(lhs)));
                    return cond1;
                }
                cond1 = lfit * fabs(1 / log(lhs));
                return cond1;
            case OP_POW:
                if(ret==0){
                    if(std::pow(lhs+lfit*doubleULP(lhs),rhs+rfit*doubleULP(rhs))==0){
                        return 1.0;
                    }else{
                        cond1 = lfit * fabs(rhs);
                        cond2 = rfit * fabs(rhs * log(lhs));
                        return cond1 + cond2;
                    }
                }
                cond1 = lfit * fabs(rhs);
                cond2 = rfit * fabs(rhs * log(lhs));
                return cond1 + cond2;
            default:
                return 1.0;
        }
        return 1.0;
    }

};

#endif