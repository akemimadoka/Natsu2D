#pragma once
#include "n2dInterface.h"
#include <natVec.h>

struct natStream;

////////////////////////////////////////////////////////////////////////////////
///	@brief	��������
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundBuffer
	: n2dInterface
{
	///	@brief	�����ʽ
	enum class BufferFormat
	{
		Mono8,
		Mono16,
		Stereo8,
		Stereo16,
	};

	///	@brief	��þ��
	virtual nuInt GetHandle() const = 0;

	///	@brief	�����ݴ��ݸ��������
	virtual void AllocData(BufferFormat Format, ncData pData, nuInt Size, nuInt Freq) = 0;

	///	@brief	���Ƶ��
	virtual nuInt GetFrequency() const = 0;
	///	@brief	��ñ�����
	virtual nuInt GetBits() const = 0;
	///	@brief	���������
	virtual nuInt GetChannel() const = 0;
	///	@brief	��ô�С
	virtual nuInt GetSize() const = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	��Դ
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundSource
	: n2dInterface
{
	///	@brief	��Դ����
	enum class SourceType
	{
		Undetermined,
		Static,
		Streaming,
	};

	///	@brief	��Դ״̬
	enum class SourceState
	{
		Initial,
		Playing,
		Paused,
		Stopped,
	};

	///	@brief	��þ��
	virtual nuInt GetHandle() const = 0;

	///	@brief	�󶨻���
	virtual void BindBuffer(const n2dSoundBuffer* pBuffer) = 0;

	///	@brief	�������
	virtual nFloat GetPitch() const = 0;
	///	@brief	��������
	virtual void SetPitch(nFloat value) = 0;

	///	@brief	�������
	virtual nFloat GetGain() const = 0;
	///	@brief	��������
	virtual void SetGain(nFloat value) = 0;

	///	@brief	���������
	virtual nFloat GetMaxDistance() const = 0;
	///	@brief	����������
	virtual void SetMaxDistance(nFloat value) = 0;

	///	@brief	���˥������
	virtual nFloat GetRolloffFactor() const = 0;
	///	@brief	����˥������
	virtual void SetRolloffFactor(nFloat value) = 0;

	///	@brief	��ù�������
	virtual nFloat GetReferenceDistance() const = 0;
	///	@brief	���ù�������
	virtual void SetReferenceDistance(nFloat value) = 0;

	///	@brief	�����С����
	virtual nFloat GetMinGain() const = 0;
	///	@brief	����������
	virtual nFloat GetMaxGain() const = 0;
	///	@brief	������С����
	virtual void SetMinGain(nFloat value) = 0;
	///	@brief	�����������
	virtual void SetMaxGain(nFloat value) = 0;

	///	@brief	���׶������
	virtual nFloat GetConeOuterGain() const = 0;
	///	@brief	����׶������
	virtual void SetConeOuterGain(nFloat value) = 0;

	///	@brief	�����׶�Ƕ�
	virtual nFloat GetConeInnerAngle() const = 0;
	///	@brief	�����׶�Ƕ�
	virtual nFloat GetConeOuterAngle() const = 0;
	///	@brief	������׶�Ƕ�
	virtual void SetConeInnerAngle(nFloat value) = 0;
	///	@brief	������׶�Ƕ�
	virtual void SetConeOuterAngle(nFloat value) = 0;

	///	@brief	���λ��
	virtual natVec3<> GetPosition() const = 0;
	///	@brief	����λ��
	virtual void SetPosition(natVec3<> const& pos) = 0;
	///	@brief	����ٶ�
	virtual natVec3<> GetVelocity() const = 0;
	///	@brief	�����ٶ�
	virtual void SetVelocity(natVec3<> const& v) = 0;
	///	@brief	��ó���
	virtual natVec3<> GetDirection() const = 0;
	///	@brief	���ó���
	virtual void SetDirection(natVec3<> const& direction) = 0;

	///	@brief	�����Դ������
	virtual nBool GetSourceRelative() const = 0;
	///	@brief	������Դ������
	virtual void SetSourceRelative(nBool value) = 0;
	///	@brief	�����Դ����
	virtual SourceType GetSourceType() const = 0;
	///	@brief	������Դ����
	virtual void SetSourceType(SourceType type) = 0;

	///	@brief	�Ƿ�ѭ��
	virtual nBool IsLooping() const = 0;
	///	@brief	�����Ƿ�ѭ��
	virtual void SetLooping(nBool value = true) = 0;

	///	@brief	���״̬
	virtual SourceState GetState() const = 0;

	///	@brief	��ö����л�������
	virtual nuInt GetBufferQueuedCount() const = 0;
	///	@brief	��ô����л�������
	virtual nuInt GetBufferProcessedCount() const = 0;

	///	@brief	��ð���ƫ��
	virtual nFloat GetSecOffset() const = 0;
	///	@brief	��ð�������ƫ��
	virtual nFloat GetSampleOffset() const = 0;
	///	@brief	��ð��ֽ�ƫ��
	virtual nFloat GetByteOffset() const = 0;
	///	@brief	���ð���ƫ��
	virtual void SetSecOffset(nFloat value) = 0;
	///	@brief	���ð�������ƫ��
	virtual void SetSampleOffset(nFloat value) = 0;
	///	@brief	���ð��ֽ�ƫ��
	virtual void SetByteOffset(nFloat value) = 0;

	///	@brief	����
	virtual void Play() = 0;
	///	@brief	��ͣ
	virtual void Pause() = 0;
	///	@brief	ֹͣ
	virtual void Stop() = 0;
	///	@brief	�ط�
	virtual void Rewind() = 0;

	///	@brief	������������
	///	@param[in]	Count			���������С
	///	@param[in]	pBufferArray	��������
	virtual nResult QueueBuffers(nuInt Count, const n2dSoundBuffer* pBufferArray) = 0;
	///	@brief	�Ӷ������Ƴ�����
	///	@param[in]	Count			���������С
	///	@param[in]	pBufferArray	��������
	virtual nResult UnqueueBuffers(nuInt Count, const n2dSoundBuffer* pBufferArray) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	����������
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundListener
	: n2dInterface
{
	///	@brief	�������
	virtual nFloat GetGain() const = 0;
	///	@brief	��������
	virtual void SetGain(nFloat value) = 0;

	///	@brief	���λ��
	virtual natVec3<> GetPostion() const = 0;
	///	@brief	����λ��
	virtual void SetPostion(natVec3<> const& pos) = 0;

	///	@brief	����ٶ�
	virtual natVec3<> GetVelocity() const = 0;
	///	@brief	�����ٶ�
	virtual void SetVelocity(natVec3<> const& v) = 0;

	///	@brief	��÷���
	virtual void GetOrientation(natVec3<>& at, natVec3<>& up) const = 0;
	///	@brief	���÷���
	virtual void SetOrientation(natVec3<> const& at, natVec3<> const& up) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///	@brief	����ϵͳ
////////////////////////////////////////////////////////////////////////////////
struct n2dSoundSys
	: n2dInterface
{
	///	@brief	����
	///	@note	OpenAL1.1�������ԣ������˽ӿ�
	enum class Capability
	{

	};

	///	@brief	��������
	///	@param[in]	capa	Ҫ����������
	///	@param[in]	value	�Ƿ���
	///	@note	OpenAL1.1�������ԣ������˽ӿ�
	virtual void EnableCapability(Capability capa, nBool value = true) = 0;
	///	@brief	�����Ƿ�����
	///	@param[in]	capa	����
	///	@return	�����Ƿ�������
	///	@note	OpenAL1.1�������ԣ������˽ӿ�
	virtual nBool IsCapabilityEnabled(Capability capa) = 0;

	///	@brief	���������
	///	@note	�����ͷ�
	virtual n2dSoundListener* GetListener() = 0;
	///	@brief	������������
	///	@param[out]	pOut	��������������
	///	@return	������
	virtual nResult CreateSoundBuffer(n2dSoundBuffer** pOut) = 0;
	///	@brief	�����д���������������
	///	@param[out]	pOut	��������������
	///	@return	������
	virtual nResult CreateWaveSoundBufferFromStream(natStream* pStream, n2dSoundBuffer** pOut) = 0;
	///	@brief	������Դ
	///	@param[out]	pOut	��������Դ
	///	@return	������
	virtual nResult CreateSoundSource(n2dSoundSource** pOut) = 0;
};