#include "StdAfx.h"
#include ".\HiFrPlainData.h"

char *_ChPlainData_getNextSpace(char *str)
{
	while(str[0]!=' ' && str[0]!='\t' && str[0]!='\n' && str[0]!=0){ str++; }
	while(str[0]==' ' || str[0]=='\t'){ str++; }
	return str;
}
char *_ChPlainData_getNextColon(char *str)
{
	while(str[0]!=':' && str[0]!='\n' && str[0]!='\0'){ str++; }
	return ++str;
}

ChPlainData_node::ChPlainData_node()
{
	previous = NULL; next = NULL;
}
ChPlainData_node::~ChPlainData_node()
{
}

ChPlainData_floatNode::ChPlainData_floatNode()
{
	type = ChPlainData_node::FLOAT;
}
ChPlainData_floatNode::~ChPlainData_floatNode()
{
}
void ChPlainData_floatNode::setFloat(int inNum, char *inName, int inCnt, float *in)
{
	num = inNum;
	sprintf(name, "%s", inName);
	cnt = inCnt;
	memcpy(floatData, in, inCnt*4);
}
void ChPlainData_floatNode::getFloat(int &outCnt, float *out)
{
	outCnt = cnt;
	memcpy(out, floatData, cnt*4);
}
void ChPlainData_floatNode::print(FILE *pFile)
{
	int i;
	fprintf(pFile, "%d %s FLOAT %d:", num, name, cnt);
	for(i=0; i<cnt-1; i++){ fprintf(pFile, "%f ", floatData[i]); }
	fprintf(pFile, "%f\n", floatData[i]);
}
void ChPlainData_floatNode::printDataOnly(FILE *pFile)
{
	for(int i=0; i<cnt; i++){ fprintf(pFile, "%f ", floatData[i]); }
}
bool ChPlainData_floatNode::scanString(int inNum, char *inName, int inCnt, char *str)
{
	num = inNum;
	sprintf(name, "%s", inName);
	cnt = inCnt;
	for(int i=0; i<cnt; i++)
	{
		floatData[i] = (float)atof(str);
		str = _ChPlainData_getNextSpace(str);
	}
	return true;
}
///// CHAR /////
ChPlainData_charNode::ChPlainData_charNode()
{
	type = ChPlainData_node::CHAR;
}
ChPlainData_charNode::~ChPlainData_charNode()
{
}
void ChPlainData_charNode::setChar(int inNum, char *inName, int inCnt, char *in)
{
	num = inNum;
	sprintf(name, "%s", inName);
	if(inCnt==-1){
		cnt = (int)strlen(in);
	}else{
		cnt = inCnt;
	}
	if(cnt>ChPlainData_CharMax-1)cnt=ChPlainData_CharMax-1;
	strncpy(charData, in, cnt);
	charData[cnt]=0;
}
void ChPlainData_charNode::getChar(int &outCnt, char *out)
{
	outCnt = cnt;
	strncpy(out, charData, cnt); out[cnt]=0;
}
void ChPlainData_charNode::print(FILE *pFile)
{
	fprintf(pFile, "%d %s CHAR %d:%s\n", num, name, cnt, charData);
}
void ChPlainData_charNode::printDataOnly(FILE *pFile)
{
	fprintf(pFile, "%s ", charData);
}
bool ChPlainData_charNode::scanString(int inNum, char *inName, int inCnt, char *str)
{
	num = inNum;
	sprintf(name, "%s", inName);
	if(inCnt==-1){
		cnt = (int)strlen(str);
		if(cnt>ChPlainData_CharMax-1){ cnt=ChPlainData_CharMax-1; }
		sprintf(charData, "%s", str);
		if(cnt>0 && charData[cnt-1]=='\n'){ charData[cnt-1]='\0'; cnt -= 1; }
	}else{
		cnt = inCnt;
		if(cnt>ChPlainData_CharMax-1){ cnt=ChPlainData_CharMax-1; }
		strncpy(charData, str, cnt);
		charData[cnt]='\0';
	}
	return true;
}
///// INT /////
ChPlainData_intNode::ChPlainData_intNode()
{
	type = ChPlainData_node::INT;
}
ChPlainData_intNode::~ChPlainData_intNode()
{
}
void ChPlainData_intNode::setInt(int inNum, char *inName, int inCnt, int *in)
{
	num = inNum;
	sprintf(name, "%s", inName);
	cnt = inCnt;
	memcpy(intData, in, inCnt*4);
}
void ChPlainData_intNode::getInt(int &outCnt, int *out)
{
	outCnt = cnt;
	memcpy(out, intData, cnt*4);
}
void ChPlainData_intNode::print(FILE *pFile)
{
	int i;
	fprintf(pFile, "%d %s INT %d:", num, name, cnt);
	for(i=0; i<cnt-1; i++){ fprintf(pFile, "%d ", intData[i]); }
	fprintf(pFile, "%d\n", intData[i]);
}
void ChPlainData_intNode::printDataOnly(FILE *pFile)
{
	for(int i=0; i<cnt; i++){ fprintf(pFile, "%d ", intData[i]); }
}
bool ChPlainData_intNode::scanString(int inNum, char *inName, int inCnt, char *str)
{
	num = inNum;
	sprintf(name, "%s", inName);
	cnt = inCnt;
	for(int i=0; i<cnt; i++)
	{
		intData[i] = atoi(str);
		str = _ChPlainData_getNextSpace(str);
	}
	return true;
}

///// ChPlainData /////
ChPlainData::ChPlainData(void)
{
	m_startNode = NULL; m_endNode = NULL;
}
ChPlainData::~ChPlainData(void)
{
	destroy();
}

/*!
	@brief �C���f�b�N�Xnum,���Oname�̃��[�t��T��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
*/
ChPlainData_node *ChPlainData::search(int num, char *name)
{
	ChPlainData_node *tmpNode = m_startNode;
	while(tmpNode != NULL)
	{
		if(tmpNode->equalNum(num) && tmpNode->equalName(name)){ return tmpNode; }
		tmpNode = tmpNode->getNext();
	}
	return NULL;
}

/*!
	@brief ���[�t���Ō�ɑ���
	@param[in] node ���[�t
*/
bool ChPlainData::insertAtEnd(ChPlainData_node *node)
{
	if(m_startNode == NULL){
		m_startNode = node; node->setPrevious(NULL); node->setNext(NULL);
		m_endNode = node;
	}else{
		m_endNode->setNext(node); node->setPrevious(m_endNode);
		m_endNode = node;
	}
	return true;
}

/*!
	@brief ���[�t���ŏ��ɑ���
	@param[in] node ���[�t
*/
bool ChPlainData::insertAtFirst(ChPlainData_node *node)
{
	if(m_endNode = NULL){
		m_startNode = node; node->setPrevious(NULL); node->setNext(NULL);
		m_endNode = node;
	}else{
		m_startNode->setPrevious(node); node->setNext(m_startNode);
		m_startNode = node;
	}
	return true;
}

/*!
	@brief ���[�t���폜����
	@param[in] node ���[�t
*/
void ChPlainData::deleteNode(ChPlainData_node *node)
{
	if(node == m_startNode){
		if(node == m_endNode)
		{
			m_startNode = NULL; m_endNode = NULL;
			delete node;
		}else{
			m_startNode = node->getNext(); m_startNode->setPrevious(NULL);
			delete node;
		}
	}else if(node == m_endNode){
		m_endNode = node->getPrevious(); m_endNode->setNext(NULL);
		delete node;
	}else{
		node->getPrevious()->setNext(node->getNext());
		node->getNext()->setPrevious(node->getPrevious());
		delete node;
	}
}

/*!
	@brief ���������s���B�g��Ȃ��ėǂ�
*/
bool ChPlainData::create()
{
	m_startNode = NULL; m_endNode = NULL;
	return true;
}

/*!
	@brief �I�������s���B
*/
bool ChPlainData::destroy()
{
	ChPlainData_node *tmpNode=m_startNode, *delNode;
	while(tmpNode != NULL)
	{
		delNode = tmpNode;
		tmpNode=tmpNode->getNext();
		delete delNode;
	}
	m_startNode = NULL; m_endNode = NULL;
	return true;
}

/*!
	@brief �����z����i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ��
	@param[in] in �����z��ւ̃|�C���^
*/
bool ChPlainData::set(int num, char *name, int cnt, int *in)
{
	ChPlainData_intNode *tmpNode = (ChPlainData_intNode*)(search(num, name));
	if(tmpNode != NULL)
	{
		if(tmpNode->typeIsNotINT())return false;
		tmpNode->setInt(num, name, cnt, in);
	}else{
		tmpNode = new ChPlainData_intNode();
		tmpNode->setInt(num, name, cnt, in);
		insertAtEnd(tmpNode);
	}
	return true;
}

/*!
	@brief ���������z����i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ��
	@param[in] in ���������z��ւ̃|�C���^
*/
bool ChPlainData::set(int num, char *name, int cnt, float *in)
{
	ChPlainData_floatNode *tmpNode = (ChPlainData_floatNode*)(search(num, name));
	if(tmpNode != NULL)
	{
		if(tmpNode->typeIsNotFLOAT())return false;
		tmpNode->setFloat(num, name, cnt, in);
	}else{
		tmpNode = new ChPlainData_floatNode();
		tmpNode->setFloat(num, name, cnt, in);
		insertAtEnd(tmpNode);
	}
	return true;
}

/*!
	@brief ���������z����i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ��
	@param[in] in ���������z��ւ̃|�C���^
*/
bool ChPlainData::set(int num, char *name, int cnt, double *in)
{
	float tmpIn[32];
	for(int t=0; t<cnt && t<32; t++){ tmpIn[t]=(float)in[t]; }
	if(set(num, name, cnt, tmpIn)==false)return false;
	return true;
}

/*!
	@brief ��������i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt �������B-1�ł��悢
	@param[in] in ������ւ̃|�C���^
*/
bool ChPlainData::set(int num, char *name, int cnt, char *in)
{
	ChPlainData_charNode *tmpNode = (ChPlainData_charNode*)(search(num, name));
	if(tmpNode != NULL)
	{
		if(tmpNode->typeIsNotCHAR())return false;
		tmpNode->setChar(num, name, cnt, in);
	}else{
		tmpNode = new ChPlainData_charNode();
		tmpNode->setChar(num, name, cnt, in);
		insertAtEnd(tmpNode);
	}
	return true;
}

/*!
	@brief �����z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] cnt ��
	@param[out] out �����z��ւ̃|�C���^
*/
bool ChPlainData::get(int num, char *name, int &cnt, int *out)
{
	ChPlainData_intNode *tmpNode = (ChPlainData_intNode*)(search(num, name));
	if(tmpNode == NULL || tmpNode->typeIsNotINT())return false;
	tmpNode->getInt(cnt, out);
	return true;
}

/*!
	@brief ���������z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] cnt ��
	@param[out] out ���������z��ւ̃|�C���^
*/
bool ChPlainData::get(int num, char *name, int &cnt, float *out)
{
	ChPlainData_floatNode *tmpNode = (ChPlainData_floatNode*)(search(num, name));
	if(tmpNode == NULL || tmpNode->typeIsNotFLOAT())return false;
	tmpNode->getFloat(cnt, out);
	return true;
}

/*!
	@brief ���������z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] cnt ��
	@param[out] out ���������z��ւ̃|�C���^
*/
bool ChPlainData::get(int num, char *name, int &cnt, double *out)
{
	float tmpOut[32];
	if(get(num, name, cnt, tmpOut)==false)return false;
	for(int t=0; t<cnt && t<32; t++){ out[t]=(double)tmpOut[t]; }
	return true;
}

/*!
	@brief ����������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] cnt ������
	@param[out] out ������ւ̃|�C���^
*/
bool ChPlainData::get(int num, char *name, int &cnt, char *out)
{
	ChPlainData_charNode *tmpNode = (ChPlainData_charNode*)(search(num, name));
	if(tmpNode == NULL || tmpNode->typeIsNotCHAR())return false;
	tmpNode->getChar(cnt, out);
	return true;
}

/*!
	@brief �����z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out �����z��ւ̃|�C���^
	@return �z��̌��A���s�����ꍇ��0
*/
int ChPlainData::get(int num, char *name, int *out)
{
	ChPlainData_intNode *tmpNode = (ChPlainData_intNode*)(search(num, name));
	if(tmpNode == NULL || tmpNode->typeIsNotINT())return 0;
	int cnt;
	tmpNode->getInt(cnt, out);
	return cnt;
}

/*!
	@brief ���������z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out ���������z��ւ̃|�C���^
	@return �z��̌��A���s�����ꍇ��0
*/
int ChPlainData::get(int num, char *name, float *out)
{
	ChPlainData_floatNode *tmpNode = (ChPlainData_floatNode*)(search(num, name));
	if(tmpNode == NULL || tmpNode->typeIsNotFLOAT())return 0;
	int cnt;
	tmpNode->getFloat(cnt, out);
	return cnt;
}

/*!
	@brief ���������z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out ���������z��ւ̃|�C���^
	@return �z��̌��A���s�����ꍇ��0
*/
int ChPlainData::get(int num, char *name, double *out)
{
	int cnt;
	float tmpOut[32];
	if(get(num, name, cnt, tmpOut)==false)return 0;
	for(int t=0; t<cnt && t<32; t++){ out[t]=(double)tmpOut[t]; }
	return cnt;
}

/*!
	@brief ����������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out ������ւ̃|�C���^
	@return �����̌��A���s�����ꍇ��0
*/
int ChPlainData::get(int num, char *name, char *out)
{
	ChPlainData_charNode *tmpNode = (ChPlainData_charNode*)(search(num, name));
	if(tmpNode == NULL || tmpNode->typeIsNotCHAR())return 0;
	int cnt;
	tmpNode->getChar(cnt, out);
	return cnt;
}

/*!
	@brief �c���[���t�@�C���ɕۑ�����
	@param[in] fileName �t�@�C���̈ʒu
*/
bool ChPlainData::save(char *fileName)
{
	FILE *file = fopen(fileName, "w");
	ChPlainData_node *tmpNode = m_startNode;
	while(tmpNode != NULL)
	{
		tmpNode->print(file);
		tmpNode = tmpNode->getNext();
	}
	fclose(file);
	return true;
}

/*!
	@brief �c���[���\�[�g���Ȃ���t�@�C���ɕۑ����A����ɏI����������
	@param[in] fileName �t�@�C���̈ʒu
*/
bool ChPlainData::saveSortDestroy(char *fileName)
{
	FILE *file = fopen(fileName, "w");
	ChPlainData_node *tmpNode, *delNode;
	while(m_startNode != NULL)
	{
		tmpNode = m_startNode;
		delNode = m_startNode;
		while(tmpNode != NULL)
		{
			if(delNode->moreNum(tmpNode->getNum())){
				delNode=tmpNode;
			}else if(delNode->equalNum(tmpNode->getNum())){
				if(delNode->moreName(tmpNode->getName())){ delNode=tmpNode; }
			}
			tmpNode = tmpNode->getNext();
		}
		delNode->print(file);
		deleteNode(delNode);
	}
	fclose(file);
	return true;
}

/*!
	@brief �c���[���t�@�C������ǂݍ���
	@param[in] fileName �t�@�C���̈ʒu
*/
bool ChPlainData::load(char *fileName)
{
	FILE *file = fopen(fileName, "r"); if(file==NULL)return false;
	int num, cnt;
	char line[1024], name[64], type[16]; char *data;
	line[1023] = 0;
	if(m_startNode != NULL || m_endNode != NULL){ destroy(); }

	while(fgets(line, 512, file) != NULL)
	{
		if(sscanf(line, "%d %s %s %d:", &num, name, type, &cnt)==0)continue;
		data = _ChPlainData_getNextColon(line);
		if(strcmp(type, "INT")==0){
			ChPlainData_intNode *newIntNode = new ChPlainData_intNode();
			newIntNode->scanString(num, name, cnt, data);
			insertAtEnd(newIntNode);
		}else if(strcmp(type, "FLOAT")==0){
			ChPlainData_floatNode *newFloatNode = new ChPlainData_floatNode();
			newFloatNode->scanString(num, name, cnt, data);
			insertAtEnd(newFloatNode);
		}else if(strcmp(type, "CHAR")==0){
			ChPlainData_charNode *newCharNode = new ChPlainData_charNode();
			newCharNode->scanString(num, name, cnt, data);
			insertAtEnd(newCharNode);
		}
	}
	fclose(file);
	return true;
}

///// Ch2PlainData /////
Ch2PlainData_numNode::Ch2PlainData_numNode(int n)
{
	m_num = n;
	m_previous = NULL; m_next = NULL;
	m_childTop = NULL; m_childBottom = NULL;
}
Ch2PlainData_numNode::~Ch2PlainData_numNode()
{
}

/*!
	@brief ���[�t���C���f�b�N�X�E�m�[�h�Ɋi�[����
	@param[in] pN ���[�t
*/
bool Ch2PlainData_numNode::setChild(ChPlainData_node *pN)
{
	if(m_childTop == NULL){
		m_childTop = pN; m_childBottom = pN;
	}else{
		m_childBottom->setNext(pN);
		pN->setPrevious(m_childBottom);
		m_childBottom = pN;
	}
	return true;
}

/*!
	@brief ���Oname�̃��[�t����������
	@param[in] name ���O
	@return name�̃��[�t�B������Ȃ��ꍇ��NULL
*/
ChPlainData_node* Ch2PlainData_numNode::searchChild(char* name)
{
	ChPlainData_node *passNode = m_childTop;
	while(passNode!= NULL)
	{
		if( passNode->equalName(name) )return passNode;
		passNode = passNode->getNext();
	}
	return NULL;
}

/*!
	@brief ���[�t���폜����
	@param[in] pN ���[�t
*/
bool Ch2PlainData_numNode::deleteChild(ChPlainData_node *pN)
{
	if(pN == m_childTop){
		if(pN == m_childBottom)
		{
			m_childTop = NULL; m_childBottom = NULL;
			delete pN;
		}else{
			m_childTop = m_childTop->getNext(); m_childTop->setPrevious(NULL);
			delete pN;
		}
	}else if(pN == m_childBottom){
		m_childBottom = m_childBottom->getPrevious(); m_childBottom->setNext(NULL);
		delete pN;
	}else{
		pN->getPrevious()->setNext(pN->getNext());
		pN->getNext()->setPrevious(pN->getPrevious());
		delete pN;
	}
	return true;
}

/*!
	@brief ���[�t�����ׂĔj������
*/
bool Ch2PlainData_numNode::destroyChildren()
{
	ChPlainData_node *delNode;
	ChPlainData_node *passNode = m_childTop;
	while(passNode != NULL)
	{
		delNode = passNode;
		passNode = passNode->getNext();
		delete delNode;
	}
	m_childTop = NULL; m_childBottom = NULL;
	return true;
}

/*!
	@brief ���[�t���\�[�g���Ȃ���A�t�@�C���ɏ����o���A����I��������
	@param[in] file �t�@�C���̈ʒu
*/
bool Ch2PlainData_numNode::destroySortPrint(FILE *file)
{
	ChPlainData_node *passNode, *delNode;
	while(m_childTop != NULL)
	{
		passNode = m_childTop;
		delNode = m_childTop;
		while(passNode != NULL)
		{
			if(delNode->moreNum(passNode->getNum())){
				delNode = passNode;
			}else if(delNode->equalNum(passNode->getNum())){
				if(delNode->moreName(passNode->getName())){
					delNode = passNode;
				}
			}
			passNode = passNode->getNext();
		}
		delNode->print(file);
		deleteChild(delNode);
	}
	m_childTop = NULL; m_childBottom = NULL;
	return true;
}

/////
/*!
	@brief �ԍ�num�̃m�[�h��T��
	@param[in] num �ԍ�
	@return num�̃m�[�h
*/
Ch2PlainData_numNode *Ch2PlainData::searchNumNode(int num)
{
	Ch2PlainData_numNode *passNode = m_numNodeStart;
	while(passNode != NULL)
	{
		if( passNode->numIsEqual(num) )return passNode;
		passNode = passNode->getNext();
	}
	return NULL;
}

/*!
	@brief �ԍ�num,���Oname�̃��[�t��T��
	@param[in] num �ԍ�
	@param[in] name ���O
	@return num,name�̃��[�t
*/
ChPlainData_node *Ch2PlainData::searchNode(int num, char *name)
{
	Ch2PlainData_numNode *findNode = searchNumNode(num);
	if(findNode == NULL)return NULL;
	return findNode->searchChild(name);
}

/*!
	@brief �C���f�b�N�X�E�m�[�h���c���[�ɑ}������
	@param[in] pN �m�[�h
*/
bool Ch2PlainData::insertNumNode(Ch2PlainData_numNode *pN)
{
	if(m_numNodeStart == NULL){ m_numNodeStart = pN; m_numNodeEnd = pN; return true; }
	if(pN->numIsMoreThan( m_numNodeEnd->getNum() ) ){ m_numNodeEnd->setNext(pN); pN->setPrevious(m_numNodeEnd); m_numNodeEnd = pN; return true; }
	if(pN->numIsLessThan( m_numNodeStart->getNum() ) ){ pN->setNext(m_numNodeStart); m_numNodeStart->setPrevious(pN); m_numNodeStart=pN; return true; }

	Ch2PlainData_numNode *passNode = m_numNodeStart;
	while(passNode->getNext() != NULL)
	{
		if( passNode->numIsLessThan(pN->getNum()) && pN->numIsLessThan(passNode->getNext()->getNum()) )
		{
			pN->setPrevious(passNode); pN->setNext(passNode->getNext());
			passNode->setNext(pN); pN->getNext()->setPrevious(pN);
			return true;
		}
		passNode = passNode->getNext();
	}
	return false;
}

/*!
	@brief ���[�t���c���[�ɑ}������
	@param[in] pN ���[�t
*/
bool Ch2PlainData::insertNode(ChPlainData_node *pN)
{
	Ch2PlainData_numNode *findNode = searchNumNode( pN->getNum() );
	if(findNode == NULL){
		findNode = new Ch2PlainData_numNode( pN->getNum() );
		insertNumNode(findNode);
	}
	findNode->setChild(pN);
	return true;
}

/*!
	@brief ���������s���B�g��Ȃ��ėǂ�
*/
bool Ch2PlainData::create()
{
	m_numNodeStart = NULL; m_numNodeEnd = NULL;
	return true;
}

/*!
	@brief �I�������s���B
*/
bool Ch2PlainData::destroy()
{
	Ch2PlainData_numNode *delNode, *passNode = m_numNodeStart;
	while(passNode != NULL)
	{
		delNode = passNode;
		passNode = passNode->getNext();
		delNode->destroyChildren();
		delete delNode;
	}
	m_numNodeStart = NULL; m_numNodeEnd = NULL;
	return true;
}

/*!
	@brief �����z����i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ��
	@param[in] in �����z��ւ̃|�C���^
*/
bool Ch2PlainData::set(int num, char *name, int cnt, int *in)
{
	ChPlainData_intNode *findNode = (ChPlainData_intNode*)(searchNode(num, name));
	if(findNode != NULL)
	{
		if(findNode->typeIsNotINT())return false;
		findNode->setInt(num, name, cnt, in);
	}else{
		findNode = new ChPlainData_intNode();
		findNode->setInt(num, name, cnt, in);
		insertNode(findNode);
	}
	return true;
}

/*!
	@brief ���������z����i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ��
	@param[in] in ���������z��ւ̃|�C���^
*/
bool Ch2PlainData::set(int num, char *name, int cnt, float *in)
{
	ChPlainData_floatNode *findNode = (ChPlainData_floatNode*)(searchNode(num, name));
	if(findNode != NULL)
	{
		if(findNode->typeIsNotFLOAT())return false;
		findNode->setFloat(num, name, cnt, in);
	}else{
		findNode = new ChPlainData_floatNode();
		findNode->setFloat(num, name, cnt, in);
		insertNode(findNode);
	}
	return true;
}

/*!
	@brief ���������z����i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ��
	@param[in] in ���������z��ւ̃|�C���^
*/
bool Ch2PlainData::set(int num, char *name, int cnt, double *in)
{
	float buf[ChPlainData_DataMax]; for(int i=0; i<cnt; i++){ buf[i]=(float)in[i]; }
	return set(num, name, cnt, buf);
}

/*!
	@brief ��������i�[����
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[in] cnt ������
	@param[in] in ������ւ̃|�C���^
*/
bool Ch2PlainData::set(int num, char *name, int cnt, char *in)
{
	ChPlainData_charNode *findNode = (ChPlainData_charNode*)(searchNode(num, name));
	if(findNode != NULL)
	{
		if(findNode->typeIsNotCHAR())return false;
		findNode->setChar(num, name, cnt, in);
	}else{
		findNode = new ChPlainData_charNode();
		findNode->setChar(num, name, cnt, in);
		insertNode(findNode);
	}
	return true;
}

/*!
	@brief �����z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out �����z��ւ̃|�C���^
	@return ��
*/
int Ch2PlainData::get(int num, char *name, int *out)
{
	ChPlainData_intNode *findNode = (ChPlainData_intNode*)(searchNode(num, name));
	if(findNode == NULL || findNode->typeIsNotINT())return 0;
	int cnt;
	findNode->getInt(cnt, out);
	return cnt;
}

/*!
	@brief ���������z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out ���������z��ւ̃|�C���^
	@return ��
*/
int Ch2PlainData::get(int num, char *name, float *out)
{
	ChPlainData_floatNode *findNode = (ChPlainData_floatNode*)(searchNode(num, name));
	if(findNode == NULL || findNode->typeIsNotFLOAT())return 0;
	int cnt;
	findNode->getFloat(cnt, out);
	return cnt;
}

/*!
	@brief ���������z������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out ���������z��ւ̃|�C���^
	@return ��
*/
int Ch2PlainData::get(int num, char *name, double *out)
{
	float buf[ChPlainData_DataMax];
	int cnt = get(num, name, buf);
	for(int i=0; i<cnt; i++){ out[i] = (double)buf[i]; }
	return cnt;
}

/*!
	@brief ����������o��
	@param[in] num �C���f�b�N�X
	@param[in] name ���O
	@param[out] out ������ւ̃|�C���^
	@return ������
*/
int Ch2PlainData::get(int num, char *name, char *out)
{
	ChPlainData_charNode *findNode = (ChPlainData_charNode*)(searchNode(num, name));
	if(findNode == NULL || findNode->typeIsNotCHAR())return 0;
	int cnt;
	findNode->getChar(cnt, out);
	return cnt;
}

/*!
	@brief �c���[���\�[�g���Ȃ���A�t�@�C���ɕۑ����āA����ɏI��������
	@param[in] fileName �t�@�C���̈ʒu
*/
bool Ch2PlainData::saveDestroy(char *fileName)
{
	FILE *file = fopen(fileName, "w");
	Ch2PlainData_numNode *delNode, *passNode = m_numNodeStart;
	while(passNode != NULL)
	{
		delNode = passNode;
		passNode = passNode->getNext();
		delNode->destroySortPrint(file);
		delete delNode;
	}
	m_numNodeStart = NULL; m_numNodeEnd = NULL;
	fclose(file);
	return true;
}

/*!
	@brief �c���[����name�������t�@�C���ɕۑ�����
	@param[in] fileName �t�@�C���̈ʒu
	@param[in] name ���O
*/
bool Ch2PlainData::savePickUp(char *fileName, char *name)
{
	FILE *file = fopen(fileName, "w");
	Ch2PlainData_numNode *passNode = m_numNodeStart;
	while(passNode != NULL)
	{
		ChPlainData_node *findNode;
		findNode = passNode->searchChild(name);
		if(findNode != NULL)
		{
			findNode->print(file);
		}
		passNode = passNode->getNext();
	}	
	fclose(file);
	return true;
}

/*!
	@brief �c���[����arrName�������t�@�C���ɕۑ�����
	@param[in] fileName �t�@�C���̈ʒu
	@param[in] name ���O�̔z��
*/
bool Ch2PlainData::savePickUp(char *fileName, char **arrName)
{
	FILE *file = fopen(fileName, "w");
	fprintf(file, "#");
	for(int i=0; i<ChPlainData_DataMax; i++)
	{
		if(arrName[i]==NULL)break;
		fprintf(file, "%s ", arrName[i]);
	}
	fprintf(file, "\n");

	Ch2PlainData_numNode *passNode = m_numNodeStart;
	while(passNode != NULL)
	{
		for(int i=0; i<ChPlainData_DataMax; i++)
		{
			ChPlainData_node *findNode;
			if(arrName[i]==NULL)break;
			findNode = passNode->searchChild(arrName[i]);
			if(findNode != NULL)
			{
				findNode->printDataOnly(file);
			}
		}
		fprintf(file, "\n");
		passNode = passNode->getNext();
	}	
	fclose(file);
	return true;
}

/*!
	@brief �c���[���t�@�C������ǂݍ���
	@param[in] fileName �t�@�C���̈ʒu
*/
bool Ch2PlainData::load(char *fileName)
{
	FILE *file = fopen(fileName, "r"); if(file==NULL)return false;
	int num, cnt;
	char line[512], name[32], type[16], *data;
	line[511]='\0';
	if(m_numNodeStart != NULL || m_numNodeEnd != NULL){ return false; }
	while(fgets(line, 512, file) != NULL)
	{
		sprintf(type, "NONE");
		if(line[0]=='\n' || line[0]=='#' || sscanf(line, "%d %s %s %d:", &num, name, type, &cnt)==0 )continue;

		data = _ChPlainData_getNextColon(line);
		if(strcmp(type, "INT")==0 || strcmp(type, "I")==0){
			ChPlainData_intNode *newIntNode = new ChPlainData_intNode();
			newIntNode->scanString(num, name, cnt, data);
			insertNode(newIntNode);
		}else if(strcmp(type, "FLOAT")==0 || strcmp(type, "F")==0){
			ChPlainData_floatNode *newFloatNode = new ChPlainData_floatNode();
			newFloatNode->scanString(num, name, cnt, data);
			insertNode(newFloatNode);
		}else if(strcmp(type, "CHAR")==0 || strcmp(type, "C")==0){
			ChPlainData_charNode *newCharNode = new ChPlainData_charNode();
			newCharNode->scanString(num, name, cnt, data);
			insertNode(newCharNode);
		}
	}
	fclose(file);
	return true;
}

/*!
	@brief �c���[�𕶎���X�g���[��str����ǂݍ���
	@param[in] str �X�g���[��
*/
bool Ch2PlainData::retrieve(char *str)
{
	int num, cnt, len;
	char line[512], name[32], type[16], *data;
	char *strBreak;
	len = (int)strlen( str );

	while( (strBreak = strchr( str, '\n' )) != NULL )
	{
		len = (int)(strBreak - str);
		if( len < 1 ){	str++; continue;	}
		strncpy( line, str, len );	line[ len ] = 0;

		sprintf(type, "NONE");
		if(line[0]=='\n' || line[0]=='#' || sscanf(line, "%d %s %s %d:", &num, name, type, &cnt)==0 )continue;

		data = _ChPlainData_getNextColon(line);
		if(strcmp(type, "INT")==0){
			ChPlainData_intNode *newIntNode = new ChPlainData_intNode();
			newIntNode->scanString(num, name, cnt, data);
			insertNode(newIntNode);
		}else if(strcmp(type, "FLOAT")==0){
			ChPlainData_floatNode *newFloatNode = new ChPlainData_floatNode();
			newFloatNode->scanString(num, name, cnt, data);
			insertNode(newFloatNode);
		}else if(strcmp(type, "CHAR")==0){
			ChPlainData_charNode *newCharNode = new ChPlainData_charNode();
			newCharNode->scanString(num, name, cnt, data);
			insertNode(newCharNode);
		}

		str = ++strBreak;
	}
	return true;
}
