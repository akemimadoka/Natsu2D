////////////////////////////////////////////////////////////////////////////////
///	@file	n2dObjLoader.h
///	@brief	Objģ�ͼ���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <natType.h>
#include <natVec.h>
#include "..\n2dModel.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
///	@brief	Obj������
////////////////////////////////////////////////////////////////////////////////
class n2dObjLoader final
	: public natRefObjImpl<n2dModelLoader>
{
public:
	n2dObjLoader();

	///	@brief	����ʱ���ع��캯��
	///	@note	����ʧ�ܻ��׳��쳣
	///	@param[in]	filename	�ļ���
	explicit n2dObjLoader(ncTStr filename);
	~n2dObjLoader();

	///	@brief	����Objģ��
	///	@param[in]	filename	�ļ���
	nResult LoadFromFile(ncTStr lpPath) override;
	nResult LoadFromStream(natStream* pStream) override;

	void SetDefaultTexture(n2dTexture2D* Texture) override;

	n2dModelData* GetModel() override;

	///	@brief	��ö��㻺��
	///	@note	��δ���������㻺�潫�ᴴ��һ��������ֱ�ӷ����Ѵ����Ķ��㻺�棬���໺����Ϊ��ͬ
	nuInt GetVertexBuffer() override;
	///	@brief	���UV����
	///	@see	n2dObjLoader::GetVertexBuffer
	nuInt GetUVBuffer() override;
	///	@brief	��÷��߻���
	///	@see	n2dObjLoader::GetVertexBuffer
	nuInt GetNormalBuffer() override;
	///	@brief	���Ԫ����������
	///	@note	���ڻ�ı�ԭ�л������ݣ����ڴ���Ԫ����������֮ǰ�����������������ɾ��֮ǰ�Ļ�����������������ɣ���������Ԫ������������ֱ�ӷ����Ѵ����Ļ���
	nuInt GetIndexBuffer() override;

	///	@brief	��ö�����Ŀ
	nuInt GetVertexCount() const override;
	///	@brief	���Ԫ��������Ŀ
	nuInt GetIndexCount() const override;
private:
	std::vector<natVec3<>> m_Vertices;
	std::vector<natVec2<>> m_UVs; 
	std::vector<natVec3<>> m_Normals;
	std::vector<nuShort> m_ElementIndexes;

	n2dTexture2D* m_Texture;
	nuInt m_VertexBuffer, m_UVBuffer, m_NormalBuffer, m_ElementBuffer;

	void init();
};