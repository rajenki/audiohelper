#include "pch.h"
#include "AudioBuffer.h"
#include "AudioPlayer.h"
#include "DebugOut.h"

using namespace Microsoft::WRL;

namespace AudioHelper
{
	byte* GetRawIBuffer(Object^ buf)
	{
		Object^ obj = buf;
		ComPtr<IInspectable> insp(reinterpret_cast<IInspectable*>(buf));
		ComPtr<IBufferByteAccess> bufferByteAccess;
		HRESULT hr = insp.As(&bufferByteAccess);
		if (FAILED(hr)) {
			return nullptr;
		}

		byte* rawBuf = nullptr;
		hr = bufferByteAccess->Buffer(&rawBuf);
		return rawBuf;
	}

	uint64 AudioBuffer::FindChunk(uint32 tag, uint64 startLoc, uint64 endLoc)
	{
		uint64 newLoc = startLoc;
		while (endLoc > (newLoc + sizeof(ChunkHeader))) {
			ChunkHeader* header = reinterpret_cast<ChunkHeader*>(&pRawBuffer[newLoc]);
			if (header->tag == tag) {
				// Found the requested tag
				return newLoc;
			}
			newLoc += header->size + sizeof(*header);
		}

		// Chunk with sent tag was not found
		throw ref new ::Platform::FailureException();
	}

	AudioBuffer::AudioBuffer(IBuffer^ waveBuffer)
	{
		this->waveBuffer = waveBuffer;
		pRawBuffer = GetRawIBuffer(waveBuffer);
	}

	bool AudioBuffer::ParseAudioBuffer()
	{
		if (pRawBuffer == nullptr) return false;

		uint64 riffLoc = FindChunk(FOURCC_RIFF_TAG, 0, waveBuffer->Length);
		ChunkHeader *pChunkHeader = (ChunkHeader *) &pRawBuffer[riffLoc];

		byte* pb = &pRawBuffer[riffLoc + sizeof(*pChunkHeader)];
		uint32 tag = *reinterpret_cast<uint32*>(pb);
		if (tag != FOURCC_WAVE_FILE_TAG) {
			// Only support .wav files
			throw ref new ::Platform::FailureException();
		}
		uint64 riffChildrenStart = riffLoc + sizeof(*pChunkHeader) + sizeof(tag);
		uint64 riffChildrenEnd = riffLoc + sizeof(*pChunkHeader) + pChunkHeader->size;

		uint64 formatLoc = FindChunk(FOURCC_FORMAT_TAG, riffChildrenStart, riffChildrenEnd);
		pChunkHeader = (ChunkHeader *)&pRawBuffer[formatLoc];

		pWex = (PWAVEFORMATEX)&pRawBuffer[formatLoc + sizeof(*pChunkHeader)];
		if (pWex->wFormatTag != WAVE_FORMAT_PCM && pWex->wFormatTag != WAVE_FORMAT_ADPCM) {
			throw ref new ::Platform::FailureException();
		}

		uint64 dataChunkStart = FindChunk(FOURCC_DATA_TAG, riffChildrenStart, riffChildrenEnd);
		pChunkHeader = (ChunkHeader *)&pRawBuffer[dataChunkStart];
		pAudioData = &pb[dataChunkStart + sizeof(pChunkHeader)];
		audioDataLength = pChunkHeader->size;
		return true;
	}

	void AudioCallbackHandler::OnVoiceProcessingPassStart(UINT32 bytesRequired) {}
	void AudioCallbackHandler::OnVoiceProcessingPassEnd() {}
	void AudioCallbackHandler::OnStreamEnd() {}

	void AudioCallbackHandler::OnBufferStart(void* bufferContext) {}

	void AudioCallbackHandler::OnBufferEnd(void* bufferContext) {}

	void AudioCallbackHandler::OnLoopEnd(void* bufferContext) {}
	void AudioCallbackHandler::OnVoiceError(void* bufferContext, HRESULT error) {}
}
