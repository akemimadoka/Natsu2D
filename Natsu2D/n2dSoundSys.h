#pragma once
#include "n2dInterface.h"
#include <natVec.h>

struct natStream;

////////////////////////////////////////////////////////////////////////////////
///	@brief	声音缓存
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundBuffer
	: n2dInterface
{
	///	@brief	缓存格式
	enum class BufferFormat
	{
		Mono8,
		Mono16,
		Stereo8,
		Stereo16,
	};

	///	@brief	获得句柄
	virtual nuInt GetHandle() const = 0;

	///	@brief	将数据传递给缓存对象
	virtual void AllocData(BufferFormat Format, ncData pData, nuInt Size, nuInt Freq) = 0;

	///	@brief	获得频率
	virtual nuInt GetFrequency() const = 0;
	///	@brief	获得比特率
	virtual nuInt GetBits() const = 0;
	///	@brief	获得声道数
	virtual nuInt GetChannel() const = 0;
	///	@brief	获得大小
	virtual nuInt GetSize() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	声源
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundSource
	: n2dInterface
{
	///	@brief	声源类型
	enum class SourceType
	{
		Undetermined,
		Static,
		Streaming,
	};

	///	@brief	声源状态
	enum class SourceState
	{
		Initial,
		Playing,
		Paused,
		Stopped,
	};

	///	@brief	获得句柄
	virtual nuInt GetHandle() const = 0;

	///	@brief	绑定缓存
	virtual void BindBuffer(const n2dSoundBuffer* pBuffer) = 0;

	///	@brief	获得仰角
	virtual nFloat GetPitch() const = 0;
	///	@brief	设置仰角
	virtual void SetPitch(nFloat value) = 0;

	///	@brief	获得增益
	virtual nFloat GetGain() const = 0;
	///	@brief	设置增益
	virtual void SetGain(nFloat value) = 0;

	///	@brief	获得最大距离
	virtual nFloat GetMaxDistance() const = 0;
	///	@brief	设置最大距离
	virtual void SetMaxDistance(nFloat value) = 0;

	///	@brief	获得衰减因子
	virtual nFloat GetRolloffFactor() const = 0;
	///	@brief	设置衰减因子
	virtual void SetRolloffFactor(nFloat value) = 0;

	///	@brief	获得关联距离
	virtual nFloat GetReferenceDistance() const = 0;
	///	@brief	设置关联距离
	virtual void SetReferenceDistance(nFloat value) = 0;

	///	@brief	获得最小增益
	virtual nFloat GetMinGain() const = 0;
	///	@brief	获得最大增益
	virtual nFloat GetMaxGain() const = 0;
	///	@brief	设置最小增益
	virtual void SetMinGain(nFloat value) = 0;
	///	@brief	设置最大增益
	virtual void SetMaxGain(nFloat value) = 0;

	///	@brief	获得锥体增益
	virtual nFloat GetConeOuterGain() const = 0;
	///	@brief	设置锥体增益
	virtual void SetConeOuterGain(nFloat value) = 0;

	///	@brief	获得内锥角度
	virtual nFloat GetConeInnerAngle() const = 0;
	///	@brief	获得外锥角度
	virtual nFloat GetConeOuterAngle() const = 0;
	///	@brief	设置内锥角度
	virtual void SetConeInnerAngle(nFloat value) = 0;
	///	@brief	设置外锥角度
	virtual void SetConeOuterAngle(nFloat value) = 0;

	///	@brief	获得位置
	virtual natVec3<> GetPosition() const = 0;
	///	@brief	设置位置
	virtual void SetPosition(natVec3<> const& pos) = 0;
	///	@brief	获得速度
	virtual natVec3<> GetVelocity() const = 0;
	///	@brief	设置速度
	virtual void SetVelocity(natVec3<> const& v) = 0;
	///	@brief	获得朝向
	virtual natVec3<> GetDirection() const = 0;
	///	@brief	设置朝向
	virtual void SetDirection(natVec3<> const& direction) = 0;

	///	@brief	获得声源关联性
	virtual nBool GetSourceRelative() const = 0;
	///	@brief	设置声源关联性
	virtual void SetSourceRelative(nBool value) = 0;
	///	@brief	获得声源类型
	virtual SourceType GetSourceType() const = 0;
	///	@brief	设置声源类型
	virtual void SetSourceType(SourceType type) = 0;

	///	@brief	是否循环
	virtual nBool IsLooping() const = 0;
	///	@brief	设置是否循环
	virtual void SetLooping(nBool value = true) = 0;

	///	@brief	获得状态
	virtual SourceState GetState() const = 0;

	///	@brief	获得队列中缓存数量
	virtual nuInt GetBufferQueuedCount() const = 0;
	///	@brief	获得处理中缓存数量
	virtual nuInt GetBufferProcessedCount() const = 0;

	///	@brief	获得按秒偏移
	virtual nFloat GetSecOffset() const = 0;
	///	@brief	获得按采样数偏移
	virtual nFloat GetSampleOffset() const = 0;
	///	@brief	获得按字节偏移
	virtual nFloat GetByteOffset() const = 0;
	///	@brief	设置按秒偏移
	virtual void SetSecOffset(nFloat value) = 0;
	///	@brief	设置按采样数偏移
	virtual void SetSampleOffset(nFloat value) = 0;
	///	@brief	设置按字节偏移
	virtual void SetByteOffset(nFloat value) = 0;

	///	@brief	播放
	virtual void Play() = 0;
	///	@brief	暂停
	virtual void Pause() = 0;
	///	@brief	停止
	virtual void Stop() = 0;
	///	@brief	回放
	virtual void Rewind() = 0;

	///	@brief	将缓存放入队列
	///	@param[in]	Count			缓存数组大小
	///	@param[in]	pBufferArray	缓存数组
	virtual nResult QueueBuffers(nuInt Count, const n2dSoundBuffer* pBufferArray) = 0;
	///	@brief	从队列中移除缓存
	///	@param[in]	Count			缓存数组大小
	///	@param[in]	pBufferArray	缓存数组
	virtual nResult UnqueueBuffers(nuInt Count, const n2dSoundBuffer* pBufferArray) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	声音收听者
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundListener
	: n2dInterface
{
	///	@brief	获得增益
	virtual nFloat GetGain() const = 0;
	///	@brief	设置增益
	virtual void SetGain(nFloat value) = 0;

	///	@brief	获得位置
	virtual natVec3<> GetPostion() const = 0;
	///	@brief	设置位置
	virtual void SetPostion(natVec3<> const& pos) = 0;

	///	@brief	获得速度
	virtual natVec3<> GetVelocity() const = 0;
	///	@brief	设置速度
	virtual void SetVelocity(natVec3<> const& v) = 0;

	///	@brief	获得方向
	virtual void GetOrientation(natVec3<>& at, natVec3<>& up) const = 0;
	///	@brief	设置方向
	virtual void SetOrientation(natVec3<> const& at, natVec3<> const& up) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	声音系统
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundSys
	: n2dInterface
{
	///	@brief	特性
	///	@note	OpenAL1.1暂无特性，保留此接口
	enum class Capability
	{

	};

	///	@brief	启用特性
	///	@param[in]	capa	要开启的特性
	///	@param[in]	value	是否开启
	///	@note	OpenAL1.1暂无特性，保留此接口
	virtual void EnableCapability(Capability capa, nBool value = true) = 0;
	///	@brief	特性是否启用
	///	@param[in]	capa	特性
	///	@return	特性是否已启用
	///	@note	OpenAL1.1暂无特性，保留此接口
	virtual nBool IsCapabilityEnabled(Capability capa) = 0;

	///	@brief	获得收听者
	///	@note	请勿释放
	virtual n2dSoundListener* GetListener() = 0;
	///	@brief	创建声音缓存
	///	@param[out]	pOut	创建的声音缓存
	///	@return	处理结果
	virtual nResult CreateSoundBuffer(n2dSoundBuffer** pOut) = 0;
	///	@brief	从流中创建波形声音缓存
	///	@param[out]	pOut	创建的声音缓存
	///	@return	处理结果
	virtual nResult CreateWaveSoundBufferFromStream(natStream* pStream, n2dSoundBuffer** pOut) = 0;
	///	@brief	创建声源
	///	@param[out]	pOut	创建的声源
	///	@return	处理结果
	virtual nResult CreateSoundSource(n2dSoundSource** pOut) = 0;
};