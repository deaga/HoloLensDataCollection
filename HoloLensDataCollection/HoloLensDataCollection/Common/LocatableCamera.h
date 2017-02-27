// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

namespace HoloLensDataCollection
{
	class LocatableCameraSink;
	class LocatableCamera {
	public:
		LocatableCamera();
		virtual ~LocatableCamera() {};

	private:
		Platform::Agile<Windows::Media::Capture::MediaCapture^> mMediaCapture;
		Microsoft::WRL::ComPtr<LocatableCameraSink> mLocatableCameraSink;
		::Windows::Media::IMediaExtension^ mMediaExtension;
	};
}
