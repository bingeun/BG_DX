#include "bgAnimMax.h"

bgAnimMax::bgAnimMax()
{
	m_iDeltaTick = 160;
}

bgAnimMax::~bgAnimMax()
{
}

void bgAnimMax::DumpPosSample(INode* pNode, bgMesh* pMesh)
{
	TimeValue start = m_Interval.Start();
	TimeValue end = m_Interval.End();
	AffineParts ap;

	for (TimeValue t = start; t <= end; t += m_iDeltaTick)
	{
		GetDecompAffine(t, pNode, &ap);

		bgAnimTrack Anim;
		Anim.iTick = t;
		Anim.vValue.x = ap.t.x;
		Anim.vValue.y = ap.t.z;
		Anim.vValue.z = ap.t.y;
		pMesh->PosTrack.push_back(Anim);
	}
}

void bgAnimMax::DumpRotSample(INode* pNode, bgMesh* pMesh)
{
	TimeValue start = m_Interval.Start();
	TimeValue end = m_Interval.End();
	AffineParts ap;

	for (TimeValue t = start; t <= end; t += m_iDeltaTick)
	{
		GetDecompAffine(t, pNode, &ap);

		bgAnimTrack Anim;
		Anim.iTick = t;
		Anim.qValue.x = ap.q.x;
		Anim.qValue.y = ap.q.z;
		Anim.qValue.z = ap.q.y;
		Anim.qValue.w = ap.q.w;
		pMesh->RotTrack.push_back(Anim);
	}
}

void bgAnimMax::DumpSclSample(INode* pNode, bgMesh* pMesh)
{
	TimeValue start = m_Interval.Start();
	TimeValue end = m_Interval.End();
	AffineParts ap;

	for (TimeValue t = start; t <= end; t += m_iDeltaTick)
	{
		GetDecompAffine(t, pNode, &ap);

		bgAnimTrack Anim;
		Anim.iTick = t;
		Anim.vValue.x = ap.k.x;
		Anim.vValue.y = ap.k.z;
		Anim.vValue.z = ap.k.y;
		Anim.qValue.x = ap.u.x;
		Anim.qValue.y = ap.u.z;
		Anim.qValue.z = ap.u.y;
		Anim.qValue.w = ap.u.w;
		pMesh->SclTrack.push_back(Anim);
	}
}

void bgAnimMax::DumpFloatKeys(Control* pControl, bgMesh* pMesh)
{
	if (!pControl)
		return;

	int i;
	bgAnimTrack Anim;
	IKeyControl *ikc = NULL;

	ikc = GetKeyControlInterface(pControl);
	if (ikc && pControl->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0))
	{
		for (i = 0; i < ikc->GetNumKeys(); i++)
		{
			ITCBFloatKey key;
			ikc->GetKey(i, &key);
			Anim.iTick = key.time;
			Anim.vValue.x = key.val;
			pMesh->AlpTrack.push_back(Anim);
		}
	}
	else if (ikc && pControl->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0))
	{
		for (i = 0; i < ikc->GetNumKeys(); i++)
		{
			IBezFloatKey key;
			ikc->GetKey(i, &key);
			Anim.iTick = key.time;
			Anim.vValue.x = key.val;
			pMesh->AlpTrack.push_back(Anim);
		}
	}
	else if (ikc && pControl->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0))
	{
		for (i = 0; i < ikc->GetNumKeys(); i++)
		{
			ILinFloatKey key;
			ikc->GetKey(i, &key);
			Anim.iTick = key.time;
			Anim.vValue.x = key.val;
			pMesh->AlpTrack.push_back(Anim);
		}
	}
	else
	{
		TSTR name;
		pControl->GetClassName(name);

		if (pControl->IsAnimated())
		{
			float value;
			Interval range;
			Interval animRange = m_p3DMax->GetAnimRange();
			TimeValue t = pControl->GetTimeRange(TIMERANGE_ALL).Start();

			while (animRange.InInterval(t))
			{
				range = FOREVER;
				pControl->GetValue(t, &value, range);

				t = range.Start();

				Anim.iTick = t;
				Anim.vValue.x = value;
				pMesh->AlpTrack.push_back(Anim);

				if (range.End() > pControl->GetTimeRange(TIMERANGE_ALL).End())
				{
					break;
				}
				else
				{
					t = (range.End() / GetTicksPerFrame()) * GetTicksPerFrame();
				}
			}
		}
	}
}

void bgAnimMax::GetAnimKeys(INode* pNode, bgMesh* pMesh)
{
	BOOL bPosAnim;
	BOOL bRotAnim;
	BOOL bSclAnim;

	m_iDeltaTick = m_Scene.iTickPerFrame;

	if (CheckForAnimation(pNode, bPosAnim, bRotAnim, bSclAnim))
	{
		if (bPosAnim)
		{
			pMesh->PosTrack.clear();
			DumpPosSample(pNode, pMesh);
		}
		if (bRotAnim)
		{
			pMesh->RotTrack.clear();
			DumpRotSample(pNode, pMesh);
		}
		if (bSclAnim)
		{
			pMesh->SclTrack.clear();
			DumpSclSample(pNode, pMesh);
		}
	}

	Control* AlpCont = pNode->GetVisController();
	if (AlpCont)
	{
		pMesh->AlpTrack.clear();
		DumpFloatKeys(AlpCont, pMesh);
	}
}

bool bgAnimMax::GetDecompAffine(TimeValue time, INode* pNode, AffineParts* pAP, Point3* pRotAxis, float* pRotAngle)
{
	Matrix3 tm = pNode->GetNodeTM(time) * Inverse(pNode->GetParentTM(time));
	decomp_affine(tm, pAP);

	Point3 vRotAxis;
	float  fRotAngle;
	if (pRotAngle != NULL && pRotAngle != NULL)
	{
		AngAxisFromQ(pAP->q, pRotAngle, *pRotAxis);
	}
	return true;
}

BOOL bgAnimMax::CheckForAnimation(INode* pNode, BOOL& bPos, BOOL& bRot, BOOL& bScl)
{
	AffineParts ap;
	Point3 firstPos, firstRotAxis, firstScaleFactor;
	Point3 rotAxis;
	float firstRotAngle;
	float rotAngle;

	GetDecompAffine(m_Interval.Start(), pNode, &ap, &firstRotAxis, &firstRotAngle);
	firstPos = ap.t;
	firstScaleFactor = ap.k;

	TimeValue start = m_Interval.Start() + GetTicksPerFrame();
	TimeValue end = m_Interval.End();

	bPos = bRot = bScl = FALSE;

	for (TimeValue t = start; t <= end; t += m_iDeltaTick)
	{
		GetDecompAffine(t, pNode, &ap, &rotAxis, &rotAngle);

		if (!bPos)
		{
			if (!EqualPoint3(ap.t, firstPos))
			{
				bPos = TRUE;
			}
		}
		if (!bRot)
		{
			if (fabs(rotAngle - firstRotAngle) > ALMOST_ZERO)
			{
				bRot = TRUE;
			}
			else if (!EqualPoint3(rotAxis, firstRotAxis))
			{
				bRot = TRUE;
			}
		}
		if (!bScl)
		{
			if (!EqualPoint3(ap.k, firstScaleFactor))
			{
				bScl = TRUE;
			}
		}

		if (bPos && bRot && bScl)
			break;
	}
	return bPos || bRot || bScl;
}
