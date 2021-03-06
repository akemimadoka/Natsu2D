#pragma once
#include "../n2dSoundSys.h"
#include "OpenAL.h"

class n2dSoundBufferImpl
	: public natRefObjImpl<n2dSoundBufferImpl, n2dSoundBuffer>
{
	static nuInt GetBufferFormatEnum(BufferFormat format)
	{
		switch (format)
		{
		case BufferFormat::Mono8:
			return AL_FORMAT_MONO8;
		case BufferFormat::Mono16:
			return AL_FORMAT_MONO16;
		case BufferFormat::Stereo8:
			return AL_FORMAT_STEREO8;
		case BufferFormat::Stereo16:
			return AL_FORMAT_STEREO16;
		default:
			nat_Throw(natException, "Invaild BufferFormat"_nv);
		}
	}
public:
	n2dSoundBufferImpl();
	~n2dSoundBufferImpl();

	nuInt GetHandle() const override;

	void AllocData(BufferFormat Format, ncData pData, nuInt Size, nuInt Freq) override;

	nuInt GetFrequency() const override;
	nuInt GetBits() const override;
	nuInt GetChannel() const override;
	nuInt GetSize() const override;
private:
	nuInt m_Handle;
};