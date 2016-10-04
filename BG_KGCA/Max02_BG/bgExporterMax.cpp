#include "bgExporterMax.h"

bgExporterMax::bgExporterMax()
{
}

bgExporterMax::~bgExporterMax()
{
}

bool bgExporterMax::Init(Interface* p3DMax, TSTR strFileName)
{
	m_strFileName = strFileName;
	return Initialize(p3DMax);
}

bool bgExporterMax::Release()
{
	m_pMtlList.clear();
	m_MatrixMap.Release();
	return true;
}

TCHAR* bgExporterMax::SaveFileDialog(TCHAR* szExt, TCHAR* szTitle)
{
	static TCHAR* szFilter;

	TCHAR szFile[256], szFileTitle[256];
	_tcscpy_s(szFile, _T("*."));
	_tcscat_s(szFile, szExt);
	_tcscat_s(szFile, _T("\0"));

	OPENFILENAME OpenFileName;
	ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));
	OpenFileName.lStructSize = sizeof(OPENFILENAME);
	OpenFileName.hwndOwner = GetActiveWindow();
	OpenFileName.lpstrFilter = szFilter;
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter = 0L;
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFile = szFile;
	OpenFileName.nMaxFile = sizeof(szFile);
	OpenFileName.lpstrFileTitle = szFileTitle;
	OpenFileName.nMaxFileTitle = sizeof(szFileTitle);
	OpenFileName.lpstrInitialDir = NULL;
	OpenFileName.lpstrTitle = szTitle;
	OpenFileName.Flags = 0L;
	OpenFileName.nFileOffset = 0;
	OpenFileName.nFileExtension = 0;
	OpenFileName.lpstrDefExt = szExt;

	if (!GetSaveFileName(&OpenFileName))
	{
		return NULL;
	}

	return szFile;
}

int bgExporterMax::IsEqulVertexList(vector<PNCT_Vertex>& VertexArray, PNCT_Vertex& Vertex)
{
	for (int iVertex = 0; iVertex < VertexArray.size(); iVertex++)
	{
		if (EqualPoint3(VertexArray[iVertex].pos, Vertex.pos) &&
			EqualPoint3(VertexArray[iVertex].nor, Vertex.nor) &&
			EqualPoint4(VertexArray[iVertex].col, Vertex.col) &&
			EqualPoint2(VertexArray[iVertex].tex, Vertex.tex))
		{
			return iVertex;
		}
	}
	return -1;
}

int bgExporterMax::SetUniqueBuffer(bgMesh* pMesh, int iMtrl, int iStartTri, vector<PNCT_Vertex>& VertexArray, vector<int>& IndexArray)
{
	int iNumFaces = pMesh->Triangles.size();
	if (iMtrl >= 0)
	{
		g_iSearchIndex = iMtrl;
		iNumFaces = count_if(pMesh->Triangles.begin(), pMesh->Triangles.end(), IsSameInt());
	}

	int iPosPushCnt = 0;
	for (int iFace = 0; iFace < iNumFaces; iFace++)
	{
		for (int iCnt = 0; iCnt < 3; iCnt++)
		{
			int iPosReturn = IsEqulVertexList(VertexArray, pMesh->Triangles[iStartTri + iFace].vertex[iCnt]);
			if (iPosReturn < 0)
			{
				VertexArray.push_back(pMesh->Triangles[iStartTri + iFace].vertex[iCnt]);
				iPosReturn = iPosPushCnt++;
			}
			IndexArray.push_back(iPosReturn);
		}
	}
	return iNumFaces;
}
