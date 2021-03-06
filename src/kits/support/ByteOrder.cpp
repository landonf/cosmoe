/* 
** Copyright 2004, Axel Dörfler, axeld@pinc-software.de. All rights reserved.
** Distributed under the terms of the Haiku License.
*/


#include <ByteOrder.h>
#include <Messenger.h>


status_t
swap_data(type_code type, void *_data, size_t length, swap_action action)
{
	if (_data == NULL || length == 0)
		return B_BAD_VALUE;

	switch (type) {
		// allowed types
		case B_INT16_TYPE:
		case B_UINT16_TYPE:
		case B_FLOAT_TYPE:
		case B_INT32_TYPE:
		case B_UINT32_TYPE:
		case B_SIZE_T_TYPE:
		case B_SSIZE_T_TYPE:
		case B_TIME_TYPE:
		case B_POINTER_TYPE:
		case B_RECT_TYPE:
		case B_POINT_TYPE:
		case B_DOUBLE_TYPE:
		case B_INT64_TYPE:
		case B_UINT64_TYPE:
		case B_OFF_T_TYPE:
		case B_MESSENGER_TYPE:
			break;

		default:
			// not swappable or recognized type!
			return B_BAD_VALUE;
	}

	// is there anything to do?
#if B_HOST_IS_LENDIAN
	if (action == B_SWAP_HOST_TO_LENDIAN || action == B_SWAP_LENDIAN_TO_HOST)
		return B_OK;
#else
	if (action == B_SWAP_HOST_TO_BENDIAN || action == B_SWAP_BENDIAN_TO_HOST)
		return B_OK;
#endif

	switch (type) {
		// 16 bit types
		case B_INT16_TYPE:
		case B_UINT16_TYPE:
		{
			uint16 *data = (uint16 *)_data;
			uint16 *end = (uint16 *)((addr_t)_data + length);

			while (data < end) {
				*data = __swap_int16(*data);
				data++;
			}
			break;
		}

		// 32 bit types
		case B_FLOAT_TYPE:
		case B_INT32_TYPE:
		case B_UINT32_TYPE:
		case B_SIZE_T_TYPE:
		case B_SSIZE_T_TYPE:
		case B_TIME_TYPE:
		case B_POINTER_TYPE:
		case B_RECT_TYPE:
		case B_POINT_TYPE:
		{
			// ToDo: some of these types may not be 32-bit on 64-bit platforms!
			uint32 *data = (uint32 *)_data;
			uint32 *end = (uint32 *)((addr_t)_data + length);

			while (data < end) {
				*data = __swap_int32(*data);
				data++;
			}
			break;
		}

		// 64 bit types
		case B_DOUBLE_TYPE:
		case B_INT64_TYPE:
		case B_UINT64_TYPE:
		case B_OFF_T_TYPE:
		{
			uint64 *data = (uint64 *)_data;
			uint64 *end = (uint64 *)((addr_t)_data + length);

			while (data < end) {
				*data = __swap_int64(*data);
				data++;
			}
			break;
		}

		// special types
		case B_MESSENGER_TYPE:
		{
			BMessenger *messenger = (BMessenger *)_data;
			BMessenger *end = (BMessenger *)((addr_t)_data + length);

			while (messenger < end) {
				// ToDo: if the additional fields change, this function has to be updated!
				messenger->fPort = __swap_int32(messenger->fPort);
				messenger->fHandlerToken = __swap_int32(messenger->fHandlerToken);
				messenger->fTeam = __swap_int32(messenger->fTeam);
				messenger++;
			}
			break;
		}
	}

	return B_OK;
}


bool
is_type_swapped(type_code type)
{
	// Returns true when the type is in the host's native format
	// Looks like a pretty strange function to me :)

	switch (type) {
		case B_ANY_TYPE:
		case B_BOOL_TYPE:
		case B_CHAR_TYPE:
		case B_COLOR_8_BIT_TYPE:
		case B_DOUBLE_TYPE:
		case B_FLOAT_TYPE:
		case B_GRAYSCALE_8_BIT_TYPE:
		case B_INT64_TYPE:
		case B_INT32_TYPE:
		case B_INT16_TYPE:
		case B_INT8_TYPE:
		case B_MESSAGE_TYPE:
		case B_MESSENGER_TYPE:
		case B_MIME_TYPE:
		case B_MONOCHROME_1_BIT_TYPE:
		case B_OBJECT_TYPE:
		case B_OFF_T_TYPE:
		case B_PATTERN_TYPE:
		case B_POINTER_TYPE:
		case B_POINT_TYPE:
		case B_RAW_TYPE:
		case B_RECT_TYPE:
		case B_REF_TYPE:
		case B_RGB_32_BIT_TYPE:
		case B_RGB_COLOR_TYPE:
		case B_SIZE_T_TYPE:
		case B_SSIZE_T_TYPE:
		case B_STRING_TYPE:
		case B_TIME_TYPE:
		case B_UINT64_TYPE:
		case B_UINT32_TYPE:
		case B_UINT16_TYPE:
		case B_UINT8_TYPE:
		case B_MEDIA_PARAMETER_TYPE:
		case B_MEDIA_PARAMETER_WEB_TYPE:
		case B_MEDIA_PARAMETER_GROUP_TYPE:
			return true;
	}

	return false;
}

double __swap_double(double arg)
{
	// FIXME: unimplemented
	return arg;
}

float  __swap_float(float arg)
{
	// FIXME: unimplemented
	return arg;
}

uint64 __swap_int64(uint64 uarg)
{
	unsigned char	b1, b2, b3, b4, b5, b6, b7, b8;

	// Separate out each of the 8-bytes of this uint64
	b1 = (uarg >> 56) & 0xff;
	b2 = (uarg >> 48) & 0xff;
	b3 = (uarg >> 40) & 0xff;
	b4 = (uarg >> 32) & 0xff;
	b5 = (uarg >> 24) & 0xff;
	b6 = (uarg >> 16) & 0xff;
	b7 = (uarg >>  8) & 0xff;
	b8 =  uarg        & 0xff;

	// Return them reassembled in reverse order
	return ((uint64)b8 << 56) | ((uint64)b7 << 48) |
		   ((uint64)b6 << 40) | ((uint64)b5 << 32) |
		   ((uint64)b4 << 24) | ((uint64)b3 << 16) |
		   ((uint64)b2 << 8)  |  (uint64)b1;
}

uint32 __swap_int32(uint32 uarg)
{
	unsigned char	b1, b2, b3, b4;

	// Separate out each of the 4-bytes of this uint32
	b1 = (uarg >> 24) & 0xff;
	b2 = (uarg >> 16) & 0xff;
	b3 = (uarg >>  8) & 0xff;
	b4 =  uarg        & 0xff;

	// Return them reassembled in reverse order
	return ((uint32)b4 << 24) | ((uint32)b3 << 16) | ((uint32)b2 << 8) | (uint32)b1;
}

uint16 __swap_int16(uint16 uarg)
{
	unsigned char	b1, b2;

	// Separate out the 2-bytes of this uint16
	b1 = (uarg >>  8) & 0xff;
	b2 =  uarg        & 0xff;

	// Return them reassembled in reverse order
	return (b2 << 8) | b1;
}

