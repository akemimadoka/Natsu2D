////////////////////////////////////////////////////////////////////////////////
///	@file	n2dModel.h
///	@brief	Natsu2Dģ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dInterface.h"
#include <natQuat.h>

struct n2dModelData;
namespace NatsuLib
{
	struct natStream;
}
struct n2dTexture2D;

////////////////////////////////////////////////////////////////////////////////
///	@brief	��������
////////////////////////////////////////////////////////////////////////////////
struct n2dMeshData
	: n2dInterface
{
	///	@brief	ͨ�ò���
	struct Material
	{
		nBool WireFrame;
		nBool Both_sided;
		natRefPointer<n2dTexture2D> Texture;

		natVec4<> Diffuse;
		natVec4<> Specular;
		natVec4<> Ambient;
		natVec4<> Emission;
		nFloat Shininess;
		nFloat Strength;
	};

	///	@brief	��̬ģ���ò���
	struct DynamicMaterial
	{
		Material BaseMaterial;

		nByte Toon;
		nByte EdgeFlag;

		nuInt Start;
		nuInt End;
		nuInt Length;

		natRefPointer<n2dTexture2D> Spa;
	};

	///	@brief	��ö�������
	virtual nuInt GetVertexCount() const = 0;
	///	@brief	�����������
	virtual nuInt GetIndexCount() const = 0;

	///	@brief	���������Ƿ�Ϊ��̬
	virtual nBool IsStatic() const = 0;
	///	@brief	Ӧ�ñ���
	///	@param[in]	Name	��������
	///	@param[in]	Weight	Ȩ��
	///	@return	������
	///	@note	���Զ�̬����������Ч
	virtual nResult ApplyExpression(nStrView const& Name, nFloat Weight) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	ģ������
////////////////////////////////////////////////////////////////////////////////
struct n2dModelData
	: n2dInterface
{
	///	@brief	ģ���Ƿ�Ϊ��̬
	virtual nBool IsStatic() const = 0;

	///	@brief	����ģ��
	///	@param[in]	nFrame	֡��
	///	@note	���Զ�̬ģ����Ч
	virtual void Update(nuInt nFrame) = 0;
	///	@brief	�����������
	virtual nuInt GetMeshCount() const = 0;
	///	@brief	�����������
	///	@param[in]	Index	����
	virtual n2dMeshData* GetMesh(nuInt Index) = 0;

	///	@brief	��������
	///	@param[in]	Factor	��������
	///	@note	�Ծ�̬ģ�ͽ���һ�λ���ǰ������Ч
	virtual void SetZoom(nFloat Factor) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	ģ�ͼ�����
////////////////////////////////////////////////////////////////////////////////
struct n2dModelLoader
	: n2dInterface
{
	///	@brief	�����д�����̬ģ��
	///	@param[in]	pStream	��
	///	@param[out]	pOut	������ģ������
	///	@return	������
	virtual nResult CreateStaticModelFromStream(natStream* pStream, n2dModelData** pOut) = 0;
	///	@brief	���ļ��д�����̬ģ��
	///	@param[in]	lpPath	�ļ�·��
	///	@param[out]	pOut	������ģ������
	///	@return	������
	virtual nResult CreateStaticModelFromFile(nStrView lpPath, n2dModelData** pOut) = 0;

	///	@brief	�����д�����̬ģ��
	///	@param[in]	pStream	��
	///	@param[out]	pOut	������ģ������
	///	@return	������
	virtual nResult CreateDynamicModelFromStream(natStream* pStream, n2dModelData** pOut) = 0;
	///	@brief	���ļ��д�����̬ģ��
	///	@param[in]	lpPath	�ļ�·��
	///	@param[out]	pOut	������ģ������
	///	@return	������
	virtual nResult CreateDynamicModelFromFile(nStrView lpPath, n2dModelData** pOut) = 0;

	///	@brief	��������ȱʧʱʹ�õ�����
	///	@param[in]	Texture	ѡ����Ĭ������
	///	@note	�ÿ�ָ����������ʾ��ȱʧʱ��������
	virtual void SetDefaultTexture(n2dTexture2D* Texture) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	����������
////////////////////////////////////////////////////////////////////////////////
struct n2dMotionManager
	: n2dInterface
{
	///	@brief	�����м��ض���
	///	@param[in]	lpName	��������
	///	@param[in]	pStream	��
	///	@return	������
	///	@note	����������Ϊ��ʶ��
	virtual nResult LoadMotionFromStream(nStrView lpName, natStream* pStream) = 0;
	///	@brief	���ļ��м��ض���
	///	@param[in]	lpName	��������
	///	@param[in]	lpPath	�ļ�·��
	///	@return	������
	///	@note	����������Ϊ��ʶ��
	virtual nResult LoadMotionFromFile(nStrView lpName, nStrView lpPath) = 0;

	///	@brief	������Ӧ����ģ����
	///	@param[in]	lpName	��������
	///	@param[in]	pModel	Ӧ�õ���ģ��
	///	@return	������
	///	@note	���Զ�̬ģ����Ч
	virtual nResult ApplyToModel(nStrView lpName, n2dModelData* pModel) = 0;
	///	@brief	�ظ���Ӧģ�͵Ķ���
	virtual void RestoreMotion(n2dModelData* pModel) = 0;
	///	@brief	�ظ���Ӧģ�͵ı���
	virtual void RestoreMorph(n2dModelData* pModel) = 0;
};
