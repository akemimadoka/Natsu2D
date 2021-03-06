////////////////////////////////////////////////////////////////////////////////
///	@file	n2dVirtualFileSystem.h
///	@brief	Natsu2D虚拟文件系统
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dInterface.h"
#include <natStream.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	流信息接口
////////////////////////////////////////////////////////////////////////////////
struct IStreamInfo
	: n2dInterface
{
	///	@brief	获得路径，所有的流信息都包含路径信息
	virtual nStrView GetPath() const = 0;

	///	@brief	获得修改时间，并非所有的流信息都包含修改时间信息
	///	@param[out]	time	修改时间
	virtual nResult GetEditTime(std::chrono::system_clock::time_point& time) const = 0;

	///	@brief	获得流大小，并非所有的流信息都包含流大小信息
	///	@param[out]	size	流大小
	virtual nResult GetSize(nLen& size) const = 0;

	///	@brief	打开流
	///	@note	若失败则会抛出异常，多次调用结果由实现决定
	virtual natRefPointer<natStream> OpenStream(nBool Readable, nBool Writable) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	模式接口
////////////////////////////////////////////////////////////////////////////////
struct ISchema
	: n2dInterface
{
	///	@brief	获得模式名
	///	@note	返回值在模式的整个生命期内必定有效，其值不会变化\n
	///			模式名用来标识模式，大小写敏感，不能包含字符串"://"，必须是唯一的，不推荐包含特殊字符和空白字符
	virtual nStrView GetName() const = 0;

	///	@brief	从路径获得流信息
	///	@note	若失败则会抛出异常
	///	@param[in]	path	路径
	virtual natRefPointer<IStreamInfo> GetStreamInfoFromPath(nStrView path) = 0;

	/// @brief	路径是否存在
	/// @param[in]	path	路径
	virtual nBool PathExist(nStrView path) const = 0;

	/// @brief	移除指定路径的文件或文件夹
	///	@note	可选支持，若模式不支持该操作则会返回NatErr_NotSupport
	/// @param[in]	path	路径
	virtual nResult RemoveFromPath(nStrView path) = 0;

	/// @brief	枚举指定路径下的文件或文件夹
	///	@note	可选支持，若模式不支持该操作则会返回NatErr_NotSupport\n
	///			每次符合条件的枚举都会调用一次enumCallback，若enumCallback返回true则枚举立即终止并返回NatErr_Interrupted，若枚举全部完成则会返回NatErr_OK
	/// @param[in]	path			路径
	///	@param[in]	recursive		递归地枚举
	///	@param[in]	includeFolder	包含文件夹
	/// @param[in]	enumCallback	枚举回调
	virtual nResult EnumPath(nStrView path, nBool recursive, nBool includeFolder, std::function<nBool(IStreamInfo*)> enumCallback) = 0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief	模式工厂
////////////////////////////////////////////////////////////////////////////////
struct n2dSchemaFactory
	: n2dInterface
{
	/// @brief	创建本地文件系统模式
	///	@note	使用当前目录为根目录
	/// @param	schemaName	模式名
	virtual natRefPointer<ISchema> CreateLocalFilesystemSchema(nStrView schemaName) const = 0;

	/// @brief	创建本地文件系统模式
	/// @param	schemaName	模式名
	///	@param	rootPath	根目录
	virtual natRefPointer<ISchema> CreateLocalFilesystemSchema(nStrView schemaName, nStrView rootPath) const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	虚拟文件系统
////////////////////////////////////////////////////////////////////////////////
struct n2dVirtualFileSystem
	: n2dInterface
{
	///	@brief	注册模式，若模式名重复会失败并返回NatErr_Duplicated
	///	@note	使用返回值判断注册是否成功
	///	@param[in]	pSchema	模式
	virtual nResult RegisterSchema(ISchema* pSchema) = 0;

	virtual nBool SchemaExist(nStrView schemaName) const = 0;

	///	@brief	从模式名获得模式
	///	@note	若失败则会抛出异常
	///	@param[in]	name	模式名
	virtual natRefPointer<ISchema> GetSchemaFromName(nStrView name) = 0;

	///	@brief	从Uri获得流信息
	///	@note	Uri格式：[Schema名]://[路径]
	virtual natRefPointer<IStreamInfo> GetStreamInfoFromUri(nStrView uri) = 0;
};
