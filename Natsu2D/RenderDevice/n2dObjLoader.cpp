#include "n2dObjLoader.h"
#include "OpenGL.h"
#include "n2dVBOHelper.h"
#include <natStream.h>
#include <natUtil.h>
#include <sstream>
#include <algorithm>

n2dObjLoader::n2dObjLoader()
	: m_Texture(nullptr), m_VertexBuffer(0u),
	m_UVBuffer(0u),
	m_NormalBuffer(0u),
	m_ElementBuffer(0u)
{
}

n2dObjLoader::n2dObjLoader(ncTStr filename)
{
	if (!CreateStaticModelFromFile(filename, nullptr))
	{
		nat_Throw(natException, _T("Load object model \"%s\" failed"), filename);
	}
}

n2dObjLoader::~n2dObjLoader()
{
	init();
}

nResult n2dObjLoader::CreateStaticModelFromFile(ncTStr lpPath, n2dModelData** pOut)
{
	natStream* tpStream = new natFileStream(lpPath, true, false);
	nResult tRet = CreateStaticModelFromStream(tpStream, pOut);
	SafeRelease(tpStream);

	return tRet;
}

nResult n2dObjLoader::CreateStaticModelFromStream(natStream* pStream, n2dModelData** pOut)
{
	init();

	std::string tmpline;
	std::stringstream tfs, ss;

	std::vector<nuInt> vertexIndices, uvIndices, normalIndices;
	std::vector<natVec3<>> tmpvertices;
	std::vector<natVec2<>> tmpuvs;
	std::vector<natVec3<>> tmpnormals;

	if (!pStream)
	{
		return NatErr_InternalErr;
	}

	nLen tLen = pStream->GetSize() - pStream->GetPosition();
	std::vector<nChar> tmpstr(static_cast<nuInt>(tLen) + 1);

	pStream->ReadBytes(reinterpret_cast<nData>(tmpstr.data()), tLen);
	tfs.str(tmpstr.data());

	while (!tfs.eof())
	{
		getline(tfs, tmpline);
		ss.clear();
		ss.str(tmpline);

		std::string tmpbuf;

		ss >> tmpbuf;

		if (tmpbuf == "v")
		{
			natVec3<> ver;
			ss >> ver.x >> ver.y >> ver.z;
			tmpvertices.push_back(ver);
		}
		else if (tmpbuf == "vt")
		{
			natVec2<> uv;
			ss >> uv.x >> uv.y;
			tmpuvs.push_back(uv);
		}
		else if (tmpbuf == "vn")
		{
			natVec3<> normal;
			ss >> normal.x >> normal.y >> normal.z;
			tmpnormals.push_back(normal);
		}
		else if (tmpbuf == "f")
		{
			std::string tstr[3];
			nuInt Index[3][3]; // vertex, uv, normal

			for (nuInt j = 0u; j < 3u; ++j)
			{
				ss >> tstr[j];
				std::vector<std::string> vi;
				natUtil::split(tstr[j], std::string("/"), [&vi](ncStr str, size_t len)
				{
					vi.emplace_back(str, len);
				});
				std::stringstream tss;

				for (nuInt i = 0u; i < 3u; ++i)
				{
					tss.str(vi[i]);
					tss >> Index[i][j];
					tss.clear();
				}
			}

			vertexIndices.insert(vertexIndices.end(), &Index[0][0], &Index[0][2] + 1);
			uvIndices.insert(uvIndices.end(), &Index[1][0], &Index[1][2] + 1);
			normalIndices.insert(normalIndices.end(), &Index[2][0], &Index[2][2] + 1);
		}
	}

#undef min

	nuInt size = static_cast<nuInt>(std::min({ vertexIndices.size(), uvIndices.size(), normalIndices.size() }));

	/*for (auto i : vertexIndices)
	{
	m_Vertices.push_back(tmpvertices[i - 1u]);
	}

	for (auto i : uvIndices)
	{
	m_UVs.push_back(tmpuvs[i - 1u]);
	}

	for (auto i : normalIndices)
	{
	m_Normals.push_back(tmpnormals[i - 1u]);
	}*/

	for (nuInt i = 0u; i < size; ++i)
	{
		m_Vertices.push_back(tmpvertices[vertexIndices[i] - 1u]);
		m_UVs.push_back(tmpuvs[uvIndices[i] - 1u]);
		m_Normals.push_back(tmpnormals[normalIndices[i] - 1u]);
	}

	return NatErr_OK;
}

void n2dObjLoader::SetDefaultTexture(n2dTexture2D* Texture)
{
	m_Texture = Texture;
}

n2dModelData* n2dObjLoader::GetModel()
{
	return nullptr;
}

nuInt n2dObjLoader::GetVertexBuffer()
{
	if (m_VertexBuffer == 0u)
	{
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(natVec3<>), &m_Vertices[0], GL_STATIC_DRAW);
	}

	return m_VertexBuffer;
}

nuInt n2dObjLoader::GetUVBuffer()
{
	if (m_UVBuffer == 0u)
	{
		glGenBuffers(1, &m_UVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(natVec2<>), &m_UVs[0], GL_STATIC_DRAW);
	}

	return m_UVBuffer;
}

nuInt n2dObjLoader::GetNormalBuffer()
{
	if (m_NormalBuffer == 0u)
	{
		glGenBuffers(1, &m_NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(natVec3<>), &m_Normals[0], GL_STATIC_DRAW);
	}

	return m_NormalBuffer;
}

nuInt n2dObjLoader::GetIndexBuffer()
{
	if (m_ElementBuffer == 0u)
	{
		if (m_VertexBuffer != 0u)
		{
			glDeleteBuffers(1, &m_VertexBuffer);
			m_VertexBuffer = 0u;
		}

		if (m_UVBuffer != 0u)
		{
			glDeleteBuffers(1, &m_UVBuffer);
			m_UVBuffer = 0u;
		}

		if (m_NormalBuffer != 0u)
		{
			glDeleteBuffers(1, &m_NormalBuffer);
			m_NormalBuffer = 0u;
		}

		n2dVBOHelper::indexVBO(m_Vertices, m_UVs, m_Normals, m_ElementIndexes);

		GetVertexBuffer();
		GetUVBuffer();
		GetNormalBuffer();

		glGenBuffers(1, &m_ElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ElementIndexes.size() * sizeof(nuShort), &m_ElementIndexes[0], GL_STATIC_DRAW);
	}

	return m_ElementBuffer;
}

nuInt n2dObjLoader::GetVertexCount() const
{
	return static_cast<nuInt>(m_Vertices.size());
}

nuInt n2dObjLoader::GetIndexCount() const
{
	return static_cast<nuInt>(m_ElementIndexes.size());
}

void n2dObjLoader::init()
{
	m_Vertices.clear();
	m_UVs.clear();
	m_Normals.clear();

	if (m_VertexBuffer != 0u)
	{
		glDeleteBuffers(1, &m_VertexBuffer);
		m_VertexBuffer = 0u;
	}

	if (m_UVBuffer != 0u)
	{
		glDeleteBuffers(1, &m_UVBuffer);
		m_UVBuffer = 0u;
	}

	if (m_NormalBuffer != 0u)
	{
		glDeleteBuffers(1, &m_NormalBuffer);
		m_NormalBuffer = 0u;
	}

	if (m_ElementBuffer != 0u)
	{
		glDeleteBuffers(1, &m_ElementBuffer);
		m_ElementBuffer = 0u;
	}
}