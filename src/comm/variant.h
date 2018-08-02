#ifndef S_VARIANT_H
#define S_VARIANT_H
#include <string>
#include <vector>
#include <map>
#include "ns.h"
#include "intType.h"
#include "binary.h"
S_NAMESPACE_BEGIN



enum EVariantType
{
	EVariantType_none,

	// value type ---------
	EVariantType_int8,
	EVariantType_uint8,
	EVariantType_int16,
	EVariantType_uint16,
	EVariantType_int32,
	EVariantType_uint32,
	EVariantType_int64,
	EVariantType_uint64,
	EVariantType_byte,
    EVariantType_char,
    EVariantType_float,
    EVariantType_double,
	EVariantType_ptr,
    EVariantType_string,

	// ref type ---------
	EVariantType_sz,
	EVariantType_byteArray,
};


/*
comm data type.

value type:
	get:		return value
	set:		set value

ref type:
	copyFrom:   free old data, and copy data (own data)
	attach:		free old data, and ref to new data (own data)
	ref:		free old data, and ref to new data (not own data)
	copyTo:		return copied data
	getRef:		return data ref
	detach:		return data ref (not own data)
*/
class Variant
{
private:
	Variant(const Variant& v) {}
	Variant& operator =(const Variant& v) { return *this; }

public:
    Variant();
    Variant(int8_t i8);
    Variant(uint8_t ui8);
    Variant(int16_t i16);
    Variant(uint16_t ui16);
    Variant(int32_t i32);
    Variant(uint32_t ui32);
    Variant(int64_t i64);
    Variant(uint64_t ui64);
    Variant(char c);
    Variant(float f);
    Variant(double d);
	Variant(const void* ptr);
    Variant(const std::string& str);
	Variant(const byte_t* data, size_t len);
	~Variant();


	// value type -----------------------------------------------------------------------
	int8_t      getInt8() const;
	uint8_t     getUint8() const;
	int16_t     getInt16() const;
	uint16_t    getUint16() const;
	int32_t     getInt32() const;
	uint32_t    getUint32() const;
	int64_t     getInt64() const;
	uint64_t    getUint64() const;
	byte_t      getByte() const;
	char        getChar() const;
	float       getFloat() const;
	double      getDouble() const;
	const void*	getPtr() const;
	const std::string& getString() const;

    void setInt8(int8_t i8);
    void setUint8(uint8_t ui8);
    void setInt16(int16_t i16);
    void setUint16(uint16_t ui16);
    void setInt32(int32_t i32);
    void setUint32(uint32_t ui32);
    void setInt64(int64_t i64);
    void setUint64(uint64_t ui64);
    void setByte(byte_t b);
    void setChar(char c);
    void setFloat(float f);
    void setDouble(double d);
	void setPtr(const void* ptr);
	void setString(const std::string& str);


    // sz -----------------------------------------------------------------------------------------
	char*	getSzRef() const;				// return ref
	void	copySzFrom(const char* sz);		// copy from
	void	copySzTo(char** sz) const;		// copy to
	void	refSz(const char* sz);			// no copy, no own string, just ref
	void	attachSz(const char* sz);		// no copy, own sz
	char*	detachSz();						// no copy, no own sz


	// byte array ------------------------------------------------------------------------------------
	void	getByteArrayRef(byte_t** data, size_t* len) const;
	Binary*	getByteArrayRef() const;

	void	copyByteArrayFrom(const byte_t* data, size_t len);	// copy, own byte array
	void	copyByteArrayFrom(const Binary& bin);				// copy, own byte array
	void	copyByteArrayTo(byte_t** data, size_t* len) const;
	void	copyByteArrayTo(Binary* bin) const;

	void	refByteArray(const byte_t* data, size_t len);		// no copy, no own byte array, just ref

	void	attachByteArrayFrom(const byte_t* data, size_t len);	// no copy, own byte array
	void	attachByteArrayFrom(Binary* bin);						// no copy, own byte array
	void	detachByteArrayTo(byte_t** data, size_t* len);
	void	detachByteArrayTo(Binary* bin);


    std::string toString() const;


private:
    void __clear();
	void __initMemeber();

    union
    {
		int8_t m_i8;
		uint8_t m_ui8;
		int16_t m_i16;
		uint16_t m_ui16;
		int32_t m_i32;
		uint32_t m_ui32;
		int64_t m_i64;
		uint64_t m_ui64;
		byte_t m_b;
		char m_c;
		float m_f;
		double m_d;
		void* m_ptr;
		char* m_sz;
		std::string* m_str;
		Binary* m_bin;
    };
    EVariantType m_variantType;
	bool m_is_ref;
};



class VariantMap
{
public:
	~VariantMap();

	int8_t      getInt8(const std::string& key) const;
	uint8_t     getUint8(const std::string& key) const;
	int16_t     getInt16(const std::string& key) const;
	uint16_t    getUint16(const std::string& key) const;
	int32_t     getInt32(const std::string& key) const;
	uint32_t    getUint32(const std::string& key) const;
	int64_t     getInt64(const std::string& key) const;
	uint64_t    getUint64(const std::string& key) const;
	byte_t      getByte(const std::string& key) const;
	char        getChar(const std::string& key) const;
	float       getFloat(const std::string& key) const;
	double      getDouble(const std::string& key) const;
	std::string getString(const std::string& key) const;
	const void*	getPtr(const std::string& key) const;

    void setInt8(const std::string& key, int8_t i8);
    void setUint8(const std::string& key, uint8_t ui8);
    void setInt16(const std::string& key, int16_t i16);
    void setUint16(const std::string& key, uint16_t ui16);
    void setInt32(const std::string& key, int32_t i32);
    void setUint32(const std::string& key, uint32_t ui32);
    void setInt64(const std::string& key, int64_t i64);
    void setUint64(const std::string& key, uint64_t ui64);
    void setByte(const std::string& key, byte_t b);
    void setChar(const std::string& key, char c);
    void setFloat(const std::string& key, float f);
    void setDouble(const std::string& key, double d);
	void setString(const std::string& key, const std::string& str);
	void setPtr(const std::string& key, const void* ptr);


	void set(const std::string& key, int8_t v) { setInt8(key, v); }
	void set(const std::string& key, uint8_t v) { setUint8(key, v); }
	void set(const std::string& key, int16_t v) { setInt16(key, v); }
	void set(const std::string& key, uint16_t v) { setUint16(key, v); }
	void set(const std::string& key, int32_t v) { setInt32(key, v); }
	void set(const std::string& key, uint32_t v) { setUint32(key, v); }
	void set(const std::string& key, int64_t v) { setInt64(key, v); }
	void set(const std::string& key, uint64_t v) { setUint64(key, v); }
	//void set(const std::string& key, byte_t v) { setByte(key, v); }
	void set(const std::string& key, char v) { setChar(key, v); }
	void set(const std::string& key, float v) { setFloat(key, v); }
	void set(const std::string& key, double v) { setDouble(key, v); }
	void set(const std::string& key, const char* v) { setString(key, v); }
	void set(const std::string& key, const std::string& v) { setString(key, v); }
	void set(const std::string& key, const void* v) { setPtr(key, v); }


	void getByteArrayAndCopyTo(const std::string& key, byte_t** d, size_t* len) const;
	void getByteArrayAndCopyTo(const std::string& key, Binary* bin) const;
	void getByteArrayAndDetachTo(const std::string& key, byte_t** d, size_t* len);
	void getByteArrayAndDetachTo(const std::string& key, Binary* bin);
	void getByteArrayRef(const std::string& key, byte_t** d, size_t* len);
	void setByteArrayAndCopyFrom(const std::string& key, const byte_t* d, size_t len);
	void setByteArrayAndCopyFrom(const std::string& key, const Binary& bin);
	void setByteArrayAndAttachFrom(const std::string& key, byte_t* d, size_t len);
	void setByteArrayAndAttachFrom(const std::string& key, Binary* bin);


    void remove(const std::string& key);
    bool has(const std::string& key) const;




private:
	typedef std::map<std::string, Variant*>::iterator vit;
	typedef std::map<std::string, Variant*>::const_iterator vcit;

	Variant* __getOrCreateVariantByKey(const std::string& key);

    std::map<std::string, Variant*> m_vs;
};





S_NAMESPACE_END
#endif

