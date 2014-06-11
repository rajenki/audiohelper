#pragma once

using namespace std;
using namespace Platform;
using namespace Windows::Storage::Streams;

namespace AudioHelper
{
	//
	// 4 Character Tags in the RIFF File of Interest (read backwards)
	//
	const uint32 FOURCC_RIFF_TAG = 'FFIR';
	const uint32 FOURCC_FORMAT_TAG = ' tmf';
	const uint32 FOURCC_DATA_TAG = 'atad';
	const uint32 FOURCC_WAVE_FILE_TAG = 'EVAW';

	//
	// The header of every 'chunk' of data in the RIFF file
	//
	struct ChunkHeader
	{
		uint32 tag;
		uint32 size;
	};

	class AudioCallbackHandler : public IXAudio2VoiceCallback
	{
	public:
		AudioCallbackHandler() {}
		~AudioCallbackHandler()
		{
		}

		STDMETHOD_(void, OnVoiceProcessingPassStart) (UINT32 bytesRequired);
		STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS);
		STDMETHOD_(void, OnStreamEnd) ();
		STDMETHOD_(void, OnBufferStart) (void* bufferContext);
		STDMETHOD_(void, OnBufferEnd) (void* bufferContext);
		STDMETHOD_(void, OnLoopEnd) (void* bufferContext);
		STDMETHOD_(void, OnVoiceError) (void* bufferContext, HRESULT error);
	};

	class AudioBuffer
	{
	private:
		IBuffer^ waveBuffer;
		byte* pRawBuffer;

		uint64 FindChunk(uint32 tag, uint64 startLoc, uint64 endLoc);
	public:
		AudioBuffer(IBuffer^ waveBuffer);
		bool ParseAudioBuffer();
		IXAudio2SourceVoice* pSourceVoice;
		PWAVEFORMATEX pWex;
		AudioCallbackHandler callbackHandler;
		uint64 audioDataLength;
		byte* pAudioData;
	};
}
