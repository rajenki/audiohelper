# AudioHelper

AudioHelper is a Windows Runtime Component for Universal Apps that provides functionality to to play a WAV file or a [SpeechSynthesisStream] (http://msdn.microsoft.com/en-us/library/windows.media.speechsynthesis.speechsynthesisstream.aspx) using the DirectX [XAudio2] (http://msdn.microsoft.com/en-us/library/windows/desktop/hh405049(v=vs.85).aspx) APIs.
The purpose is to provide capabilities to play audio in Universal Apps without having to interrupt the current audio (stream) that's being played on the device (which is the default [MediaElement] (http://msdn.microsoft.com/en-us/library/system.windows.controls.mediaelement(v=vs.110).aspx) behavior).

## How to use

* Download or clone the source code from the GitHub repository. 
* Add the Shared, Windows and WindowsPhone projects to your solution and reference the corresponding project from your app project.

### Play a WAV file

* Add a WAV file to your project and set its Build Action to Content
* Use the AudioPlayer's PlayAudio method, passing in the path to the WAV file, to play the audio
```csharp
AudioHelper.AudioPlayer.Instance.PlayAudio(@"HelloWorld.wav");
```

### Play a fragment using the speech synthesis engine

* Use the SpeechSynthesizer to create the stream, read the resulting stream into a byte buffer and play the resulting buffer using the AudioPlayer
```csharp
// The object for controlling the speech synthesis engine (voice).
var synth = new Windows.Media.SpeechSynthesis.SpeechSynthesizer();

// Generate the audio stream from plain text.
var synthesizedText = "Hello World";
SpeechSynthesisStream stream = await synth.SynthesizeTextToStreamAsync(synthesizedText);

// Initialize the byte buffer
byte[] bytes = new byte[stream.Size];
IBuffer buffer = bytes.AsBuffer();

// Read the stream into the buffer
await stream.ReadAsync(buffer, (uint)stream.Size, InputStreamOptions.None);
AudioPlayer.Instance.PlayAudio(synthesizedText, buffer);
```

## Credits

This library has been written by [Anders Thun] (https://twitter.com/AndersThun) and is managed by [Rajen Kishna] (https://twitter.com/rajen_k).
While both working for Microsoft, this library has been created as a side-project to assist the community and is provided "as is" with no warranty whatsoever.
