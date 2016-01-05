#pragma once
#include "..\n2dRenderDevice.h"
#include "n2dBufferImpl.h"

class n2dLightControllerImpl
	: public natRefObjImpl<n2dLightController>
{
public:
	n2dLightControllerImpl(nuInt index, n2dBufferImpl* pBuffer);
	~n2dLightControllerImpl();

	nuInt GetIndex() const override;

	LightProperties const& GetProperties() const override;
	void SetProperties(LightProperties const& prop) override;
private:
	nuInt m_Index;
	n2dBufferImpl* m_pBuffer;
	n2dLightController::LightProperties m_Properties;
};