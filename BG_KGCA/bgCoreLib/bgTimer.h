#pragma once
#include "bgStd.h"

const int FRAME_BITSET_SIZE = 1000;
const float FPS_UPDATE_DELAY = 0.1f;

class bgTimer : public bgSingleton<bgTimer>
{
public:
	LARGE_INTEGER	m_Frequency;				// 현재 시스템의 초당 주파수
	LARGE_INTEGER	m_AppStartTime;				// 프로그램 시작 시간
	LARGE_INTEGER	m_Current;					// 현재 시간
	LARGE_INTEGER	m_PrevFrame;				// 1프레임 이전 시간
	LARGE_INTEGER	m_PrevSecond;				// 1초 이전 시간
	float			m_fSPF;						// 1프레임 경과 시간 (이전프레임 시간부터의 길이)
	int				m_iFPS;						// 1초당 프레임수 (FPS)
	int				m_iCountFrame;				// 프레임 카운트 (1초동안)

	bitset<FRAME_BITSET_SIZE>	m_FrameBitset;	// 캡쳐된 프레임의 시간에 해당하는 비트만 on시킬 비트셋
	float			m_fUpdateDelay;				// 업데이트 간격
	LARGE_INTEGER	m_PrevUpdate;				// 이전 업데이트 시간

	LARGE_INTEGER	m_EventStart;				// 시작 시간
	LARGE_INTEGER	m_EventElapse;				// 경과 시간
	float			m_fEventLength;				// 종료 시간 (시작시간부터의 길이)
	bool			m_bStarted;					// 활성 상태

public:
	bgTimer();
	virtual ~bgTimer();

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	void	Reset();
	void	Start();
	void	Stop();
	float	GetElapsedTime();

	bool	IsStarted() { return m_bStarted; }
	int		GetFPS() { return m_iFPS; }
	float	GetSPF() { return m_fSPF; }
};
