#include "pch.h"

LocatableCameraSinkStream::LocatableCameraSinkStream(
	__in const Microsoft::WRL::ComPtr<IMFMediaSink>& mediaSink,
	__in DWORD id,
	__in const Microsoft::WRL::ComPtr<IMFMediaType>& mediaType
) : mShutDown(false), mId(id) {
	auto hr = MFCreateEventQueue(&mMediaEventQueue);
	hr = MFCreateMediaType(&mMediaType);
	UpdateMediaType(mediaType);
	mMediaSink = mediaSink;
}

HRESULT LocatableCameraSinkStream::GetMediaSink(__deref_out IMFMediaSink **sink)
{
	return ExceptionBoundary([this, sink]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(sink);
		*sink = nullptr;
		auto hr = CheckShutDown();
		if(SUCCEEDED(hr))
			hr = mMediaSink.CopyTo(sink);
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::GetIdentifier(__out DWORD *identifier)
{
	return ExceptionBoundary([this, identifier]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(identifier);
		auto hr = CheckShutDown();
		if(SUCCEEDED(hr))
			*identifier = mId;
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::GetMediaTypeHandler(__deref_out IMFMediaTypeHandler **handler)
{
	return ExceptionBoundary([this, handler]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(handler);
		*handler = nullptr;
		auto hr = CheckShutDown();
		if (SUCCEEDED(hr)) {
			*handler = this;
			this->AddRef();
		}
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::ProcessSample(__in_opt IMFSample *sample)
{
	return ExceptionBoundary([this, sample]()
	{
		//auto mediaSample = ref new Windows::Media::MediaSample
		{
			auto lock = mLock.LockExclusive();
			auto hr = CheckShutDown();

			if (sample == nullptr)
			{
				return S_OK;
			}

			return hr;
			// TODO : Fill this up 
			//mediaSample->Sample = sample;
			//sampleHandler = _sampleHandler;
		}

		// Call back without the lock taken to avoid deadlocks
		//sampleHandler(mediaSample); // TODO : Write the sample 
	});
}


HRESULT LocatableCameraSinkStream::PlaceMarker(__in MFSTREAMSINK_MARKER_TYPE /*markerType*/, __in const PROPVARIANT * /*markerValue*/, __in const PROPVARIANT * contextValue)
{
	return ExceptionBoundary([this, contextValue]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(contextValue);
		auto hr = CheckShutDown();
		if(SUCCEEDED(hr))
			hr = mMediaEventQueue->QueueEventParamVar(MEStreamSinkMarker, GUID_NULL, S_OK, contextValue);
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::Flush()
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}

HRESULT LocatableCameraSinkStream::GetEvent(__in DWORD flags, __deref_out IMFMediaEvent **event)
{
	return ExceptionBoundary([this, flags, event]()
	{
		NULL_CHK(event);
		*event = nullptr;
		ComPtr<IMFMediaEventQueue> eventQueue;
		{
			auto lock = mLock.LockExclusive();
			auto hr = CheckShutDown();
			if (FAILED(hr))
				return hr;
			eventQueue = mMediaEventQueue;
		}
		// May block for a while
		return eventQueue->GetEvent(flags, event);
	});
}

HRESULT LocatableCameraSinkStream::BeginGetEvent(__in IMFAsyncCallback *callback, __in_opt IUnknown *state)
{
	return ExceptionBoundary([this, callback, state]()
	{
		auto lock = mLock.LockExclusive();
		auto hr = CheckShutDown();
		if(SUCCEEDED(hr))
			hr = mMediaEventQueue->BeginGetEvent(callback, state);
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::EndGetEvent(__in IMFAsyncResult *result, __deref_out IMFMediaEvent **event)
{
	return ExceptionBoundary([this, result, event]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(event);
		*event = nullptr;
		auto hr = CheckShutDown();
		if(SUCCEEDED(hr))
			hr = mMediaEventQueue->EndGetEvent(result, event);
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::QueueEvent(
	__in MediaEventType met,
	__in REFGUID extendedType,
	__in HRESULT status,
	__in_opt const PROPVARIANT *value
)
{
	return ExceptionBoundary([this, met, extendedType, status, value]()
	{
		auto lock = mLock.LockExclusive();
		CheckShutDown();
		return mMediaEventQueue->QueueEventParamVar(met, extendedType, status, value);
	});
}

HRESULT LocatableCameraSinkStream::IsMediaTypeSupported(__in IMFMediaType *mediaType, __deref_out_opt  IMFMediaType **closestMediaType)
{
	bool supported = false;

	HRESULT hr = ExceptionBoundary([this, mediaType, closestMediaType, &supported]()
	{
		auto lock = mLock.LockExclusive();

		if (closestMediaType != nullptr)
		{
			*closestMediaType = nullptr;
		}

		NULL_CHK(mediaType);

		CheckShutDown();

		GUID majorType;
		
		auto hrInner = mediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
		if (FAILED(hrInner))
			throw ref new Platform::COMException(hrInner);

		if (majorType == MFMediaType_Video)
			supported = true;
		return hrInner;
	});

	// Avoid throwing an exception to return MF_E_INVALIDMEDIATYPE as this is not a exceptional case
	return FAILED(hr) ? hr : supported ? S_OK : MF_E_INVALIDMEDIATYPE;
}

HRESULT LocatableCameraSinkStream::GetMediaTypeCount(__out DWORD *typeCount)
{
	return ExceptionBoundary([this, typeCount]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(typeCount);
		CheckShutDown();
		// No media type provided by default (app needs to specify it)
		*typeCount = 0; // TODO : Verify this is true
		return S_OK;
	});
}

HRESULT LocatableCameraSinkStream::GetMediaTypeByIndex(__in DWORD /*index*/, __deref_out  IMFMediaType **mediaType)
{
	HRESULT hr = ExceptionBoundary([this, mediaType]()
	{
		auto lock = mLock.LockExclusive();

		NULL_CHK(mediaType);
		*mediaType = nullptr;

		return CheckShutDown();
	});

	// Avoid throwing an exception to return MF_E_NO_MORE_TYPES as this is not a exceptional case
	return FAILED(hr) ? hr : MF_E_NO_MORE_TYPES;
}

HRESULT LocatableCameraSinkStream::SetCurrentMediaType(__in IMFMediaType *mediaType)
{
	return ExceptionBoundary([this, mediaType]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(mediaType);
		CheckShutDown();
		UpdateMediaType(mediaType);
		return S_OK;
	});
}

HRESULT LocatableCameraSinkStream::GetCurrentMediaType(__deref_out_opt IMFMediaType **mediaType)
{
	return ExceptionBoundary([this, mediaType]()
	{
		auto lock = mLock.LockExclusive();

		NULL_CHK(mediaType);
		*mediaType = nullptr;

		CheckShutDown();

		ComPtr<IMFMediaType> mt;
		auto hr = MFCreateMediaType(&mt);
		// TODO : Do this correctly
		if (SUCCEEDED(hr)) {
			hr = mMediaType->CopyAllItems(mt.Get());
		}
		*mediaType = mt.Detach();
		return hr;
	});
}

HRESULT LocatableCameraSinkStream::GetMajorType(__out GUID *majorType)
{
	return ExceptionBoundary([this, majorType]()
	{
		auto lock = mLock.LockExclusive();
		NULL_CHK(majorType);
		CheckShutDown();
		// TODO : Set this correctly
		GUID majorTypeTemp;
		auto hr = mMediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorTypeTemp);
		if (FAILED(hr)) {
			throw ref new Platform::COMException(hr);
		}
		*majorType = majorTypeTemp;
		return hr;
	});
}

void LocatableCameraSinkStream::UpdateMediaType(__in const ComPtr<IMFMediaType>& mediaType)
{
	GUID majorType, subType;
	unsigned int width, height;
	auto hr = mediaType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
	if (FAILED(hr)) {
		throw ref new Platform::COMException(hr);
	}
	hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subType);
	if (FAILED(hr)) {
		throw ref new Platform::COMException(hr);
	}

	if (majorType == MFMediaType_Video)
	{
		hr = MFGetAttributeSize(mediaType.Get(), MF_MT_FRAME_SIZE, &width, &height);
		if (FAILED(hr)) {
			throw ref new Platform::COMException(hr);
		}
	}

	hr = mediaType->CopyAllItems(mMediaType.Get());
	if (FAILED(hr)) {
		throw ref new Platform::COMException(hr);
	}
}

void LocatableCameraSinkStream::ShutDown() {
	if (mShutDown)
		return;

	mShutDown = true;

	(void)mMediaEventQueue->Shutdown();
	mMediaEventQueue = nullptr;
	mMediaSink = nullptr;
	mMediaType = nullptr;
}