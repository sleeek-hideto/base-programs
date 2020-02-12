#pragma once

class IGlSolverNode
{
public:
	char m_Type[ 16 ];

	IGlSolverNode(void){
	}
	~IGlSolverNode(void){
	}

	virtual void Destroy()=0;
};

class IGlSolverManager
{
public:
	IGlSolverNode* m_NodeTop;

	IGlSolverManager(void){
		m_NodeTop = NULL;
	}
	~IGlSolverManager(void){
	}

	virtual bool Run()=0;
};
