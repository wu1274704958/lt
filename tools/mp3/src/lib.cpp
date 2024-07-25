#include <mp3.h>
#include <MusicPlayer.h>
#include <Pumper.h>


fv::MusicPlayer* mp = nullptr;
fv::Pumper* pumper = nullptr;


extern "C" {

	void MP3_init(const wchar_t* rootDir)
	{
		mp = new fv::MusicPlayer();
		pumper = new fv::Pumper(*mp);

		pumper->init(rootDir);
	}
	void MP3_release()
	{
		if (pumper != nullptr)
		{
			delete pumper;
			pumper = nullptr;
		}

		if (mp != nullptr)
		{
			delete mp;
			mp = nullptr;
		}
	}

	void MP3_getFFTData(float* data, unsigned int sizeTag)
	{
		if (mp != nullptr)
			mp->getData(data, sizeTag);
	}
	void MP3_tick()
	{
		if (!mp->isOff() && mp->getActive() == BASS_ACTIVE_STOPPED && pumper->getMode() != fv::Pumper::NONE)
		{
			pumper->pump();
		}
	}
	void MP3_setMode(int mode)
	{
		if (pumper != nullptr)
			pumper->setMode((fv::Pumper::PUMP_MODE)mode);
	}
	
	void MP3_pump()
	{
		if (pumper != nullptr)
			pumper->pump();
	}

	int MP3_setNext(const wchar_t* name)
	{
		if (pumper != nullptr)
			return pumper->setNextMusic(name) ? 1 : 0;
		return 0;
	}

}