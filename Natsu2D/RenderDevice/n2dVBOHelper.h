////////////////////////////////////////////////////////////////////////////////
///	@file	n2dVBOHelper.h
///	@brief	VBO辅助
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <natVec.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
///	@brief	VBO辅助
////////////////////////////////////////////////////////////////////////////////
namespace n2dVBOHelper
{
	///	@brief	生成VBO索引
	///	@param[inout]	vertices	顶点
	///	@param[inout]	uvs			UV
	///	@param[inout]	normals		法线
	///	@param[out]		out_indices	生成的索引
	void indexVBO(std::vector<natVec3<>> & vertices, std::vector<natVec2<>> & uvs, std::vector<natVec3<>> & normals, std::vector<nuShort> & out_indices);
}