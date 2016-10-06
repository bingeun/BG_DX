#include "bgExporterSkin.h"

bool bgExporterSkin::ExportBGSKN()
{
	int iSkinMeshCount = m_SkinMeshList.size();
	if (iSkinMeshCount <= 0)
	{
		MessageBox(NULL, _T("출력할 정보가 없습니다."), _T("파일생성에러"), MB_ICONERROR);
		return FALSE;
	}

	TCHAR  szFile[256] = { 0, };
	_tcscpy(szFile, I_Exporter.SaveFileDialog(_T("BGSKN"), _T("BG Skin Export")));

	FILE	*fp;
	fp = _tfopen(szFile, _T("wb"));
	if (!fp)
	{
		MessageBox(NULL, _T("파일을 생성하지 못했습니다."), _T("파일생성에러"), MB_ICONERROR);
		return FALSE;
	}

	struct tm *newtime;
	time_t aclock;
	time(&aclock);
	newtime = localtime(&aclock);
	fwrite(newtime, sizeof(tm), 1, fp);

	m_Scene.iNumMesh = m_SkinMeshList.size();
	fwrite(&m_Scene, sizeof(bgScene), 1, fp);

	for (vector<bgSkinMesh*>::iterator iter = m_SkinMeshList.begin(); iter != m_SkinMeshList.end(); iter++)
	{
		bgSkinMesh* pMesh = (*iter);
		TCHAR szName[256] = { 0, };
		TCHAR szParentName[256] = { 0, };
		int iLen = pMesh->strNodeName.Length();
		fwrite(&iLen, sizeof(int), 1, fp);
		if (iLen > 0)
		{
			_tcscpy(szName, pMesh->strNodeName);
			fwrite(szName, sizeof(TCHAR)*iLen, 1, fp);
		}

		iLen = pMesh->strParentName.Length();
		_tcscpy(szParentName, pMesh->strParentName);
		fwrite(&iLen, sizeof(int), 1, fp);
		if (iLen > 0)
		{
			fwrite(szParentName, sizeof(TCHAR)*iLen, 1, fp);
		}

		fwrite(&pMesh->matWorld, sizeof(D3D_MATRIX), 1, fp);

		int iRealSubMtrlConter = pMesh->pSubMeshes.size();
		fwrite(&iRealSubMtrlConter, sizeof(int), 1, fp);

		if (iRealSubMtrlConter <= 0)
		{
			ExpMesh(fp, pMesh);
		}
		else
		{
			for (int iMesh = 0; iMesh < pMesh->pSubMeshes.size(); iMesh++)
			{
				//if (pMesh->m_pSkinSubMesh.size() > 0)
				{
					bgSkinMesh* pSubMesh = pMesh->m_pSkinSubMesh[iMesh];
					ExpMesh(fp, pSubMesh);
				}
			}
		}
	}

	bgMatrixIndex* matIndex;
	D3D_MATRIX d3dMat;
	int iNumMatrix = I_Exporter.m_MatrixMap.m_TMap.size();
	fwrite(&iNumMatrix, sizeof(int), 1, fp);

	for (int iMat = 0; iMat < iNumMatrix; iMat++)
	{
		matIndex = I_Exporter.m_MatrixMap.GetPtr(iMat);
		I_Exporter.DumpMatrix3(&matIndex->m_InvNodeTM, d3dMat);
		fwrite(&d3dMat, sizeof(D3D_MATRIX), 1, fp);
	}

	fclose(fp);
	Release();
	MessageBox(GetActiveWindow(), szFile, _T("Succeed!!"), MB_OK);

	return true;
}
