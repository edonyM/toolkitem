#include "PyCall.h"
#include <string>
#include <iostream>
using namespace std;
void PyCall::PyRunFunc(string apppath,const char* func,const char* module,PyObject**funcarg,int argnum)
{
	PyObject *pName, *pModule, *pDict, *pFunc, *pValue,*pArgs, *pRetVal,*pImport;
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		printf("ERROR: Python Initialize failed.\n");
		return;
	}

	string origin = "sys.path.append('";
	string END = "')";
	string appendpath = origin+apppath+END;
	const char * runstr = appendpath.c_str();
	PyRun_SimpleString("import sys\n");
	PyRun_SimpleString("sys.path.append('./')\n");
	PyRun_SimpleString(runstr);
	pName = PyString_FromString(module);
	pModule = PyImport_Import(pName);

	if ( !pModule )
	{
		printf("WARNING: can't find import module\n");
		return;
	}
	pDict = PyModule_GetDict(pModule);
	if ( !pDict )
	{
		printf("ERROR: can't find __dict__ of imported module\n");
		return;
	}
	// �ҳ�����
	pFunc = PyDict_GetItemString(pDict, func);
	if ( !pFunc || !PyCallable_Check(pFunc) )
	{
		printf("ERROR: can't find attribute function\n");
		return;
	}
	// ������ջ
	pArgs = PyTuple_New(argnum);

	// PyObject* Py_BuildValue(char *format, ...)
	// ��C++�ı���ת����һ��Python���󡣵���Ҫ��
	// C++���ݱ�����Pythonʱ���ͻ�ʹ������������˺���
	// �е�����C��printf������ʽ��ͬ�����õĸ�ʽ��
	// s ��ʾ�ַ�����
	// i ��ʾ���ͱ�����
	// f ��ʾ��������
	// O ��ʾһ��Python����

	for(int i=0;i<argnum;i++)
		PyTuple_SetItem(pArgs, i, *(funcarg+i));

	// ����Python����
	pRetVal = PyObject_CallObject(pFunc, pArgs);
	Py_Finalize();
}
int PyCall::PyInit()
{
	PyRun_SimpleString("import sys\n");
	PyRun_SimpleString("sys.path.append('./')\n");
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		printf("ERROR: Python Initialize failed.\n");
		return -1;
	}
    else
        return 1;
}
void PyCall::addpath(string path)
{
	PyRun_SimpleString("import sys");
	string origin = "sys.path.append('";
	string END = "')";
	string appendpath = origin+path+END;
	const char * runstr = appendpath.c_str();
	PyRun_SimpleString(runstr);
}
PyObject* PyCall::PyImport(string strmod,PyObject* Pymod)
{
    if(!strmod.empty() || Pymod)
    {
        PyObject* strModule = PyString_FromString(strmod.c_str());
        PyObject* pstrModule = PyImport_Import(strModule);
	    if ( !pstrModule )
	    {
		    printf("WARNING: can't find import module\n");
	    }
        else
            return pstrModule;

	    PyObject* pModule = PyImport_Import(Pymod);
	    if ( !pModule )
	    {
		    printf("WARNING: can't find import module\n");
		    return 0;
	    }
        else
            return pModule;
    }
    else
    {
        std::cout<<"error: only one parameter taken\n";
        return 0;
    }
}

PyObject* PyCall::caller()
{
    int flag = this->PyInit();
    if(flag != -1)
    {
        PyObject* mod = this->PyImport(this->importmod,this->pyimportmod);
    	PyObject* pDict = PyModule_GetDict(mod);
    	if ( !pDict )
    	{
    		printf("ERROR: can't find __dict__ of imported module\n");
    		return 0;
    	}
    	// �ҳ�����
	    PyObject* pFunc = PyDict_GetItemString(pDict, this->func.c_str());
	    if ( !pFunc || !PyCallable_Check(pFunc) )
	    {
    		printf("ERROR: can't find attribute function\n");
    		return 0;
    	}
    	// ������ջ
        size_t argnum = this->pyargv.size();
    	PyObject* pArgs = PyTuple_New(argnum);

    	// PyObject* Py_BuildValue(char *format, ...)
    	// ��C++�ı���ת����һ��Python���󡣵���Ҫ��
    	// C++���ݱ�����Pythonʱ���ͻ�ʹ������������˺���
    	// �е�����C��printf������ʽ��ͬ�����õĸ�ʽ��
    	// s ��ʾ�ַ�����
    	// i ��ʾ���ͱ�����
    	// f ��ʾ��������
	    // O ��ʾһ��Python����

	    for(int i=0;i<argnum;i++)
		    PyTuple_SetItem(pArgs, i, this->pyargv[i]);

	    // ����Python����
	    PyObject* pRetVal = PyObject_CallObject(pFunc, pArgs);
        Py_Finalize();
        return pRetVal;
    }
}
void PyCall::PyRunner()
{
    int flag = this->PyInit();
    if(flag != -1)
    {
        cout<<"RUNNER\n";
	    PyRun_SimpleString("import sys");
	    PyRun_SimpleString("import subprocess");
        string tmp = "[sys.executable, "+this->filename+",";
        for(size_t i=0;i<this->strargv.size();i++)
        {
            if(1==strargv.size()-i)
                tmp += strargv[i] + "]";
            else
                tmp += strargv[i] + ",";
        }
        cout<<tmp<<endl;
        string cmd = "subprocess.call(" + tmp + ")";
        //string file = "execfile(";
        //file = file + this->filename + ")";
        //cout<<file.c_str()<<endl;
        PyRun_SimpleString(cmd.c_str());
    }
    Py_Finalize();
}

