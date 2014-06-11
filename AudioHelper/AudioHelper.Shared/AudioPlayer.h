#pragma once

using namespace std;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Metadata;

namespace AudioHelper
{
	public ref class AudioPlayer sealed
    {
	private:
		interface IXAudio2* m_pAudioEngine;
		interface IXAudio2MasteringVoice* m_pMasteringVoice;
		map<const wstring, shared_ptr<AudioBuffer>> m_audioBufferList;

		void Initialize(uint32 sampleRate);
		~AudioPlayer();
		shared_ptr<AudioBuffer> LoadAudio(IBuffer^ buf);
		bool CheckAudioCacheAndPlay(wstring audioId);
		bool LoadAndPlayAudio(wstring audioId, IBuffer^ buf);
		bool InternalPlayAudio(shared_ptr<AudioBuffer> audioBuffer);

	public:
        AudioPlayer();

		[DefaultOverload]
		IAsyncOperation<bool>^ PlayAudio(String^ file);
		IAsyncOperation<bool>^ PlayAudio(String^ synthesizedText, IBuffer^ buf);

		static property AudioPlayer^ Instance {
			AudioPlayer^ get()
			{
				static AudioPlayer^ inst = ref new AudioPlayer();
				return inst;
			}
		}
    };
}