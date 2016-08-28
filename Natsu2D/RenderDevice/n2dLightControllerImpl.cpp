#include "n2dLightControllerImpl.h"


n2dLightControllerImpl::n2dLightControllerImpl(nuInt index, n2dBufferImpl * pBuffer)
	: m_Index(index),
	m_pBuffer(pBuffer),
	m_Properties{false}
{
}

n2dLightControllerImpl::~n2dLightControllerImpl()
{
}

nuInt n2dLightControllerImpl::GetIndex() const
{
	return m_Index;
}

n2dLightController::LightProperties const & n2dLightControllerImpl::GetProperties() const
{
	return m_Properties;
}

void n2dLightControllerImpl::SetProperties(LightProperties const & prop)
{
	m_Properties = prop;
	m_pBuffer->AllocSubData(sizeof(LightProperties) * m_Index, sizeof(LightProperties), reinterpret_cast<ncData>(&m_Properties));
}
