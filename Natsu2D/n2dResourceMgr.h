#pragma once
#include "n2dVirtualFileSystem.h"
#include <natUtil.h>

struct IResource
	: n2dInterface
{

};

struct n2dResourceMgr
	: n2dInterface
{
	virtual natRefPointer<IResource> CreateResourceFromUri(ncTStr Uri) = 0;

	template <typename ResourceType>
	natRefPointer<ResourceType> CreateResourceFromUri(ncTStr Uri)
	{
		auto ret = static_cast<natRefPointer<ResourceType>>(CreateResourceFromUri(Uri));
		if (!ret)
		{
			nat_Throw(natException, _T("Cannot create resource of uri \"{0}\" as type {1}."), Uri, natUtil::ToTString(typeid(ResourceType).name()));
		}

		return std::move(ret);
	}
};
