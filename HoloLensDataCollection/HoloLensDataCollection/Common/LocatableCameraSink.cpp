#include "pch.h"

LocatableCameraSink::LocatableCameraSink(
	_In_opt_ Windows::Media::MediaProperties::VideoEncodingProperties^ videoProps)
	: mShutDown(false)
{

	Microsoft::WRL::ComPtr<IMFMediaType> videoMT;
	if (videoProps != nullptr)
	{
		Microsoft::WRL::ComPtr<IUnknown> castedVideoEncodingProperties;
		reinterpret_cast<IInspectable*>(videoProps)->QueryInterface(IID_PPV_ARGS(&castedVideoEncodingProperties));
		MFCreateMediaTypeFromProperties(castedVideoEncodingProperties.Get(), &videoMT);
		mVideoStreamSink = Microsoft::WRL::Make<LocatableCameraSinkStream>(
			this,
			0, // TODO : Don't hard code this, at least create a const expr
			videoMT
			);
	}
}

HRESULT LocatableCameraSink::SetProperties(_In_ ABI::Windows::Foundation::Collections::IPropertySet * /*configuration*/)
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}


HRESULT LocatableCameraSink::GetCharacteristics(_Out_ DWORD *characteristics)
{
	return ExceptionBoundary([this, characteristics]()
	{
		auto hr = CheckShutDown();

		if(SUCCEEDED(hr))
			NULL_CHK(characteristics);
		*characteristics = MEDIASINK_RATELESS | MEDIASINK_FIXED_STREAMS;
		return hr;
	});
}

HRESULT LocatableCameraSink::AddStreamSink(
	DWORD /*streamSinkIdentifier*/,
	_In_ IMFMediaType * /*mediaType*/,
	_COM_Outptr_ IMFStreamSink **streamSink
)
{
	return ExceptionBoundary([this, streamSink]()
	{
		auto hr = CheckShutDown();

		if (SUCCEEDED(hr)) {
			NULL_CHK(streamSink);
			*streamSink = nullptr;
		}

		return MF_E_STREAMSINKS_FIXED; // TODO : Check if this is right
	});
}


HRESULT LocatableCameraSink::RemoveStreamSink(DWORD /*streamSinkIdentifier*/)
{
	return ExceptionBoundary([this]()
	{
		auto hr = CheckShutDown();

		if (SUCCEEDED(hr))
			return MF_E_STREAMSINKS_FIXED;
		else
			return hr;
		
	});
}


HRESULT LocatableCameraSink::GetStreamSinkCount(_Out_ DWORD *streamSinkCount)
{
	return ExceptionBoundary([this, streamSinkCount]()
	{
		NULL_CHK(streamSinkCount);

		auto hr = CheckShutDown();

		*streamSinkCount =  (mVideoStreamSink != nullptr);

		return S_OK;
	});
}


HRESULT LocatableCameraSink::GetStreamSinkByIndex(DWORD index, _COM_Outptr_ IMFStreamSink **streamSink)
{
	return ExceptionBoundary([this, index, streamSink]()
	{
		auto lock = mLock.LockExclusive();

		NULL_CHK(streamSink);
		*streamSink = nullptr;

		auto hr = CheckShutDown();

		if (SUCCEEDED(hr)) {
			switch (index)
			{
			case 0:
				if ((mVideoStreamSink != nullptr))
				{
					IFR(mVideoStreamSink.CopyTo(streamSink));
				}
				else
				{
					return E_INVALIDARG;
				}
				break;

			default:
				return E_INVALIDARG;
			}
		}
		return hr;
	});
}

HRESULT LocatableCameraSink::GetStreamSinkById(DWORD identifier, _COM_Outptr_ IMFStreamSink **streamSink)
{
	return ExceptionBoundary([this, identifier, streamSink]()
	{
		auto lock = mLock.LockExclusive();

		NULL_CHK(streamSink);
		*streamSink = nullptr;

		auto hr = CheckShutDown();
		if (SUCCEEDED(hr)) {
			if ((identifier == 0) && (mVideoStreamSink != nullptr))
			{
				IFR(mVideoStreamSink.CopyTo(streamSink));
			}
			else
			{
				return E_INVALIDARG;
			}
		}
		return hr;
	});
}
HRESULT LocatableCameraSink::SetPresentationClock(_In_ IMFPresentationClock *clock)
{
	return ExceptionBoundary([this, clock]()
	{
		auto lock = mLock.LockExclusive();

		auto hr = CheckShutDown();
		if (SUCCEEDED(hr)) {
			if (mClock != nullptr)
			{
				IFR(mClock->RemoveClockStateSink(this));
				mClock = nullptr;
			}

			if (clock != nullptr)
			{
				IFR(clock->AddClockStateSink(this));
				mClock = clock;
			}
		}
		return hr;
	});
}

HRESULT LocatableCameraSink::GetPresentationClock(_COM_Outptr_ IMFPresentationClock **clock)
{
	return ExceptionBoundary([this, clock]()
	{
		auto lock = mLock.LockExclusive();

		NULL_CHK(clock);
		*clock = nullptr;

		auto hr = CheckShutDown();

		if (SUCCEEDED(hr)) {
			if (mClock != nullptr)
			{
				IFR(mClock.CopyTo(clock))
			}
		}
		return hr;
	});
}

HRESULT LocatableCameraSink::Shutdown()
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();

		if (mShutDown)
		{
			return S_OK;
		}
		mShutDown = true;

		if (mVideoStreamSink != nullptr)
		{
			mVideoStreamSink->ShutDown();
			mVideoStreamSink = nullptr;
		}

		if (mClock != nullptr)
		{
			(void)mClock->RemoveClockStateSink(this);
			mClock = nullptr;
		}

		return S_OK;
	});
}

HRESULT LocatableCameraSink::OnClockStart(MFTIME /*hnsSystemTime*/, LONGLONG /*llClockStartOffset*/)
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}
HRESULT LocatableCameraSink::OnClockStop(MFTIME /*hnsSystemTime*/)
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}

HRESULT LocatableCameraSink::OnClockPause(MFTIME /*hnsSystemTime*/)
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}

HRESULT LocatableCameraSink::OnClockRestart(MFTIME /*hnsSystemTime*/)
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}

HRESULT LocatableCameraSink::OnClockSetRate(MFTIME /*hnsSystemTime*/, float /*flRate*/)
{
	return ExceptionBoundary([this]()
	{
		auto lock = mLock.LockExclusive();
		return CheckShutDown();
	});
}