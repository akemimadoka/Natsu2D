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
	virtual natRefPointer<IResource> CreateResourceFromUri(nStrView Uri) = 0;

	template <typename ResourceType>
	natRefPointer<ResourceType> CreateResourceFromUri(nStrView Uri)
	{
		auto ret = static_cast<natRefPointer<ResourceType>>(CreateResourceFromUri(Uri));
		if (!ret)
		{
			nat_Throw(natException, "Cannot create resource of uri \"{0}\" as type {1}."_nv, Uri, natUtil::ToTString(typeid(ResourceType).name()));
		}

		return std::move(ret);
	}
};
