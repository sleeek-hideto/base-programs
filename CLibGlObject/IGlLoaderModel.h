#pragma once

/*
	@brief ���f�������[�h���邽�߂̒��ۃN���X
*/
class IGlLoaderModel
{
protected:
	int m_nAmtSkins;			//!<�X�L���̌�
	//int *m_TableHeadVer;	//!<�X�̃X�L���̒��_�z��̐擪�ւ̔Ԗ�
	//int *m_TableHeadInd;	//!<�X�̃X�L���̃C���f�b�N�X�z��̐擪�ւ̔Ԗ�
	//int *m_TableCntVer;		//!<�X�̃X�L���̒��_��
	//int *m_TableCntInd;		//!<�X�̃X�L���̃C���f�b�N�X��

	char m_DirectoryNow[ 128 ];

	CArrayVector3H *m_ArrPos;		//!<���_�ʒu�z��
	CUintArrayVector3 *m_ArrInd;	//!<�C���f�b�N�X�z��
	CArrayVector3H *m_ArrNrm;		//!<�@���z��
	CArrayVector3H *m_ArrTex;		//!<�e�N�X�`�����W�z��

	Ch2PlainData m_DataSkin;		//!<�O���[�v���˃X�L���ԍ��֕ϊ�����e�[�u���B����ɁA-1��Grp2Ind�̕ϊ��e�[�u��
	Ch2PlainData m_DataMaterial;	//!<�X�L���ԍ�����F�E�e�N�X�`���Ɋւ���f�[�^��ێ��B����ɁA-1��Mtl2Ind�̕ϊ��e�[�u��

	bool m_EnableDebug;
	//bool m_EnableReindexing;

public:
	IGlLoaderModel(void){
		m_nAmtSkins = 0;
		//m_TableHeadVer = NULL;
		//m_TableHeadInd = NULL;
		//m_TableCntVer = NULL;
		//m_TableCntInd = NULL;

		m_DirectoryNow[ 0 ] = 0;
		//m_EnableReindexing = false;

		m_ArrPos = NULL;
		m_ArrInd = NULL;
		m_ArrNrm = NULL;
		m_ArrTex = NULL;

		m_EnableDebug = true;
	}
	~IGlLoaderModel(void){
		//if( m_TableHeadVer != NULL )delete [] m_TableHeadVer;
		//if( m_TableHeadInd != NULL )delete [] m_TableHeadInd;
		//if( m_TableCntVer != NULL )delete [] m_TableCntVer;
		//if( m_TableCntInd != NULL )delete [] m_TableCntInd;

		//if( m_ArrPos != NULL )delete m_ArrPos;
		//if( m_ArrInd != NULL )delete m_ArrInd;
		//if( m_ArrNrm != NULL )delete m_ArrNrm;
		//if( m_ArrTex != NULL )delete m_ArrTex;
	}

	virtual void Destroy()=0;

	int GetAmtSkins(){ return m_nAmtSkins; }
	//virtual int GetAmtSkinsVer()=0;
	//virtual int GetAmtSkinsInd()=0;

	///// �����񏈗��֐�
	char *ShiftNextNoSpace( char *str ){
		while(str[0]!=' ' && str[0]!='\t' && str[0]!='\n' && str[0]!=0)str++;
		while(str[0]==' ' || str[0]=='\t')str++;
		return str;
	}

	//!<�K�w�A�N���X���A���I���O���l���A�܂��ǂݍ��񂾖��O�̐���Ԃ�
	int ScanFile( FILE *file, int &floorNow, char *nameClass, char *namePrivate ){
		char line[ 1024 ];
		nameClass[ 0 ] = 0;	namePrivate[ 0 ] = 0;
		if( fgets( line, 1024, file ) == NULL )return -1;
		if( line[ 0 ] == '\n' )return 0;
		char *p = line;
		while( *p != 0 ){		if( *p == '{' )floorNow++; if( *p == '}' )floorNow--; p++;		}
		p = line; while( *p == ' ' || *p == '\t' )p++;
		return sscanf( p, "%s %s", nameClass, namePrivate );
	}
	int ScanFile( FILE *file, int &floorNow, int &num ){
		char line[ 1024 ];
		if( fgets( line, 1024, file ) == NULL )return -1;
		if( line[ 0 ] == '\n' )return 0;
		char *p = line;
		while( *p != 0 ){		if( *p == '{' )floorNow++; if( *p == '}' )floorNow--; p++;		}
		p = line; while( *p == ' ' || *p == '\t' )p++;
		return sscanf( line, "%d", &num );
	}
	void Trim( char *str ){
		int len = (int)strlen( str );
		if( str[ len -1 ] == '\n' )str[ len -1 ] = 0;
	}
	char *PickupString( char *p ){
		char *pStart, *pEnd;
		while( *p != '"' ){		if( *p == 0 )return NULL; p++;		}
		pStart = ++p;
		while( *p != '"' ){		if( *p == 0 )return NULL; p++;		}
		pEnd = p;

		*pEnd = 0;	return pStart;
	}

	///// ���O����C���f�b�N�X�ւ̕ϊ��e�[�u��
	void SetGrp2Index( char *name, int ind ){
		m_DataSkin.set( -1, name, 1, &ind );
	}
	int Grp2Index( char *name ){
		int ind = -1;
		if( m_DataSkin.get( -1, name, &ind ) == 0 )return -1;
		return ind;
	}
	void SetMtl2Index( char *mtl, int ind ){
		m_DataMaterial.set( -1, mtl, 1, &ind );
	}
	int Mtl2Index( char *name ){
		int ind = -1;
		if( m_DataMaterial.get( -1, name, &ind ) == 0 )return -1;
		return ind;
	}

	virtual int GetCntSkinPos( int skin )=0;
	virtual int GetCntSkinInd( int skin )=0;
	virtual int GetCntSkinNrm( int skin )=0;
	virtual int GetCntSkinTex( int skin )=0;

	virtual double *GetDataSkinPos( int skin )=0;
	virtual unsigned int *GetDataSkinInd( int skin )=0;
	virtual double *GetDataSkinNrm( int skin )=0;
	virtual double *GetDataSkinTex( int skin )=0;

	bool GetSkinColAmbi( int skin, double *o ){
		if( m_DataMaterial.get( skin, "colAmbi", o ) < 4 )return false;
		return true;
	}
	bool GetSkinColAmbi( int skin, CVector4 *o ){
		if( m_DataMaterial.get( skin, "colAmbi", o->GetData() ) < 4 )return false;
		return true;
	}
	bool GetSkinColDiff( int skin, double *o ){
		if( m_DataMaterial.get( skin, "colDiff", o ) < 4 )return false;
		return true;
	}
	bool GetSkinColDiff( int skin, CVector4 *o ){
		if( m_DataMaterial.get( skin, "colDiff", o->GetData() ) < 4 )return false;
		return true;
	}
	bool GetSkinColSpec( int skin, double *o ){
		if( m_DataMaterial.get( skin, "colSpec", o ) < 4 )return false;
		return true;
	}
	bool GetSkinColSpec( int skin, CVector4 *o ){
		if( m_DataMaterial.get( skin, "colSpec", o->GetData() ) < 4 )return false;
		return true;
	}

	bool GetSkinLocTex( int skin, char *o ){
		if( m_DataMaterial.get( skin, "locTex", o ) == 0 )return false;
		return true;
	}

	virtual bool MakeDataPolygon( int ind, CGlSkin *skin )=0;

public:
	virtual bool Load( char *loc )=0;
	virtual bool MakeSkin( int index, CGlSkin *skin );
	virtual bool MakeSkin( char *name, CGlSkin *skin );
};
