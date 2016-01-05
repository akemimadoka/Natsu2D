#pragma once
#include "..\n2dSoundSys.h"
#include "OpenAL.h"

struct n2dEngine;

class n2dSoundListenerImpl
	: public natRefObjImpl<n2dSoundListener>
{
public:
	nFloat GetGain() const override;
	void SetGain(nFloat value) override;

	natVec3<> GetPostion() const override;
	void SetPostion(natVec3<> const& pos) override;

	natVec3<> GetVelocity() const override;
	void SetVelocity(natVec3<> const& v) override;

	void GetOrientation(natVec3<>& at, natVec3<>& up) const override;
	void SetOrientation(natVec3<> const& at, natVec3<> const& up) override;
};

class n2dSoundSysImpl
	: public natRefObjImpl<n2dSoundSys>
{
public:
	explicit n2dSoundSysImpl(n2dEngine* pEngine);
	~n2dSoundSysImpl();

	void EnableCapability(Capability capa, nBool value = true) override;
	nBool IsCapabilityEnabled(Capability capa) override;

	n2dSoundListener* GetListener() override;
	nResult CreateSoundBuffer(n2dSoundBuffer** pOut) override;
	nResult CreateWaveSoundBufferFromStream(natStream* pStream, n2dSoundBuffer** pOut) override;
	nResult CreateSoundSource(n2dSoundSource** pOut) override;
private:
	n2dEngine* m_pEngine;
	n2dSoundListenerImpl m_Listener;
};