#include "bgParserASE.h"

bgParserASE::bgParserASE()
{
}

bgParserASE::~bgParserASE()
{
}

bool bgParserASE::Init(bgModelASE* pModel)
{
	m_pModel = pModel;
	return true;
}

bool bgParserASE::Read()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH * 4];

	// m_ASE ������ �ʱ�ȭ
	ZeroMemory(&m_pModel->m_Scene, sizeof(SceneInfo));
	m_pModel->m_MaterialList.clear();
	m_pModel->m_ObjectList.clear();

	// ���� �պκ� 1ȸ�� �����ϴ� ���� �б�
	IF_FALSE_RETURN(FindWord(_T("*SCENE")));
	IF_FALSE_RETURN(ReadScene());
	IF_FALSE_RETURN(FindWord(_T("*MATERIAL_LIST")));
	IF_FALSE_RETURN(ReadMaterial());

	// ������ �����ϴ� ���� �ݺ��ؼ� �б�
	_tcscpy(szWordArray[0], _T("*GEOMOBJECT"));
	_tcscpy(szWordArray[1], _T("*HELPEROBJECT"));
	while (!feof(m_pFile))
	{
		switch (FindWordArray(szWordArray, 2))
		{
		case 0:	IF_FALSE_RETURN(ReadGeomObject());		break;
		case 1:	IF_FALSE_RETURN(ReadHelperObject());	break;
		case -1:	// ã�� �ܾ� ���� (������ ��)
		{
			ConvertToModel();	// ���� �����͸� �𵨿� �����ͷ� ������
			LinkNode();			// ��� ���� ����
			OperationTM();		// ��� ���迡 ���� ��� ����
		}
		break;
		default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
			return false;
			break;
		}
	}

	return hr;
}

bool bgParserASE::ReadScene()
{
	bool hr = true;

	IF_FALSE_RETURN(FindWord(_T("*SCENE_FIRSTFRAME")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iFirstFrame);
	IF_FALSE_RETURN(FindWord(_T("*SCENE_LASTFRAME")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iLastFrame);
	IF_FALSE_RETURN(FindWord(_T("*SCENE_FRAMESPEED")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iFrameSpeed);
	IF_FALSE_RETURN(FindWord(_T("*SCENE_TICKSPERFRAME")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &m_pModel->m_Scene.iTicksPerFrame);

	IF_FALSE_RETURN(FindWord(_T("}"))); // SCENE Ż��

	return hr;
}

bool bgParserASE::ReadMaterial()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH * 4];
	int iData;
	int iLoop, iLoopMax;
	int iLoopSub, iLoopSubMax;

	// �ֻ��� ���͸��� ����
	IF_FALSE_RETURN(FindWord(_T("*MATERIAL_COUNT")));
	_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
	m_pModel->m_MaterialList.resize(iData);

	// �ֻ��� ���͸��� ������ŭ �ݺ��ϸ� �б�
	iLoopMax = m_pModel->m_MaterialList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MATERIAL")));

		IF_FALSE_RETURN(FindWord(_T("*MATERIAL_NAME")));
		ZeroMemory(m_pModel->m_MaterialList[iLoop].szMaterialName, MAX_PATH * 4);
		m_pszToken = _tcstok(m_szLine, _T("\""));
		m_pszToken = _tcstok(NULL, _T("\""));
		_tcscpy(m_pModel->m_MaterialList[iLoop].szMaterialName, m_pszToken);

		// ���� ���͸����� �ִ��� ���� Ȯ��
		_tcscpy(szWordArray[0], _T("*NUMSUBMTLS"));
		_tcscpy(szWordArray[1], _T("*MAP_SUBNO"));
		switch (FindWordArray(szWordArray, 2))
		{
		case 0:		// *NUMSUBMTLS		���� ���͸����� �ִٸ� =======================
		{
			_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);
			m_pModel->m_MaterialList[iLoop].SubMaterialList.resize(iData);

			iLoopSubMax = m_pModel->m_MaterialList[iLoop].SubMaterialList.size();
			for (iLoopSub = 0; iLoopSub < iLoopSubMax; iLoopSub++)
			{
				IF_FALSE_RETURN(FindWord(_T("*SUBMATERIAL")));

				_tcscpy(szWordArray[0], _T("*MAP_SUBNO"));
				_tcscpy(szWordArray[1], _T("}"));
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *MAP_SUBNO		MAP_SUBNO�� �ִٸ� ------------------
				{
					_stscanf(m_szLine, _T("%s%d"), m_szWord, &iData);

					// ���� SUBNO�� 1(Diffuse)�� �ƴϸ�
					if (iData != 1)
					{
						// �ϳ��� *MAP_XXX { Ż��
						IF_FALSE_RETURN(FindWord(_T("}")));
						continue;
					}

					_tcscpy(szWordArray[0], _T("*BITMAP"));
					_tcscpy(szWordArray[1], _T("}"));
					switch (FindWordArray(szWordArray, 2))
					{
					case 0:		// *BITMAP		��Ʈ���� �ִٸ� ---------------
					{
						ZeroMemory(m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap, MAX_PATH * 4);
						m_pszToken = _tcstok(m_szLine, _T("\""));
						m_pszToken = _tcstok(NULL, _T("\""));
						_tcscpy(m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap, GetPathToFileName(m_pszToken));

						// �ϳ��� *MAP_XXX { Ż��
						IF_FALSE_RETURN(FindWord(_T("}")));
					}
					break;
					case 1:		// }			��Ʈ���� ���� ���͸����̸� -----
					{
					}
					break;
					case -1:	// ã�� �ܾ� ���� -----------------------------
					default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)---
						return false;
						break;
					}
				}
				break;
				case 1:		// }				MAP_SUBNO�� ������ ------------------
				{
				}
				break;
				case -1:	// ã�� �ܾ� ���� ----------------------------------------
				default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)--------------
					return false;
					break;
				}

				// �ϳ��� *SUBMATERIAL { Ż��
				IF_FALSE_RETURN(FindWord(_T("}")));
			}
		}
		break;
		case 1:		// *MAP_SUBNO		���� ���͸����̶�� ===========================
		{
			m_pModel->m_MaterialList.resize(1);

			_tcscpy(szWordArray[0], _T("*BITMAP"));
			_tcscpy(szWordArray[1], _T("}"));
			switch (FindWordArray(szWordArray, 2))
			{
			case 0:		// *BITMAP		��Ʈ���� �ִٸ� ------------------------
			{
				ZeroMemory(m_pModel->m_MaterialList[iLoop].szBitmap, MAX_PATH * 4);
				m_pszToken = _tcstok(m_szLine, _T("\""));
				m_pszToken = _tcstok(NULL, _T("\""));
				_tcscpy(m_pModel->m_MaterialList[iLoop].szBitmap, GetPathToFileName(m_pszToken));

				// �ϳ��� *MAP_XXX { Ż��
				IF_FALSE_RETURN(FindWord(_T("}")));
			}
			break;
			case 1:		// }			��Ʈ���� ���� ���͸����̸� --------------
			{
				// �ش� ���͸��� �۾�
			}
			break;
			case -1:			// ã�� �ܾ� ���� ------------------------------
			default:			// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
				return false;
				break;
			}
			// �ϳ��� *MATERIAL { Ż��
			IF_FALSE_RETURN(FindWord(_T("}")));
		}
		break;
		case -1:	// ã�� �ܾ� ���� =================================================
		default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
			return false;
			break;
		}
	}
	
	IF_FALSE_RETURN(FindWord(_T("}"))); // MATERIAL_LIST Ż��

	return hr;
}

bool bgParserASE::ReadGeomObject()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH * 4];
	TCHAR szCheck[MAX_PATH * 4];
	D3DXVECTOR3 v3Data;
	FaceInfo i4Data;
	int iData;
	int iLoop, iLoopMax;

	int iNumObj = m_pModel->m_ObjectList.size();
	m_pModel->m_ObjectList.resize(iNumObj + 1);
	m_pModel->m_ObjectList[iNumObj].vpObj = new GeomObject;
	m_pModel->m_ObjectList[iNumObj].eNodeType = OBJECT_NODE_TYPE_GEOMOBJECT;

	IF_FALSE_RETURN(ReadNodeInfo(iNumObj));

	///// MESH - �޽� ������
	IF_FALSE_RETURN(FindWord(_T("*MESH")));
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMVERTEX")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->VertexList.resize(iData);

	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMFACES")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->FaceList.resize(iData);
	
	///// VERTEX_LIST - ���ؽ� ����Ʈ
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse, NULL, &m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld);

	IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEX_LIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->VertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEX")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		D3DXVec3TransformCoord(&v3Data, &v3Data, &matInverse);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->VertexList[iLoop] = v3Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// FACE_LIST - ���̽� ����Ʈ
	IF_FALSE_RETURN(FindWord(_T("*MESH_FACE_LIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->FaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_FACE")));

		// 1�� Ȯ�� -> *MESH_SMOOTHING ���� �������� �ʴ°���� ó��
		// ex) *MESH_SMOOTHING   *MESH_MTLID   10                    ���
		_stscanf(m_szLine, _T("%s %s %s%d %s%d %s%d %s%d %s%d %s%d %s%s"),
			m_szWord, m_szWord,			// *MESH_FACE  0:
			m_szWord, &i4Data.iA,		// A:
			m_szWord, &i4Data.iC,		// B:
			m_szWord, &i4Data.iB,		// C:
			m_szWord, &iData,			// AB:
			m_szWord, &iData,			// BC:
			m_szWord, &iData,			// CA:
			m_szWord,					// *MESH_SMOOTHING
			szCheck);					// ??? ���� ���� �ٷ� *MESH_MTLID �ΰ�?

		// *MESH_SMOOTHING ���� ���� ��� ���� ó��
		if (_tcsicmp(szCheck, _T("*MESH_MTLID")))
		{
			_stscanf(m_szLine, _T("%s %s %s%d %s%d %s%d %s%d %s%d %s%d %s%d %s%d"),
				m_szWord, m_szWord,			// *MESH_FACE  0:
				m_szWord, &i4Data.iA,		// A:
				m_szWord, &i4Data.iC,		// B:
				m_szWord, &i4Data.iB,		// C:
				m_szWord, &iData,			// AB:
				m_szWord, &iData,			// BC:
				m_szWord, &iData,			// CA:
				m_szWord, &iData,			// *MESH_SMOOTHING
				m_szWord, &i4Data.iID);		// *MESH_MTLID
		}
		// *MESH_SMOOTHING ���� ���� ��� �ǳʶ�� �б�
		else
		{
			// ex) *MESH_SMOOTHING   *MESH_MTLID   10                    ���
			_stscanf(m_szLine, _T("%s %s %s%d %s%d %s%d %s%d %s%d %s%d %s   %s%d"),
				m_szWord, m_szWord,			// *MESH_FACE  0:
				m_szWord, &i4Data.iA,		// A:
				m_szWord, &i4Data.iC,		// B:
				m_szWord, &i4Data.iB,		// C:
				m_szWord, &iData,			// AB:
				m_szWord, &iData,			// BC:
				m_szWord, &iData,			// CA:
				m_szWord,					// *MESH_SMOOTHING ? ����
				m_szWord, &i4Data.iID);		// *MESH_MTLID
		}

		// *MTLID ���� ������͸��� �������� ũ�� 0���� ó���ؾ���
		// ���͸��� Ref ���� �ʿ��ϹǷ� ������ ��� ���� �� �������Ҷ� ó��
		//if (i4Data.iID >= m_pModel->m_MaterialList[ /*MaterialRef*/ ].SubMaterialList.size())
			//i4Data.iID = 0;

		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->FaceList[iLoop] = i4Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// TVERTLIST - �ؽ��� ���ؽ� ����Ʈ
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMTVERTEX")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->TexVertexList.resize(iData);

	IF_FALSE_RETURN(FindWord(_T("*MESH_TVERTLIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->TexVertexList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_TVERT")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->TexVertexList[iLoop] = v3Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// TFACELIST - �ؽ��� ���̽� ����Ʈ
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMTVFACES")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->TexFaceList.resize(iData);

	IF_FALSE_RETURN(FindWord(_T("*MESH_TFACELIST")));
	iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->TexFaceList.size();
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_TFACE")));
		_stscanf(m_szLine, _T("%s %d %d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[2], &i4Data.i[1]);
		i4Data.i[3] = 0;
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->TexFaceList[iLoop] = i4Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}")));

	///// CVERTLIST - �÷� ���ؽ�&���̽� ����Ʈ
	IF_FALSE_RETURN(FindWord(_T("*MESH_NUMCVERTEX")));
	_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
	// ASE ���Ͽ� ���� ������ ���ٸ� ���ǻ��� �߰�
	if (iData == 0)
	{
		// ���ؽ� �÷�
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->VertexList.size();
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColVertexList.resize(iLoopMax);
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColVertexList[iLoop] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		}

		// ���̽� �ε���
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->FaceList.size();
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColFaceList.resize(iLoopMax);
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			// ���̽��� �ε��� ��ȣ�� �����ϰ� ����
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColFaceList[iLoop] =
				static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->FaceList[iLoop];
		}
	}
	// ASE ���Ͽ� ���� ������ �ִٸ� �ش���� ����
	else
	{
		// ���ؽ� �÷�
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColVertexList.resize(iData);

		IF_FALSE_RETURN(FindWord(_T("*MESH_CVERTLIST")));
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColVertexList.size();
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			IF_FALSE_RETURN(FindWord(_T("*MESH_VERTCOL")));
			_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.y, &v3Data.z);
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColVertexList[iLoop] = v3Data;
		}
		IF_FALSE_RETURN(FindWord(_T("}")));

		// ���̽� �ε���
		IF_FALSE_RETURN(FindWord(_T("*MESH_NUMCVFACES")));
		_stscanf(m_szLine, _T("%s %d"), m_szWord, &iData);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColFaceList.resize(iData);

		i4Data.i[3] = 0; // �����ʴ� [3]��� �׻� 0���� ����
		IF_FALSE_RETURN(FindWord(_T("*MESH_CFACELIST")));
		iLoopMax = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColFaceList.size();
		for (iLoop = 0; iLoop < iLoopMax; iLoop++)
		{
			IF_FALSE_RETURN(FindWord(_T("*MESH_CFACE")));
			_stscanf(m_szLine, _T("%s %d %d%d%d"), m_szWord, &iData, &i4Data.i[0], &i4Data.i[2], &i4Data.i[1]);
			static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->ColFaceList[iLoop] = i4Data;
		}
		IF_FALSE_RETURN(FindWord(_T("}")));
	}

	///// NORMALS - �븻 ���ؽ�&���̽� ����Ʈ
	IF_FALSE_RETURN(FindWord(_T("*MESH_NORMALS")));
	iData = static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->FaceList.size();
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->NorFaceList.resize(iData);
	static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->NorVertexList.resize(iData * 3);

	iLoopMax = iData;
	for (iLoop = 0; iLoop < iLoopMax; iLoop++)
	{
		IF_FALSE_RETURN(FindWord(_T("*MESH_FACENORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->NorFaceList[iLoop] = v3Data;

		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEXNORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->NorVertexList[iLoop * 3 + 0] = v3Data;

		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEXNORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->NorVertexList[iLoop * 3 + 2] = v3Data;

		IF_FALSE_RETURN(FindWord(_T("*MESH_VERTEXNORMAL")));
		_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &iData, &v3Data.x, &v3Data.z, &v3Data.y);
		static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->NorVertexList[iLoop * 3 + 1] = v3Data;
	}
	IF_FALSE_RETURN(FindWord(_T("}"))); // MESH_NORMALS Ż��

	IF_FALSE_RETURN(FindWord(_T("}"))); // MESH Ż��

	///// TM_ANIMATION - �ִϸ��̼�
	_tcscpy(szWordArray[0], _T("*TM_ANIMATION"));
	_tcscpy(szWordArray[1], _T("*MATERIAL_REF"));
	switch (FindWordArray(szWordArray, 2))
	{
	case 0:		// *TM_ANIMATION	�ִϸ��̼��� �ִٸ� ====================================
	{
		m_pModel->m_ObjectList[iNumObj].bAnim = true;
		IF_FALSE_RETURN(ReadTMAnimation(iNumObj));

		IF_FALSE_RETURN(FindWord(_T("*MATERIAL_REF")));
		_stscanf(m_szLine, _T("%s %d"), m_szWord, &static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->iMaterialRef);
	}
	break;
	case 1:		// *MATERIAL_REF	�ִϸ��̼� ���� ��ٷ� ���͸��� REF�̸� =================
	{
		m_pModel->m_ObjectList[iNumObj].bAnim = false;
		_stscanf(m_szLine, _T("%s %d"), m_szWord, &static_cast<GeomObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->iMaterialRef);
	}
	break;
	case -1:	// ã�� �ܾ� ���� =========================================================
	default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
		return false;
		break;
	}

	IF_FALSE_RETURN(FindWord(_T("}"))); // GEOMOBJECT Ż��

	return hr;
}

bool bgParserASE::ReadHelperObject()
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH * 4];
	D3DXVECTOR3 v3Data;
	FaceInfo i4Data;
	int iData;
	int iLoop, iLoopMax;

	int iNumObj = m_pModel->m_ObjectList.size();
	m_pModel->m_ObjectList.resize(iNumObj + 1);
	m_pModel->m_ObjectList[iNumObj].vpObj = new HelperObject;
	m_pModel->m_ObjectList[iNumObj].eNodeType = OBJECT_NODE_TYPE_HELPEROBJECT;

	IF_FALSE_RETURN(ReadNodeInfo(iNumObj));

	IF_FALSE_RETURN(FindWord(_T("*BOUNDINGBOX_MIN")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord, &v3Data.x, &v3Data.z, &v3Data.y);
	static_cast<HelperObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->vBBoxMin = v3Data;

	IF_FALSE_RETURN(FindWord(_T("*BOUNDINGBOX_MAX")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord, &v3Data.x, &v3Data.z, &v3Data.y);
	static_cast<HelperObject*>(m_pModel->m_ObjectList[iNumObj].vpObj)->vBBoxMax = v3Data;

	///// TM_ANIMATION - �ִϸ��̼�
	_tcscpy(szWordArray[0], _T("*TM_ANIMATION"));
	_tcscpy(szWordArray[1], _T("}"));
	switch (FindWordArray(szWordArray, 2))
	{
	case 0:		// *TM_ANIMATION	�ִϸ��̼��� �ִٸ� ====================================
	{
		m_pModel->m_ObjectList[iNumObj].bAnim = true;
		IF_FALSE_RETURN(ReadTMAnimation(iNumObj));
	}
	break;
	case 1:		// }				�ִϸ��̼� ���� �����ٸ� ===============================
	{
		m_pModel->m_ObjectList[iNumObj].bAnim = false;
	}
	break;
	case -1:	// ã�� �ܾ� ���� =========================================================
	default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
		return false;
		break;
	}

	return hr;
}

bool bgParserASE::ReadShapeObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadCameraObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadLightObject()
{
	bool hr = true;
	return hr;
}

bool bgParserASE::ReadNodeInfo(int iNumObj)
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH * 4];

	// ������� ���� ����
	IF_FALSE_RETURN(FindWord(_T("*NODE_NAME")));
	ZeroMemory(m_pModel->m_ObjectList[iNumObj].szNodeName, MAX_PATH * 4);
	m_pszToken = _tcstok(m_szLine, _T("\""));
	m_pszToken = _tcstok(NULL, _T("\""));
	_tcscpy(m_pModel->m_ObjectList[iNumObj].szNodeName, m_pszToken);

	_tcscpy(szWordArray[0], _T("*NODE_PARENT"));
	_tcscpy(szWordArray[1], _T("*NODE_TM"));
	switch (FindWordArray(szWordArray, 2))
	{
	case 0:		// *NODE_PARENT		�θ� ��尡 �ִٸ� ============================
	{
		ZeroMemory(m_pModel->m_ObjectList[iNumObj].szNodeParent, MAX_PATH * 4);
		m_pszToken = _tcstok(m_szLine, _T("\""));
		m_pszToken = _tcstok(NULL, _T("\""));
		_tcscpy(m_pModel->m_ObjectList[iNumObj].szNodeParent, m_pszToken);

		IF_FALSE_RETURN(FindWord(_T("*NODE_TM")));
	}
	break;
	case 1:		// *NODE_TM			�θ� ��尡 ���ٸ� ============================
	{
		ZeroMemory(m_pModel->m_ObjectList[iNumObj].szNodeParent, MAX_PATH * 4);
		m_pModel->m_ObjectList[iNumObj].pNodeParent = NULL;
	}
	break;
	case -1:	// ã�� �ܾ� ���� =================================================
	default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
		return false;
		break;
	}

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW0")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._11,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._13,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._12);
	m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._14 = 0.0f;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW1")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._31,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._33,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._32);
	m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._34 = 0.0f;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW2")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._21,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._23,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._22);
	m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._24 = 0.0f;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROW3")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._41,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._43,
		&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._42);
	m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld._44 = 1.0f;

	// �ι��� ��Ʈ���� Ȯ�� �ڵ�
	D3DXVECTOR3 v0, v1, v2, v3;
	v0 = m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld.m[0];
	v1 = m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld.m[1];
	v2 = m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld.m[2];
	D3DXVec3Cross(&v3, &v1, &v2);
	if (D3DXVec3Dot(&v3, &v0) < 0.0f)
	{
		D3DXMATRIX matW;
		D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
		D3DXMatrixMultiply(&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld,
			&m_pModel->m_ObjectList[iNumObj].nodeTM.matWorld,
			&matW);
	}

	// ���ص� ������� ���� ����
	float			fAngle;
	D3DXQUATERNION	qRotate;
	D3DXVECTOR3		vVector, vAxis;
	D3DXMATRIX		matRotation, matRotationInv;

	IF_FALSE_RETURN(FindWord(_T("*TM_POS")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord, &vVector.x, &vVector.z, &vVector.y);
	m_pModel->m_ObjectList[iNumObj].nodeTM.vPos = vVector;

	D3DXMatrixTranslation(&m_pModel->m_ObjectList[iNumObj].matWorldPos, vVector.x, vVector.y, vVector.z);

	IF_FALSE_RETURN(FindWord(_T("*TM_ROTAXIS")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord, &vVector.x, &vVector.z, &vVector.y);
	m_pModel->m_ObjectList[iNumObj].nodeTM.vRotAxis = vVector;

	IF_FALSE_RETURN(FindWord(_T("*TM_ROTANGLE")));
	_stscanf(m_szLine, _T("%s %f"), m_szWord, &fAngle);
	m_pModel->m_ObjectList[iNumObj].nodeTM.fRotAngle = fAngle;

	D3DXQuaternionRotationAxis(&qRotate, &vVector, fAngle);
	D3DXMatrixRotationQuaternion(&m_pModel->m_ObjectList[iNumObj].matWorldRot, &qRotate);

	IF_FALSE_RETURN(FindWord(_T("*TM_SCALE")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord, &vVector.x, &vVector.z, &vVector.y);
	m_pModel->m_ObjectList[iNumObj].nodeTM.vScale = vVector;

	D3DXMatrixScaling(&m_pModel->m_ObjectList[iNumObj].matWorldScl, vVector.x, vVector.y, vVector.z);

	IF_FALSE_RETURN(FindWord(_T("*TM_SCALEAXIS")));
	_stscanf(m_szLine, _T("%s %f%f%f"), m_szWord, &vAxis.x, &vAxis.z, &vAxis.y);
	m_pModel->m_ObjectList[iNumObj].nodeTM.vScaleAxis = vAxis;

	IF_FALSE_RETURN(FindWord(_T("*TM_SCALEAXISANG")));
	_stscanf(m_szLine, _T("%s %f"), m_szWord, &fAngle);
	m_pModel->m_ObjectList[iNumObj].nodeTM.fScaleAxisAngle = fAngle;

	D3DXMatrixRotationAxis(&matRotation, &vAxis, fAngle);
	D3DXMatrixInverse(&matRotationInv, NULL, &matRotation);
	m_pModel->m_ObjectList[iNumObj].matWorldScl = matRotationInv * m_pModel->m_ObjectList[iNumObj].matWorldScl  * matRotation;

	IF_FALSE_RETURN(FindWord(_T("}"))); // NODE_TM Ż��

	return hr;
}

bool bgParserASE::ReadTMAnimation(int iNumObj)
{
	bool hr = true;

	TCHAR szWordArray[2][MAX_PATH * 4];
	TCHAR szWordTrack[4][MAX_PATH * 4];
	AnimTrackInfo animData;
	bool bAnimEnd, bTrackEnd;

	m_pModel->m_ObjectList[iNumObj].Anim.PosTrack.clear();
	m_pModel->m_ObjectList[iNumObj].Anim.RotTrack.clear();
	m_pModel->m_ObjectList[iNumObj].Anim.SclTrack.clear();

	_tcscpy(szWordTrack[0], _T("*CONTROL_POS_TRACK"));
	_tcscpy(szWordTrack[1], _T("*CONTROL_ROT_TRACK"));
	_tcscpy(szWordTrack[2], _T("*CONTROL_SCALE_TRACK"));
	_tcscpy(szWordTrack[3], _T("}"));

	bAnimEnd = false;
	while (bAnimEnd == false)
	{
		switch (FindWordArray(szWordTrack, 4))
		{
		case 0:		// *CONTROL_POS_TRACK ==========================================
		{
			_tcscpy(szWordArray[0], _T("*CONTROL_POS_SAMPLE"));
			_tcscpy(szWordArray[1], _T("}"));
			animData.Init();
			bTrackEnd = false;
			while (bTrackEnd == false)
			{
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *CONTROL_POS_SAMPLE	----------------------------
				{
					// Ʈ�� �߰�
					_stscanf(m_szLine, _T("%s %d %f%f%f"), m_szWord, &animData.iTick,
						&animData.vVector.x, &animData.vVector.z, &animData.vVector.y);
					m_pModel->m_ObjectList[iNumObj].Anim.PosTrack.push_back(animData);
					continue;
				}
				break;
				case 1:		// }					----------------------------
				{
					// Ʈ�� ���� ó��
					bTrackEnd = true;
				}
				break;
				case -1:	// ã�� �ܾ� ���� -----------------------------------
				default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
					return false;
					break;
				}
			}
		}
		break;
		case 1:		// *CONTROL_ROT_TRACK ==========================================
		{
			_tcscpy(szWordArray[0], _T("*CONTROL_ROT_SAMPLE"));
			_tcscpy(szWordArray[1], _T("}"));
			animData.Init();
			bTrackEnd = false;
			while (bTrackEnd == false)
			{
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *CONTROL_ROT_SAMPLE	----------------------------
				{
					// Ʈ�� �߰�
					_stscanf(m_szLine, _T("%s %d %f%f%f %f"), m_szWord, &animData.iTick,
						&animData.qRotate.x, &animData.qRotate.z, &animData.qRotate.y, &animData.qRotate.w);

					D3DXQuaternionRotationAxis(&animData.qRotate,
						&D3DXVECTOR3(animData.qRotate.x, animData.qRotate.y, animData.qRotate.z),
						animData.qRotate.w);

					int iRotTrackSize = m_pModel->m_ObjectList[iNumObj].Anim.RotTrack.size();
					if (iRotTrackSize)
					{
						D3DXQuaternionMultiply(&animData.qRotate,
							&m_pModel->m_ObjectList[iNumObj].Anim.RotTrack[iRotTrackSize - 1].qRotate,
							&animData.qRotate);
					}
					m_pModel->m_ObjectList[iNumObj].Anim.RotTrack.push_back(animData);
					continue;
				}
				break;
				case 1:		// }					----------------------------
				{
					// Ʈ�� ���� ó��
					bTrackEnd = true;
				}
				break;
				case -1:	// ã�� �ܾ� ���� -----------------------------------
				default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
					return false;
					break;
				}
			}
		}
		break;
		case 2:		// *CONTROL_SCALE_TRACK ========================================
		{
			_tcscpy(szWordArray[0], _T("*CONTROL_SCALE_SAMPLE"));
			_tcscpy(szWordArray[1], _T("}"));
			animData.Init();
			bTrackEnd = false;
			while (bTrackEnd == false)
			{
				switch (FindWordArray(szWordArray, 2))
				{
				case 0:		// *CONTROL_SCALE_SAMPLE	------------------------
				{
					// Ʈ�� �߰�
					_stscanf(m_szLine, _T("%s %d %f%f%f %f%f%f%f"), m_szWord, &animData.iTick,
						&animData.vVector.x, &animData.vVector.z, &animData.vVector.y,
						&animData.qRotate.x, &animData.qRotate.z, &animData.qRotate.y, &animData.qRotate.w);

					D3DXQuaternionRotationAxis(&animData.qRotate,
						&D3DXVECTOR3(animData.qRotate.x, animData.qRotate.y, animData.qRotate.z),
						animData.qRotate.w);

					m_pModel->m_ObjectList[iNumObj].Anim.SclTrack.push_back(animData);
					continue;
				}
				break;
				case 1:		// }					----------------------------
				{
					// Ʈ�� ���� ó��
					bTrackEnd = true;
				}
				break;
				case -1:	// ã�� �ܾ� ���� -----------------------------------
				default:	// ������ (�迭 ��Ұ� 2���̹Ƿ� ���� �� ����)
					return false;
					break;
				}
			}
		}
		break;
		case 3:		// }	========================================================
		{
			// �ִϸ��̼� ���� ó��
			bAnimEnd = true;
		}
		break;
		case -1:	// ã�� �ܾ� ���� ===============================================
		default:	// ������ (�迭 ��Ұ� 4���̹Ƿ� ���� �� ����)
			return false;
			break;
		}
	}

	return hr;
}

/*
ship.ase

*MESH_NUM     VERTEX 2656
*MESH_NUM     FACES  4158
*MESH_NUM Tex VERTEX 3803
*MESH_NUM Tex VFACES 4158
*MESH_NUM Col VERTEX 2656
*MESH_NUM Col VFACES 4158
*MESH_  FACE  NORMAL 4158
*MESH_ VERTEX NORMAL 4158*3
*/

// ASE ���Ͽ��� ���� �����͸� ��(��¿�) �����ͷ� ��ȯ
void bgParserASE::ConvertToModel()
{
	int iLoop, iLoopSub;
	int iObj, iFace, iTri;
	int iNumMaterial;
	int iNumSubMaterial;
	int iMaterialRef;

	// ���͸��� �ؽ��� �ε�
	m_pModel->m_TexIDList.resize(m_pModel->m_MaterialList.size());
	iNumMaterial = 0;
	for (iLoop = 0; iLoop < m_pModel->m_MaterialList.size(); iLoop++)
	{
		// ���� ���͸����� ������
		if (m_pModel->m_MaterialList[iLoop].SubMaterialList.size() > 0)
		{
			iNumSubMaterial = m_pModel->m_MaterialList[iLoop].SubMaterialList.size();
			m_pModel->m_TexIDList[iLoop].SubIDList.resize(iNumSubMaterial);
			for (iLoopSub = 0; iLoopSub < iNumSubMaterial; iLoopSub++)
			{
				iNumMaterial++;
				m_pModel->m_TexIDList[iLoop].SubIDList[iLoopSub].iID =
					I_TextureMgr.Add(m_pModel->m_MaterialList[iLoop].SubMaterialList[iLoopSub].szBitmap);
			}
		}
		// ���� ���͸����� ������
		else
		{
			iNumMaterial++;
			m_pModel->m_TexIDList[iLoop].iID = I_TextureMgr.Add(m_pModel->m_MaterialList[iLoop].szBitmap);
		}
	}

	// �������� �з��Ͽ� �����迭�� ������ �߰�
	VertexPNCT vertexTemp;
	int iSubTexID, iTriIndex;
	for (iObj = 0; iObj < m_pModel->m_ObjectList.size(); iObj++)
	{
		switch (m_pModel->m_ObjectList[iObj].eNodeType)
		{
		case OBJECT_NODE_TYPE_GEOMOBJECT:
		{
			GeomObject* GeomObj = static_cast<GeomObject*>(m_pModel->m_ObjectList[iObj].vpObj);
			iMaterialRef = GeomObj->iMaterialRef;
			iNumSubMaterial = m_pModel->m_MaterialList[iMaterialRef].SubMaterialList.size();

			if (iNumSubMaterial == 0)
				m_pModel->m_ObjectList[iObj].m_VertexList.resize(1);
			else
				m_pModel->m_ObjectList[iObj].m_VertexList.resize(iNumSubMaterial);

			for (iFace = 0; iFace < GeomObj->FaceList.size(); iFace++)
			{
				// ������͸����� ������ ���� ID�� ��� (0��)
				if (iNumSubMaterial == 0)
					iSubTexID = 0;
				// MTLID �� ������͸��� �������� ũ�� 0 ������ ���
				else if (GeomObj->FaceList[iFace].iID >= m_pModel->m_MaterialList[GeomObj->iMaterialRef].SubMaterialList.size())
					iSubTexID = GeomObj->FaceList[iFace].iID = 0;
				else
					iSubTexID = GeomObj->FaceList[iFace].iID;

				for (iTri = 0; iTri < 3; iTri++)
				{
					iTriIndex = GeomObj->FaceList[iFace].i[iTri];
					vertexTemp.pos = GeomObj->VertexList[iTriIndex];
					vertexTemp.nor = GeomObj->NorVertexList[iFace * 3 + iTri];
					iTriIndex = GeomObj->ColFaceList[iFace].i[iTri];
					vertexTemp.col = D3DXVECTOR4(GeomObj->ColVertexList[iTriIndex], 1.0f);
					iTriIndex = GeomObj->TexFaceList[iFace].i[iTri];
					vertexTemp.tex = D3DXVECTOR2(GeomObj->TexVertexList[iTriIndex].x, 1.0f - GeomObj->TexVertexList[iTriIndex].y);
					m_pModel->m_ObjectList[iObj].m_VertexList[iSubTexID].push_back(vertexTemp);
				}
			}
		}
		break;
		case OBJECT_NODE_TYPE_HELPEROBJECT:
		{
			HelperObject* HelperObj = static_cast<HelperObject*>(m_pModel->m_ObjectList[iObj].vpObj);
			// ���ۿ�����Ʈ�� ���� ó��
		}
		break;
		}
	}

	// �ؽ��� ������(iLoop => MTLID)�� �ش������� ����(iLoopSub)��ŭ �ݺ��ϸ� �ε�����ȣ ����
	for (iObj = 0; iObj < m_pModel->m_ObjectList.size(); iObj++)
	{
		if (m_pModel->m_ObjectList[iObj].eNodeType == OBJECT_NODE_TYPE_GEOMOBJECT)
		{
			GeomObject* GeomObj = static_cast<GeomObject*>(m_pModel->m_ObjectList[iObj].vpObj);
			iMaterialRef = GeomObj->iMaterialRef;
			iNumSubMaterial = m_pModel->m_MaterialList[iMaterialRef].SubMaterialList.size();

			if (iNumSubMaterial > 0)
			{
				m_pModel->m_ObjectList[iObj].m_IndexList.resize(iNumSubMaterial);

				for (iLoopSub = 0; iLoopSub < iNumSubMaterial; iLoopSub++)
				{
					for (iLoop = 0; iLoop < m_pModel->m_ObjectList[iObj].m_VertexList[iLoopSub].size(); iLoop++)
					{
						m_pModel->m_ObjectList[iObj].m_IndexList[iLoopSub].push_back(iLoop);
					}
				}
			}
			else
			{
				m_pModel->m_ObjectList[iObj].m_IndexList.resize(1);

				for (iLoop = 0; iLoop < m_pModel->m_ObjectList[iObj].m_VertexList[0].size(); iLoop++)
				{
					m_pModel->m_ObjectList[iObj].m_IndexList[0].push_back(iLoop);
				}
			}
		}
	}
}

void bgParserASE::LinkNode()
{
	int iNode, iFindParent;

	// ��ü ����� ���� ����
	for (iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		// �θ����� �̸��� �������� Ž��
		if (_tcsicmp(m_pModel->m_ObjectList[iNode].szNodeParent, _T("")))
		{
			for (iFindParent = 0; iFindParent < m_pModel->m_ObjectList.size(); iFindParent++)
			{
				// �θ��带 ã���� �θ���� �ڽĳ�� ���ΰ��� ������ ����
				if (!_tcsicmp(m_pModel->m_ObjectList[iNode].szNodeParent, m_pModel->m_ObjectList[iFindParent].szNodeName))
				{
					m_pModel->m_ObjectList[iNode].pNodeParent = &m_pModel->m_ObjectList[iFindParent];
					m_pModel->m_ObjectList[iFindParent].pNodeChildList.push_back(&m_pModel->m_ObjectList[iNode]);
					break;
				}
			}

			// ã�� �θ��尡 ������ (�������� ASE �������̸� �׻� �����̾�� ��)
			if (iFindParent >= m_pModel->m_ObjectList.size())
			{
				m_pModel->m_ObjectList[iNode].pNodeParent = NULL;
			}
		}
	}

	// ���ۿ�����Ʈ�̰� �ڽĳ�尡 ������ ���� (������ �ʴ� ������Ʈ)
	for (iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		if (m_pModel->m_ObjectList[iNode].eNodeType == OBJECT_NODE_TYPE_HELPEROBJECT &&
			m_pModel->m_ObjectList[iNode].pNodeChildList.size() == 0)
		{
			// ���� ó��
		}
	}

	iNode = 0;
}

void bgParserASE::OperationTM()
{
	D3DXMATRIX* matParentWorld;
	D3DXMATRIX matChildWorld;
	D3DXMATRIX matInvParentWorld;
	D3DXMATRIX matPos, matRot, matScl;
	D3DXVECTOR3 vPos, vScl;
	D3DXQUATERNION qRot;
	// �ͷ� �ִϸ��̼� ��! (0������ ��� ����� �ʿ�)
	for (int iNode = 0; iNode < m_pModel->m_ObjectList.size(); iNode++)
	{
		if (m_pModel->m_ObjectList[iNode].pNodeParent)
		{
			matParentWorld = &m_pModel->m_ObjectList[iNode].pNodeParent->nodeTM.matWorld;

			D3DXMatrixInverse(&matInvParentWorld, NULL, matParentWorld);
			matChildWorld = m_pModel->m_ObjectList[iNode].nodeTM.matWorld * matInvParentWorld;
			D3DXMatrixDecompose(&vScl, &qRot, &vPos, &matChildWorld);
			D3DXMatrixScaling(&m_pModel->m_ObjectList[iNode].matWorldScl, vScl.x, vScl.y, vScl.z);
			D3DXMatrixTranslation(&m_pModel->m_ObjectList[iNode].matWorldPos, vPos.x, vPos.y, vPos.z);
			D3DXMatrixRotationQuaternion(&m_pModel->m_ObjectList[iNode].matWorldRot, &qRot);
		}
	}
}
