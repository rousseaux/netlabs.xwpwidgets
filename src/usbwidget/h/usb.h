#ifndef __USB_H__
#define __USB_H__

#ifdef __cplusplus
  extern "C" {
#endif

// Put these into an USB.h file

/*
 * USB spec information
 *
 * This is all stuff grabbed from various USB specs and is pretty much
 * not subject to change
 */

#define USB_MAX_DEVICES						128
#define USB_START_ADDR						0

#define USB_CONTROL_ENDPOINT				0
#define USB_MAX_ENDPOINTS					16

#define USB_FRAMES_PER_SECOND				1000

/*
 * Device and/or Interface Class codes
 */
#define		USB_CLASS_PER_INTERFACE			0									// for DeviceClass
#define		USB_CLASS_AUDIO					1
#define		USB_CLASS_COMM					2
#define		USB_CLASS_HID					3
#define		USB_CLASS_PHYSICAL				5
#define		USB_CLASS_IMAGE					6

#define		USB_CLASS_PRINTER				7
#define		USB_SUBCLASS_PRINTER			1
#define		USB_PROTO_PRINTER_UNI			1
#define		USB_PROTO_PRINTER_BI			2
#define		USB_PROTO_PRINTER_1284			3

#define		USB_CLASS_MASS_STORAGE			8
#define		USB_SUBCLASS_RBC				1
#define		USB_SUBCLASS_SFF8020I			2
#define		USB_SUBCLASS_QIC157				3
#define		USB_SUBCLASS_UFI				4
#define		USB_SUBCLASS_SFF8070I			5
#define		USB_SUBCLASS_SCSI				6
#define		USB_PROTOCOL_MASS_CBI_I			0
#define		USB_PROTOCOL_MASS_CBI			1
#define		USB_PROTOCOL_MASS_BBB_OLD		2									// Not in the spec anymore
#define		USB_PROTOCOL_MASS_BBB			80									// 'P' for the Iomega Zip drive

#define		USB_CLASS_HUB					9
#define		USB_SUBCLASS_HUB				0
#define		USB_PROTO_FSHUB					0
#define		USB_PROTO_HSHUBSTT				0									// Yes, same as previous
#define		USB_PROTO_HSHUBMTT				1

#define		USB_CLASS_DATA					10
#define		USB_CLASS_SMARTCARD				11
#define		USB_CLASS_FIRM_UPD				12
#define		USB_CLASS_SECURITY				13
#define		USB_CLASS_DIAGNOSTIC			0xdc
#define		USB_CLASS_WIRELESS				0xe0
#define		USB_CLASS_APPL_SPEC				0xfe
#define		USB_CLASS_VENDOR_SPEC			0xff

/*
 * Descriptor types
 */
#define		USB_DT_DEVICE					0x01
#define		USB_DT_CONFIG					0x02
#define		USB_DT_STRING					0x03
#define		USB_DT_INTERFACE				0x04
#define		USB_DT_ENDPOINT					0x05

#define		USB_DT_HID						0x21
#define		USB_DT_REPORT					0x22
#define		USB_DT_PHYSICAL					0x23
#define		USB_DT_HUB						0x29

/* conventional codes for class-specific descriptors */
#define		USB_DT_CS_DEVICE				0x21
#define		USB_DT_CS_CONFIG				0x22
#define		USB_DT_CS_STRING				0x23
#define		USB_DT_CS_INTERFACE				0x24
#define		USB_DT_CS_ENDPOINT				0x25

/*
 * Descriptor sizes per descriptor type
 */
#define		USB_DT_DEVICE_SIZE				18
#define		USB_DT_CONFIG_SIZE				9
#define		USB_DT_INTERFACE_SIZE			9
#define		USB_DT_ENDPOINT_SIZE			7
#define		USB_DT_ENDPOINT_AUDIO_SIZE		9									// Audio extension
#define		USB_DT_HUB_NONVAR_SIZE			7


#ifndef		_UINT8_T_DECLARED
	typedef unsigned char	u_int8_t;
#endif
#ifndef		_UINT16_T_DECLARED
	typedef unsigned short	u_int16_t;
#endif

#ifndef		__GNUC__
	#define __attribute__(attr)
	#pragma pack(1)
#endif

/* All standard descriptors have these 2 fields in common */
struct usb_descriptor_header {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
} __attribute__ ((packed));

/* Device descriptor */
struct usb_device_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int16_t	bcdUSB;
	u_int8_t	bDeviceClass;
	u_int8_t	bDeviceSubClass;
	u_int8_t	bDeviceProtocol;
	u_int8_t	bMaxPacketSize0;
	u_int16_t	idVendor;
	u_int16_t	idProduct;
	u_int16_t	bcdDevice;
	u_int8_t	iManufacturer;
	u_int8_t	iProduct;
	u_int8_t	iSerialNumber;
	u_int8_t	bNumConfigurations;
} __attribute__ ((packed));

/* Endpoint descriptor */
struct usb_endpoint_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int8_t	bEndpointAddress;
	u_int8_t	bmAttributes;
	u_int16_t	wMaxPacketSize;
	u_int8_t	bInterval;
	u_int8_t	bRefresh;
	u_int8_t	bSynchAddress;
} __attribute__ ((packed));

/* HID descriptor */
#if 0
struct usb_hid_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int16_t	bcdHID;
	u_int8_t	bCountryCode;
	u_int8_t	bNumDescriptors;
} __attribute__ ((packed));
#else
struct usb_hid_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int16_t	bcdHID;
	u_int8_t	bCountry;
	u_int8_t	bNumDescriptors;
   	struct usb_hid_descriptor_list {
		u_int8_t	bReportType;
		u_int16_t	wReportLength;
	} __attribute__ ((packed)) *DescriptorList;									// Rousseau: Changed from DescriptorList[0] to *DescriptorList. (VAC3 arraybound error)
}  __attribute__ ((packed));
#endif

/* Interface descriptor */
struct usb_interface_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int8_t	bInterfaceNumber;
	u_int8_t	bAlternateSetting;
	u_int8_t	bNumEndpoints;
	u_int8_t	bInterfaceClass;
	u_int8_t	bInterfaceSubClass;
	u_int8_t	bInterfaceProtocol;
	u_int8_t	iInterface;
} __attribute__ ((packed));

/* Configuration descriptor information.. */
struct usb_config_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int16_t	wTotalLength;
	u_int8_t	bNumInterfaces;
	u_int8_t	bConfigurationValue;
	u_int8_t	iConfiguration;
	u_int8_t	bmAttributes;
	u_int8_t	MaxPower;
} __attribute__ ((packed));

/* String descriptor */
struct usb_string_descriptor {
	u_int8_t	bLength;
	u_int8_t	bDescriptorType;
	u_int16_t	wData[1];
} __attribute__ ((packed));
#define USB_MAX_STRING_LEN 128


#ifndef __GNUC__
	#pragma pack()
#endif

/*
 * Standard requests
 */
#define		USB_REQ_GET_STATUS				0x00
#define		USB_REQ_CLEAR_FEATURE			0x01
/* 0x02 is reserved */
#define		USB_REQ_SET_FEATURE				0x03
/* 0x04 is reserved */
#define		USB_REQ_SET_ADDRESS				0x05
#define		USB_REQ_GET_DESCRIPTOR			0x06
#define		USB_REQ_SET_DESCRIPTOR			0x07
#define		USB_REQ_GET_CONFIGURATION		0x08
#define		USB_REQ_SET_CONFIGURATION		0x09
#define		USB_REQ_GET_INTERFACE			0x0A
#define		USB_REQ_SET_INTERFACE			0x0B
#define		USB_REQ_SYNCH_FRAME				0x0C

#define		USB_TYPE_STANDARD				(0x00 << 5)
#define		USB_TYPE_CLASS					(0x01 << 5)
#define		USB_TYPE_VENDOR					(0x02 << 5)
#define		USB_TYPE_RESERVED				(0x03 << 5)

#define		USB_RECIP_DEVICE				0x00
#define		USB_RECIP_INTERFACE				0x01
#define		USB_RECIP_ENDPOINT				0x02
#define		USB_RECIP_OTHER					0x03

#define		USB_TYPE_MASK					(0x03 << 5)
#define		USB_RECIP_MASK					0x1f

#define		USB_DIR_OUT						0x00
#define		USB_DIR_IN						0x80

#define		USB_ENDPOINT_IN					USB_DIR_IN
#define		USB_ENDPOINT_OUT				USB_DIR_OUT

#define		USB_ENDPOINT_ADDRESS_MASK		0x0f
#define		USB_ENDPOINT_DIR_MASK			0x80
#define		USB_ENDPOINT_TYPE_MASK			0x03
#define		USB_ENDPOINT_TYPE_CONTROL		0
#define		USB_ENDPOINT_TYPE_ISOCHRONOUS	1
#define		USB_ENDPOINT_TYPE_BULK			2
#define		USB_ENDPOINT_TYPE_INTERRUPT		3

#ifdef __cplusplus
}
#endif

#endif /* __USB_H__ */
