#include "bgMaxExporter.h"

bgMaxExporter::bgMaxExporter()
{
}

bgMaxExporter::~bgMaxExporter()
{
}

void bgMaxExporter::Init(TSTR name, Interface* ip)
{
	m_szFileName = name;
	m_p3DSMax = ip;

	m_pRootNode = m_p3DSMax->GetRootNode();
	NodeProcess(m_pRootNode);

	m_Scene.iVersion = 100;
	m_Scene.iNumMtl = m_pMtlList.size();
	m_Scene.iNumObject = m_ObjectList.size();
	
	// 파일 열기
	m_pFile = _tfopen(m_szFileName, _T("wt"));
	_ftprintf(m_pFile, _T("%s %d\n"), _T("#KGCA_BG_3D_MODEL"), m_Scene.iVersion);
}

void bgMaxExporter::Release()
{
	// 파일 닫기
	fclose(m_pFile);
}

bool bgMaxExporter::Exporter()
{
	ExportScene();
	ExportMaterial();
	ExportObject();
	return true;
}

bool bgMaxExporter::ExportScene()
{
	m_Interval = m_p3DSMax->GetAnimRange();

	m_Scene.iTickPerFrame = GetTicksPerFrame();
	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	m_Scene.iFrameSpeed = GetFrameRate();

	//                      #  1  2  3  4  5  6
	_ftprintf(m_pFile, _T("%s %d %d %d %d %d %d\n"),
		_T("#SCENE"),				// #
		m_Scene.iFirstFrame,		// 1
		m_Scene.iLastFrame,			// 2
		m_Scene.iFrameSpeed,		// 3
		m_Scene.iTickPerFrame,		// 4
		m_Scene.iNumMtl,			// 5
		m_Scene.iNumObject			// 6
	);

	return true;
}

bool bgMaxExporter::ExportMaterial()
{
	Mtl* pMtl;

	for (int iMtl = 0; iMtl < m_pMtlList.size(); iMtl++)
	{
		bgMaxMtl MtlData;
		pMtl = m_pMtlList[iMtl];
		MtlData.iNumSubMtls = pMtl->NumSubMtls();
		if (pMtl->NumSubMtls() > 0)
		{
			for (int iSubMtl = 0; iSubMtl < pMtl->NumSubMtls(); iSubMtl++)
			{
				bgMaxMtl SubMtlData;
				SubMtlData.iNumSubMtls = 0;
				GetTexture(pMtl, SubMtlData);
				MtlData.SubMtlsList.push_back(SubMtlData);
			}
		}
		else
		{
			GetTexture(pMtl, MtlData);
		}
		m_MtlList.push_back(MtlData);
	}

	for (int iMtl = 0; iMtl < m_MtlList.size(); iMtl++)
	{
		//                      #  1  2  3
		_ftprintf(m_pFile, _T("%s %d %d %d\n"),
			_T("#MATERIAL"),				// #
			iMtl,							// 1
			m_MtlList[iMtl].iNumSubMtls,	// 2
			m_MtlList[iMtl].TexList.size()	// 3
		);

		if (m_MtlList[iMtl].SubMtlsList.size() > 0)
		{
			for (int iSubMtl = 0; iSubMtl < m_MtlList[iMtl].SubMtlsList.size(); iSubMtl++)
			{
				//                        #  1  2  3
				_ftprintf(m_pFile, _T("\t%s %d %d %d\n"),
					_T("#SUBMATERIAL"),									// #
					iSubMtl,											// 1
					m_MtlList[iMtl].SubMtlsList[iSubMtl].iNumSubMtls,	// 2
					m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList.size()	// 3
				);

				for (int iTex = 0; iTex < m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList.size(); iTex++)
				{
					//                          #  1  2
					_ftprintf(m_pFile, _T("\t\t%s %d %s\n"),
						_T("#TEXTURE"),													// #
						m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList[iTex].iMapType,	// 1
						m_MtlList[iMtl].SubMtlsList[iSubMtl].TexList[iTex].szTexName	// 2
					);
				}
			}
		}
		else
		{
			for (int iTex = 0; iTex < m_MtlList[iMtl].TexList.size(); iTex++)
			{
				//                        #  1  2
				_ftprintf(m_pFile, _T("\t%s %d %s\n"),
					_T("#TEXTURE"),							// #
					m_MtlList[iMtl].TexList[iTex].iMapType,	// 1
					m_MtlList[iMtl].TexList[iTex].szTexName	// 2
				);
			}
		}
	}

	return true;
}

bool bgMaxExporter::ExportObject()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{

	}

	return true;
}

void bgMaxExporter::NodeProcess(INode* pNode)
{
	if (pNode == NULL)
		return;

	AddMtl(pNode);
	AddObject(pNode);

	int iNumChild;
	INode* pChild;

	iNumChild = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChild; iChild++)
	{
		pChild = pNode->GetChildNode(iChild);
		NodeProcess(pChild);
	}
}

void bgMaxExporter::AddMtl(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl != NULL)
	{
		bool bAdd = true;
		for (int iFind = 0; iFind < m_pMtlList.size(); iFind++)
		{
			if (m_pMtlList[iFind] == pMtl)
			{
				bAdd = false;
				break;
			}
		}
		if (bAdd)
		{
			m_pMtlList.push_back(pMtl);
		}
	}
}

void bgMaxExporter::AddObject(INode* pNode)
{
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());

	if (os.obj)
	{
		switch (os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID:
		case HELPER_CLASS_ID:
		{
			m_ObjectList.push_back(pNode);
		}
		break;
		}
	}
}

void bgMaxExporter::GetTexture(Mtl* pMtl, bgMaxMtl& MtlData)
{
	bgMaxTex tex;
	tex.iMapType = 1;
	tex.szTexName = _T("NULL");
	MtlData.TexList.push_back(tex);
}
