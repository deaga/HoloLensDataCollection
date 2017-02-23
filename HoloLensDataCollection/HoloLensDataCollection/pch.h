#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif

// Standard C++ first
#include <array>
#include <assert.h>
#include <fstream>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>


// Windows

#include <agile.h>
#include <d2d1_2.h>
#include <d3d11_4.h>
#include <dwrite_2.h>
#include <DirectXColors.h>
#include <initguid.h>
#include <robuffer.h>
#include <strsafe.h>
#include <wincodec.h>
#include <WindowsNumerics.h>
#include <wrl.h>
#include <wrl\async.h>
#include <wrl\wrappers\corewrappers.h>
#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.foundation.numerics.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#include <windows.media.effects.h>
#include <windows.media.mediaproperties.h>
#include <windows.networking.sockets.h>
#include <windows.perception.h>
#include <windows.perception.spatial.h>
#include <windows.storage.h>
#include <windows.storage.streams.h>
#include <windows.system.threading.h>

#pragma comment(lib, "runtimeobject")

#include <ppltasks.h>
#include <concurrent_queue.h>

// Media Foundation
#include <mfidl.h>
#include <mferror.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfmediacapture.h>
#include <mfmediaengine.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mf")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "mfreadwrite.lib")


// mf guids for pulling sample data
//EXTERN_GUID(MFSampleExtension_PinholeCameraIntrinsics, 0x4ee3b6c5, 0x6a15, 0x4e72, 0x97, 0x61, 0x70, 0xc1, 0xdb, 0x8b, 0x9f, 0xe3);
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
EXTERN_GUID(MFSampleExtension_Spatial_CameraCoordinateSystem, 0x9d13c82f, 0x2199, 0x4e67, 0x91, 0xcd, 0xd1, 0xa4, 0x18, 0x1f, 0x25, 0x34);
EXTERN_GUID(MFSampleExtension_Spatial_CameraViewTransform, 0x4e251fa4, 0x830f, 0x4770, 0x85, 0x9a, 0x4b, 0x8d, 0x99, 0xaa, 0x80, 0x9b);
EXTERN_GUID(MFSampleExtension_Spatial_CameraProjectionTransform, 0x47f9fcb5, 0x2a02, 0x4f26, 0xa4, 0x77, 0x79, 0x2f, 0xdf, 0x95, 0x88, 0x6a);
#endif
EXTERN_GUID(Spatial_CameraTransform, 0x49d793d7, 0x5378, 0x43dd, 0xb2, 0xb3, 0xfe, 0x17, 0x18, 0xaa, 0xcb, 0x1d);

template <typename T>
inline T GetDataType(_In_ ABI::Windows::Storage::Streams::IBuffer* pBuffer)
{
	// take a reference to the passed in buffer
	ComPtr<ABI::Windows::Storage::Streams::IBuffer> spBuffer(pBuffer);

	// QI for BufferByteAccess
	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> spBufferByteAccess;
	if (SUCCEEDED(spBuffer.As(&spBufferByteAccess)))
	{
		byte* pBytes = nullptr;
		if (SUCCEEDED(spBufferByteAccess->Buffer(&pBytes)))
		{
			// cast to correct type
			return reinterpret_cast<T>(pBytes);
		};
	}

	return nullptr;
}

inline std::wstring to_wstring(unsigned int d)
{
	wchar_t buffer[65] = { 0 }; //64 + sign
	_ui64tow_s(d, buffer, sizeof(buffer) / sizeof(buffer[0]), 10);

	return std::wstring(buffer);
}


using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::Storage;

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Foundation::Numerics;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Media;
using namespace ABI::Windows::Media::Capture;
using namespace ABI::Windows::Media::MediaProperties;
using namespace ABI::Windows::Networking;
using namespace ABI::Windows::Networking::Sockets;
using namespace ABI::Windows::Perception::Spatial;
using namespace ABI::Windows::Storage::Streams;
using namespace ABI::Windows::System::Threading;

#include "Common\AsyncOperations.h"
#include "Common\ErrorHandling.h"
#include "Common\LinkList.h"
#include "Common\LocatableCamera.h"
#include "Common\Utils.h"

using namespace HoloLensDataCollection;