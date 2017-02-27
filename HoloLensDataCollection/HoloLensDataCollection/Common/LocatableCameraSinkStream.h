#pragma once

namespace HoloLensDataCollection
{

	class LocatableCameraSinkStream WrlSealed :
		public Microsoft::WRL::RuntimeClass<
		Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
		IMFStreamSink,
		IMFMediaEventGenerator,
		IMFMediaTypeHandler
		>
	{
	public:
		LocatableCameraSinkStream(
			__in const Microsoft::WRL::ComPtr<IMFMediaSink>& mediaSink,
			__in DWORD id,
			__in const Microsoft::WRL::ComPtr<IMFMediaType>& mediaType
			);

		//
		// IMFStreamSink
		//

		IFACEMETHODIMP GetMediaSink(__deref_out IMFMediaSink **sink);
		IFACEMETHODIMP GetIdentifier(__out DWORD *identifier);
		IFACEMETHODIMP GetMediaTypeHandler(__deref_out IMFMediaTypeHandler **handler);
		IFACEMETHODIMP ProcessSample(__in_opt IMFSample *sample);
		IFACEMETHODIMP PlaceMarker(__in MFSTREAMSINK_MARKER_TYPE markerType, __in const PROPVARIANT * markerValue, __in const PROPVARIANT * contextValue);
		IFACEMETHODIMP Flush();

		//
		// IMFMediaEventGenerator
		//

		IFACEMETHODIMP GetEvent(__in DWORD flags, __deref_out IMFMediaEvent **event);
		IFACEMETHODIMP BeginGetEvent(__in IMFAsyncCallback *callback, __in_opt IUnknown *state);
		IFACEMETHODIMP EndGetEvent(__in IMFAsyncResult *result, __deref_out IMFMediaEvent **event);
		IFACEMETHODIMP QueueEvent(__in MediaEventType met, __in REFGUID extendedType, __in HRESULT status, __in_opt const PROPVARIANT *value);

		//
		// IMFMediaTypeHandler
		//

		IFACEMETHODIMP IsMediaTypeSupported(__in IMFMediaType *mediaType, __deref_out_opt  IMFMediaType **closestMediaType);
		IFACEMETHODIMP GetMediaTypeCount(__out DWORD *typeCount);
		IFACEMETHODIMP GetMediaTypeByIndex(__in DWORD index, __deref_out  IMFMediaType **mediaType);
		IFACEMETHODIMP SetCurrentMediaType(__in IMFMediaType *mediaType);
		IFACEMETHODIMP GetCurrentMediaType(__deref_out_opt IMFMediaType **mediaType);
		IFACEMETHODIMP GetMajorType(__out GUID *majorType);

		void ShutDown();


	private:
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
		
		void UpdateMediaType(__in const ComPtr<IMFMediaType>& mt);
		Microsoft::WRL::ComPtr<IMFMediaSink> mMediaSink;
		Microsoft::WRL::ComPtr<IMFMediaEventQueue> mMediaEventQueue;
		Microsoft::WRL::ComPtr<IMFMediaType> mMediaType;
		DWORD mId;
		
		bool mShutDown;
		Microsoft::WRL::Wrappers::SRWLock mLock;
	};
}