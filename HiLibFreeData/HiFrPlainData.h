#pragma once

class ChPlainData_node
{
public:
	const static int ChPlainData_DataMax = 32;
	const static int ChPlainData_CharMax = 64;
	int num;
	char name[32];
	int cnt;
	enum _type { INT, FLOAT, CHAR } type;
	ChPlainData_node *previous, *next;

	ChPlainData_node();
	~ChPlainData_node();
	int getNum(){ return num; }
	char *getName(){ return name; }
	int getCnt(){ return cnt; }
	bool typeIsINT(){ if(type==INT){ return true; } return false; }
	bool typeIsNotINT(){ if(type!=INT){ return true; } return false; }
	bool typeIsFLOAT(){ if(type==FLOAT){ return true; } return false; }
	bool typeIsNotFLOAT(){ if(type!=FLOAT){ return true; } return false; }
	bool typeIsCHAR(){ if(type==CHAR){ return true; } return false; }
	bool typeIsNotCHAR(){ if(type!=CHAR){ return true; } return false; }

	void setPrevious(ChPlainData_node *p){ previous=p; }
	ChPlainData_node *getPrevious(){ return previous; }
	void setNext(ChPlainData_node *p){ next=p; }	
	ChPlainData_node *getNext(){ return next; }

	bool lessNum(int inNum){ if(num<inNum){ return true; } return false; }
	bool lessName(char *inName){ if(strcmp(name, inName)<0){ return true; } return false; }
	bool equalNum(int inNum){ if(num==inNum){ return true; } return false; }
	bool equalName(char *inName){ if(strcmp(name, inName)==0){ return true; } return false; }
	bool moreNum(int inNum){ if(num>inNum){ return true; } return false; }
	bool moreName(char *inName){ if(strcmp(name, inName)>0){ return true; } return false; }
	virtual void print(FILE *pFile)=0;
	virtual void printDataOnly(FILE *pFile)=0;
	virtual bool scanString(int inNum, char *inName, int inCnt, char *str)=0;
};
class ChPlainData_floatNode : public ChPlainData_node
{
public:
	float floatData[32];

	ChPlainData_floatNode();
	~ChPlainData_floatNode();
	void setFloat(int inNum, char *inName, int inCnt, float *in);
	void getFloat(int &outCnt, float *out);
	void print(FILE *pFile);
	void printDataOnly(FILE *pFile);
	bool scanString(int inNum, char *inName, int inCnt, char *str);
};
class ChPlainData_intNode : public ChPlainData_node
{
public:
	int intData[32];

	ChPlainData_intNode();
	~ChPlainData_intNode();
	void setInt(int inNum, char *inName, int inCnt, int *in);
	void getInt(int &outCnt, int *out);
	void print(FILE *pFile);
	void printDataOnly(FILE *pFile);
	bool scanString(int inNum, char *inName, int inCnt, char *str);
};
class ChPlainData_charNode : public ChPlainData_node
{
public:
	char charData[ChPlainData_CharMax];

	ChPlainData_charNode();
	~ChPlainData_charNode();
	void setChar(int inNum, char *inName, int inCnt, char *in);
	void getChar(int &outCnt, char *out);
	void print(FILE *pFile);
	void printDataOnly(FILE *pFile);
	bool scanString(int inNum, char *inName, int inCnt, char *str); // inCntは適当でよい
};

///// ///// ChPlainData ///// /////
///// 機能：データの読み込み・書き込み
/////
class ChPlainData
{
private:
	ChPlainData_node *m_startNode, *m_endNode;
	ChPlainData_node *search(int num, char *name);
	bool insertAtEnd(ChPlainData_node *node);
	bool insertAtFirst(ChPlainData_node *node);
	void deleteNode(ChPlainData_node *node);
public:
	ChPlainData(void);
	~ChPlainData(void);
	bool create();
	bool destroy();
	bool set(int num, char *name, int cnt, int *in);
	bool set(int num, char *name, int cnt, float *in);
	bool set(int num, char *name, int cnt, double *in);
	bool set(int num, char *name, int cnt, char *in);
	bool get(int num, char *name, int &cnt, int *out);
	bool get(int num, char *name, int &cnt, float *out);
	bool get(int num, char *name, int &cnt, double *out);
	bool get(int num, char *name, int &cnt, char *out);
	int get(int num, char *name, int *out);
	int get(int num, char *name, float *out);
	int get(int num, char *name, double *out);
	int get(int num, char *name, char *out);
	bool save(char *fileName);
	bool saveSortDestroy(char *fileName);
	bool load(char *fileName);
};

///// ///// Ch2PlainData ///// /////
///// 機能：データの書き込み・読み込み
///// 特徴：ChPlainDataより高速にデータを読み込める

class Ch2PlainData_numNode
{
private:
	int m_num;
	Ch2PlainData_numNode *m_previous, *m_next;
	ChPlainData_node *m_childTop, *m_childBottom;
public:
	Ch2PlainData_numNode(int n);
	~Ch2PlainData_numNode();
	int getNum()								{ return m_num; }
	bool numIsLessThan(int num)					{ if(m_num<num){ return true; } return false; }
	bool numIsEqual(int num)					{ if(num==m_num){ return true; } return false; }
	bool numIsMoreThan(int num)					{ if(m_num>num){ return true; } return false; }
	void setNext(Ch2PlainData_numNode *pN)		{ m_next = pN; }
	void setPrevious(Ch2PlainData_numNode *pN)	{ m_previous = pN; }
	Ch2PlainData_numNode *getNext()				{ return m_next; }
	Ch2PlainData_numNode *getPrevious()			{ return m_previous; }

	bool setChild(ChPlainData_node *pN);
	ChPlainData_node* searchChild(char* name);
	bool deleteChild(ChPlainData_node *pN);
	bool destroyChildren();
	bool destroySortPrint(FILE *file);
};

class Ch2PlainData
{
private:
	Ch2PlainData_numNode *m_numNodeStart, *m_numNodeEnd;
	Ch2PlainData_numNode *searchNumNode(int num);
	ChPlainData_node *searchNode(int num, char *name);
	bool insertNumNode(Ch2PlainData_numNode *pN);
	bool insertNode(ChPlainData_node *pN);
public:
	const static int ChPlainData_DataMax = 32;

	Ch2PlainData(void)		{ create(); }
	~Ch2PlainData(void)		{ destroy(); }
	bool create();
	bool destroy();
	bool set(int num, char *name, int cnt, int *in);
	bool set(int num, char *name, int cnt, float *in);
	bool set(int num, char *name, int cnt, double *in);
	bool set(int num, char *name, int cnt, char *in);
	int get(int num, char *name, int *out);
	int get(int num, char *name, float *out);
	int get(int num, char *name, double *out);
	int get(int num, char *name, char *out);
	bool saveDestroy(char *fileName);
	bool savePickUp(char *fileName, char *name);
	bool savePickUp(char *fileName, char **arrName);
	bool load(char *fileName);
	bool retrieve(char *str);
};
