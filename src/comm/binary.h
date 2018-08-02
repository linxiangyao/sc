#ifndef S_BINARY_H_
#define S_BINARY_H_
#include <string.h>
#include <stdio.h>
#include "ns.h"
#include "intType.h"
S_NAMESPACE_BEGIN




class Binary
{
public:
	Binary() { __setMemeberToNull(); }
	Binary(const Binary& b) { __setMemeberToNull(); copy(b); }
    ~Binary() { clear();  }

	Binary& operator = (const Binary&b) {
		copy(b);
		return *this;
	}

	void preallocate(size_t cap)
	{
		if (cap == 0)
			return;
		if (m_data != NULL || m_cap != 0 || m_len != 0)
			return;

		m_cap = cap;
		m_data = new byte_t[cap];
	}
    
    void attach(Binary& binary) {
		__attach(binary.m_data, binary.m_len, binary.m_cap);
        binary.detach();
    }

	void attach(Binary* binary) {
		if (binary == NULL)
		{
			clear();
			return;
		}
		__attach(binary->m_data, binary->m_len, binary->m_cap);
		binary->detach();
	}

    void attach(byte_t* data, size_t len) {
		__attach(data, len, len);
    }

	void detach() {
		__setMemeberToNull();
	}

	void detachTo(Binary* bin)
	{
		if (bin == NULL)
			return;
		bin->attach(this);
	}

	void detachTo(byte_t** data, size_t* len)
	{
		if (data == NULL || len == NULL)
			return;
		*data = getData();
		*len = getLen();
		detach();
	}

	void copy(const Binary& binary) {
		__copy(binary.m_data, binary.m_len);
	}

    void copy(const byte_t* data, size_t len) {
		__copy(data, len);
    }

	void copyTo(byte_t** data, size_t* len) const
	{
		if (data == NULL || len == NULL)
			return;
		*data = NULL;
		*len = 0;
		if (m_len == 0)
			return;

		*data = new byte_t[m_len];
		memcpy(*data, m_data, m_len);
		*len = m_len;
	}

    void append(const Binary& bin) {
		__append(bin.getData(), bin.getLen());
    }

	void append(uint8_t i) {
		__append((const byte_t*)&i, 1);
	}

	void append(uint16_t i) {
		__append((const byte_t*)&i, 2);
	}

	void append(uint32_t i) {
		__append((const byte_t*)&i, 4);
	}

	void append(uint64_t i) {
		__append((const byte_t*)&i, 8);
	}

	void append(int8_t i) {
		__append((const byte_t*)&i, 1);
	}

	void append(int16_t i) {
		__append((const byte_t*)&i, 2);
	}

	void append(int32_t i) {
		__append((const byte_t*)&i, 4);
	}

	void append(int64_t i) {
		__append((const byte_t*)&i, 8);
	}

    void append(const byte_t* data, size_t len) {
		__append(data, len);
    }

    void shrinkBegin(size_t len)
    {
		if (len == 0)
			return;

        if (m_len <= len)
        {
            clear();
            return;
        }

        memcpy(m_data, m_data + len, m_len - len);
        m_len = m_len - len;
    }

    void shrinkEnd(size_t len)
    {
		if (len == 0)
			return;

        if (m_len <= len)
        {
            clear();
            return;
        }

        m_len = len;
    }

    void clear() {
		if (m_data == NULL)
			return;

        delete[] m_data;
		__setMemeberToNull();
    }

    const byte_t* getData() const {
		if (m_len == 0)
			return NULL;
        return m_data;
    }

    byte_t* getData() {
		if (m_len == 0)
			return NULL;
        return m_data;
    }

    size_t getLen() const {
        return m_len;
    }

	size_t getCap() const {
		return m_cap;
	}


private:
	void __attach(byte_t* data, size_t len, size_t cap)
	{
		if (data == NULL || len == 0)
		{
			clear();
			return;
		}
		if (m_data == data)
		{
			m_len = len;
			return;
		}

		clear();
		m_data = data;
		m_len = len;
		m_cap = cap;
	}

	void __copy(const byte_t* data, size_t len) {
		if (data == NULL || len == 0)
			return;
		if (data == m_data)
		{
			m_len = len;
			return;
		}
		
		if (len > m_cap)
		{
			size_t new_cap = __calNewCap(len);
			clear();
			m_cap = new_cap;
			m_data = new byte_t[m_cap];
		}

		m_len = len;
		memcpy(m_data, data, len);
	}

	void __append(const byte_t* data, size_t len) {
		if (data == NULL || len == 0)
			return;

		size_t new_len = m_len + len;
		if (new_len > m_cap)
		{
			size_t new_cap = __calNewCap(new_len);
			byte_t* new_data = new byte_t[new_cap];
			if(m_len > 0)
				memcpy(new_data, m_data, m_len);
			memcpy(new_data + m_len, data, len);

			clear();
			m_len = new_len;
			m_data = new_data;
			m_cap = new_cap;
		}
		else
		{
			memcpy(m_data + m_len, data, len);
			m_len = new_len;
		}
	}

	size_t __calNewCap(size_t data_len)
	{
		size_t new_cap = m_cap;
		if (new_cap == 0)
			new_cap = 1;
		while (true)
		{
			if (new_cap > data_len)
				return new_cap;
			if (new_cap > 1024 * 1024)
				new_cap += 1024 * 1024;
			else
				new_cap *= 2;
			if (new_cap > 100 * 1024 * 1024)
			{
				printf("err! mem is too big!! new_cap=%zd\n", new_cap);
				return new_cap;
			}
		}
	}

	void __setMemeberToNull()
	{
		m_data = NULL;
		m_len = 0;
		m_cap = 0;
	}

    byte_t* m_data;
    size_t m_len;
	size_t m_cap;
};




class BinaryReader
{
public:
	void attach(const Binary& bin)
	{
		attach(bin.getData(), bin.getLen());
	}

	void attach(const byte_t* data, size_t len)
	{
		m_data = data;
		m_len = len;
		m_read_pos = 0;
	}

	const byte_t* read(size_t len)
	{
		if (m_read_pos + len > m_len)
			return NULL;

		size_t cur_pos = m_read_pos;
		m_read_pos += len;
		return m_data + cur_pos;
	}

	size_t getReadPos() const 
	{
		return m_read_pos;
	}

private:
	const byte_t* m_data;
	size_t m_len;
	size_t m_read_pos;
};



S_NAMESPACE_END
#endif
