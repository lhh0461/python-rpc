#ifndef __RPC__
#define __RPC__

#include <Python.h>
#include <list>
#include <map>
#include <string>

#include <msgpack.hpp>
#include <tinyxml2.h>

using namespace std;
using namespace tinyxml2;

namespace XEngine
{

typedef uint16_t RPC_PID;

enum eRpcType
{
    RPC_SERVER = 1,
    RPC_CLIENT = 2,
    RPC_HOST = 3,
};

enum eRpcFieldType
{
    RPC_INT32 = 1,
    RPC_STRING = 2,
    RPC_FLOAT = 3,
    RPC_PB = 4,
};

enum eRpcDeamonType
{
    RPC_GAMDE = 1,
    RPC_GATED = 2,
    RPC_DBD = 3,
};

struct stRpcFunction
{
    eRpcType type;
    RPC_PID pid;
    string name;
    string module;
    eRpcDeamonType deamon;
    list<eRpcFieldType> args;
};

class CRpc
{
public:
    CRpc();
    ~CRpc();
    int Init(const string & cCfgPath);
    RPC_PID GetPidByName(const string & cFuncName);
    int Pack(RPC_PID pid, PyObject *obj, msgpack::sbuffer &sbuf);
    PyObject *UnPack(const char *buf, size_t len);
private:
    stRpcFunction *GetFunctionById(RPC_PID pid);
    int PackField(eRpcFieldType field, PyObject *item, msgpack::packer<msgpack::sbuffer> &packer);
    PyObject *UnPackField(eRpcFieldType field, msgpack::unpacker &unpacker);
private:
    stRpcFunction *ParseFunc(XMLElement *elem, eRpcType type);
    void ParseSection(XMLElement *root, eRpcType type, const char * name, std::list<stRpcFunction *> &lRpcList);
    void ParseCfg(const string &cfg);
    eRpcFieldType GetArgTypeByName(const string & name);
    int CheckFieldType(int field_type, PyObject *item);
private:
    map<string, RPC_PID> m_Name2Pid;
    map<RPC_PID, stRpcFunction *> m_RpcTable;
};

extern "C" CRpc *g_Rpc;

}

#endif //__RPC__

