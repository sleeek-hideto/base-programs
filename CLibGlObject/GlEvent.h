#pragma once

/*!
	@brief スクリプトを記述する
*/
class CGlEventScript
{
public:
	static const int IND_INT = -2;
	static const int IND_FLOAT = -3;
	static const int IND_CHAR = -4;
protected:
	enum _TypeOrder{
		NOP,
		MOVE, ADD, SUB,
		CMPGE, CMPG, CMPLE, CMPL, CMPE, CMPNE,
		JMPT, JMPF,
		SIN, COS, TAN, SQRT,
		RETURN
	};
	enum _TypeData{
		PARAM, ///// ストレージ
		EAX, ///// レジスタ
		INT, FLOAT, CHAR
	};

	typedef struct _DataCell {
		_TypeData type;
		union _DataUnion {
			int Int;
			float Float;
			char Str[ 32 ];
		} data;
	} DataCell;

	typedef struct _EventOrder{
		_TypeOrder order;
		DataCell cell[ 2 ];
	} EventOrder;

	const static int ORDER_MAX = 64;
	EventOrder *m_Script;//!<スクリプトへのポインタ
	int m_ScriptCnt;//!<スクリプトの総数

	bool CmpE( DataCell *cella, DataCell *cellb );
	bool CmpG( DataCell *cella, DataCell *cellb );
	bool CmpGE( DataCell *cella, DataCell *cellb );
	bool RetrieveParam( Ch3PlainData *data, DataCell *incell, DataCell *outcell );
	bool Move( Ch3PlainData *data, DataCell *cella, DataCell *cellb, DataCell *outcell );

	void SetCell( DataCell *cell, int val ){ cell->type=INT; cell->data.Int=val; }
	void SetCell( DataCell *cell, char *val ){ cell->type=CHAR; strncpy( cell->data.Str, val, 32 ); }
	void SetCell( DataCell *cell, float val ){ cell->type=FLOAT; cell->data.Float=val; }
	void SetCellParam( DataCell *cell, char *name ){ cell->type=PARAM; strncpy( cell->data.Str, name, 32 ); }

public:
	CGlEventScript(){
		m_Script = new EventOrder[ ORDER_MAX ];
		for( int l=0; l<ORDER_MAX; l++ ){ m_Script[l].order=RETURN; }
		m_ScriptCnt = 0;
	}
	~CGlEventScript(){
		if( m_Script != NULL ){ delete m_Script; m_Script=NULL; }
	}

	bool Run( Ch3PlainData *data );

	bool PlainCompiler( Ch3PlainData *data );
	bool AddOrder_GoTo( int frameEntry, int frameMax );
	bool AddOrder_GotoWaiting( int frameGoto, int frameTarget, char *param );
	bool AddOrder_JumpFrame_ParamE( int frame, char *nameParam, int value );
	bool AddOrder_JumpFrame_ParamNE( int frame, char *nameParam, int value );
	//bool AddOrder_Return_ParamE( int frame, char *nameParam, int value );
	//bool AddOrder_Return_ParamNE( int frame, char *nameParam, int value );
};

class CGlEventCard
{
protected:
	enum _TypeMotion{
		POS, ADD_POS,
		ROT,
		ANGLE, ADD_ANGLE
	};

	int m_FrameMax, m_FrameEntry, m_FrameOut, m_FrameNow;
	Ch3PlainData m_StorageFrame;//!< [ frame ]{ name/part/type } = data;

	CGlEventScript *m_Script;
	void CreateScript(){ if(m_Script==NULL)m_Script=new CGlEventScript(); }

public:
	CGlEventCard(void){
		m_FrameMax = 1;
		m_FrameEntry = 0;
		m_FrameOut = 1;
		m_FrameNow = 0;

		m_Script = NULL;
	}
	~CGlEventCard(void){
		if( m_Script != NULL ){ delete m_Script; m_Script = NULL; }
	}

	void ResetFrame(){ m_FrameNow = m_FrameEntry; }
	void SetFrame( int frame ){ m_FrameNow = frame % m_FrameMax; }
	int GetFrame(){ return m_FrameNow; }
	int AddFrame( int frame=1 ){ m_FrameNow += frame; return m_FrameNow; }
	Ch3PlainData *GetDataFrame(){ return &m_StorageFrame; }

	bool LoadCard( char *loc );
	bool SetPosture( int frame, char *name, char *part, _TypeMotion motion, double *data );

	//bool CallSignal( char *nameParam, char *val );
	//bool CallSignal( char *nameParam, int val );
	//bool CallSignal( char *nameParam, float val );
	void Run( char *message );
	//bool GetPosture( char *name, int part, TypeMotion *type, double *dataOut );
	//bool ForeachStart_Posture();
	//bool ForeachGet_Posture( char *name, int part, _TypeMotion *type, double *dataOut );
};

class CGlEventManager
{
protected:
	const static int CARD_MAX = 3;
	int m_CardIndNow;
	CGlEventCard *m_Card[ CARD_MAX ];
	int CardNext(){
		m_Card[ m_CardIndNow ]=NULL;
		m_CardIndNow++; if( m_CardIndNow>=CARD_MAX ) m_CardIndNow=0;
		return m_CardIndNow;
	}
	CGlEventCard *CardNow(){ return m_Card[ m_CardIndNow ]; }

	char m_NameObject[ 32 ];
	IGlObjectDraw *m_Object;

public:
	CGlEventManager(){
		m_CardIndNow = 0;
		for( int c=0; c<CARD_MAX; c++ ){ m_Card[c]=NULL; }

		m_NameObject[ 0 ] = 0;
		m_Object = NULL;
	};
	~CGlEventManager(){
		//for( int c=0; c<CARD_MAX; c++ ){
		//	if( m_Card[c]!=NULL ){ delete m_Card[c]; m_Card[c]=NULL; }
		//}
	};

	void SetObject( IGlObjectDraw *obj ){ m_Object=obj; }

	//bool LoadCard( char *loc );
	bool SetCard( CGlEventCard *card );
	bool ResetCard( CGlEventCard *card=NULL );

	void SetFrame( int frame ){ if(CardNow()==NULL)return; CardNow()->SetFrame(frame); }
	int GetFrame(){ if( CardNow()==NULL )return -1; return CardNow()->GetFrame(); }
	char *GetNameObject(){ if( m_NameObject[ 0 ]==0 )return NULL; return m_NameObject; }
	Ch3PlainData *GetDataFrameNow(){ if(CardNow()==NULL)return NULL; return CardNow()->GetDataFrame(); }

	int Run();

	bool CallSignal( char *param, char *val );
	bool CallSignal( char *param, int val );
	bool CallSignal( char *param, double val );
	bool CheckSignal( char *param, char *val );
	bool CheckSignal( char *param, int &val );
	bool CheckSignal( char *param, double &val );
};
