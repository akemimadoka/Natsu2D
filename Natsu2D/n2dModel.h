////////////////////////////////////////////////////////////////////////////////
///	@file	n2dModel.h
///	@brief	Natsu2D模型
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
///	@brief	网格数据
////////////////////////////////////////////////////////////////////////////////
struct n2dMeshData
	: n2dInterface
{
	///	@brief	通用材质
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

	///	@brief	动态模型用材质
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

	///	@brief	获得顶点数量
	virtual nuInt GetVertexCount() const = 0;
	///	@brief	获得索引数量
	virtual nuInt GetIndexCount() const = 0;

	///	@brief	网格数据是否为静态
	virtual nBool IsStatic() const = 0;
	///	@brief	应用表情
	///	@param[in]	Name	表情名称
	///	@param[in]	Weight	权重
	///	@return	处理结果
	///	@note	仅对动态网格数据有效
	virtual nResult ApplyExpression(nStrView const& Name, nFloat Weight) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	模型数据
////////////////////////////////////////////////////////////////////////////////
struct n2dModelData
	: n2dInterface
{
	///	@brief	模型是否为静态
	virtual nBool IsStatic() const = 0;

	///	@brief	更新模型
	///	@param[in]	nFrame	帧数
	///	@note	仅对动态模型有效
	virtual void Update(nuInt nFrame) = 0;
	///	@brief	获得网格数量
	virtual nuInt GetMeshCount() const = 0;
	///	@brief	获得网格数据
	///	@param[in]	Index	索引
	virtual n2dMeshData* GetMesh(nuInt Index) = 0;

	///	@brief	设置缩放
	///	@param[in]	Factor	缩放因子
	///	@note	对静态模型仅第一次绘制前设置有效
	virtual void SetZoom(nFloat Factor) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	模型加载器
////////////////////////////////////////////////////////////////////////////////
struct n2dModelLoader
	: n2dInterface
{
	///	@brief	从流中创建静态模型
	///	@param[in]	pStream	流
	///	@param[out]	pOut	创建的模型数据
	///	@return	处理结果
	virtual nResult CreateStaticModelFromStream(natStream* pStream, n2dModelData** pOut) = 0;
	///	@brief	从文件中创建静态模型
	///	@param[in]	lpPath	文件路径
	///	@param[out]	pOut	创建的模型数据
	///	@return	处理结果
	virtual nResult CreateStaticModelFromFile(nStrView lpPath, n2dModelData** pOut) = 0;

	///	@brief	从流中创建动态模型
	///	@param[in]	pStream	流
	///	@param[out]	pOut	创建的模型数据
	///	@return	处理结果
	virtual nResult CreateDynamicModelFromStream(natStream* pStream, n2dModelData** pOut) = 0;
	///	@brief	从文件中创建动态模型
	///	@param[in]	lpPath	文件路径
	///	@param[out]	pOut	创建的模型数据
	///	@return	处理结果
	virtual nResult CreateDynamicModelFromFile(nStrView lpPath, n2dModelData** pOut) = 0;

	///	@brief	设置纹理缺失时使用的纹理
	///	@param[in]	Texture	选定的默认纹理
	///	@note	用空指针做参数表示在缺失时不绑定纹理
	virtual void SetDefaultTexture(n2dTexture2D* Texture) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	动作管理器
////////////////////////////////////////////////////////////////////////////////
struct n2dMotionManager
	: n2dInterface
{
	///	@brief	从流中加载动作
	///	@param[in]	lpName	动作名称
	///	@param[in]	pStream	流
	///	@return	处理结果
	///	@note	动作名称作为标识符
	virtual nResult LoadMotionFromStream(nStrView lpName, natStream* pStream) = 0;
	///	@brief	从文件中加载动作
	///	@param[in]	lpName	动作名称
	///	@param[in]	lpPath	文件路径
	///	@return	处理结果
	///	@note	动作名称作为标识符
	virtual nResult LoadMotionFromFile(nStrView lpName, nStrView lpPath) = 0;

	///	@brief	将动作应用在模型上
	///	@param[in]	lpName	动作名称
	///	@param[in]	pModel	应用到的模型
	///	@return	处理结果
	///	@note	仅对动态模型有效
	virtual nResult ApplyToModel(nStrView lpName, n2dModelData* pModel) = 0;
	///	@brief	回复对应模型的动作
	virtual void RestoreMotion(n2dModelData* pModel) = 0;
	///	@brief	回复对应模型的变型
	virtual void RestoreMorph(n2dModelData* pModel) = 0;
};
