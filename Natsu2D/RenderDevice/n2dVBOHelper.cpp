#include "n2dVBOHelper.h"
#include <map>
#include <algorithm>

#ifdef max
#	undef max
#endif
#ifdef min
#	undef min
#endif

namespace n2dVBOHelper
{
	struct PackedVertex
	{
		natVec3<> position;
		natVec2<> uv;
		natVec3<> normal;

		bool operator<(const PackedVertex& other) const
		{
			return memcmp(this, &other, sizeof(PackedVertex)) > 0;
		};
	};

	bool getSimilarVertexIndex(PackedVertex & packed, std::map<PackedVertex, unsigned short> & VertexToOutIndex, unsigned short & result)
	{
		auto it = VertexToOutIndex.find(packed);
		if (it == VertexToOutIndex.end()){
			return false;
		}
		
		result = it->second;
		return true;
	}

	void indexVBO(std::vector<natVec3<>>& vertices, std::vector<natVec2<>>& uvs, std::vector<natVec3<>>& normals, std::vector<unsigned short>& out_indices)
	{
		out_indices.clear();
		std::vector<natVec3<>> out_vertices(vertices);
		std::vector<natVec2<>> out_uvs(uvs);
		std::vector<natVec3<>> out_normals(normals);

		std::map<PackedVertex, nuShort> VertexToOutIndex;

		nuInt count = std::min({ vertices.size(), uvs.size(), normals.size() });

		for (nuInt i = 0u; i < count; ++i)
		{
			PackedVertex packed = { vertices[i], uvs[i], normals[i] };

			// Try to find a similar vertex in out_XXXX
			nuShort index;

			if (getSimilarVertexIndex(packed, VertexToOutIndex, index))
			{
				// A similar vertex is already in the VBO, use it instead !
				out_indices.push_back(index);
			}
			else
			{
				// If not, it needs to be added in the output data.
				out_vertices.push_back(vertices[i]);
				out_uvs.push_back(uvs[i]);
				out_normals.push_back(normals[i]);
				unsigned short newindex = static_cast<unsigned short>(out_vertices.size()) - 1;
				out_indices.push_back(newindex);
				VertexToOutIndex[packed] = newindex;
			}
		}

		/*vertices.swap(out_vertices);
		uvs.swap(out_uvs);
		normals.swap(out_normals);*/

		vertices.assign(out_vertices.begin(), out_vertices.end());
		uvs.assign(out_uvs.begin(), out_uvs.end());
		normals.assign(out_normals.begin(), out_normals.end());
	}
}
