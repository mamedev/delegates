// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    device.h

    Device interface functions.

***************************************************************************/

#ifndef __DEVICE_H__
#define __DEVICE_H__

class device_t;

#include <assert.h>
#include <string>
#include "osdcomm.h"
#include "devdelegate.h"


//**************************************************************************
//  EXCEPTION CLASSES
//**************************************************************************

// emu_exception is the base class for all emu-related exceptions
class emu_exception : public std::exception { };


// emu_fatalerror is a generic fatal exception that provides an error string
class emu_fatalerror : public emu_exception
{
public:
	emu_fatalerror(const char *format, ...) ATTR_PRINTF(2, 3) { }
	emu_fatalerror(const char *format, va_list ap) { }
	emu_fatalerror(int _exitcode, const char *format, ...) ATTR_PRINTF(3, 4) { }
	emu_fatalerror(int _exitcode, const char *format, va_list ap) { }

	const char *string() const { return text; }
	int exitcode() const { return code; }

private:
	char text[1024];
	int code;
};


#define DISABLE_COPYING(_Type) \
private: \
	_Type(const _Type &) = delete; \
	_Type &operator=(const _Type &) = delete

// ======================> device_t

// device_t represents a device
class device_t : public delegate_late_bind
{
	DISABLE_COPYING(device_t);

	// construction/destruction
	device_t(const char *tag) { m_tag = tag; }
public:
	virtual ~device_t() { }

	// getters
	const char *tag() const { return m_tag.c_str(); }

	device_t* subdevice(const char* m_device_name) { return nullptr; }
private:
	std::string             m_tag;                  // full tag for this instance
};


#endif  /* __DEVICE_H__ */
