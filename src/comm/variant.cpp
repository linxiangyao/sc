#include "variant.h"
#include<stdio.h>
#include "stlComm.h"
S_NAMESPACE_BEGIN


// Variant ----------------------------------------------------------------------------
Variant::Variant()
{
	__initMemeber();
}

Variant::Variant(int8_t i8)
{
	__initMemeber();
    setInt8(i8);
}

Variant::Variant(uint8_t ui8)
{
	__initMemeber();
    setUint8(ui8);
}

Variant::Variant(int16_t i16)
{
	__initMemeber();
    setInt16(i16);
}

Variant::Variant(uint16_t ui16)
{
	__initMemeber();
    setUint16(ui16);
}

Variant::Variant(int32_t i32)
{
	__initMemeber();
    setInt32(i32);
}

Variant::Variant(uint32_t ui32)
{
	__initMemeber();
    setUint32(ui32);
}

Variant::Variant(int64_t i64)
{
	__initMemeber();
    setInt64(i64);
}

Variant::Variant(uint64_t ui64)
{
	__initMemeber();
    setUint64(ui64);
}

Variant::Variant(char c)
{
	__initMemeber();
    setChar(c);
}

Variant::Variant(float f)
{
	__initMemeber();
    setFloat(f);
}

Variant::Variant(double d)
{
	__initMemeber();
    setDouble(d);
}

Variant::Variant(const void * ptr)
{
	__initMemeber();
	setPtr(ptr);
}

Variant::Variant(const std::string& str)
{
	__initMemeber();
    setString(str);
}

Variant::Variant(const byte_t * data, size_t len)
{
	__initMemeber();
	copyByteArrayFrom(data, len);
}

Variant::~Variant() { __clear(); }



// value type ----------------------------------------------------------
int8_t Variant::getInt8() const
{
	return m_i8;
}

uint8_t Variant::getUint8() const
{
	return m_ui8;
}

int16_t Variant::getInt16() const
{
	return m_i16;
}

uint16_t Variant::getUint16() const
{
	return m_ui16;
}

int32_t Variant::getInt32() const
{
	return m_i32;
}

uint32_t Variant::getUint32() const
{
	return m_ui32;
}

int64_t Variant::getInt64() const
{
	return m_i64;
}

uint64_t Variant::getUint64() const
{
	return m_ui64;
}

byte_t Variant::getByte() const
{
	return m_b;
}

char Variant::getChar() const
{
	return m_c;
}

float Variant::getFloat() const
{
	return m_f;
}

double Variant::getDouble() const
{
	return m_d;
}

const void * Variant::getPtr() const
{
	return m_ptr;
}

const std::string& Variant::getString() const
{
	return *m_str;
}

void Variant::setInt8(int8_t i8)
{
    __clear();
    m_i8 = i8;
    m_variantType = EVariantType_int8;
}

void Variant::setUint8(uint8_t ui8)
{
    __clear();
    m_ui8 = ui8;
    m_variantType = EVariantType_uint8;
}

void Variant::setInt16(int16_t i16)
{
    __clear();
    m_i16 = i16;
    m_variantType = EVariantType_int16;
}

void Variant::setUint16(uint16_t ui16)
{
    __clear();
    m_ui16 = ui16;
    m_variantType = EVariantType_uint16;
}

void Variant::setInt32(int32_t i32)
{
    __clear();
    m_i32 = i32;
    m_variantType = EVariantType_int32;
}

void Variant::setUint32(uint32_t ui32)
{
    __clear();
    m_ui32 = ui32;
    m_variantType = EVariantType_uint32;
}

void Variant::setInt64(int64_t i64)
{
    __clear();
    m_i64 = i64;
    m_variantType = EVariantType_int64;
}

void Variant::setUint64(uint64_t ui64)
{
    __clear();
    m_ui64 = ui64;
    m_variantType = EVariantType_uint64;
}

void Variant::setByte(byte_t b)
{
	__clear();
	m_b = b;
	m_variantType = EVariantType_byte;
}

void Variant::setChar(char c)
{
    __clear();
    m_c = c;
    m_variantType = EVariantType_char;
}

void Variant::setFloat(float f)
{
    __clear();
    m_f = f;
    m_variantType = EVariantType_float;
}

void Variant::setDouble(double d)
{
    __clear();
    m_d = d;
    m_variantType = EVariantType_double;
}

void Variant::setPtr(const void * ptr)
{
	__clear();
	m_ptr = (void*)ptr;
	m_variantType = EVariantType_ptr;
}

void Variant::setString(const std::string& str)
{
	__clear();
	m_str = new std::string();
	*m_str = str;
	m_variantType = EVariantType_string;
}


// sz ----------------------------------------------------------
char* Variant::getSzRef() const
{
	if (m_variantType != EVariantType_sz)
		return NULL;
	return m_sz;
}

void Variant::copySzFrom(const char * sz)
{
	__clear();
	m_variantType = EVariantType_sz;

	if (sz == NULL)
		return;

	size_t len = strlen(sz);
	m_sz = new char[len + 1];
	memcpy(m_sz, sz, len + 1);
}

void Variant::copySzTo(char ** sz) const
{
	if (sz == NULL)
		return;
	*sz = NULL;
	if (m_variantType != EVariantType_sz)
		return;
	if (m_sz == NULL)
		return;

	size_t len = strlen(m_sz);
	*sz = new char[len + 1];
	memcpy(*sz, m_sz, len + 1);
}

void Variant::refSz(const char * sz)
{
	__clear();
	m_variantType = EVariantType_sz;
	m_sz = (char*)sz;
	m_is_ref = true;
}

void Variant::attachSz(const char * sz)
{
	if (m_variantType == EVariantType_sz && m_sz == sz && sz != NULL)
	{
		m_is_ref = false;
		m_sz = (char*)sz;
	}
	else
	{
		__clear();
		m_variantType = EVariantType_sz;
		m_sz = (char*)sz;
	}
}

char* Variant::detachSz()
{
	if (m_variantType != EVariantType_sz)
		return NULL;
	char* sz = m_sz;
	m_is_ref = false;
	m_sz = NULL;
	return sz;
}


// byte array ----------------------------------------------------------
void Variant::getByteArrayRef(byte_t** data, size_t* len) const
{
	if (data == NULL || len == NULL)
		return;
	*data = NULL;
	*len = 0;

	if (m_variantType != EVariantType_byteArray)
		return;
	*data = m_bin->getData();
	*len = m_bin->getLen();
}

Binary* Variant::getByteArrayRef() const
{
	if (m_variantType != EVariantType_byteArray)
		return NULL;
	return m_bin;
}

void Variant::copyByteArrayFrom(const byte_t* d, size_t len)
{
	__clear();
	m_variantType = EVariantType_byteArray;
	m_bin = new Binary();
	m_bin->copy(d, len);
}

void Variant::copyByteArrayFrom(const Binary& bin)
{
	__clear();
	m_variantType = EVariantType_byteArray;
	m_bin = new Binary();
	m_bin->copy(bin);
}

void Variant::copyByteArrayTo(byte_t** data, size_t * len) const
{
	if (data == NULL || len == NULL)
		return;
	*data = NULL;
	*len = 0;

	if (m_variantType != EVariantType_byteArray)
		return;
	m_bin->copyTo(data, len);
}

void Variant::copyByteArrayTo(Binary* bin) const
{
	if (m_variantType != EVariantType_byteArray)
		return;
	bin->copy(*m_bin);
}

void Variant::refByteArray(const byte_t* data, size_t len)
{
	__clear();
	m_variantType = EVariantType_byteArray;
	m_is_ref = true;
	m_bin = new Binary();
	m_bin->attach((byte_t*)data, len);
}

void Variant::attachByteArrayFrom(const byte_t* data, size_t len)
{
	__clear();
	m_variantType = EVariantType_byteArray;
	m_bin = new Binary();
	m_bin->attach((byte_t*)data, len);
}

void Variant::attachByteArrayFrom(Binary* bin)
{
	__clear();
	m_variantType = EVariantType_byteArray;
	m_bin = new Binary();
	m_bin->attach(bin);
}

void Variant::detachByteArrayTo(byte_t** data, size_t* len)
{
	if (data == NULL || len == NULL)
		return;
	*data = NULL;
	*len = 0;

	if (m_variantType != EVariantType_byteArray)
		return;
	m_bin->detachTo(data, len);
}

void Variant::detachByteArrayTo(Binary* bin)
{
	if (bin == NULL)
		return;

	if (m_variantType != EVariantType_byteArray)
		return;
	m_bin->detachTo(bin);
}











std::string Variant::toString() const
{
    char buf[56];
	
	switch (m_variantType)
	{
	case EVariantType_byte: sprintf(buf, "%x", (unsigned char)m_b); return buf;
	case EVariantType_char: sprintf(buf, "%c", (char)m_c); return buf;
	case EVariantType_int8: sprintf(buf, "%d", (short)m_i8); return buf;
	case EVariantType_uint8: sprintf(buf, "%u", (unsigned short)m_ui8); return buf;
	case EVariantType_int16: sprintf(buf, "%d", (short)m_i16); return buf;
	case EVariantType_uint16: sprintf(buf, "%u", (unsigned short)m_ui16); return buf;
	case EVariantType_int32: sprintf(buf, "%d", (int)m_i32); return buf;
	case EVariantType_uint32: sprintf(buf, "%u", (unsigned int)m_ui32); return buf;
	case EVariantType_int64: sprintf(buf, "%" PRIi64, m_i64); return buf;
	case EVariantType_uint64: sprintf(buf, "%" PRIu64, m_ui64); return buf;
	case EVariantType_float: sprintf(buf, "%f", (float)m_f); return buf;
	case EVariantType_double: sprintf(buf, "%f", m_d); return buf;
	case EVariantType_sz: return m_sz;
	case EVariantType_string: return *m_str;

	default:
		return std::string();
	}
}

void Variant::__clear()
{
	if (m_variantType == EVariantType_none)
		return;

	if (m_variantType == EVariantType_string)
	{
		if (m_str != NULL)
		{
			delete m_str;
			m_str = NULL;
		}
	}
	else if (m_variantType == EVariantType_sz)
	{
		if (m_sz != NULL)
		{
			if (!m_is_ref)
				delete m_sz;
			m_sz = NULL;
		}
	}
	else if (m_variantType == EVariantType_byteArray)
	{
		if (m_bin != NULL)
		{
			if (m_is_ref)
			{
				m_bin->detach();
			}
			delete m_bin;
			m_bin = NULL;
		}
	}

	__initMemeber();
}

void Variant::__initMemeber()
{
	m_ui64 = 0;
	m_ptr = NULL;
	m_variantType = EVariantType_none;
	m_is_ref = false;
}







// VariantMap --------------------------------------------------------------------
static const std::string s_emptyString;

VariantMap::~VariantMap()
{
	delete_and_erase_collection_elements(&m_vs);
}

int8_t VariantMap::getInt8(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getInt8();
}

uint8_t VariantMap::getUint8(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getUint8();
}

int16_t VariantMap::getInt16(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getInt16();
}

uint16_t VariantMap::getUint16(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getUint16();
}

int32_t VariantMap::getInt32(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getInt32();
}

uint32_t VariantMap::getUint32(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getUint32();
}

int64_t VariantMap::getInt64(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getInt64();
}

uint64_t VariantMap::getUint64(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getUint64();
}

byte_t VariantMap::getByte(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getByte();
}

char VariantMap::getChar(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getChar();
}

float VariantMap::getFloat(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getFloat();
}

double VariantMap::getDouble(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return 0;

	return it->second->getDouble();
}

std::string VariantMap::getString(const std::string& key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return s_emptyString;

	return it->second->getString();
}

const void * VariantMap::getPtr(const std::string & key) const
{
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return NULL;

	return it->second->getPtr();
}




void VariantMap::setInt8(const std::string& key, int8_t i8)
{
	__getOrCreateVariantByKey(key)->setInt8(i8);
}

void VariantMap::setUint8(const std::string& key, uint8_t ui8)
{
	__getOrCreateVariantByKey(key)->setUint8(ui8);
}

void VariantMap::setInt16(const std::string& key, int16_t i16)
{
	__getOrCreateVariantByKey(key)->setInt16(i16);
}

void VariantMap::setUint16(const std::string& key, uint16_t ui16)
{
	__getOrCreateVariantByKey(key)->setUint16(ui16);
}

void VariantMap::setInt32(const std::string& key, int32_t i32)
{
    __getOrCreateVariantByKey(key)->setUint16(i32);
}

void VariantMap::setUint32(const std::string& key, uint32_t ui32)
{
    __getOrCreateVariantByKey(key)->setUint32(ui32);
}

void VariantMap::setInt64(const std::string& key, int64_t i64)
{
    __getOrCreateVariantByKey(key)->setInt64(i64);
}

void VariantMap::setUint64(const std::string& key, uint64_t ui64)
{
    __getOrCreateVariantByKey(key)->setUint64(ui64);
}

void VariantMap::setByte(const std::string& key, byte_t b)
{
    __getOrCreateVariantByKey(key)->setByte(b);
}

void VariantMap::setChar(const std::string& key, char c)
{
    __getOrCreateVariantByKey(key)->setChar(c);
}

void VariantMap::setFloat(const std::string& key, float f)
{
    __getOrCreateVariantByKey(key)->setFloat(f);
}

void VariantMap::setDouble(const std::string& key, double d)
{
    __getOrCreateVariantByKey(key)->setDouble(d);
}

void VariantMap::setString(const std::string& key, const std::string& str)
{
    __getOrCreateVariantByKey(key)->setString(str);
}

void VariantMap::setPtr(const std::string & key, const void * ptr)
{
	__getOrCreateVariantByKey(key)->setPtr(ptr);	
}



void VariantMap::getByteArrayAndCopyTo(const std::string & key, byte_t ** d, size_t * len) const
{
	if (d == NULL || len == NULL)
		return;
	*d = NULL;
	*len = 0;
	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return;
	it->second->copyByteArrayTo(d, len);
}

void VariantMap::getByteArrayAndCopyTo(const std::string & key, Binary * bin) const
{
	if (bin == NULL)
		return;

	vcit it = m_vs.find(key);
	if (it == m_vs.end())
		return;
	it->second->copyByteArrayTo(bin);
}

void VariantMap::getByteArrayAndDetachTo(const std::string& key, Binary* bin)
{
	vit it = m_vs.find(key);
	if (it == m_vs.end())
		return;
	it->second->detachByteArrayTo(bin);
}

void VariantMap::getByteArrayAndDetachTo(const std::string & key, byte_t ** d, size_t * len)
{
	vit it = m_vs.find(key);
	if (it == m_vs.end())
		return;
	it->second->detachByteArrayTo(d, len);
}

void VariantMap::getByteArrayRef(const std::string& key, byte_t** d, size_t* len)
{
	vit it = m_vs.find(key);
	if (it == m_vs.end())
		return;
	it->second->getByteArrayRef(d, len);
}

void VariantMap::setByteArrayAndCopyFrom(const std::string& key, const byte_t* d, size_t len)
{
	__getOrCreateVariantByKey(key)->copyByteArrayFrom(d, len);
}

void VariantMap::setByteArrayAndCopyFrom(const std::string& key, const Binary& bin)
{
	__getOrCreateVariantByKey(key)->copyByteArrayFrom(bin);
}

void VariantMap::setByteArrayAndAttachFrom(const std::string& key, byte_t* d, size_t len)
{
	__getOrCreateVariantByKey(key)->attachByteArrayFrom(d, len);
}

void VariantMap::setByteArrayAndAttachFrom(const std::string& key, Binary* bin)
{
	__getOrCreateVariantByKey(key)->attachByteArrayFrom(bin);
}




void VariantMap::remove(const std::string& key)
{
	delete_and_erase_map_element_by_key(&m_vs, key);
}

bool VariantMap::has(const std::string& key) const
{
    return m_vs.find(key) != m_vs.end();
}




Variant * VariantMap::__getOrCreateVariantByKey(const std::string & key) {
	vit it = m_vs.find(key);
	if (it == m_vs.end())
	{
		Variant* v = new Variant();
		m_vs[key] = v;
		return v;
	}
	else
	{
		return it->second;
	}
}








S_NAMESPACE_END

//
//void Variant::__copyFromVariant(const Variant & v) {
//	__clear();
//
//	if (v.m_variantType == EVariantType_byteArray && !v.m_is_ref)
//	{
//		copyByteArrayFrom(*(v.m_bin));
//	}
//	else if (v.m_variantType == EVariantType_string && !v.m_is_ref)
//	{
//		setString(*(v.m_str));
//	}
//	else
//	{
//		m_variantType = v.m_variantType;
//		m_is_ref = v.m_is_ref;
//		m_ui64 = v.m_ui64;
//	}
//}

