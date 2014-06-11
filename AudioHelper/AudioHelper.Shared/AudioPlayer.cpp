// AudioPlayer.cpp

#include "pch.h"
#include "AudioBuffer.h"
#include "AudioPlayer.h"
#include "DebugOut.h"

using namespace AudioHelper;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;

AudioPlayer::AudioPlayer()
{
	Initialize(48000);
}

AudioPlayer::~AudioPlayer()
{
	if (m_pMasteringVoice != nullptr) {
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	if (m_pAudioEngine != nullptr) {
		m_pAudioEngine->Release();
		m_pAudioEngine = nullptr;
	}
}

void AudioPlayer::Initialize(uint32 sampleRate)
{
	HRESULT hr;

	hr = XAudio2Create(&m_pAudioEngine, 0);
	if (FAILED(hr)) {
		DebugOut(L"XAudio2Create failed = %X", hr);
		return;
	}
	hr = m_pAudioEngine->CreateMasteringVoice(&m_pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, sampleRate);
	if (FAILED(hr)) {
		DebugOut(L"CreateMasteringVoice failed = %X", hr);
	}
}

shared_ptr<AudioBuffer> AudioPlayer::LoadAudio(IBuffer^ buf)
{
	auto audioBuffer = make_shared<AudioBuffer>(buf);
	audioBuffer->ParseAudioBuffer();
	HRESULT hr = m_pAudioEngine->CreateSourceVoice(&audioBuffer->pSourceVoice, audioBuffer->pWex, 0, XAUDIO2_DEFAULT_FREQ_RATIO,
												   &audioBuffer->callbackHandler, NULL, NULL);
	if (FAILED(hr)) {
		DebugOut(L"CreateSourceVoicefailed = %X", hr);
	}
	return audioBuffer;
}

IAsyncOperation<bool>^ AudioPlayer::PlayAudio(String^ file)
{
	wstring audioId(file->Data());

	if (CheckAudioCacheAndPlay(audioId)) {
		return create_async([=]() {
			return true;
		});
	} else {
		return create_async([=]() {
			String^ fileName = ref new String(L"ms-appx:///");
			fileName += file;
			auto uri = ref new Uri(fileName);
			return create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).then([=](task<StorageFile^> getFileTask) {
				auto waveFile = getFileTask.get();
				return create_task(FileIO::ReadBufferAsync(waveFile)).then([=](task<IBuffer^> readBufferAsyncTask) {
					auto buf = readBufferAsyncTask.get();
					return LoadAndPlayAudio(audioId, buf);
				});
			});
		});
	}
}

IAsyncOperation<bool>^ AudioPlayer::PlayAudio(String^ synthesizedText, IBuffer^ buf)
{
	wstring audioId(synthesizedText->Data());

	if (CheckAudioCacheAndPlay(audioId)) {
		return create_async([=]() {
			return true;
		});
	} else {
		return create_async([=]() {
			return LoadAndPlayAudio(audioId, buf);
		});
	}
}

bool AudioPlayer::CheckAudioCacheAndPlay(wstring audioId)
{
	map<const wstring, shared_ptr<AudioBuffer>>::const_iterator it;

	it = m_audioBufferList.find(audioId);
	if (it != m_audioBufferList.end()) {
		return InternalPlayAudio(it->second);
	} else {
		return false;
	}
}

bool AudioPlayer::LoadAndPlayAudio(wstring audioId, IBuffer^ buf)
{
	auto audioBuffer = LoadAudio(buf);
	m_audioBufferList[audioId] = audioBuffer;
	return InternalPlayAudio(audioBuffer);
}

bool AudioPlayer::InternalPlayAudio(shared_ptr<AudioBuffer> audioBuffer)
{
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = (UINT32) audioBuffer->audioDataLength;
	buffer.pAudioData = audioBuffer->pAudioData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	HRESULT hr = audioBuffer->pSourceVoice->Stop();
	if (SUCCEEDED(hr))
		hr = audioBuffer->pSourceVoice->FlushSourceBuffers();

	hr = audioBuffer->pSourceVoice->SubmitSourceBuffer(&buffer);
	if (SUCCEEDED(hr))
		hr = audioBuffer->pSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
	return SUCCEEDED(hr);
}

