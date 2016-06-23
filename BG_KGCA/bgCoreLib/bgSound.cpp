#include "bgSound.h"



int bgSound::Load(char * pLoadName, bool bPlay)
{
	m_pSystem->createSound(pLoadName, FMOD_HARDWARE, 0, &m_pSound[m_iSoundList++]);
	return m_iSoundList - 1;
}

void bgSound::Play(int iIndex, bool bPlay)
{
	bool playing = false;
	if (bPlay == false)
	{
		if (m_pChannel[iIndex] != NULL)
		{
			m_pChannel[iIndex]->isPlaying(&playing);
		}
	}
	if (playing == false)
	{
		m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pSound[iIndex], false, &m_pChannel[iIndex]);
	}
}

void bgSound::Stop(int iIndex)
{
	m_pChannel[iIndex]->stop();
}

void bgSound::Stop()
{
	for (int iIndex = 0; iIndex < g_iMaxSound; iIndex++)
	{
		m_pChannel[iIndex]->stop();
	}
}

void bgSound::Pause(int iIndex)
{
	bool paused;
	m_pChannel[iIndex]->getPaused(&paused);
	m_pChannel[iIndex]->setPaused(!paused);
}

void bgSound::Volume(int iIndex, float fVolume, bool bUp)
{
	float fCurrentVolume;
	m_pChannel[iIndex]->getVolume(&fCurrentVolume);
	if (bUp)
		fCurrentVolume += fVolume;
	else
		fCurrentVolume -= fVolume;
	m_pChannel[iIndex]->setVolume(fCurrentVolume);
}

void bgSound::SetLoop(int iIndex)
{
	m_pSound[iIndex]->setMode(FMOD_LOOP_NORMAL);
}

bool bgSound::Init()
{
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(32, FMOD_INIT_NORMAL, 0);
	return true;
}

bool bgSound::Frame()
{
	unsigned int total;
	unsigned int current;

	m_pSystem->update();

	m_pSound[0]->getLength(&total, FMOD_TIMEUNIT_MS);
	m_pChannel[0]->getPosition(&current, FMOD_TIMEUNIT_MS);
	_stprintf_s(m_szBuffer, _T("[%02d:%02d:%02d] [%02d:%02d:%02d]"),	// [현재 진행] [전체 길이]
		current / 1000 / 60, current / 1000 % 60, current / 10 % 60,	// 현재 진행
		total / 1000 / 60, total / 1000 % 60, total / 10 % 60);			// 전체 길이
	return true;
}

bool bgSound::Release()
{
	for (int iSound = 0; iSound < m_iSoundList; iSound++)
	{
		m_pSound[iSound]->release();
	}
	m_iSoundList = 0;
	m_pSystem->close();
	m_pSystem->release();
	return true;
}

bgSound::bgSound()
{
}

bgSound::~bgSound()
{
	Release();
}
