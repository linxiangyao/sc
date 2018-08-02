#ifndef S_LOG_COMM_H
#define S_LOG_COMM_H
#include <string>
#include <vector>
#include <mutex>
#include "../comm/comm.h"
S_NAMESPACE_BEGIN



class LogVariant
{
public:
	LogVariant() {}
	LogVariant(int8_t i8) { m_variant.setInt8(i8); }
	LogVariant(uint8_t ui8) { m_variant.setUint8(ui8); }
	LogVariant(int16_t i16) { m_variant.setInt16(i16); }
	LogVariant(uint16_t ui16) { m_variant.setUint16(ui16); }
	LogVariant(int32_t i32) { m_variant.setInt32(i32); }
	LogVariant(uint32_t ui32) { m_variant.setUint32(ui32); }
#ifdef S_OS_MAC
    LogVariant(size_t ui32) { m_variant.setUint32(uint32_t(ui32)); }
#endif
	LogVariant(int64_t i64) { m_variant.setInt64(i64); }
	LogVariant(uint64_t ui64) { m_variant.setUint64(ui64); }
	LogVariant(char c) { m_variant.setChar(c); }
	LogVariant(float f) { m_variant.setFloat(f); }
	LogVariant(double d) { m_variant.setDouble(d); }
	LogVariant(const char* sz) { m_variant.refSz(sz); }
	LogVariant(const std::string& str) { m_variant.setString(str); }

	std::string toString() const { return m_variant.toString(); }

private:
	Variant m_variant;
};



enum ELogLevel
{
    ELogLevel_verbose,
    ELogLevel_debug,
    ELogLevel_info,
    ELogLevel_warning,
    ELogLevel_error,
    ELogLevel_fatal,
};


std::string LogLevelToOneChar(ELogLevel logLevel);


class LogInfo
{
public:
    std::string m_tag;
    std::string m_fileName;
    std::string m_functionName;
    unsigned int m_lineNum;
    time_t      m_time;
    std::string m_msg;
    ELogLevel m_logLevel;
};


class FormatInfo
{
public:
    FormatInfo() : m_isShowLevel(true), m_isShowTag(false), m_isShowTime(true), m_isShowFileName(true), m_isShowLineNum(true), m_isShowFunction(true) {}

	bool m_isShowLevel;
    bool m_isShowTag;
    bool m_isShowTime;
    bool m_isShowFileName;
    bool m_isShowLineNum;
    bool m_isShowFunction;
};

class LogFormator
{
public:
    std::string formatLog(const LogInfo& logInfo);
	std::string formatArg(const char* sz, const std::vector<const LogVariant*>& variants);
    void setFormatInfo(const FormatInfo& info);
	const FormatInfo& getFormatInfo() { return m_formatInfo; }
    
private:
    FormatInfo m_formatInfo;
};


class LogAppender
{
 public:
    virtual~LogAppender() {}
    virtual void append(const LogInfo& logInfo) = 0;
    virtual LogFormator* getFormator() = 0;
};





S_NAMESPACE_END
#endif




















// LogVariant ----------------------------------------------------------------------------------------
//LogVariant::LogVariant()
//{
//	__clear();
//}
//
//LogVariant::LogVariant(int8_t i8)
//{
//	setInt8(i8);
//}
//
//LogVariant::LogVariant(uint8_t ui8)
//{
//	setUint8(ui8);
//}
//
//LogVariant::LogVariant(int16_t i16)
//{
//	setInt16(i16);
//}
//
//LogVariant::LogVariant(uint16_t ui16)
//{
//	setUint16(ui16);
//}
//
//LogVariant::LogVariant(int32_t i32)
//{
//	setInt32(i32);
//}
//
//LogVariant::LogVariant(uint32_t ui32)
//{
//	setUint32(ui32);
//}
//
//LogVariant::LogVariant(int64_t i64)
//{
//	setInt64(i64);
//}
//
//LogVariant::LogVariant(uint64_t ui64)
//{
//	setUint64(ui64);
//}
//
//LogVariant::LogVariant(char c)
//{
//	setChar(c);
//}
//
//LogVariant::LogVariant(float f)
//{
//	setFloat(f);
//}
//
//LogVariant::LogVariant(double d)
//{
//	setDouble(d);
//}
//
//LogVariant::LogVariant(const char * sz)
//{
//	setSz(sz);
//}
//
//LogVariant::LogVariant(const std::string & str)
//{
//	setString(str);
//}
//

//
//void LogVariant::setInt8(int8_t i8)
//{
//	__clear();
//	m_variantType = EVariantType_int8;
//	m_i8 = i8;
//}
//
//void LogVariant::setUint8(uint8_t ui8)
//{
//	__clear();
//	m_variantType = EVariantType_uint8;
//	m_ui8 = ui8;
//}
//
//void LogVariant::setInt16(int16_t i16)
//{
//	__clear();
//	m_variantType = EVariantType_int16;
//	m_i16 = i16;
//}
//
//void LogVariant::setUint16(uint16_t ui16)
//{
//	__clear();
//	m_variantType = EVariantType_uint16;
//	m_ui16 = ui16;
//}
//
//void LogVariant::setInt32(int32_t i32)
//{
//	__clear();
//	m_variantType = EVariantType_int32;
//	m_i32 = i32;
//}
//
//void LogVariant::setUint32(uint32_t ui32)
//{
//	__clear();
//	m_variantType = EVariantType_uint32;
//	m_ui32 = ui32;
//}
//
//void LogVariant::setInt64(int64_t i64)
//{
//	__clear();
//	m_variantType = EVariantType_int64;
//	m_i64 = i64;
//}
//
//void LogVariant::setUint64(uint64_t ui64)
//{
//	__clear();
//	m_variantType = EVariantType_uint64;
//	m_ui64 = ui64;
//}
//
//void LogVariant::setByte(byte_t b)
//{
//	__clear();
//	m_variantType = EVariantType_byte;
//	m_b = b;
//}
//
//void LogVariant::setChar(char c)
//{
//	__clear();
//	m_variantType = EVariantType_char;
//	m_c = c;
//}
//
//void LogVariant::setFloat(float f)
//{
//	__clear();
//	m_variantType = EVariantType_float;
//	m_f = f;
//}
//
//void LogVariant::setDouble(double d)
//{
//	__clear();
//	m_variantType = EVariantType_double;
//	m_d = d;
//}
//
//void LogVariant::setSz(const char * sz)
//{
//	__clear();
//	m_variantType = EVariantType_sz;
//	m_sz = sz;
//}
//
//void LogVariant::setString(const std::string & str)
//{
//	__clear();
//	m_variantType = EVariantType_string;
//	m_str = &str;
//}
//
//
//
//int8_t LogVariant::getInt8() const
//{
//	return m_i8;
//}
//
//uint8_t LogVariant::getUint8() const
//{
//	return m_ui8;
//}
//
//int16_t LogVariant::getInt16() const
//{
//	return m_i16;
//}
//
//uint16_t LogVariant::getUint16() const
//{
//	return m_ui16;
//}
//
//int32_t LogVariant::getInt32() const
//{
//	return m_i32;
//}
//
//uint32_t LogVariant::getUint32() const
//{
//	return m_ui32;
//}
//
//int64_t LogVariant::getInt64() const
//{
//	return m_i64;
//}
//
//uint64_t LogVariant::getUint64() const
//{
//	return m_ui64;
//}
//
//byte_t LogVariant::getByte() const
//{
//	return m_b;
//}
//
//char LogVariant::getChar() const
//{
//	return m_c;
//}
//
//float LogVariant::getFloat() const
//{
//	return m_f;
//}
//
//double LogVariant::getDouble() const
//{
//	return m_d;
//}
//
//const std::string & LogVariant::getString() const
//{
//	return *m_str;
//}
//
//
//
//std::string LogVariant::toString() const
//{
//	char buf[56];
//
//	switch (m_variantType)
//	{
//	case EVariantType_byte: sprintf(buf, "%x", (unsigned char)m_b); return buf;
//	case EVariantType_char: sprintf(buf, "%c", (char)m_c); return buf;
//	case EVariantType_int8: sprintf(buf, "%d", (short)m_i8); return buf;
//	case EVariantType_uint8: sprintf(buf, "%u", (unsigned short)m_ui8); return buf;
//	case EVariantType_int16: sprintf(buf, "%d", (short)m_i16); return buf;
//	case EVariantType_uint16: sprintf(buf, "%u", (unsigned short)m_ui16); return buf;
//	case EVariantType_int32: sprintf(buf, "%d", (int)m_i32); return buf;
//	case EVariantType_uint32: sprintf(buf, "%u", (unsigned int)m_ui32); return buf;
//	case EVariantType_int64: sprintf(buf, "%" PRIi64, m_i64); return buf;
//	case EVariantType_uint64: sprintf(buf, "%" PRIu64, m_ui64); return buf;
//	case EVariantType_float: sprintf(buf, "%f", m_f); return buf;
//	case EVariantType_double: sprintf(buf, "%f", m_d); return buf;
//	case EVariantType_sz: return m_sz == NULL ? "" : m_sz;
//	case EVariantType_string: return m_str == NULL ? "" : *m_str;
//
//	default:
//		return std::string();
//	}
//}
//
//void LogVariant::__clear()
//{
//	m_sz = NULL;
//	m_variantType = EVariantType_none;
//}
//

//void setInt8(int8_t i8);
//void setUint8(uint8_t ui8);
//void setInt16(int16_t i16);
//void setUint16(uint16_t ui16);
//void setInt32(int32_t i32);
//void setUint32(uint32_t ui32);
//void setInt64(int64_t i64);
//void setUint64(uint64_t ui64);
//void setByte(byte_t b);
//void setChar(char c);
//void setFloat(float f);
//void setDouble(double d);
//void setSz(const char* sz);
//void setString(const std::string& str);


//int8_t      getInt8() const;
//uint8_t     getUint8() const;
//int16_t     getInt16() const;
//uint16_t    getUint16() const;
//int32_t     getInt32() const;
//uint32_t    getUint32() const;
//int64_t     getInt64() const;
//uint64_t    getUint64() const;
//byte_t      getByte() const;
//char        getChar() const;
//float       getFloat() const;
//double      getDouble() const;
//const std::string& getString() const;

//int8_t      getInt8() const { return m_variant.getInt8(); }
//uint8_t     getUint8() const { return m_variant.getUint8(); }
//int16_t     getInt16() const { return m_variant.getInt8(); }
//uint16_t    getUint16() const { return m_variant.getInt8(); }
//int32_t     getInt32() const { return m_variant.getInt8(); }
//uint32_t    getUint32() const { return m_variant.getInt8(); }
//int64_t     getInt64() const { return m_variant.getInt8(); }
//uint64_t    getUint64() const { return m_variant.getInt8(); }
//byte_t      getByte() const { return m_variant.getInt8(); }
//char        getChar() const { return m_variant.getInt8(); }
//float       getFloat() const { return m_variant.getInt8(); }
//double      getDouble() const { return m_variant.getInt8(); }
//const void*	getPtr() const { return m_variant.getInt8(); }
//const std::string& getString() const { return m_variant.getInt8(); }

//void setInt8(int8_t i8);
//void setUint8(uint8_t ui8);
//void setInt16(int16_t i16);
//void setUint16(uint16_t ui16);
//void setInt32(int32_t i32);
//void setUint32(uint32_t ui32);
//void setInt64(int64_t i64);
//void setUint64(uint64_t ui64);
//void setByte(byte_t b);
//void setChar(char c);
//void setFloat(float f);
//void setDouble(double d);
//void setPtr(const void* ptr);
//void setString(const std::string& str);



//union
//{
//	int8_t m_i8;
//	uint8_t m_ui8;
//	int16_t m_i16;
//	uint16_t m_ui16;
//	int32_t m_i32;
//	uint32_t m_ui32;
//	int64_t m_i64;
//	uint64_t m_ui64;
//	byte_t m_b;
//	char m_c;
//	float m_f;
//	double m_d;
//	const std::string* m_str;
//	const char* m_sz;
//};
//EVariantType m_variantType;
