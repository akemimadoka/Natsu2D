#pragma once
#include "../n2dSoundSys.h"
#include "OpenAL.h"

class n2dSoundSourceImpl
	: public natRefObjImpl<n2dSoundSourceImpl, n2dSoundSource>
{
	static ALenum GetSourceTypeEnum(SourceType type)
	{
		switch (type)
		{
		case SourceType::Undetermined:
			return AL_UNDETERMINED;
		case SourceType::Static:
			return AL_STATIC;
		case SourceType::Streaming:
			return AL_STREAMING;
		default:
			nat_Throw(natException, "Invaild SourceType"_nv);
		}
	}

	static SourceType GetSourceType(ALenum type)
	{
		switch (type)
		{
		case AL_UNDETERMINED:
			return SourceType::Undetermined;
		case AL_STATIC :
			return SourceType::Static;
		case AL_STREAMING:
			return SourceType::Streaming;
		default:
			nat_Throw(natException, "Invaild SourceType"_nv);
		}
	}

	static ALenum GetSourceStateEnum(SourceState state)
	{
		switch (state)
		{
		case SourceState::Initial:
			return AL_INITIAL;
		case SourceState::Playing:
			return AL_PLAYING;
		case SourceState::Paused:
			return AL_PAUSED;
		case SourceState::Stopped:
			return AL_STOPPED;
		default:
			nat_Throw(natException, "Invaild SourceState"_nv);
		}
	}

	static SourceState GetSourceState(ALenum state)
	{
		switch (state)
		{
		case AL_INITIAL:
			return SourceState::Initial;
		case AL_PLAYING:
			return SourceState::Playing;
		case AL_PAUSED:
			return SourceState::Paused;
		case AL_STOPPED:
			return SourceState::Stopped;
		default:
			nat_Throw(natException, "Invaild SourceState"_nv);
		}
	}
public:
	n2dSoundSourceImpl();
	~n2dSoundSourceImpl();

	nuInt GetHandle() const override;

	void BindBuffer(natRefPointer<n2dSoundBuffer> pBuffer) override;

	nFloat GetPitch() const override;
	void SetPitch(nFloat value) override;

	nFloat GetGain() const override;
	void SetGain(nFloat value) override;

	nFloat GetMaxDistance() const override;
	void SetMaxDistance(nFloat value) override;

	nFloat GetRolloffFactor() const override;
	void SetRolloffFactor(nFloat value) override;

	nFloat GetReferenceDistance() const override;
	void SetReferenceDistance(nFloat value) override;

	nFloat GetMinGain() const override;
	nFloat GetMaxGain() const override;
	void SetMinGain(nFloat value) override;
	void SetMaxGain(nFloat value) override;

	nFloat GetConeOuterGain() const override;
	void SetConeOuterGain(nFloat value) override;

	nFloat GetConeInnerAngle() const override;
	nFloat GetConeOuterAngle() const override;
	void SetConeInnerAngle(nFloat value) override;
	void SetConeOuterAngle(nFloat value) override;

	natVec3<> GetPosition() const override;
	void SetPosition(natVec3<> const& pos) override;
	natVec3<> GetVelocity() const override;
	void SetVelocity(natVec3<> const& v) override;
	natVec3<> GetDirection() const override;
	void SetDirection(natVec3<> const& direction) override;

	nBool GetSourceRelative() const override;
	void SetSourceRelative(nBool value) override;
	SourceType GetSourceType() const override;
	void SetSourceType(SourceType type) override;

	nBool IsLooping() const override;
	void SetLooping(nBool value = true) override;

	SourceState GetState() const override;

	nuInt GetBufferQueuedCount() const override;
	nuInt GetBufferProcessedCount() const override;

	nFloat GetSecOffset() const override;
	nFloat GetSampleOffset() const override;
	nFloat GetByteOffset() const override;
	void SetSecOffset(nFloat value) override;
	void SetSampleOffset(nFloat value) override;
	void SetByteOffset(nFloat value) override;

	void Play() override;
	void Pause() override;
	void Stop() override;
	void Rewind() override;

	/*nResult QueueBuffers(nuInt Count, natRefPointer<n2dSoundBuffer> pBufferArray) override;
	nResult UnqueueBuffers(nuInt Count, natRefPointer<n2dSoundBuffer> pBufferArray) override;*/
private:
	ALuint m_Handle;
};