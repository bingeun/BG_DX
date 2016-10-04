#include "bgGlobalMax.h"

// =======================================================================================

void bgMatrixIndex::Add(int iIndex, T_STR szName)
{
	m_pObject = NULL;
	m_pControl = NULL;
	m_iIndex = iIndex;
	m_szName = szName;
}

void bgMatrixIndex::SetNode(INode* pNode, int iTick, bgMatrixIndex* pParentIndex)
{
	m_pINode = pNode;
	m_pObject = m_pINode->GetObjectRef();
	m_pControl = m_pINode->GetTMController();

	m_ClassType = CLASS_GEOM;
	if (m_pObject)
	{
		if (m_pObject->ClassID() == Class_ID(BONE_CLASS_ID, 0))
			m_ClassType = CLASS_BONE;
		if (m_pObject->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
			m_ClassType = CLASS_DUMMY;
	}
	if (m_pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID || m_pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)
		m_ClassType = CLASS_BIPED;

	m_NodeTM = pNode->GetNodeTM(iTick);
	m_InvNodeTM = Inverse(m_NodeTM);
}

void bgMatrixIndex::Release()
{
	for (int iSub = 0; iSub < m_Mesh.pSubMeshes.size(); iSub++)
		SAFE_DEL(m_Mesh.pSubMeshes[iSub]);

	m_Mesh.pSubMeshes.clear();
}

// =======================================================================================

bgGlobalMax::bgGlobalMax()
{
	m_p3DMax = NULL;
	m_pRootNode = NULL;
}

bgGlobalMax::~bgGlobalMax()
{
	m_MatrixMap.Release();
}

void bgGlobalMax::DumpMatrix3(Matrix3* m, D3D_MATRIX& mat)
{
	Point3 row;
	row = m->GetRow(0); mat._11 = row.x; mat._12 = row.z; mat._13 = row.y; mat._14 = 0.0f;
	row = m->GetRow(2); mat._21 = row.x; mat._22 = row.z; mat._23 = row.y; mat._24 = 0.0f;
	row = m->GetRow(1); mat._31 = row.x; mat._32 = row.z; mat._33 = row.y; mat._34 = 0.0f;
	row = m->GetRow(3); mat._41 = row.x; mat._42 = row.z; mat._43 = row.y; mat._44 = 1.0f;
}

void bgGlobalMax::DumpPoint3(Point3& pDest, Point3& pSrc)
{
	pDest.x = pSrc.x;
	pDest.y = pSrc.z;
	pDest.z = pSrc.y;
}

bool bgGlobalMax::EqualPoint2(Point2 p1, Point2 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;

	return true;
}

bool bgGlobalMax::EqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return false;

	return true;
}

bool bgGlobalMax::EqualPoint4(Point4 p1, Point4 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return false;
	if (fabs(p1.w - p2.w) > ALMOST_ZERO)
		return false;

	return true;
}

bool bgGlobalMax::CheckNegativeTM(Matrix3& m)
{
	return (DotProd(CrossProd(m.GetRow(0), m.GetRow(1)), m.GetRow(2)) < 0.0) ? 1 : 0;
}

TCHAR* bgGlobalMax::FixupName(MSTR name)
{
	TCHAR* szPtr = m_szTemp;

	memset(m_szTemp, 0, sizeof(TCHAR) * MAX_PATH);
	_tcscpy(m_szTemp, name);

	while (*szPtr)
	{
		if (*szPtr == '"')
			*szPtr = SINGLE_QUOTE;
		else if (*szPtr == ' ' || *szPtr <= CTL_CHARS)
			*szPtr = _T('_');

		szPtr++;
	}

	return m_szTemp;
}

bool bgGlobalMax::Initialize(Interface* p3DMax)
{
	m_p3DMax = p3DMax;
	m_pRootNode = p3DMax->GetRootNode();

	if (m_p3DMax == NULL && m_pRootNode == NULL)
		return false;
	if (CheckFile(m_p3DMax) == false)
		return true;

	m_Interval = m_p3DMax->GetAnimRange();

	memset(&m_Scene, 0, sizeof(m_Scene));
	m_Scene.iVersion = 100;
	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	m_Scene.iFrameSpeed = GetFrameRate();
	m_Scene.iTickPerFrame = GetTicksPerFrame();

	PreProcess(m_pRootNode);

	m_Scene.iNumMesh = m_MatrixMap.Count();
	m_Scene.iMaxWeight = 1;

	return true;
}

void bgGlobalMax::PreProcess(INode* pNode)
{
	if (pNode == NULL)
		return;

	AddObject(pNode);
	AddMtl(pNode);

	int iNumChildren = pNode->NumberOfChildren();
	for (int iCnt = 0; iCnt < iNumChildren; iCnt++)
	{
		INode* pChild = pNode->GetChildNode(iCnt);
		PreProcess(pChild);
	}
}

bool bgGlobalMax::CheckFile(Interface* p3DMax)
{
	TSTR strCurrentFileName = FixupName(p3DMax->GetCurFileName());
	if (strCurrentFileName.isNull() == false && m_strCurrentFileName == strCurrentFileName)
		return false;

	m_MatrixMap.Release();
	m_pMtlList.clear();

	m_strCurrentFileName = strCurrentFileName;

	return true;
}

void bgGlobalMax::AddObject(INode* pNode)
{
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());
	if (os.obj)
	{
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID:
		case HELPER_CLASS_ID:
		{
			int iIndex = m_MatrixMap.Add(pNode->GetName());
			TSTR parentName = pNode->GetParentNode()->GetName();
			m_MatrixMap.GetPtr(iIndex)->SetNode(pNode, m_Interval.Start(), m_MatrixMap.GetPtr(parentName));
		}
		break;
		}
	}
}

void bgGlobalMax::AddMtl(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();

	if (pMtl != NULL)
	{
		bool bAdd = true;

		for (int iCnt = 0; iCnt < m_pMtlList.size(); iCnt++)
		{
			// 중복일 경우
			if (m_pMtlList[iCnt] == pMtl)
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

int bgGlobalMax::GetMtlRef(Mtl* pMtl)
{
	for (int iMtl = 0; iMtl < m_pMtlList.size(); iMtl++)
	{
		if (m_pMtlList[iMtl] == pMtl)
		{
			return iMtl;
		}
	}
	return  -1;
}
