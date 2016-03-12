// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    device.h

    Device interface functions.

***************************************************************************/

#ifndef __EMU_H__
#define __EMU_H__

class device_t;

#include <assert.h>
#include <string>
#include <map>
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

// ======================> device_t

// device_t represents a device
class device_t : public delegate_late_bind
{
	device_t(const device_t &) = delete; 
    device_t &operator=(const device_t &) = delete;
public:
	// construction/destruction
	device_t(const char *tag) { m_tag = tag; }
	virtual ~device_t() { }

	// getters
	const char *tag() const { return m_tag.c_str(); }

	int read() { return m_internal; }
	void write(int j) { m_internal = j; }
	device_t* subdevice(const char* m_device_name) { return m_subdevices[m_device_name]; }
	void add_device(const char* name, device_t *device) { m_subdevices.insert(std::pair<std::string,device_t*>(name,device)); }
private:
	int				m_internal;
	std::string             m_tag;                  // full tag for this instance
	std::map<std::string,device_t*>  m_subdevices;
};


#endif  /* __EMU_H__ */
