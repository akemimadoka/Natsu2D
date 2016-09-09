////////////////////////////////////////////////////////////////////////////////
///	@file	n2dVirtualFileSystem.h
///	@brief	Natsu2D�����ļ�ϵͳ
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "n2dInterface.h"
#include <natStream.h>

////////////////////////////////////////////////////////////////////////////////
///	@brief	����Ϣ�ӿ�
////////////////////////////////////////////////////////////////////////////////
struct IStreamInfo
	: n2dInterface
{
	///	@brief	���·�������е�����Ϣ������·����Ϣ
	virtual ncTStr GetPath() const = 0;

	///	@brief	����޸�ʱ�䣬�������е�����Ϣ�������޸�ʱ����Ϣ
	///	@param[out]	time	�޸�ʱ��
	virtual nResult GetEditTime(std::chrono::system_clock::time_point& time) const = 0;

	///	@brief	�������С���������е�����Ϣ����������С��Ϣ
	///	@param[out]	size	����С
	virtual nResult GetSize(nLen& size) const = 0;

	///	@brief	����
	///	@note	��ʧ������׳��쳣����ε��ý����ʵ�־���
	virtual natRefPointer<natStream> OpenStream(nBool Readable, nBool Writable) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	ģʽ�ӿ�
////////////////////////////////////////////////////////////////////////////////
struct ISchema
	: n2dInterface
{
	///	@brief	���ģʽ��
	///	@note	����ֵ��ģʽ�������������ڱض���Ч����ֵ����仯\n
	///			ģʽ��������ʶģʽ����Сд���У����ܰ����ַ���"://"��������Ψһ�ģ����Ƽ����������ַ��Ϳհ��ַ�
	virtual ncTStr GetName() const = 0;

	///	@brief	��·���������Ϣ
	///	@note	��ʧ������׳��쳣
	///	@param[in]	path	·��
	virtual natRefPointer<IStreamInfo> GetStreamInfoFromPath(ncTStr path) = 0;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief	ģʽ����
////////////////////////////////////////////////////////////////////////////////
struct n2dSchemaFactory
	: n2dInterface
{
	/// @brief	���������ļ�ϵͳģʽ
	///	@note	ʹ�õ�ǰĿ¼Ϊ��Ŀ¼
	/// @param	schemaName	ģʽ��
	virtual natRefPointer<ISchema> CreateLocalFilesystemSchema(ncTStr schemaName) const = 0;

	/// @brief	���������ļ�ϵͳģʽ
	/// @param	schemaName	ģʽ��
	///	@param	rootPath	��Ŀ¼
	virtual natRefPointer<ISchema> CreateLocalFilesystemSchema(ncTStr schemaName, ncTStr rootPath) const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	�����ļ�ϵͳ
////////////////////////////////////////////////////////////////////////////////
struct n2dVirtualFileSystem
	: n2dInterface
{
	///	@brief	ע��ģʽ����ģʽ���ظ���ʧ�ܲ�����NatErr_Duplicated
	///	@note	ʹ�÷���ֵ�ж�ע���Ƿ�ɹ�
	///	@param[in]	pSchema	ģʽ
	virtual nResult RegisterSchema(ISchema* pSchema) = 0;

	///	@brief	��ģʽ�����ģʽ
	///	@note	��ʧ������׳��쳣
	///	@param[in]	name	ģʽ��
	virtual natRefPointer<ISchema> GetSchemaFromName(ncTStr name) = 0;

	///	@brief	��Uri�������Ϣ
	///	@note	Uri��ʽ��[Schema��]://[·��]
	virtual natRefPointer<IStreamInfo> GetStreamInfoFromUri(ncTStr uri) = 0;
};
