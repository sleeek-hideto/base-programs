#pragma once
#include "glbone.h"

class CGl2Bone : public CGlBone
{
public:
	CGl2Bone(void){
	}
	~CGl2Bone(void){
	}

	void AddBoneSibling( CGl2Bone *bone ){
		bone->m_BoneParent = m_BoneParent;
		bone->m_BoneSibling = m_BoneSibling;
		m_BoneSibling = bone;
	}
	void AddBoneChild( CGl2Bone *bone ){
		if( m_BoneChild == NULL ){
			bone->m_BoneParent = this;
			m_BoneChild = bone;
		}else{
			m_BoneChild->AddBoneSibling( bone );
		}
	}

	inline CGl2Bone* GetBoneChild(){
		return (CGl2Bone*)m_BoneChild;
	}
	inline CGl2Bone* GetBoneSibling(){
		return (CGl2Bone*)m_BoneSibling;
	}
	inline CGl2Skin* GetSkin(){
		return (CGl2Skin*)m_Skin;
	}

	void CalcPosture();
	void Draw_recursive( char *type=NULL );

	/*!
		@brief ���[�J�����W���i�[�����X�L���ƁA���[���h���W���i�[�����X�L������������
		@param[in] skinLocal ���[�J�����W���i�[�����X�L��
		@param[in] skinWorld ���[���h���W���i�[�����X�L��
	*/
	inline void AddSkin( CGl2Skin *skinLocal, CGl2Skin *skinDeformed=NULL, CGl2Skin *skinWorld=NULL ){
		CGlBone::AddSkin( skinLocal );
		if( skinDeformed != NULL )skinLocal->SetSkinDeformed( skinDeformed );
		if( skinWorld != NULL )skinLocal->SetSkinWorld( skinWorld );
	}
};
