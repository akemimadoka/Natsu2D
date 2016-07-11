#include "n2dEngine.h"
#include "n2dRenderDevice.h"

namespace n2dUtil
{
	void TransformCoord(n2dRenderDevice* pRenderer, GLfloat fScenex, GLfloat fSceney, GLint& iScrx, GLint& iScry)
	{
		natMat4<> tModelMat = pRenderer->GetCurModelMat();
		n2dWindow* pWindow = pRenderer->GetEngine()->GetWindow();
		iScrx = static_cast<GLint>((fScenex + tModelMat[0][0]) * pWindow->GetWidth() * tModelMat[0][0] * 2.0f);
		iScry = static_cast<GLint>((tModelMat[1][1] - fSceney) * pWindow->GetHeight() * tModelMat[1][1] * 2.0f);
	}

	void TransformCoord(n2dRenderDevice* pRenderer, GLint iScrx, GLint iScry, GLfloat& fScenex, GLfloat& fSceney)
	{
		natMat4<> tModelMat = pRenderer->GetCurModelMat();
		n2dWindow* pWindow = pRenderer->GetEngine()->GetWindow();
		fScenex = (float)iScrx / (float)pWindow->GetWidth() * tModelMat[0][0] * 2.0f - tModelMat[0][0];
		fSceney = tModelMat[1][1] - (float)iScry / (float)pWindow->GetHeight() * tModelMat[1][1] * 2.0f;
	}
}