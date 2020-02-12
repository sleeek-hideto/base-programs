#include "StdAfx.h"
#include "GlEvent.h"

bool CGlEventScript::CmpE( DataCell *cella, DataCell *cellb )
{
	if( cella->type != cellb->type )return false;
	if( cella->type == PARAM )return false;
	
	if( cella->type == INT ){
		if( cella->data.Int != cellb->data.Int )return false;
	}else if( cella->type == FLOAT ){
		if( cella->data.Float != cellb->data.Float )return false;
	}else if( cella->type == CHAR ){
		if( strcmp( cella->data.Str, cellb->data.Str ) != 0 )return false;
	}
	return true;
}

bool CGlEventScript::CmpG( DataCell *cella, DataCell *cellb )
{
	if( cella->type != cellb->type )return false;
	if( cella->type == PARAM )return false;
	
	if( cella->type == INT ){
		if( cella->data.Int >= cellb->data.Int )return false;
	}else if( cella->type == FLOAT ){
		if( cella->data.Float >= cellb->data.Float )return false;
	}else if( cella->type == CHAR ){
		if( strcmp( cella->data.Str, cellb->data.Str ) >= 0 )return false;
	}
	return true;
}

bool CGlEventScript::CmpGE( DataCell *cella, DataCell *cellb )
{
	if( cella->type != cellb->type )return false;
	if( cella->type == PARAM )return false;
	
	if( cella->type == INT ){
		if( cella->data.Int > cellb->data.Int )return false;
	}else if( cella->type == FLOAT ){
		if( cella->data.Float > cellb->data.Float )return false;
	}else if( cella->type == CHAR ){
		if( strcmp( cella->data.Str, cellb->data.Str ) > 0 )return false;
	}
	return true;
}

bool CGlEventScript::RetrieveParam( Ch3PlainData *data, DataCell *incell, DataCell *outcell )
{
	if( incell->type != PARAM ){
		memcpy( outcell, incell, sizeof(DataCell) );
		return true;
	}
	char *name = incell->data.Str;
	if( name[ 0 ] != '$' )return false;

	if( name[ 1 ] == '_' )///// 内部変数
	{
		if( strcmp( name, "$_FrameNow" ) == 0 ){ outcell->type=INT; data->Get( IND_INT, "$_FrameNow", &outcell->data.Int ); }
		if( strcmp( name, "$_FrameMax" ) == 0 ){ outcell->type=INT; data->Get( IND_INT, "$_FrameMax", &outcell->data.Int ); }
		return true;
	}

	if( data->Get( IND_INT, name, &outcell->data.Int ) > 0 ){
		outcell->type = INT;
	}
	if( data->Get( IND_FLOAT, name, &outcell->data.Float ) > 0 ){
		outcell->type = FLOAT;
	}
	if( data->Get( IND_CHAR, name, outcell->data.Str ) > 0 ){
		outcell->type = CHAR;
	}

	return true;
}

/*!
	@brief データをコピーする
*/
bool CGlEventScript::Move( Ch3PlainData *data, DataCell *cella, DataCell *cellb, DataCell *outcell )
{
	if( cella->type == PARAM && cellb->type == PARAM )
	{
		DataCell tmpCell;
		if( data->Get( IND_INT, cellb->data.Str, &tmpCell.data.Int ) > 0 ){
			data->Set( IND_INT, cella->data.Str, 1, &tmpCell.data.Int );
		}
		if( data->Get( IND_FLOAT, cellb->data.Str, &tmpCell.data.Float ) > 0 ){
			data->Set( IND_FLOAT, cella->data.Str, 1, &tmpCell.data.Float );
		}
		if( data->Get( IND_CHAR, cellb->data.Str, tmpCell.data.Str ) > 0 ){
			data->Set( IND_CHAR, cella->data.Str, tmpCell.data.Str );
		}
		return true;
	}

	if( cellb->type == PARAM )
	{
		outcell->type = cella->type;
		switch( outcell->type )
		{
		case INT: data->Get( IND_INT, cellb->data.Str, &outcell->data.Int ); break;
		case FLOAT: data->Get( IND_FLOAT, cellb->data.Str, &outcell->data.Float ); break;
		case CHAR: data->Get( IND_CHAR, cellb->data.Str, outcell->data.Str ); break;
		}
		return true;
	}

	if( cella->type == PARAM )
	{
		outcell->type = cellb->type;
		switch( outcell->type )
		{
		case INT: data->Set( IND_INT, cella->data.Str, 1, &cellb->data.Int ); break;
		case FLOAT: data->Set( IND_FLOAT, cella->data.Str, 1, &cellb->data.Float ); break;
		case CHAR: data->Set( IND_CHAR, cella->data.Str, cellb->data.Str ); break;
		}
		return true;
	}

	memcpy( outcell, cellb, sizeof(DataCell) );
	return true;
}

/*!
	@brief スクリプトを実行する
*/
bool CGlEventScript::Run( Ch3PlainData *data )
{
	data->SetNameSpace( "Script" );

	bool fragCmp = false;
	DataCell eax, ebx;

	for( int l=0; l<m_ScriptCnt; l++ )
	{
		switch( m_Script[ l ].order )
		{
		case NOP:
			continue;
		case RETURN:
			return true;

		case CMPE:
			RetrieveParam( data, m_Script[ l ].cell, &eax );	RetrieveParam( data, m_Script[ l ].cell +1, &ebx );
			fragCmp = CmpE( &eax, &ebx );
			break;
		case CMPNE:
			RetrieveParam( data, m_Script[ l ].cell, &eax );	RetrieveParam( data, m_Script[ l ].cell +1, &ebx );
			fragCmp = !CmpE( &eax, &ebx );
			break;
		case CMPG:
			RetrieveParam( data, m_Script[ l ].cell, &eax );	RetrieveParam( data, m_Script[ l ].cell +1, &ebx );
			fragCmp = CmpG( &eax, &ebx );
			break;
		case CMPGE:
			RetrieveParam( data, m_Script[ l ].cell, &eax );	RetrieveParam( data, m_Script[ l ].cell +1, &ebx );
			fragCmp = CmpGE( &eax, &ebx );
			break;
		case CMPL:
			RetrieveParam( data, m_Script[ l ].cell, &eax );	RetrieveParam( data, m_Script[ l ].cell +1, &ebx );
			fragCmp = CmpG( &ebx, &eax );
			break;
		case CMPLE:
			RetrieveParam( data, m_Script[ l ].cell, &eax );	RetrieveParam( data, m_Script[ l ].cell +1, &ebx );
			fragCmp = CmpGE( &ebx, &eax );
			break;

		case JMPT:
			if( fragCmp ){ l=m_Script[ l ].cell[ 0 ].data.Int; }
			break;
		case JMPF:
			if( !fragCmp ){ l=m_Script[ l ].cell[ 0 ].data.Int; }
			break;

		case MOVE:
			Move( data, m_Script[ l ].cell, m_Script[ l ].cell +1, &eax );
		}
	}
	return true;
}

bool CGlEventScript::PlainCompiler( Ch3PlainData *data )
{
	if( data == NULL )return false;
	data->SetNameSpace( "Script" );
	int scriptMax = 0;
	data->Get( -1, "scriptMax", &scriptMax );

	int lenNow = 0;
	for( int s=0; s<scriptMax; s++ )
	{
		int lenIf;
		lenIf = 0;
		int valIf;
		if( data->Get( s, "IfE", &valIf ) > 0 ){
			m_Script[ lenNow ].order = CMPE;
		}else if( data->Get( s, "IfNe", &valIf ) > 0 ){
			m_Script[ lenNow ].order = CMPNE;
		}else if( data->Get( s, "IfG", &valIf ) > 0 ){
			m_Script[ lenNow ].order = CMPG;
		}else if( data->Get( s, "IfGe", &valIf ) > 0 ){
			m_Script[ lenNow ].order = CMPGE;
		}else if( data->Get( s, "IfL", &valIf ) > 0 ){
			m_Script[ lenNow ].order = CMPL;
		}else if( data->Get( s, "IfLe", &valIf ) > 0 ){
			m_Script[ lenNow ].order = CMPLE;
		}else{
			continue;
		}
		char nameParam[ 64 ];
		if( data->Get( s, "IfParam", nameParam ) > 0 ){
			SetCell( m_Script[ lenNow ].cell +0, valIf );
			SetCellParam( m_Script[ lenNow ].cell +1, nameParam );

			/////初期化、注意
			int valInit = 0;
			data->Set( IND_INT, nameParam, 1, &valInit );
		}
		lenIf++;
		///// jump
		lenIf++;
		int frameGoto;
		bool yesorno;
		if( data->Get( s, "ActGotoFrame", &frameGoto ) > 0 ){
			m_Script[ lenNow +lenIf ].order = MOVE;
			SetCellParam( m_Script[ lenNow +lenIf ].cell +0, "$_FrameNow" );
			SetCell( m_Script[ lenNow +lenIf ].cell +1, frameGoto );
			lenIf++;
		}
		if( data->Get( s, "ActClearParam", nameParam ) > 0 ){
			m_Script[ lenNow +lenIf ].order = MOVE;
			SetCellParam( m_Script[ lenNow +lenIf ].cell +0, nameParam );
			SetCell( m_Script[ lenNow +lenIf ].cell +1, 0 );
			lenIf++;
		}
		if( data->Get( s, "ActReturn", &yesorno ) > 0 ){
			m_Script[ lenNow +lenIf ].order = RETURN;
			lenIf++;
		}

		m_Script[ lenNow +1 ].order = JMPF;
		SetCell( m_Script[ lenNow +1 ].cell +0, lenNow +lenIf -1 );

		lenNow += lenIf;
	}
	m_ScriptCnt = lenNow;

	data->SetNameSpace( "" );
	return true;
}

bool CGlEventScript::AddOrder_GoTo( int frameEntry, int frameMax )
{
	if( m_ScriptCnt +3 > ORDER_MAX )return false;
	EventOrder *orderNow = m_Script +m_ScriptCnt;
	orderNow[ 0 ].order = CMPG;
	SetCell( orderNow[ 0 ].cell +0, frameMax );
	SetCellParam( orderNow[ 0 ].cell +1, "$_FrameNow" );
	orderNow[ 1 ].order = JMPF;
	SetCell( orderNow[ 1 ].cell +0, 3 );
	orderNow[ 2 ].order = MOVE;
	SetCellParam( orderNow[ 2 ].cell +0, "$_FrameNow" );
	SetCell( orderNow[ 2 ].cell +1, frameEntry );
	m_ScriptCnt += 3;
	return true;
}

bool CGlEventScript::AddOrder_GotoWaiting( int frameGoto, int frameTarget, char *param )
{
	if( m_ScriptCnt +5 > ORDER_MAX )return false;
	EventOrder *orderNow = m_Script +m_ScriptCnt;
	orderNow[ 0 ].order = CMPGE;
	SetCell( orderNow[ 0 ].cell +0, 1 );
	SetCellParam( orderNow[ 0 ].cell +1, param );
	orderNow[ 1 ].order = JMPT;
	SetCell( orderNow[ 1 ].cell +0, 5 );
	orderNow[ 2 ].order = CMPGE;
	SetCellParam( orderNow[ 2 ].cell +0, "$_FrameNow" );
	SetCell( orderNow[ 2 ].cell +1, frameTarget );
	orderNow[ 3 ].order = JMPT;
	SetCell( orderNow[ 3 ].cell +0, 5 );

	orderNow[ 4 ].order = MOVE;
	SetCellParam( orderNow[ 4 ].cell +0, "$_FrameNow" );
	SetCell( orderNow[ 4 ].cell +1, frameGoto );
	m_ScriptCnt += 5;
	return true;
}

bool CGlEventScript::AddOrder_JumpFrame_ParamE( int frame, char *nameParam, int value )
{
	if( m_ScriptCnt +3 > ORDER_MAX )return false;
	EventOrder *orderNow = m_Script +m_ScriptCnt;
	orderNow[ 0 ].order = CMPE;
	orderNow[ 0 ].cell[ 0 ].type = PARAM; strncpy( orderNow[ 0 ].cell[ 0 ].data.Str, nameParam, 32 );
	orderNow[ 0 ].cell[ 1 ].type = INT; orderNow[ 0 ].cell[ 1 ].data.Int = value;
	orderNow[ 1 ].order = JMPF;
	orderNow[ 1 ].cell[ 0 ].type = INT; orderNow[ 1 ].cell[ 0 ].data.Int = m_ScriptCnt +3;
	orderNow[ 2 ].order = MOVE;
	orderNow[ 2 ].cell[ 0 ].type = PARAM; strncpy( orderNow[ 2 ].cell[ 0 ].data.Str, "$_FrameNow", 32 );
	orderNow[ 2 ].cell[ 1 ].type = INT; orderNow[ 2 ].cell[ 1 ].data.Int = frame;
	m_ScriptCnt += 3;
	return true;
}
bool CGlEventScript::AddOrder_JumpFrame_ParamNE( int frame, char *nameParam, int value )
{
	if( m_ScriptCnt +3 > ORDER_MAX )return false;
	EventOrder *orderNow = m_Script +m_ScriptCnt;
	orderNow[ 0 ].order = CMPNE;
	orderNow[ 0 ].cell[ 0 ].type = PARAM; strncpy( orderNow[ 0 ].cell[ 0 ].data.Str, nameParam, 32 );
	orderNow[ 0 ].cell[ 1 ].type = INT; orderNow[ 0 ].cell[ 1 ].data.Int = value;
	orderNow[ 1 ].order = JMPF;
	orderNow[ 1 ].cell[ 0 ].type = INT; orderNow[ 1 ].cell[ 0 ].data.Int = m_ScriptCnt +3;
	orderNow[ 2 ].order = MOVE;
	orderNow[ 2 ].cell[ 0 ].type = PARAM; strncpy( orderNow[ 2 ].cell[ 0 ].data.Str, "$_FrameNow", 32 );
	orderNow[ 2 ].cell[ 1 ].type = INT; orderNow[ 2 ].cell[ 1 ].data.Int = frame;
	m_ScriptCnt += 3;
	return true;
}
//bool CGlEventScript::AddOrder_Return_ParamE( int frame, char *nameParam, int value )
//{
//}
//bool CGlEventScript::AddOrder_Return_ParamNE( int frame, char *nameParam, int value )
//{
//}

/////
///// ///// CGlEventCard ///// /////
/////
bool CGlEventCard::LoadCard( char *loc )
{
	if( !m_StorageFrame.Load( loc ) )return false;

	m_StorageFrame.SetNameSpace( "Frame" );
	if( m_StorageFrame.Get( -1, "FrameEntry", &m_FrameEntry ) > 0 ){
		m_FrameNow = m_FrameEntry;
	}
	m_StorageFrame.Get( -1, "FrameOut", &m_FrameOut );
	if( m_StorageFrame.Get( -1, "FrameMax", &m_FrameMax ) == 0 )return false;

	int scriptMax = 0;
	m_StorageFrame.SetNameSpace( "Script" );
	if( m_StorageFrame.Get( -1, "scriptMax", &scriptMax ) > 0 && scriptMax > 0 )
	{
		if( m_Script == NULL )m_Script = new CGlEventScript();

		m_Script->PlainCompiler( &m_StorageFrame );
	}
	return true;
}

bool CGlEventCard::SetPosture( int frame, char *name, char *part, _TypeMotion motion, double *data )
{
	if( frame < 0 )return false;
	int cnt;
	char strMotion[ 8 ], str[ 32 ];
	switch( motion )
	{
	case POS: sprintf( strMotion, "Pos" ); cnt=3; break;
	case ADD_POS: sprintf( strMotion, "AddPos" ); cnt=3; break;
	case ANGLE: sprintf( strMotion, "Angle" ); cnt=3; break;
	case ADD_ANGLE: sprintf( strMotion, "AddAngle" ); cnt=3; break;
	case ROT: sprintf( strMotion, "Rot" ); cnt=16; break;
	}

	if( name != NULL ){
		sprintf( str, "%s/%s/%s", name, part, strMotion );
	}else{
		sprintf( str, "/%s/%s", part, strMotion );
	}

	m_StorageFrame.SetNameSpace( "Frame" );
	m_StorageFrame.Set( frame, str, cnt, data );
	return true;
}

//bool CGlEventCard::CallSignal( char *nameParam, char *val )
//{
//	m_StorageFrame.SetNameSpace( "Script" );
//	m_StorageFrame.Set( CGlEventScript::IND_CHAR, nameParam, val );
//	return true;
//}
//bool CGlEventCard::CallSignal( char *nameParam, int val )
//{
//	m_StorageFrame.SetNameSpace( "Script" );
//	m_StorageFrame.Set( CGlEventScript::IND_INT, nameParam, 1, &val );
//	return true;
//}
//bool CGlEventCard::CallSignal( char *nameParam, float val )
//{
//	m_StorageFrame.SetNameSpace( "Script" );
//	m_StorageFrame.Set( CGlEventScript::IND_FLOAT, nameParam, 1, &val );
//	return true;
//}

void CGlEventCard::Run( char *message )
{
	message[ 0 ] = 0;

	if( m_Script != NULL )
	{
		m_StorageFrame.SetNameSpace( "Script" );
		m_StorageFrame.Set( CGlEventScript::IND_INT, "$_FrameNow", 1, &m_FrameNow );
		m_StorageFrame.Set( CGlEventScript::IND_INT, "$_FrameEntry", 1, &m_FrameEntry );
		m_StorageFrame.Set( CGlEventScript::IND_INT, "$_FrameMax", 1, &m_FrameMax );
		m_Script->Run( &m_StorageFrame );
		m_StorageFrame.Get( CGlEventScript::IND_INT, "$_FrameNow", &m_FrameNow );
	}
	
	if( m_FrameNow >= m_FrameOut ){
		sprintf( message, "Out" );
	}
	if( m_FrameNow >= m_FrameMax ){
		sprintf( message, "End" );
	}
}

///// ///// Manager ///// /////
/*
	@brief イベント・カードを読み込む
*/
//bool CGlEventManager::LoadCard( char *loc )
//{
//	for( int c=m_CardIndNow; c<CARD_MAX; c++ )
//	{
//		if( m_Card[ c ] == NULL ){
//			m_Card[ c ] = new CGlEventCard();
//			return m_Card[ c ]->LoadCard( loc );
//		}
//	}
//	for( int c=0; c<m_CardIndNow; c++ )
//	{
//		if( m_Card[ c ] == NULL ){
//			m_Card[ c ] = new CGlEventCard();
//			return m_Card[ c ]->LoadCard( loc );
//		}
//	}
//	return false;
//}

/*
	@brief イベント・カードを追加して貼り付ける
*/
bool CGlEventManager::SetCard( CGlEventCard *card )
{
	for( int c=m_CardIndNow; c<CARD_MAX; c++ )
	{
		if( m_Card[ c ] == NULL ){
			m_Card[ c ] = card;
			return true;
		}
	}
	for( int c=0; c<m_CardIndNow; c++ )
	{
		if( m_Card[ c ] == NULL ){
			m_Card[ c ] = card;
			return true;
		}
	}
	return false;
}

/*
	@brief イベント・カードを新しく貼り付ける
*/
bool CGlEventManager::ResetCard( CGlEventCard *card )
{
	for( int c=0; c<CARD_MAX; c++ ){ m_Card[ c ] = NULL; }

	m_CardIndNow = 0;
	if( card != NULL ){
		m_Card[ m_CardIndNow ] = card;
		card->ResetFrame();
	}

	return true;
}

int CGlEventManager::Run()
{	/////まずスクリプトを実行し、その次に姿勢を読み込む、最後にFrameを一つ増やす
	char message[ 64 ];
	if( CardNow() == NULL || m_Object == NULL )return false;

	CardNow()->Run( message );
	if( strcmp( message, "End" ) == 0 )CardNext();
	if( m_Object == NULL || CardNow() == NULL )return -1;

	//printf( "event frame %d\n", CardNow()->GetFrame() );
	m_Object->LoadPosture( CardNow()->GetDataFrame(), CardNow()->GetFrame(), GetNameObject() );

	return CardNow()->AddFrame();
}

bool CGlEventManager::CallSignal( char *param, char *val )
{
	if( CardNow()==NULL ) return false;
	Ch3PlainData *data = CardNow()->GetDataFrame();
	data->SetNameSpace( "Script" );
	data->Set( CGlEventScript::IND_CHAR, param, val );
	return true;
}
bool CGlEventManager::CallSignal( char *param, int val )
{
	if( CardNow()==NULL ) return false;
	Ch3PlainData *data = CardNow()->GetDataFrame();
	data->SetNameSpace( "Script" );
	data->Set( CGlEventScript::IND_INT, param, 1, &val );
	return true;
}
bool CGlEventManager::CallSignal( char *param, double val )
{
	if( CardNow()==NULL ) return false;
	Ch3PlainData *data = CardNow()->GetDataFrame();
	data->SetNameSpace( "Script" );
	data->Set( CGlEventScript::IND_FLOAT, param, 1, &val );
	return true;
}

bool CGlEventManager::CheckSignal( char *param, char *val )
{
	if( CardNow()==NULL ) return false;
	Ch3PlainData *data = CardNow()->GetDataFrame();
	data->SetNameSpace( "Script" );
	if( data->Get( CGlEventScript::IND_CHAR, param, val ) == 0 ) return false;
	return true;
}
bool CGlEventManager::CheckSignal( char *param, int &val )
{
	if( CardNow()==NULL ) return false;
	Ch3PlainData *data = CardNow()->GetDataFrame();
	data->SetNameSpace( "Script" );
	if( data->Get( CGlEventScript::IND_INT, param, &val ) == 0 ) return false;
	return true;
}
bool CGlEventManager::CheckSignal( char *param, double &val )
{
	if( CardNow()==NULL ) return false;
	Ch3PlainData *data = CardNow()->GetDataFrame();
	data->SetNameSpace( "Script" );
	if( data->Get( CGlEventScript::IND_FLOAT, param, &val ) == 0 ) return false;
	return true;
}
