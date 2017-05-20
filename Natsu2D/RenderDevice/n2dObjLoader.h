////////////////////////////////////////////////////////////////////////////////
///	@file	n2dObjLoader.h
///	@brief	Objģ�ͼ���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <natType.h>
#include <natVec.h>
#include "../n2dModel.h"
#include "../n2dRenderDevice.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
///	@brief	Obj������
///	@deprecated	�Ѿ�ʧЧ������ʹ��
////////////////////////////////////////////////////////////////////////////////
class n2dObjLoader final
	: public natRefObjImpl<n2dObjLoader, n2dModelLoader>
{
public:
	n2dObjLoader();

	///	@brief	����ʱ���ع��캯��
	///	@note	����ʧ�ܻ��׳��쳣
	///	@param[in]	filename	�ļ���
	explicit n2dObjLoader(nStrView filename);
	~n2dObjLoader();

	///	@brief	����Objģ��
	///	@param[in]	filename	�ļ���
	nResult CreateStaticModelFromFile(nStrView lpPath, natRefPointer<n2dModelData>& pOut) override;
	nResult CreateStaticModelFromStream(natRefPointer<natStream> pStream, natRefPointer<n2dModelData>& pOut) override;

	nResult CreateDynamicModelFromStream(natRefPointer<natStream> pStream, natRefPointer<n2dModelData>& pOut) override
	{
		return NatErr_NotImpl;
	}

	nResult CreateDynamicModelFromFile(nStrView lpPath, natRefPointer<n2dModelData>& pOut) override
	{
		return NatErr_NotImpl;
	}

	void SetDefaultTexture(natRefPointer<n2dTexture2D> Texture) override;

	n2dModelData* GetModel();

	///	@brief	��ö��㻺��
	///	@note	��δ���������㻺�潫�ᴴ��һ��������ֱ�ӷ����Ѵ����Ķ��㻺�棬���໺����Ϊ��ͬ
	nuInt GetVertexBuffer();
	///	@brief	���UV����
	///	@see	n2dObjLoader::GetVertexBuffer
	nuInt GetUVBuffer();
	///	@brief	��÷��߻���
	///	@see	n2dObjLoader::GetVertexBuffer
	nuInt GetNormalBuffer();
	///	@brief	���Ԫ����������
	///	@note	���ڻ�ı�ԭ�л������ݣ����ڴ���Ԫ����������֮ǰ�����������������ɾ��֮ǰ�Ļ�����������������ɣ���������Ԫ������������ֱ�ӷ����Ѵ����Ļ���
	nuInt GetIndexBuffer();

	///	@brief	��ö�����Ŀ
	nuInt GetVertexCount() const;
	///	@brief	���Ԫ��������Ŀ
	nuInt GetIndexCount() const;
private:
	std::vector<natVec3<>> m_Vertices;
	std::vector<natVec2<>> m_UVs; 
	std::vector<natVec3<>> m_Normals;
	std::vector<nuShort> m_ElementIndexes;

	natRefPointer<n2dTexture2D> m_Texture;
	nuInt m_VertexBuffer, m_UVBuffer, m_NormalBuffer, m_ElementBuffer;

	void init();
};