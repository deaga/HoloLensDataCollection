#pragma once

namespace HoloLensDataCollection
{
	class LocatableCameraSinkStream;
	class LocatableCameraSink WrlSealed
		: public Microsoft::WRL::RuntimeClass<
		Microsoft::WRL::RuntimeClassFlags<
		Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>
		, ABI::Windows::Media::IMediaExtension
		, IMFMediaSink
		, IMFClockStateSink
		, Microsoft::WRL::FtmBase
		>
	{
		InspectableClass(L"MediaSink", BaseTrust)

	public:

		LocatableCameraSink(_In_opt_ Windows::Media::MediaProperties::VideoEncodingProperties^ videoProps);


		//
		// IMediaExtension
		//

		IFACEMETHODIMP SetProperties(_In_ ABI::Windows::Foundation::Collections::IPropertySet * /*configuration*/);

		//
		// IMFMediaSink
		//

		IFACEMETHODIMP GetCharacteristics(_Out_ DWORD *characteristics);

		IFACEMETHODIMP AddStreamSink(
			DWORD /*streamSinkIdentifier*/,
			_In_ IMFMediaType * /*mediaType*/,
			_COM_Outptr_ IMFStreamSink **streamSink
		);

		IFACEMETHODIMP RemoveStreamSink(DWORD /*streamSinkIdentifier*/);
		IFACEMETHODIMP GetStreamSinkCount(_Out_ DWORD *streamSinkCount);

		IFACEMETHODIMP GetStreamSinkByIndex(DWORD index, _COM_Outptr_ IMFStreamSink **streamSink);

		IFACEMETHODIMP GetStreamSinkById(DWORD identifier, _COM_Outptr_ IMFStreamSink **streamSink);

		IFACEMETHODIMP SetPresentationClock(_In_ IMFPresentationClock *clock);

		IFACEMETHODIMP GetPresentationClock(_COM_Outptr_ IMFPresentationClock **clock);

		IFACEMETHODIMP Shutdown();

		//
		// IMFClockStateSink methods
		//

		IFACEMETHODIMP OnClockStart(MFTIME /*hnsSystemTime*/, LONGLONG /*llClockStartOffset*/);

		IFACEMETHODIMP OnClockStop(MFTIME /*hnsSystemTime*/);

		IFACEMETHODIMP OnClockPause(MFTIME /*hnsSystemTime*/);

		IFACEMETHODIMP OnClockRestart(MFTIME /*hnsSystemTime*/);

		IFACEMETHODIMP OnClockSetRate(MFTIME /*hnsSystemTime*/, float /*flRate*/);

	private:

		bool mShutDown;

		HRESULT CheckShutDown() {
			if (mShutDown)
			{
				return MF_E_SHUTDOWN;
			}
			else
			{
				return S_OK;
			}
		}

		Microsoft::WRL::ComPtr<LocatableCameraSinkStream> mVideoStreamSink;
		Microsoft::WRL::ComPtr<IMFPresentationClock> mClock;

		Microsoft::WRL::Wrappers::SRWLock mLock;

	};
}