#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cinttypes>
#include "communicator.h"

extern "C" bool fCmpInstHandler(double, double, bool, int32_t,
                                char*, char*, uint64_t, int32_t, int32_t);
extern "C" bool iCmpInstHandler(uint64_t, uint64_t, bool, int32_t,
                                char*, char*, uint64_t, int32_t, int32_t);
extern "C" void fAddHandler(double, double, double, uint64_t, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void fSubHandler(double, double, double, uint64_t, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void fMulHandler(double, double, double, uint64_t, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void fDivHandler(double, double, double, uint64_t, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void callOneArgHandler(int32_t, double, double, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void callTwoArgsHandler(int32_t, double, double, double, uint64_t, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void callTwoArgsHandler(int32_t, double, double, double, uint64_t, uint64_t, char*, char*, uint64_t, int32_t, int32_t);
extern "C" void storeManyArgsHandler(uint64_t *aID,int size);
extern "C" void loadManyArgsHandler(uint64_t *aID,int size);
extern "C" void loadInstHandler(uint64_t aID,uint64_t bID);
extern "C" void storeInstHandler(uint64_t aID,uint64_t bID);
bool fCmpInstHandler(
        double a,
        double b,
        bool origin_res,
        int32_t predicate,
        char* funcName,
        char* modName,
        uint64_t instID,
        int32_t debugLine,
        int32_t debugCol)
{
    return origin_res;
}

bool iCmpInstHandler(
        uint64_t a,
        uint64_t b,
        bool origin_res,
        int32_t predicate,
        char* funcName,
        char* modName,
        uint64_t instID,
        int32_t debugLine,
        int32_t debugCol)
{
    return origin_res;
}

void fAddHandler(
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(OP_ADD, a, b, c, aID, bID, instID, funcName, modName,debugLine,debugCol);
}

void fSubHandler(
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(OP_SUB, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}

void fMulHandler(
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(OP_MUL, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}

void fDivHandler(
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(OP_DIV, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}

void callOneArgHandler(
    int32_t functionID,
    double a,
    double c,
    uint64_t aID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(functionID, a, a,c, aID, aID,  instID, funcName, modName,debugLine,debugCol);
}

void callTwoArgsHandler(
    int32_t functionID,
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(functionID, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}

void loadHandler(
    int32_t functionID,
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(functionID, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}
void getelementerPtrHandler(
    int32_t functionID,
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(functionID, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}
void storeHandler(
    int32_t functionID,
    double a,
    double b,
    double c,
    uint64_t aID,
    uint64_t bID,
    char* funcName,
    char* modName,
    uint64_t instID,
    int32_t debugLine,
    int32_t debugCol)
{
    Communicator &comm = Communicator::getInstance();
    comm.writeInstInfo(functionID, a, b, c, aID, bID,  instID, funcName, modName,debugLine,debugCol);
}
void storeManyArgsHandler(
    uint64_t *aID,int size)
{
    //get all args id before call a function; 
    Communicator &comm = Communicator::getInstance();
    comm.clearCallArgsList();
    comm.writeCallInfo(aID,size);

}
void loadManyArgsHandler(uint64_t *aID,int size)
{
    //find all args id at the begin of a function; 
    Communicator &comm = Communicator::getInstance();
    std::vector<uint64_t> callArgsList = comm.getCallInfoList(); 
    char* name=new char[5];
    strcpy(name,"hello");
    if(callArgsList.size()==size){
        for(size_t i = 0;i<size;++i){
            comm.writeMapInfo(aID[i],-1,0,0,0,0,0,0,name,name,0,1,callArgsList[0]);
        }
    }else{
        if(comm.getOutID()==0){
            comm.writeOutputID(aID[size-1]);
        }
    }
    comm.clearCallArgsList();
}
void loadInstHandler(uint64_t aID,uint64_t bID){
    Communicator &comm = Communicator::getInstance();
    comm.loadToMap(aID,bID);
}
void storeInstHandler(uint64_t aID,uint64_t bID){
    Communicator &comm = Communicator::getInstance();
    comm.storeToMap(aID,bID);
}