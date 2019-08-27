#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string.h>
#include <Python.h>

#include <msgpack.hpp>
#include <tinyxml2.h>

#include "Rpc.h"
#include "Util.h"

using namespace std;
using namespace tinyxml2;
using namespace XEngine;

CRpc *g_Rpc;

CRpc::CRpc()
{
}

CRpc::~CRpc()
{
    m_Name2Pid.clear();
    for (auto it = m_RpcTable.begin(); it != m_RpcTable.end(); it++) {
        if (it->second) {
            delete it->second;
        }
    }
    m_RpcTable.clear();
}

int CRpc::Init(const string & cCfgPath)
{
    ParseCfg(cCfgPath);
}

eRpcFieldType CRpc::GetArgTypeByName(const string & name)
{
    if (name == "RPC_INT32") {
        return RPC_INT32;
    } else if (name == "RPC_INT16") {
        return RPC_INT32;
    } else if (name == "RPC_PB") {
        return RPC_PB;
    } else if (name == "RPC_INT8") {
        return RPC_INT32;
    } else if (name == "RPC_STRING") {
        return RPC_STRING;
    } else if (name == "RPC_FLOAT") {
        return RPC_FLOAT;
    }
}

stRpcFunction * CRpc::ParseFunc(XMLElement *elem, eRpcType type)
{
    stRpcFunction *pFunction = new stRpcFunction();
    pFunction->type = type;

    XMLElement *field_elem;
    if (type == RPC_SERVER || type == RPC_HOST) {
        field_elem = elem->FirstChildElement("path");
        pFunction->module = string(field_elem->GetText());

        field_elem = elem->FirstChildElement("deamon");
        field_elem->QueryIntText(&pFunction->c_imp);
    }

    field_elem = elem->FirstChildElement("function");
    pFunction->name = string(field_elem->GetText());

    field_elem = elem->FirstChildElement("args");
    XMLElement *args_elem = field_elem->FirstChildElement("arg");
    pFunction->args.push_back(GetArgTypeByName(args_elem->GetText()));

    while ((args_elem = args_elem->NextSiblingElement("arg")) != NULL) {
        pFunction->args.push_back(GetArgTypeByName(args_elem->GetText()));
    }

    return pFunction;
}

void CRpc::ParseSection(XMLElement *root, eRpcType type, const char * name, std::list<stRpcFunction *> &rpc_list)
{
    XMLElement *elem = root->FirstChildElement(name);
    if (elem) {
        rpc_list.push_back(ParseFunc(elem, type));
        while ((elem = elem->NextSiblingElement(name)) != NULL) {
            rpc_list.push_back(ParseFunc(elem, type));
        }
    } else {
       //TODO 
    }
}

bool CompareFunc(const stRpcFunction *first, const stRpcFunction *second)
{
    return first->module < second->module && first->name < second->name;
}

void CRpc::ParseCfg(const string &cCfgPath)
{
    XMLDocument doc;
    std::list<stRpcFunction *> lRpcList;
    vector<string> vecCfgList;
    GetPathFiles(cCfgPath.c_str(), vecCfgList);
    for (auto it = vecCfgList.begin(); it != vecCfgList.end(); it++)
    {
        cout << *it << endl; 
        doc.LoadFile(it->c_str());
        XMLElement *root = doc.FirstChildElement("root");
        if (root) {
            ParseSection(root, RPC_SERVER, "server", lRpcList);
            ParseSection(root, RPC_CLIENT, "client", lRpcList);
            ParseSection(root, RPC_HOST, "host", lRpcList);
        } else {
            assert(0);
        }
    }
    lRpcList.sort(CompareFunc);
    RPC_PID pid = 1;
    for (auto it : lRpcList) {
        m_Name2Pid[it->name] = pid;
        m_RpcTable[pid] = it;
        pid++;
    }
}

inline stRpcFunction *CRpc::GetFunctionById(RPC_PID function_id)
{
    auto iter = m_RpcTable.find(function_id);
    if (iter != m_RpcTable.end()) {
        return iter->second;
    }
    return NULL;
}

RPC_PID CRpc::GetPidByName(const string & func_name)
{
    auto iter = m_Name2Pid.find(func_name);
    if (iter != m_Name2Pid.end()) {
        return iter->second;
    }
    return 0;
}

int CRpc::CheckFieldType(int field_type, PyObject *item)
{
    switch(field_type) {
        case RPC_INT32:
            if (!PyLong_CheckExact(item)) return -1;
            break;
        case RPC_STRING:
            if (!PyUnicode_CheckExact(item)) return -1;
            break;
        case RPC_FLOAT:
            if (!PyFloat_CheckExact(item)) return -1;
            break;
        case RPC_PB:
            if (!PyBytes_CheckExact(item)) return -1;
            break;
        default:
            fprintf(stderr, "unknown field type=%d", field_type);
            return -1;
    }
    return 0;
}

int CRpc::PackField(eRpcFieldType field, PyObject *item, msgpack::packer<msgpack::sbuffer> &packer)
{
    if (CheckFieldType(field, item) == -1) {
        fprintf(stderr, "pack field type error. expected_type=%d,type_name=%s\n", field, Py_TYPE(item)->tp_name);
        return -1;
    }

    switch(field) {
        case RPC_INT32:
            packer.pack((uint32_t)PyLong_AsLong(item));
            break;
        case RPC_STRING: 
            {
                Py_ssize_t size; 
                const char *str = PyUnicode_AsUTF8AndSize(item, &size);
                packer.pack_str((uint32_t)size);
                packer.pack_str_body(str, (uint32_t)size);
                break;
            }
        case RPC_FLOAT:
            packer.pack((float)PyFloat_AS_DOUBLE(item));
            break;
        case RPC_PB:
            {
                const char *str = PyBytes_AsString(item);
                Py_ssize_t size = PyBytes_Size(item);
                packer.pack_bin((uint32_t)size);
                packer.pack_bin_body(str, (uint32_t)size);
                break;
            }
        default:
            fprintf(stderr, "unknown field type=%d", field);
            return -1;
    }
    return 0;
}

//pack obj data to buf
int CRpc::Pack(RPC_PID pid, PyObject *obj, msgpack::sbuffer &sbuf)
{
    if (!PyTuple_CheckExact(obj)) {
        fprintf(stderr, "expected pack obj is tuple\n");
        return -1; 
    }

    stRpcFunction * pFunction = this->GetFunctionById(pid);
    if (pFunction == NULL) {
        fprintf(stderr, "can't find pid function. pid=%d\n", pid);
        return -1;
    }

    if (pFunction->args.size() != PyTuple_Size(obj)) {
        fprintf(stderr, "tuple obj size is not equal arg cnt. expected=%ld, size=%ld\n", pFunction->args.size(), PyTuple_Size(obj));
        return -1;
    }
    
    msgpack::packer<msgpack::sbuffer> packer(sbuf);
    packer.pack(pid);
    int i = 0;
    for (auto iter : pFunction->args) {
        cout << iter << endl;
        if (this->PackField(iter, PyTuple_GetItem(obj, i++), packer) != 0) return -1;
    }

    return 0;
}

//return obj 正常
//return null 异常
PyObject *CRpc::UnPackField(eRpcFieldType field, msgpack::unpacker &unpacker)
{
    msgpack::object_handle result;
    if (unpacker.next(result)) {
        msgpack::object obj = result.get();
        switch(field) {
            case RPC_INT32:
                return PyLong_FromLong(obj.via.u64);
            case RPC_STRING: 
            case RPC_PB:
                return PyBytes_FromStringAndSize(obj.via.bin.ptr, obj.via.bin.size);
            case RPC_FLOAT:
                return PyFloat_FromDouble(obj.via.f64);
            default:
                assert(0);
        }
    } else {
        assert(0);
    }
}

//unpack buf to obj data
PyObject * CRpc::UnPack(const char *buf, size_t len)
{
    PyObject *obj;
    PyObject *field_value;

    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(len);

    memcpy(unpacker.buffer(), buf, len);
    unpacker.buffer_consumed(len);
    msgpack::object_handle result;

    unpacker.next(result);
    msgpack::object data = result.get();
    RPC_PID pid = data.via.u64;

    const stRpcFunction *pFunction = GetFunctionById(pid);
    obj = PyTuple_New(pFunction->args.size());

    int i = 0;
    for (auto iter : pFunction->args) {
        if ((field_value = this->UnPackField(iter, unpacker)) == NULL) {
            Py_DECREF(obj);
            return NULL;
        }
        PyTuple_SetItem(obj, i++, field_value);
    }
    return obj;
}

int main()
{
    CRpc *pRpc = new CRpc();
    pRpc->Init("./config.xml");
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(&sbuf);
    Py_Initialize();
    PyObject *value = PyTuple_New(3);
    PyObject *io = PyLong_FromLong(100);
    //PyObject *so = PyBytes_FromString("msg");
    PyObject *so = PyLong_FromLong(123);
    PyObject *so2 = PyUnicode_FromString("msg2");

    int ret = PyTuple_SetItem(value, 0, io);
    int ret2 = PyTuple_SetItem(value, 1, so);
    PyTuple_SetItem(value, 2, so2);

    assert(ret == 0 && ret2 == 0);
    RPC_PID pid = pRpc->GetPidByName("rpc_server_hello");
    cout << "pid = " << pid <<  endl;

    pRpc->Pack(pid, value, sbuf);
    PyObject *up = pRpc->UnPack(sbuf.data(), sbuf.size());

    PyObject_Print((PyObject *)up, stdout, 0);
    printf("\n");
}
