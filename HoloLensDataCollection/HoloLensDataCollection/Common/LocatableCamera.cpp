// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "LocatableCamera.h"

inline HRESULT FindAndSetMinimumResolution(Platform::Agile<Windows::Media::Capture::MediaCapture^> mediaCapture) throw()
{
	auto availableVideoProperties = mediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(Windows::Media::Capture::MediaStreamType::VideoPreview);
	unsigned int minimumResolution = 1024 * 1024; // TODO : Use std::numeric_limit<>::max()
	Platform::Agile<Windows::Media::MediaProperties::VideoEncodingProperties^> requiredVideoProperty;
	for (auto videoPropertyIterator = availableVideoProperties->First(); videoPropertyIterator->HasCurrent; videoPropertyIterator->MoveNext())
	{
		auto videoProperty = static_cast<Windows::Media::MediaProperties::VideoEncodingProperties^>(videoPropertyIterator->Current);
		if (videoProperty != nullptr)
		{
			auto currentResolution = videoProperty->Width*videoProperty->Height;
			if (currentResolution < minimumResolution) {
				minimumResolution = currentResolution;
				requiredVideoProperty = videoProperty;
			}
		}
	}
	if (requiredVideoProperty.Get() != nullptr) 
	{
		auto requiredMediaEncoding = static_cast<Windows::Media::MediaProperties::IMediaEncodingProperties^>(requiredVideoProperty.Get());
		Concurrency::create_task(mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(Windows::Media::Capture::MediaStreamType::VideoPreview, requiredMediaEncoding)).then([requiredVideoProperty]()
		{
			writeToTheFile(string_format("Width : %d, Height : %d , FrameRate : %d / %d \n", 
				requiredVideoProperty->Width,requiredVideoProperty->Height,requiredVideoProperty->FrameRate->Numerator, requiredVideoProperty->FrameRate->Denominator));
		});
		
	}

	return S_OK;
}

LocatableCamera::LocatableCamera() {

	Platform::Agile<Windows::Devices::Enumeration::DeviceInformationCollection> m_devices;
	Concurrency::create_task(Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(Windows::Devices::Enumeration::DeviceClass::VideoCapture))
		.then([this](Concurrency::task<Windows::Devices::Enumeration::DeviceInformationCollection^> findTask)
	{
		auto devices = findTask.get();
		auto numberOfDevices = devices->Size;

		auto initialisationSettings = ref new Windows::Media::Capture::MediaCaptureInitializationSettings();
		initialisationSettings->StreamingCaptureMode = Windows::Media::Capture::StreamingCaptureMode::Video;

		// TODO : Only 1 camera is accessible on HoloLens for now, Should find a better way to get the camera id, instead of hard coding it
		if (numberOfDevices > 0) {
			auto myCameraInfo = devices->GetAt(0);
			initialisationSettings->VideoDeviceId = myCameraInfo->Id;

			mMediaCapture = ref new Windows::Media::Capture::MediaCapture();
			Concurrency::create_task(mMediaCapture->InitializeAsync(initialisationSettings)).then([this]() {
				FindAndSetMinimumResolution(mMediaCapture);
				auto properties = safe_cast<Windows::Media::MediaProperties::VideoEncodingProperties^>(mMediaCapture->VideoDeviceController->GetMediaStreamProperties(Windows::Media::Capture::MediaStreamType::VideoPreview));
			}).then([]() {writeToTheFile("Camera is accessible and set with lowest possible resolution"); });
		}
	});
}
