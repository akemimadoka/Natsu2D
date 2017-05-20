#include "n2dSoundSourceImpl.h"
#include <vector>

n2dSoundSourceImpl::n2dSoundSourceImpl()
{
	alGenSources(1, &m_Handle);
}

n2dSoundSourceImpl::~n2dSoundSourceImpl()
{
	alDeleteSources(1, &m_Handle);
}

nuInt n2dSoundSourceImpl::GetHandle() const
{
	return m_Handle;
}

void n2dSoundSourceImpl::BindBuffer(natRefPointer<n2dSoundBuffer> pBuffer)
{
	alSourcei(m_Handle, AL_BUFFER, pBuffer->GetHandle());
}

nFloat n2dSoundSourceImpl::GetPitch() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_PITCH, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetPitch(nFloat value)
{
	alSourcef(m_Handle, AL_PITCH, value);
}

nFloat n2dSoundSourceImpl::GetGain() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_GAIN, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetGain(nFloat value)
{
	alSourcef(m_Handle, AL_GAIN, value);
}

nFloat n2dSoundSourceImpl::GetMaxDistance() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_MAX_DISTANCE, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetMaxDistance(nFloat value)
{
	alSourcef(m_Handle, AL_MAX_DISTANCE, value);
}

nFloat n2dSoundSourceImpl::GetRolloffFactor() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_ROLLOFF_FACTOR, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetRolloffFactor(nFloat value)
{
	alSourcef(m_Handle, AL_ROLLOFF_FACTOR, value);
}

nFloat n2dSoundSourceImpl::GetReferenceDistance() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_REFERENCE_DISTANCE, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetReferenceDistance(nFloat value)
{
	alSourcef(m_Handle, AL_REFERENCE_DISTANCE, value);
}

nFloat n2dSoundSourceImpl::GetMinGain() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_MIN_GAIN, &tRet);

	return tRet;
}

nFloat n2dSoundSourceImpl::GetMaxGain() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_MAX_GAIN, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetMinGain(nFloat value)
{
	alSourcef(m_Handle, AL_MIN_GAIN, value);
}

void n2dSoundSourceImpl::SetMaxGain(nFloat value)
{
	alSourcef(m_Handle, AL_MAX_GAIN, value);
}

nFloat n2dSoundSourceImpl::GetConeOuterGain() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_CONE_OUTER_GAIN, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetConeOuterGain(nFloat value)
{
	alSourcef(m_Handle, AL_CONE_OUTER_GAIN, value);
}

nFloat n2dSoundSourceImpl::GetConeInnerAngle() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_CONE_INNER_ANGLE, &tRet);

	return tRet;
}

nFloat n2dSoundSourceImpl::GetConeOuterAngle() const
{
	nFloat tRet = 0;
	alGetSourcef(m_Handle, AL_CONE_OUTER_ANGLE, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetConeInnerAngle(nFloat value)
{
	alSourcef(m_Handle, AL_CONE_INNER_ANGLE, value);
}

void n2dSoundSourceImpl::SetConeOuterAngle(nFloat value)
{
	alSourcef(m_Handle, AL_CONE_OUTER_ANGLE, value);
}

natVec3<> n2dSoundSourceImpl::GetPosition() const
{
	nFloat tRet[3] = { 0.0f };
	alGetSourcefv(m_Handle, AL_POSITION, tRet);

	return natVec3<>(tRet);
}

void n2dSoundSourceImpl::SetPosition(natVec3<> const & pos)
{
	alSourcefv(m_Handle, AL_POSITION, &pos[0]);
}

natVec3<> n2dSoundSourceImpl::GetVelocity() const
{
	nFloat tRet[3] = { 0.0f };
	alGetSourcefv(m_Handle, AL_VELOCITY, tRet);

	return natVec3<>(tRet);
}

void n2dSoundSourceImpl::SetVelocity(natVec3<> const & v)
{
	alSourcefv(m_Handle, AL_POSITION, &v[0]);
}

natVec3<> n2dSoundSourceImpl::GetDirection() const
{
	nFloat tRet[3] = { 0.0f };
	alGetSourcefv(m_Handle, AL_DIRECTION, tRet);

	return natVec3<>(tRet);
}

void n2dSoundSourceImpl::SetDirection(natVec3<> const & direction)
{
	alSourcefv(m_Handle, AL_DIRECTION, &direction[0]);
}

nBool n2dSoundSourceImpl::GetSourceRelative() const
{
	ALint tRet = AL_FALSE;
	alGetSourcei(m_Handle, AL_SOURCE_RELATIVE, &tRet);

	return tRet == AL_TRUE;
}

void n2dSoundSourceImpl::SetSourceRelative(nBool value)
{
	alSourcei(m_Handle, AL_SOURCE_RELATIVE, value);
}

n2dSoundSource::SourceType n2dSoundSourceImpl::GetSourceType() const
{
	ALint tRet = 0;
	alGetSourcei(m_Handle, AL_SOURCE_TYPE, &tRet);

	return GetSourceType(tRet);
}

void n2dSoundSourceImpl::SetSourceType(n2dSoundSource::SourceType type)
{
	alSourcei(m_Handle, AL_SOURCE_TYPE, GetSourceTypeEnum(type));
}

nBool n2dSoundSourceImpl::IsLooping() const
{
	ALint tRet = 0;
	alGetSourcei(m_Handle, AL_LOOPING, &tRet);

	return tRet == AL_TRUE;
}

void n2dSoundSourceImpl::SetLooping(nBool value)
{
	alSourcei(m_Handle, AL_LOOPING, value);
}

n2dSoundSource::SourceState n2dSoundSourceImpl::GetState() const
{
	ALint tRet = 0;
	alGetSourcei(m_Handle, AL_SOURCE_STATE, &tRet);

	return GetSourceState(tRet);
}

nuInt n2dSoundSourceImpl::GetBufferQueuedCount() const
{
	ALint tRet = 0;
	alGetSourcei(m_Handle, AL_BUFFERS_QUEUED, &tRet);

	return tRet;
}

nuInt n2dSoundSourceImpl::GetBufferProcessedCount() const
{
	ALint tRet = 0;
	alGetSourcei(m_Handle, AL_BUFFERS_PROCESSED, &tRet);

	return tRet;
}

nFloat n2dSoundSourceImpl::GetSecOffset() const
{
	nFloat tRet = 0.0f;
	alGetSourcef(m_Handle, AL_SEC_OFFSET, &tRet);

	return tRet;
}

nFloat n2dSoundSourceImpl::GetSampleOffset() const
{
	nFloat tRet = 0.0f;
	alGetSourcef(m_Handle, AL_SAMPLE_OFFSET, &tRet);

	return tRet;
}

nFloat n2dSoundSourceImpl::GetByteOffset() const
{
	nFloat tRet = 0.0f;
	alGetSourcef(m_Handle, AL_BYTE_OFFSET, &tRet);

	return tRet;
}

void n2dSoundSourceImpl::SetSecOffset(nFloat value)
{
	alSourcef(m_Handle, AL_SEC_OFFSET, value);
}

void n2dSoundSourceImpl::SetSampleOffset(nFloat value)
{
	alSourcef(m_Handle, AL_SAMPLE_OFFSET, value);
}

void n2dSoundSourceImpl::SetByteOffset(nFloat value)
{
	alSourcef(m_Handle, AL_BYTE_OFFSET, value);
}

void n2dSoundSourceImpl::Play()
{
	alSourcePlay(m_Handle);
}

void n2dSoundSourceImpl::Pause()
{
	alSourcePause(m_Handle);
}

void n2dSoundSourceImpl::Stop()
{
	alSourceStop(m_Handle);
}

void n2dSoundSourceImpl::Rewind()
{
	alSourceRewind(m_Handle);
}
/*
nResult n2dSoundSourceImpl::QueueBuffers(nuInt Count, const n2dSoundBuffer * pBufferArray)
{
	std::vector<ALuint> tBuffers(Count);
	for (nuInt i = 0u; i < Count; ++i)
	{
		tBuffers[i] = pBufferArray[i].GetHandle();
	}
	alSourceQueueBuffers(m_Handle, Count, tBuffers.data());

	return alGetError() == AL_NO_ERROR ? NatErr_OK : NatErr_InternalErr;
}

nResult n2dSoundSourceImpl::UnqueueBuffers(nuInt Count, const n2dSoundBuffer * pBufferArray)
{
	std::vector<ALuint> tBuffers(Count);
	for (nuInt i = 0u; i < Count; ++i)
	{
		tBuffers[i] = pBufferArray[i].GetHandle();
	}
	alSourceUnqueueBuffers(m_Handle, Count, tBuffers.data());

	return alGetError() == AL_NO_ERROR ? NatErr_OK : NatErr_InternalErr;
}
*/
