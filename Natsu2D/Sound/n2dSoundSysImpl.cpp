#include "n2dSoundSysImpl.h"
#include "n2dSoundBufferImpl.h"
#include "n2dSoundSourceImpl.h"
#include "../n2dEngine.h"
#include <natStream.h>

n2dSoundSysImpl::n2dSoundSysImpl(n2dEngine* pEngine)
	: m_pEngine(pEngine)
{
	ALFWInit();

	if (!ALFWInitOpenAL())
	{
		nat_Throw(natException, "ALFW initializing failed"_nv);
	}
}

n2dSoundSysImpl::~n2dSoundSysImpl()
{
	ALFWShutdownOpenAL();
	ALFWShutdown();
}

void n2dSoundSysImpl::EnableCapability(Capability capa, nBool value)
{
}

nBool n2dSoundSysImpl::IsCapabilityEnabled(Capability capa)
{
	return false;
}

natRefPointer<n2dSoundListener> n2dSoundSysImpl::GetListener()
{
	return m_Listener.ForkRef();
}

nResult n2dSoundSysImpl::CreateSoundBuffer(natRefPointer<n2dSoundBuffer>& pOut)
{
	pOut = make_ref<n2dSoundBufferImpl>();

	return NatErr_OK;
}

nResult n2dSoundSysImpl::CreateWaveSoundBufferFromStream(natRefPointer<natStream> pStream, natRefPointer<n2dSoundBuffer>& pOut)
{
	auto pBuffer = make_ref<n2dSoundBufferImpl>();
	std::vector<nByte> tBuffer(static_cast<nuInt>(pStream->GetSize() - pStream->GetPosition()));
	pStream->ReadBytes(tBuffer.data(), tBuffer.size());
	if (!ALFWLoadWaveBufferToBuffer(tBuffer.data(), static_cast<DWORD>(tBuffer.size()), pBuffer->GetHandle()))
	{
		nat_Throw(natException, "Cannot load wave buffer"_nv);
	}
	pOut = pBuffer;

	return NatErr_OK;
}

nResult n2dSoundSysImpl::CreateSoundSource(natRefPointer<n2dSoundSource>& pOut)
{
	pOut = make_ref<n2dSoundSourceImpl>();

	return NatErr_OK;
}

nFloat n2dSoundListenerImpl::GetGain() const
{
	nFloat tRet = 0.0f;
	alGetListenerf(AL_GAIN, &tRet);

	return tRet;
}

void n2dSoundListenerImpl::SetGain(nFloat value)
{
	alListenerf(AL_GAIN, value);
}

natVec3<> n2dSoundListenerImpl::GetPostion() const
{
	nFloat tRet[3] = { 0.0f };
	alGetListenerfv(AL_POSITION, tRet);

	return natVec3<>(tRet);
}

void n2dSoundListenerImpl::SetPostion(natVec3<> const & pos)
{
	alListenerfv(AL_POSITION, &pos[0]);
}

natVec3<> n2dSoundListenerImpl::GetVelocity() const
{
	nFloat tRet[3] = { 0.0f };
	alGetListenerfv(AL_VELOCITY, tRet);

	return natVec3<>(tRet);
}

void n2dSoundListenerImpl::SetVelocity(natVec3<> const & v)
{
	alListenerfv(AL_VELOCITY, &v[0]);
}

void n2dSoundListenerImpl::GetOrientation(natVec3<>& at, natVec3<>& up) const
{
	nFloat tRet[6] = { 0.0f };
	alGetListenerfv(AL_ORIENTATION, tRet);

	at = natVec3<>(&tRet[0]);
	up = natVec3<>(&tRet[3]);
}

void n2dSoundListenerImpl::SetOrientation(natVec3<> const & at, natVec3<> const & up)
{
	nFloat tOrient[6];
	memcpy_s(&tOrient[0], sizeof(tOrient), &at[0], at.length() * sizeof(nFloat));
	memcpy_s(&tOrient[3], sizeof(tOrient), &up[0], up.length() * sizeof(nFloat));

	alListenerfv(AL_ORIENTATION, tOrient);
}
