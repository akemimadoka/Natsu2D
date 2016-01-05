#include "n2dSoundBufferImpl.h"

n2dSoundBufferImpl::n2dSoundBufferImpl()
	: m_Handle(0u)
{
	alGenBuffers(1, &m_Handle);
}

n2dSoundBufferImpl::~n2dSoundBufferImpl()
{
	alDeleteBuffers(1, &m_Handle);
}

nuInt n2dSoundBufferImpl::GetHandle() const
{
	return m_Handle;
}

void n2dSoundBufferImpl::AllocData(BufferFormat Format, ncData pData, nuInt Size, nuInt Freq)
{
	alBufferData(m_Handle, GetBufferFormatEnum(Format), pData, Size, Freq);
}

nuInt n2dSoundBufferImpl::GetFrequency() const
{
	nInt tRet = 0;
	alGetBufferi(m_Handle, AL_FREQUENCY, &tRet);

	return tRet;
}

nuInt n2dSoundBufferImpl::GetBits() const
{
	nInt tRet = 0;
	alGetBufferi(m_Handle, AL_BITS, &tRet);

	return tRet;
}

nuInt n2dSoundBufferImpl::GetChannel() const
{
	nInt tRet = 0;
	alGetBufferi(m_Handle, AL_CHANNELS, &tRet);

	return tRet;
}

nuInt n2dSoundBufferImpl::GetSize() const
{
	nInt tRet = 0;
	alGetBufferi(m_Handle, AL_SIZE, &tRet);

	return tRet;
}
