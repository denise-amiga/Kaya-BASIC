/***
* comutil.h - Native C++ compiler COM support - BSTR, VARIANT wrappers header
*
*	Copyright (C) 1996-1997 Microsoft Corporation
*	All rights reserved.
*
****/

#if !defined(_INC_COMUTIL)
#pragma option push -b -a8 -pc -A- -w-inl /*P_O_Push __BORLANDC__: This file was forgotten in the PSDK so it was taken from the VC++ 6.0 */
#define _INC_COMUTIL

//#include <windows.h>
#include <objbase.h>

#define msgerr(x) ;//MessageBox(NULL, x,"Error!",MB_ICONEXCLAMATION|MB_OK);

#define _com_raise_error(x) msgerr("unknown error:_com_raise_error"); 
#define _com_issue_error(x) msgerr("unknown error:_com_issue_error"); 


/*
void __stdcall _com_raise_error(long hr, IErrorInfo* perrinfo = 0) throw (_com_error) 
{
  throw _com_error((HRESULT)hr, perrinfo);
}

void __stdcall _com_issue_error(long hr) 
{

  throw _com_error((HRESULT)hr);
}
*/
//////////////////////////////////////////////////////////////////////////////
//
// Forward class declarations
//
//////////////////////////////////////////////////////////////////////////////

class _bstr_t;
class _variant_t;

//////////////////////////////////////////////////////////////////////////////
//
// Error checking routines
//
//////////////////////////////////////////////////////////////////////////////

namespace _com_utilBPP {
	inline void CheckError(HRESULT hr)
	{
		if (FAILED(hr)) {
			_com_issue_error(hr);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Routines for handling conversions between BSTR and char*
//
//////////////////////////////////////////////////////////////////////////////

namespace _com_utilBPP {
	// Convert char * to BSTR
	//
	inline BSTR ConvertAnsiStrToBStr(const char* pSrc) {
		
	    if(!pSrc) return NULL;

	    DWORD cwch;

	    BSTR wsOut(NULL);

	    if(cwch = ::MultiByteToWideChar(CP_ACP, 0, pSrc, 
	         -1, NULL, 0))//get size minus NULL terminator
	    {
	            cwch--;
	            wsOut = ::SysAllocStringLen(NULL, cwch);

	        if(wsOut)
	        {
	            if(!::MultiByteToWideChar(CP_ACP, 
	                     0, pSrc, -1, wsOut, cwch))
	            {
	                if(ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
	                    return wsOut;
	                ::SysFreeString(wsOut);//must clean up
	                wsOut = NULL;
	            }
	        }

	    };

	    return wsOut;
	}

	// Convert BSTR to char *
	//
	inline char* ConvertBStrToAnsiStr(BSTR pSrc) {
	
	    if(!pSrc) return NULL;

	    //convert even embeded NULL
	    DWORD cb,cwch = ::SysStringLen(pSrc);

	    char *szOut = NULL;

	    if(cb = ::WideCharToMultiByte(CP_ACP, 0, 
	               pSrc, cwch + 1, NULL, 0, 0, 0))
	    {
	        szOut = new char[cb];
	        if(szOut)
	        {
	            szOut[cb - 1]  = '\0';

	            if(!::WideCharToMultiByte(CP_ACP, 0, 
	                pSrc, cwch + 1, szOut, cb, 0, 0))
	            {
	                delete []szOut;//clean up if failed;
	                szOut = NULL;
	            }
	        }
	    }

	    return szOut;
		
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Wrapper class for BSTR
//
//////////////////////////////////////////////////////////////////////////////

class _bstr_t {
public:
	// Constructors
	//
	_bstr_t() ;
	_bstr_t(const _bstr_t& s);
	_bstr_t(const char* s) ;
	_bstr_t(const wchar_t* s) ;
	//_bstr_t(const _variant_t& var) ;
	_bstr_t(BSTR bstr, bool fCopy);

	// Destructor
	//
	~_bstr_t();

	// Assignment operators
	//
	_bstr_t& operator=(const _bstr_t& s);
	_bstr_t& operator=(const char* s) ;
	_bstr_t& operator=(const wchar_t* s) ;
	_bstr_t& operator=(const _variant_t& var) ;

	// Operators
	//
	_bstr_t& operator+=(const _bstr_t& s) ;
	_bstr_t operator+(const _bstr_t& s) const ;

	// Friend operators
	//
	friend _bstr_t operator+(const char* s1, const _bstr_t& s2) ;
	friend _bstr_t operator+(const wchar_t* s1, const _bstr_t& s2) ;

	// Extractors
	//
	operator const wchar_t*() const ;
	operator wchar_t*() const ;
	operator const char*() const ;
	operator char*() const ;

	// Comparison operators
	//
	bool operator!() const ;
	bool operator==(const _bstr_t& str) const ;
	bool operator!=(const _bstr_t& str) const ;
	bool operator<(const _bstr_t& str) const ;
	bool operator>(const _bstr_t& str) const ;
	bool operator<=(const _bstr_t& str) const ;
	bool operator>=(const _bstr_t& str) const ;

	// Low-level helper functions
	//
	BSTR copy() const ;
	unsigned int length() const ;

	// Binary string assign
	//
	void Assign(BSTR s) ;

private:
	// Referenced counted wrapper
	//
	class Data_t {
	public:
		// Constructors
		//
		Data_t(const char* s) ;
		Data_t(const wchar_t* s) ;
		Data_t(BSTR bstr, bool fCopy) ;
		Data_t(const _bstr_t& s1, const _bstr_t& s2) ;

		// Reference counting routines
		//
		unsigned long AddRef() ;
		unsigned long Release() ;

		// Extractors
		//
		operator const wchar_t*() const ;
		operator const char*() const ;

		// Low-level helper functions
		//
		const wchar_t* GetWString() const ;
		const char* GetString() const ;

		BSTR Copy() const ;
		void Assign(BSTR s) ;
		unsigned int Length() const ;
		int Compare(const Data_t& str) const ;

	private:
		wchar_t*		m_wstr;
		mutable char*	m_str;
		unsigned long	m_RefCount;

		// Never allow default construction
		//
		Data_t() ;

		// Never allow copy
		//
		Data_t(const Data_t& s) ;

		// Prevent deletes from outside. Release() must be used.
		//
		~Data_t() ;

		void _Free() ;
	};

private:
	// Reference counted representation
	//
	Data_t* m_Data;

private:
	// Low-level utilities
	//
	void _AddRef() ;
	void _Free() ;
	int _Compare(const _bstr_t& str) const ;
};

//////////////////////////////////////////////////////////////////////////////
//
// Constructors
//
//////////////////////////////////////////////////////////////////////////////

// Default constructor
//
inline _bstr_t::_bstr_t() 
	: m_Data(NULL)
{
}

// Copy constructor
//
inline _bstr_t::_bstr_t(const _bstr_t& s) 
	: m_Data(s.m_Data)
{
	_AddRef();
}

// Construct a _bstr_t from a const char*
//
inline _bstr_t::_bstr_t(const char* s) //
	: m_Data(new Data_t(s))
{
	if (m_Data == NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a _bstr_t from a const whar_t*
//
inline _bstr_t::_bstr_t(const wchar_t* s) 
	: m_Data(new Data_t(s))
{
	if (m_Data == NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a _bstr_t from a BSTR.  If fCopy is FALSE, give control of
// data to the _bstr_t without making a new copy.
//
inline _bstr_t::_bstr_t(BSTR bstr, bool fCopy) 
	: m_Data(new Data_t(bstr, fCopy))
{
	if (m_Data == NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Destructor
//
inline _bstr_t::~_bstr_t() 
{
	_Free();
}

//////////////////////////////////////////////////////////////////////////////
//
// Assignment operators
//
//////////////////////////////////////////////////////////////////////////////

// Default assign operator
//
inline _bstr_t& _bstr_t::operator=(const _bstr_t& s) 
{
	const_cast<_bstr_t*>(&s)->_AddRef();
	_Free();
	m_Data = s.m_Data;

	return *this;
}

// Assign a const char* to a _bstr_t
//
inline _bstr_t& _bstr_t::operator=(const char* s) 
{
	_Free();
	m_Data = new Data_t(s);

	return *this;
}

// Assign a const wchar_t* to a _bstr_t
//
inline _bstr_t& _bstr_t::operator=(const wchar_t* s) 
{
	_Free();
	m_Data = new Data_t(s);

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//
// Operators
//
//////////////////////////////////////////////////////////////////////////////

// Concatenate a _bstr_t onto this _bstr_t
//
inline _bstr_t& _bstr_t::operator+=(const _bstr_t& s) 
{
	Data_t* newData = new Data_t(*this, s);

	_Free();
	m_Data = newData;

	return *this;
}

// Return the concatenation of this _bstr_t with another _bstr_t
//
inline _bstr_t _bstr_t::operator+(const _bstr_t& s) const 
{
	_bstr_t b = *this;
	b += s;

	return b;
}

//////////////////////////////////////////////////////////////////////////////
//
// Friend Operators
//
//////////////////////////////////////////////////////////////////////////////

// Return the concatenation of a const char* with a _bstr_t
//
inline _bstr_t operator+(const char* s1, const _bstr_t& s2) 
{
	_bstr_t b = s1; 
	b += s2;

	return b;
}

// Return the concatenation of a const char* with a _bstr_t
//
inline _bstr_t operator+(const wchar_t* s1, const _bstr_t& s2) 
{
	_bstr_t b = s1; 
	b += s2;

	return b;
}

//////////////////////////////////////////////////////////////////////////////
//
// Extractors
//
//////////////////////////////////////////////////////////////////////////////

// Extract a const wchar_t*
//
inline _bstr_t::operator const wchar_t*() const 
{
	return (m_Data != NULL) ? m_Data->GetWString() : NULL;
}

// Extract a wchar_t*
//
inline _bstr_t::operator wchar_t*() const 
{
	return const_cast<wchar_t*>((m_Data != NULL) ? m_Data->GetWString() : NULL);
}

// Extract a const char_t*
//
inline _bstr_t::operator const char*() const 
{
	return (m_Data != NULL) ? m_Data->GetString() : NULL;
}

// Extract a char_t*
//
inline _bstr_t::operator char*() const 
{
	return const_cast<char*>((m_Data != NULL) ? m_Data->GetString() : NULL);
}

//////////////////////////////////////////////////////////////////////////////
//
// Comparison operators
//
//////////////////////////////////////////////////////////////////////////////

inline bool _bstr_t::operator!() const 
{
	return (m_Data != NULL) ? !m_Data->GetWString() : true;
}

inline bool _bstr_t::operator==(const _bstr_t& str) const 
{
	return _Compare(str) == 0;
}

inline bool _bstr_t::operator!=(const _bstr_t& str) const 
{
	return _Compare(str) != 0;
}

inline bool _bstr_t::operator<(const _bstr_t& str) const 
{
	return _Compare(str) < 0;
}

inline bool _bstr_t::operator>(const _bstr_t& str) const 
{
	return _Compare(str) > 0;
}

inline bool _bstr_t::operator<=(const _bstr_t& str) const 
{
	return _Compare(str) <= 0;
}

inline bool _bstr_t::operator>=(const _bstr_t& str) const 
{
	return _Compare(str) >= 0;
}

//////////////////////////////////////////////////////////////////////////////
//
// Low-level help functions
//
//////////////////////////////////////////////////////////////////////////////

// Extract a copy of the wrapped BSTR
//
inline BSTR _bstr_t::copy() const 
{
	return (m_Data != NULL) ? m_Data->Copy() : NULL;
}

// Return the length of the wrapped BSTR
//
inline unsigned int _bstr_t::length() const 
{
	return (m_Data != NULL) ? m_Data->Length() : 0;
}

// Binary string assign
//
inline void _bstr_t::Assign(BSTR s) 
{
	if (m_Data != NULL) {
		m_Data->Assign(s); 
	} 
	else {
		m_Data = new Data_t(s, TRUE);
		if (m_Data == NULL) {
			_com_issue_error(E_OUTOFMEMORY);
		}
	}
}

// AddRef the BSTR
//
inline void _bstr_t::_AddRef() 
{
	if (m_Data != NULL) {
		m_Data->AddRef();
	}
}

// Free the BSTR
//
inline void _bstr_t::_Free() 
{
	if (m_Data != NULL) {
		m_Data->Release();
		m_Data = NULL;
	}
}

// Compare two _bstr_t objects
//
inline int _bstr_t::_Compare(const _bstr_t& str) const 
{
	if (m_Data == str.m_Data) {
		return 0;
	}

	if (m_Data == NULL) {
		return -1;
	}

	if (str.m_Data == NULL) {
		return 1;
	}

	return m_Data->Compare(*str.m_Data);
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - Constructors
//
//////////////////////////////////////////////////////////////////////////////

// Construct a Data_t from a const char*
//
inline _bstr_t::Data_t::Data_t(const char* s) 
	: m_str(NULL), m_RefCount(1)
{
	m_wstr = _com_utilBPP::ConvertAnsiStrToBStr(s);

	if (m_wstr == NULL && s != NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a Data_t from a const wchar_t*
//
inline _bstr_t::Data_t::Data_t(const wchar_t* s) 
	: m_str(NULL), m_RefCount(1)
{
	m_wstr = ::SysAllocString(s);

	if (m_wstr == NULL && s != NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a Data_t from a BSTR.  If fCopy is FALSE, give control of
// data to the Data_t without doing a SysAllocStringByteLen.
//
inline _bstr_t::Data_t::Data_t(BSTR bstr, bool fCopy) 
	: m_str(NULL), m_RefCount(1)
{
	if (fCopy && bstr != NULL) {
		m_wstr = ::SysAllocStringByteLen(reinterpret_cast<char*>(bstr),
										 ::SysStringByteLen(bstr));

		if (m_wstr == NULL) {
			_com_issue_error(E_OUTOFMEMORY);
		}
	}
	else {
		m_wstr = bstr;
	}
}

// Construct a Data_t from the concatenation of two _bstr_t objects
//
inline _bstr_t::Data_t::Data_t(const _bstr_t& s1, const _bstr_t& s2) 
	: m_str(NULL), m_RefCount(1)
{
	const unsigned int l1 = s1.length();
	const unsigned int l2 = s2.length();

	m_wstr = ::SysAllocStringByteLen(NULL, (l1 + l2) * sizeof(wchar_t));

	if (m_wstr == NULL) {
		if (l1 + l2 == 0) {
			return;
		}
		_com_issue_error(E_OUTOFMEMORY);
	}

	const wchar_t* wstr1 = static_cast<const wchar_t*>(s1);

	if (wstr1 != NULL) {
		memcpy(m_wstr, wstr1, (l1 + 1) * sizeof(wchar_t));
	}

	const wchar_t* wstr2 = static_cast<const wchar_t*>(s2);

	if (wstr2 != NULL) {
		memcpy(m_wstr + l1, wstr2, (l2 + 1) * sizeof(wchar_t));
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - reference counting routines
//
//////////////////////////////////////////////////////////////////////////////

inline unsigned long _bstr_t::Data_t::AddRef() 
{
	InterlockedIncrement(reinterpret_cast<long*>(&m_RefCount));
	return m_RefCount;
}

inline unsigned long _bstr_t::Data_t::Release() 
{
	if (!InterlockedDecrement(reinterpret_cast<long*>(&m_RefCount))) {
		delete this;
		return 0;
	}

	return m_RefCount;
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - extractors
//
//////////////////////////////////////////////////////////////////////////////

// Extract a const wchar_t*
//
inline _bstr_t::Data_t::operator const wchar_t*() const 
{
	return m_wstr;
}

// Extract a const char_t*
//
inline _bstr_t::Data_t::operator const char*() const 
{
	return GetString();
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - helper functions
//
//////////////////////////////////////////////////////////////////////////////

inline const wchar_t* _bstr_t::Data_t::GetWString() const 
{
	return m_wstr;
}

inline const char* _bstr_t::Data_t::GetString() const 
{
	if (m_str == NULL) {
		m_str = _com_utilBPP::ConvertBStrToAnsiStr(m_wstr);

		if (m_str == NULL && m_wstr != NULL) {
			_com_issue_error(E_OUTOFMEMORY);
		}
	}

	return m_str;
}

// Return a copy of the wrapped BSTR
//
inline BSTR _bstr_t::Data_t::Copy() const 
{
	if (m_wstr != NULL) {
		BSTR bstr = ::SysAllocStringByteLen(reinterpret_cast<char*>(m_wstr),
											::SysStringByteLen(m_wstr));

		if (bstr == NULL) {
			_com_issue_error(E_OUTOFMEMORY);
		}

		return bstr;
	}

	return NULL;
}

inline void _bstr_t::Data_t::Assign(BSTR s) 
{
	_Free();
	if (s != NULL) {
		m_wstr = ::SysAllocStringByteLen(reinterpret_cast<char*>(s), 
											::SysStringByteLen(s));
	}
}

// Return the length of the wrapper BSTR
//
inline unsigned int _bstr_t::Data_t::Length() const 
{
	return m_wstr ? ::SysStringLen(m_wstr) : 0;
}

// Compare two wrapped BSTRs
//
inline int _bstr_t::Data_t::Compare(const _bstr_t::Data_t& str) const
{
	if (m_wstr == NULL) {
		return str.m_wstr ? -1 : 0;
	}

	if (str.m_wstr == NULL) {
		return 1;
	}

	const unsigned int l1 = ::SysStringLen(m_wstr);
	const unsigned int l2 = ::SysStringLen(str.m_wstr);

	unsigned int len = l1;
	if (len > l2) {
		len = l2;
	}

	BSTR bstr1 = m_wstr;
	BSTR bstr2 = str.m_wstr;

	while (len-- > 0) {
		if (*bstr1++ != *bstr2++) {
			return bstr1[-1] - bstr2[-1];
		}
	}

	return (l1 < l2) ? -1 : (l1 == l2) ? 0 : 1;
}

// Destruct this object
//
inline _bstr_t::Data_t::~Data_t() 
{
	_Free();
}

// Free up this object
//
inline void _bstr_t::Data_t::_Free() 
{
	if (m_wstr != NULL) {
		::SysFreeString(m_wstr);
	}

	if (m_str != NULL) {
		delete [] m_str;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Wrapper class for VARIANT
//
//////////////////////////////////////////////////////////////////////////////

/*
 * VARENUM usage key,
 *
 * * [V] - may appear in a VARIANT
 * * [T] - may appear in a TYPEDESC
 * * [P] - may appear in an OLE property set
 * * [S] - may appear in a Safe Array
 * * [C] - supported by class _variant_t
 *
 *
 *  VT_EMPTY            [V]   [P]        nothing
 *  VT_NULL             [V]   [P]        SQL style Null
 *  VT_I2               [V][T][P][S][C]  2 byte signed int
 *  VT_I4               [V][T][P][S][C]  4 byte signed int
 *  VT_R4               [V][T][P][S][C]  4 byte real
 *  VT_R8               [V][T][P][S][C]  8 byte real
 *  VT_CY               [V][T][P][S][C]  currency
 *  VT_DATE             [V][T][P][S][C]  date
 *  VT_BSTR             [V][T][P][S][C]  OLE Automation string
 *  VT_DISPATCH         [V][T][P][S][C]  IDispatch *
 *  VT_ERROR            [V][T]   [S][C]  SCODE
 *  VT_BOOL             [V][T][P][S][C]  True=-1, False=0
 *  VT_VARIANT          [V][T][P][S]     VARIANT *
 *  VT_UNKNOWN          [V][T]   [S][C]  IUnknown *
 *  VT_DECIMAL          [V][T]   [S][C]  16 byte fixed point
 *  VT_I1                  [T]           signed char
 *  VT_UI1              [V][T][P][S][C]  unsigned char
 *  VT_UI2                 [T][P]        unsigned short
 *  VT_UI4                 [T][P]        unsigned short
 *  VT_I8                  [T][P]        signed 64-bit int
 *  VT_UI8                 [T][P]        unsigned 64-bit int
 *  VT_INT                 [T]           signed machine int
 *  VT_UINT                [T]           unsigned machine int
 *  VT_VOID                [T]           C style void
 *  VT_HRESULT             [T]           Standard return type
 *  VT_PTR                 [T]           pointer type
 *  VT_SAFEARRAY           [T]          (use VT_ARRAY in VARIANT)
 *  VT_CARRAY              [T]           C style array
 *  VT_USERDEFINED         [T]           user defined type
 *  VT_LPSTR               [T][P]        null terminated string
 *  VT_LPWSTR              [T][P]        wide null terminated string
 *  VT_FILETIME               [P]        FILETIME
 *  VT_BLOB                   [P]        Length prefixed bytes
 *  VT_STREAM                 [P]        Name of the stream follows
 *  VT_STORAGE                [P]        Name of the storage follows
 *  VT_STREAMED_OBJECT        [P]        Stream contains an object
 *  VT_STORED_OBJECT          [P]        Storage contains an object
 *  VT_BLOB_OBJECT            [P]        Blob contains an object
 *  VT_CF                     [P]        Clipboard format
 *  VT_CLSID                  [P]        A Class ID
 *  VT_VECTOR                 [P]        simple counted array
 *  VT_ARRAY            [V]              SAFEARRAY*
 *  VT_BYREF            [V]              void* for local use
 */

class _variant_t : public ::tagVARIANT {
public:
	// Constructors
	//
	_variant_t() ;

	_variant_t(const VARIANT& varSrc) ;
	_variant_t(const VARIANT* pSrc) ;
	_variant_t(const _variant_t& varSrc) ;

	_variant_t(VARIANT& varSrc, bool fCopy) ;			// Attach VARIANT if !fCopy

	_variant_t(short sSrc, VARTYPE vtSrc = VT_I2) ;	// Creates a VT_I2, or a VT_BOOL
	_variant_t(long lSrc, VARTYPE vtSrc = VT_I4) ;		// Creates a VT_I4, a VT_ERROR, or a VT_BOOL
	_variant_t(int lSrc, VARTYPE vtSrc = VT_I4) ;		// Creates a VT_I4, a VT_ERROR, or a VT_BOOL
	_variant_t(float fltSrc) ;									// Creates a VT_R4
	_variant_t(double dblSrc, VARTYPE vtSrc = VT_R8) ;	// Creates a VT_R8, or a VT_DATE
	_variant_t(const CY& cySrc) ;								// Creates a VT_CY
	_variant_t(const _bstr_t& bstrSrc) ;				// Creates a VT_BSTR
	_variant_t(const wchar_t *pSrc) ;					// Creates a VT_BSTR
	_variant_t(const char* pSrc) ;						// Creates a VT_BSTR
	//_variant_t(const bpp::string pSrc) ;						// Creates a VT_BSTR
	_variant_t(IDispatch* pSrc, bool fAddRef = true) ;			// Creates a VT_DISPATCH
	_variant_t(bool bSrc) ;										// Creates a VT_BOOL
	_variant_t(IUnknown* pSrc, bool fAddRef = true) ;			// Creates a VT_UNKNOWN
	_variant_t(const DECIMAL& decSrc) ;							// Creates a VT_DECIMAL
	_variant_t(BYTE bSrc) ;										// Creates a VT_UI1

	// Destructor
	//
	~_variant_t() ;

	// Extractors
	//
	operator short() const ;			// Extracts a short from a VT_I2
	operator long() const ;			// Extracts a long from a VT_I4
	operator float() const ;			// Extracts a float from a VT_R4
	operator double() const ;			// Extracts a double from a VT_R8
	operator CY() const ;				// Extracts a CY from a VT_CY
	operator _bstr_t() const ;			// Extracts a _bstr_t from a VT_BSTR
	operator char*() const ;			// Extracts a char* from a VT_BSTR
	operator const char*() const ;			// Extracts a cont char* from a VT_BSTR
	operator IDispatch*() const ;			// Extracts a IDispatch* from a VT_DISPATCH
	operator bool() const ;			// Extracts a bool from a VT_BOOL
	operator IUnknown*() const ;			// Extracts a IUnknown* from a VT_UNKNOWN
	operator DECIMAL() const ;			// Extracts a DECIMAL from a VT_DECIMAL
	operator BYTE() const ;			// Extracts a BTYE (unsigned char) from a VT_UI1
	
	// Assignment operations
	//
	_variant_t& operator=(const VARIANT& varSrc) ;
	_variant_t& operator=(const VARIANT* pSrc) ;
	_variant_t& operator=(const _variant_t& varSrc) ;

	_variant_t& operator=(short sSrc) ;				// Assign a VT_I2, or a VT_BOOL
	_variant_t& operator=(long lSrc) ;					// Assign a VT_I4, a VT_ERROR or a VT_BOOL
	_variant_t& operator=(float fltSrc) ;				// Assign a VT_R4
	_variant_t& operator=(double dblSrc) ;				// Assign a VT_R8, or a VT_DATE
	_variant_t& operator=(const CY& cySrc) ;			// Assign a VT_CY
	_variant_t& operator=(const _bstr_t& bstrSrc) ;	// Assign a VT_BSTR
	_variant_t& operator=(const wchar_t* pSrc) ;		// Assign a VT_BSTR
	_variant_t& operator=(const char* pSrc) ;			// Assign a VT_BSTR
	_variant_t& operator=(IDispatch* pSrc) ;			// Assign a VT_DISPATCH
 	_variant_t& operator=(bool bSrc) ;					// Assign a VT_BOOL
	_variant_t& operator=(IUnknown* pSrc) ;			// Assign a VT_UNKNOWN
	_variant_t& operator=(const DECIMAL& decSrc) ;		// Assign a VT_DECIMAL
	_variant_t& operator=(BYTE bSrc) ;					// Assign a VT_UI1

	// Comparison operations
	//
	bool operator==(const VARIANT& varSrc) const ;
	bool operator==(const VARIANT* pSrc) const ;

	bool operator!=(const VARIANT& varSrc) const ;
	bool operator!=(const VARIANT* pSrc) const ;

	// Low-level operations
	//
	void Clear() ;

	void Attach(VARIANT& varSrc) ;
	VARIANT Detach() ;

	void ChangeType(VARTYPE vartype, const _variant_t* pSrc = NULL) ;

	void SetString(const char* pSrc) ; // used to set ANSI string
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// Constructors
//
//////////////////////////////////////////////////////////////////////////////////////////

// Default constructor
//
inline _variant_t::_variant_t() 
{
	::VariantInit(this);
}

// Construct a _variant_t from a const VARIANT&
//
inline _variant_t::_variant_t(const VARIANT& varSrc) 
{
	::VariantInit(this);
	_com_utilBPP::CheckError(::VariantCopy(this, const_cast<VARIANT*>(&varSrc)));
}

// Construct a _variant_t from a const VARIANT*
//
inline _variant_t::_variant_t(const VARIANT* pSrc) 
{
	::VariantInit(this);
	_com_utilBPP::CheckError(::VariantCopy(this, const_cast<VARIANT*>(pSrc)));
}

// Construct a _variant_t from a const _variant_t&
//
inline _variant_t::_variant_t(const _variant_t& varSrc) 
{
	::VariantInit(this);
	_com_utilBPP::CheckError(::VariantCopy(this, const_cast<VARIANT*>(static_cast<const VARIANT*>(&varSrc))));
}

// Construct a _variant_t from a VARIANT&.  If fCopy is FALSE, give control of
// data to the _variant_t without doing a VariantCopy.
//
inline _variant_t::_variant_t(VARIANT& varSrc, bool fCopy) 
{
	if (fCopy) {
		::VariantInit(this);
		_com_utilBPP::CheckError(::VariantCopy(this, &varSrc));
	} else {
		memcpy(this, &varSrc, sizeof(varSrc));
		V_VT(&varSrc) = VT_EMPTY;
	}
}

// Construct either a VT_I2 VARIANT or a VT_BOOL VARIANT from
// a short (the default is VT_I2)
//
inline _variant_t::_variant_t(short sSrc, VARTYPE vtSrc) 
{
	if ((vtSrc != VT_I2) && (vtSrc != VT_BOOL)) {
		_com_issue_error(E_INVALIDARG);
	}

	if (vtSrc == VT_BOOL) {
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = (sSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else {
		V_VT(this) = VT_I2;
		V_I2(this) = sSrc;
	}
}

// Construct either a VT_I4 VARIANT, a VT_BOOL VARIANT, or a
// VT_ERROR VARIANT from a long (the default is VT_I4)
//
inline _variant_t::_variant_t(long lSrc, VARTYPE vtSrc) 
{
	if ((vtSrc != VT_I4) && (vtSrc != VT_ERROR) && (vtSrc != VT_BOOL)) {
		_com_issue_error(E_INVALIDARG);
	}

	if (vtSrc == VT_ERROR) {
		V_VT(this) = VT_ERROR;
		V_ERROR(this) = lSrc;
	}
	else if (vtSrc == VT_BOOL) {
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = (lSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else {
		V_VT(this) = VT_I4;
		V_I4(this) = lSrc;
	}
}

// Construct either a VT_I4 VARIANT, a VT_BOOL VARIANT, or a
// VT_ERROR VARIANT from a int (the default is VT_I4)
//
inline _variant_t::_variant_t(int lSrc, VARTYPE vtSrc) 
{
	if ((vtSrc != VT_I4) && (vtSrc != VT_ERROR) && (vtSrc != VT_BOOL)) {
		_com_issue_error(E_INVALIDARG);
	}

	if (vtSrc == VT_ERROR) {
		V_VT(this) = VT_ERROR;
		V_ERROR(this) = lSrc;
	}
	else if (vtSrc == VT_BOOL) {
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = (lSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else {
		V_VT(this) = VT_I4;
		V_I4(this) = lSrc;
	}
}

// Construct a VT_R4 VARIANT from a float
//
inline _variant_t::_variant_t(float fltSrc) 
{
	V_VT(this) = VT_R4;
	V_R4(this) = fltSrc;
}

// Construct either a VT_R8 VARIANT, or a VT_DATE VARIANT from
// a double (the default is VT_R8)
//
inline _variant_t::_variant_t(double dblSrc, VARTYPE vtSrc) 
{
	if ((vtSrc != VT_R8) && (vtSrc != VT_DATE)) {
		_com_issue_error(E_INVALIDARG);
	}

	if (vtSrc == VT_DATE) {
		V_VT(this) = VT_DATE;
		V_DATE(this) = dblSrc;
	}
	else {
		V_VT(this) = VT_R8;
		V_R8(this) = dblSrc;
	}
}

// Construct a VT_CY from a CY
//
inline _variant_t::_variant_t(const CY& cySrc) 
{
	V_VT(this) = VT_CY;
	V_CY(this) = cySrc;
}

// Construct a VT_BSTR VARIANT from a const _bstr_t&
//
inline _variant_t::_variant_t(const _bstr_t& bstrSrc) 
{
	V_VT(this) = VT_BSTR;

	BSTR bstr = static_cast<wchar_t*>(bstrSrc);
	V_BSTR(this) = ::SysAllocStringByteLen(reinterpret_cast<char*>(bstr),
										   ::SysStringByteLen(bstr));

	if (V_BSTR(this) == NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a VT_BSTR VARIANT from a const wchar_t*
//
inline _variant_t::_variant_t(const wchar_t* pSrc) 
{
	V_VT(this) = VT_BSTR;
	V_BSTR(this) = ::SysAllocString(pSrc);

	if (V_BSTR(this) == NULL && pSrc != NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a VT_BSTR VARIANT from a const char*
//
inline _variant_t::_variant_t(const char* pSrc) 
{
	V_VT(this) = VT_BSTR;
	V_BSTR(this) = _com_utilBPP::ConvertAnsiStrToBStr(pSrc);

	if (V_BSTR(this) == NULL && pSrc != NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

// Construct a VT_DISPATCH VARIANT from an IDispatch*
//
inline _variant_t::_variant_t(IDispatch* pSrc, bool fAddRef) 
{
	V_VT(this) = VT_DISPATCH;
	V_DISPATCH(this) = pSrc;

	// Need the AddRef() as VariantClear() calls Release(), unless fAddRef
	// false indicates we're taking ownership
	//
	if (fAddRef) {
		V_DISPATCH(this)->AddRef();
	}
}

// Construct a VT_BOOL VARIANT from a bool
//
inline _variant_t::_variant_t(bool bSrc) 
{
	V_VT(this) = VT_BOOL;
	V_BOOL(this) = (bSrc ? VARIANT_TRUE : VARIANT_FALSE);
}

// Construct a VT_UNKNOWN VARIANT from an IUnknown*
//
inline _variant_t::_variant_t(IUnknown* pSrc, bool fAddRef) 
{
	V_VT(this) = VT_UNKNOWN;
	V_UNKNOWN(this) = pSrc;

	// Need the AddRef() as VariantClear() calls Release(), unless fAddRef
	// false indicates we're taking ownership
	//
	if (fAddRef) {
		V_UNKNOWN(this)->AddRef();
	}
}

// Construct a VT_DECIMAL VARIANT from a DECIMAL
//
inline _variant_t::_variant_t(const DECIMAL& decSrc) 
{
	// Order is important here! Setting V_DECIMAL wipes out the entire VARIANT
	//
	V_DECIMAL(this) = decSrc;
	V_VT(this) = VT_DECIMAL;
}

// Construct a VT_UI1 VARIANT from a BYTE (unsigned char)
//
inline _variant_t::_variant_t(BYTE bSrc) 
{
	V_VT(this) = VT_UI1;
	V_UI1(this) = bSrc;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Extractors
//
//////////////////////////////////////////////////////////////////////////////////////////

// Extracts a VT_I2 into a short
//
inline _variant_t::operator short() const 
{
	if (V_VT(this) == VT_I2) {
		return V_I2(this); 
	}

	_variant_t varDest;

	varDest.ChangeType(VT_I2, this);

	return V_I2(&varDest);
}

// Extracts a VT_I4 into a long
//
inline _variant_t::operator long() const 
{
	if (V_VT(this) == VT_I4) {
		return V_I4(this); 
	}

	_variant_t varDest;

	varDest.ChangeType(VT_I4, this);

	return V_I4(&varDest);
}

// Extracts a VT_R4 into a float
//
inline _variant_t::operator float() const 
{
	if (V_VT(this) == VT_R4) {
		return V_R4(this); 
	}

	_variant_t varDest;

	varDest.ChangeType(VT_R4, this);

	return V_R4(&varDest);
}

// Extracts a VT_R8 into a double
//
inline _variant_t::operator double() const 
{
	if (V_VT(this) == VT_R8) {
		return V_R8(this); 
	}

	_variant_t varDest;

	varDest.ChangeType(VT_R8, this);

	return V_R8(&varDest);
}

// Extracts a VT_CY into a CY
//
inline _variant_t::operator CY() const 
{
	if (V_VT(this) == VT_CY) {
		return V_CY(this); 
	}

	_variant_t varDest;

	varDest.ChangeType(VT_CY, this);

	return V_CY(&varDest);
}

// Extracts a VT_BSTR into a _bstr_t
//
inline _variant_t::operator _bstr_t() const 
{
	if (V_VT(this) == VT_BSTR) {
		return V_BSTR(this);
	}

	_variant_t varDest;

	varDest.ChangeType(VT_BSTR, this);

	return V_BSTR(&varDest);
}

// Extracts a VT_BSTR into a char*
//
inline _variant_t::operator char*() const 
{
	BSTR retVal;

	if (V_VT(this) == VT_BSTR) {
		retVal = V_BSTR(this);
		char* m_str = _com_utilBPP::ConvertBStrToAnsiStr(retVal);
		return m_str;
	}

	_variant_t varDest;

	varDest.ChangeType(VT_BSTR, this);

	retVal = V_BSTR(&varDest);

	char* m_str = _com_utilBPP::ConvertBStrToAnsiStr(retVal);

	return m_str;
}

// Extracts a VT_BSTR into a const char*
//
inline _variant_t::operator const char*() const 
{
	BSTR retVal;

	if (V_VT(this) == VT_BSTR) {
		retVal = V_BSTR(this);
		const char* m_str = _com_utilBPP::ConvertBStrToAnsiStr(retVal);
		return const_cast<char*>(m_str);
	}

	_variant_t varDest;

	varDest.ChangeType(VT_BSTR, this);

	retVal = V_BSTR(&varDest);

	const char* m_str = _com_utilBPP::ConvertBStrToAnsiStr(retVal);

	return const_cast<char*>(m_str);
}

// Extracts a VT_DISPATCH into an IDispatch*
//
inline _variant_t::operator IDispatch*() const 
{
	if (V_VT(this) == VT_DISPATCH) {
		V_DISPATCH(this)->AddRef();
		return V_DISPATCH(this);
	}

	_variant_t varDest;

	varDest.ChangeType(VT_DISPATCH, this);

	V_DISPATCH(&varDest)->AddRef();
	return V_DISPATCH(&varDest);
}

// Extract a VT_BOOL into a bool
//
inline _variant_t::operator bool() const 
{
	if (V_VT(this) == VT_BOOL) {
		return V_BOOL(this) ? true : false;
	}

	_variant_t varDest;

	varDest.ChangeType(VT_BOOL, this);

	return V_BOOL(&varDest) ? true : false;
}

// Extracts a VT_UNKNOWN into an IUnknown*
//
inline _variant_t::operator IUnknown*() const 
{
	if (V_VT(this) == VT_UNKNOWN) {
		V_UNKNOWN(this)->AddRef();
		return V_UNKNOWN(this);
	}

	_variant_t varDest;

	varDest.ChangeType(VT_UNKNOWN, this);

	V_UNKNOWN(&varDest)->AddRef();
	return V_UNKNOWN(&varDest);
}

// Extracts a VT_DECIMAL into a DECIMAL
//
inline _variant_t::operator DECIMAL() const 
{
	if (V_VT(this) == VT_DECIMAL) {
		return V_DECIMAL(this);
	}

	_variant_t varDest;

	varDest.ChangeType(VT_DECIMAL, this);

	return V_DECIMAL(&varDest);
}

// Extracts a VT_UI1 into a BYTE (unsigned char)
//
inline _variant_t::operator BYTE() const 
{
	if (V_VT(this) == VT_UI1) {
		return V_UI1(this);
	}

	_variant_t varDest;

	varDest.ChangeType(VT_UI1, this);

	return V_UI1(&varDest);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Assignment operations
//
//////////////////////////////////////////////////////////////////////////////////////////

// Assign a const VARIANT& (::VariantCopy handles everything)
//
inline _variant_t& _variant_t::operator=(const VARIANT& varSrc) 
{
	_com_utilBPP::CheckError(::VariantCopy(this, const_cast<VARIANT*>(&varSrc)));

	return *this;
}

// Assign a const VARIANT* (::VariantCopy handles everything)
//
inline _variant_t& _variant_t::operator=(const VARIANT* pSrc) 
{
	_com_utilBPP::CheckError(::VariantCopy(this, const_cast<VARIANT*>(pSrc)));

	return *this;
}

// Assign a const _variant_t& (::VariantCopy handles everything)
//
inline _variant_t& _variant_t::operator=(const _variant_t& varSrc) 
{
	_com_utilBPP::CheckError(::VariantCopy(this, const_cast<VARIANT*>(static_cast<const VARIANT*>(&varSrc))));

	return *this;
}

// Assign a short creating either VT_I2 VARIANT or a 
// VT_BOOL VARIANT (VT_I2 is the default)
//
inline _variant_t& _variant_t::operator=(short sSrc) 
{
	if (V_VT(this) == VT_I2) {
		V_I2(this) = sSrc;
	}
	else if (V_VT(this) == VT_BOOL) {
		V_BOOL(this) = (sSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else {
		// Clear the VARIANT and create a VT_I2
		//
		Clear();

		V_VT(this) = VT_I2;
		V_I2(this) = sSrc;
	}

	return *this;
}

// Assign a long creating either VT_I4 VARIANT, a VT_ERROR VARIANT
// or a VT_BOOL VARIANT (VT_I4 is the default)
//
inline _variant_t& _variant_t::operator=(long lSrc) 
{
	if (V_VT(this) == VT_I4) {
		V_I4(this) = lSrc;
	}
	else if (V_VT(this) == VT_ERROR) {
		V_ERROR(this) = lSrc;
	}
	else if (V_VT(this) == VT_BOOL) {
		V_BOOL(this) = (lSrc ? VARIANT_TRUE : VARIANT_FALSE);
	}
	else {
		// Clear the VARIANT and create a VT_I4
		//
		Clear();

		V_VT(this) = VT_I4;
		V_I4(this) = lSrc;
	}

	return *this;
}

// Assign a float creating a VT_R4 VARIANT 
//
inline _variant_t& _variant_t::operator=(float fltSrc) 
{
	if (V_VT(this) != VT_R4) {
		// Clear the VARIANT and create a VT_R4
		//
		Clear();

		V_VT(this) = VT_R4;
	}

	V_R4(this) = fltSrc;

	return *this;
}

// Assign a double creating either a VT_R8 VARIANT, or a VT_DATE
// VARIANT (VT_R8 is the default)
//
inline _variant_t& _variant_t::operator=(double dblSrc) 
{
	if (V_VT(this) == VT_R8) {
		V_R8(this) = dblSrc;
	}
	else if(V_VT(this) == VT_DATE) {
		V_DATE(this) = dblSrc;
	}
	else {
		// Clear the VARIANT and create a VT_R8
		//
		Clear();

		V_VT(this) = VT_R8;
		V_R8(this) = dblSrc;
	}

	return *this;
}

// Assign a CY creating a VT_CY VARIANT 
//
inline _variant_t& _variant_t::operator=(const CY& cySrc) 
{
	if (V_VT(this) != VT_CY) {
		// Clear the VARIANT and create a VT_CY
		//
		Clear();

		V_VT(this) = VT_CY;
	}

	V_CY(this) = cySrc;

	return *this;
}

// Assign a const _bstr_t& creating a VT_BSTR VARIANT
//
inline _variant_t& _variant_t::operator=(const _bstr_t& bstrSrc) 
{
	// Clear the VARIANT (This will SysFreeString() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_BSTR;

	if (!bstrSrc) {
		V_BSTR(this) = NULL;
	}
	else {
		BSTR bstr = static_cast<wchar_t*>(bstrSrc);
		V_BSTR(this) = ::SysAllocStringByteLen(reinterpret_cast<char*>(bstr),
											   ::SysStringByteLen(bstr));

		if (V_BSTR(this) == NULL) {
			_com_issue_error(E_OUTOFMEMORY);
		}
	}

	return *this;
}

// Assign a const wchar_t* creating a VT_BSTR VARIANT
//
inline _variant_t& _variant_t::operator=(const wchar_t* pSrc) 
{
	// Clear the VARIANT (This will SysFreeString() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_BSTR;

	if (pSrc == NULL) {
		V_BSTR(this) = NULL;
	}
	else {
		V_BSTR(this) = ::SysAllocString(pSrc);

		if (V_BSTR(this) == NULL) {
			_com_issue_error(E_OUTOFMEMORY);
		}
	}

	return *this;
}

// Assign a const char* creating a VT_BSTR VARIANT
//
inline _variant_t& _variant_t::operator=(const char* pSrc) 
{
	// Clear the VARIANT (This will SysFreeString() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_BSTR;
	V_BSTR(this) = _com_utilBPP::ConvertAnsiStrToBStr(pSrc);

	if (V_BSTR(this) == NULL && pSrc != NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}

	return *this;
}

// Assign an IDispatch* creating a VT_DISPATCH VARIANT 
//
inline _variant_t& _variant_t::operator=(IDispatch* pSrc) 
{
	// Clear the VARIANT (This will Release() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_DISPATCH;
	V_DISPATCH(this) = pSrc;

	// Need the AddRef() as VariantClear() calls Release()
	//
	V_DISPATCH(this)->AddRef();

	return *this;
}

// Assign a bool creating a VT_BOOL VARIANT 
//
inline _variant_t& _variant_t::operator=(bool bSrc) 
{
	if (V_VT(this) != VT_BOOL) {
		// Clear the VARIANT and create a VT_BOOL
		//
		Clear();

		V_VT(this) = VT_BOOL;
	}

	V_BOOL(this) = (bSrc ? VARIANT_TRUE : VARIANT_FALSE);

	return *this;
}

// Assign an IUnknown* creating a VT_UNKNOWN VARIANT 
//
inline _variant_t& _variant_t::operator=(IUnknown* pSrc) 
{
	// Clear VARIANT (This will Release() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_UNKNOWN;
	V_UNKNOWN(this) = pSrc;

	// Need the AddRef() as VariantClear() calls Release()
	//
	V_UNKNOWN(this)->AddRef();

	return *this;
}

// Assign a DECIMAL creating a VT_DECIMAL VARIANT
//
inline _variant_t& _variant_t::operator=(const DECIMAL& decSrc) 
{
	if (V_VT(this) != VT_DECIMAL) {
		// Clear the VARIANT
		//
		Clear();
	}

	// Order is important here! Setting V_DECIMAL wipes out the entire VARIANT
	V_DECIMAL(this) = decSrc;
	V_VT(this) = VT_DECIMAL;

	return *this;
}

// Assign a BTYE (unsigned char) creating a VT_UI1 VARIANT
//
inline _variant_t& _variant_t::operator=(BYTE bSrc) 
{
	if (V_VT(this) != VT_UI1) {
		// Clear the VARIANT and create a VT_UI1
		//
		Clear();

		V_VT(this) = VT_UI1;
	}

	V_UI1(this) = bSrc;

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Comparison operations
//
//////////////////////////////////////////////////////////////////////////////////////////

// Compare a _variant_t against a const VARIANT& for equality
//
inline bool _variant_t::operator==(const VARIANT& varSrc) const 
{
	return *this == &varSrc;
}

// Compare a _variant_t against a const VARIANT* for equality
//
inline bool _variant_t::operator==(const VARIANT* pSrc) const 
{
	if (this == pSrc) {
		return true;
	}

	//
	// Variants not equal if types don't match
	//
	if (V_VT(this) != V_VT(pSrc)) {
		return false;
	}

	//
	// Check type specific values
	//
	switch (V_VT(this)) {
		case VT_EMPTY:
		case VT_NULL:
			return true;

		case VT_I2:
			return V_I2(this) == V_I2(pSrc);

		case VT_I4:
			return V_I4(this) == V_I4(pSrc);

		case VT_R4:
			return V_R4(this) == V_R4(pSrc);

		case VT_R8:
			return V_R8(this) == V_R8(pSrc);

		case VT_CY:
			return memcmp(&(V_CY(this)), &(V_CY(pSrc)), sizeof(CY)) == 0;

		case VT_DATE:
			return V_DATE(this) == V_DATE(pSrc);

		case VT_BSTR:
			return (::SysStringByteLen(V_BSTR(this)) == ::SysStringByteLen(V_BSTR(pSrc))) &&
					(memcmp(V_BSTR(this), V_BSTR(pSrc), ::SysStringByteLen(V_BSTR(this))) == 0);

		case VT_DISPATCH:
			return V_DISPATCH(this) == V_DISPATCH(pSrc);

		case VT_ERROR:
			return V_ERROR(this) == V_ERROR(pSrc);

		case VT_BOOL:
			return V_BOOL(this) == V_BOOL(pSrc);

		case VT_UNKNOWN:
			return V_UNKNOWN(this) == V_UNKNOWN(pSrc);

		case VT_DECIMAL:
			return memcmp(&(V_DECIMAL(this)), &(V_DECIMAL(pSrc)), sizeof(DECIMAL)) == 0;

		case VT_UI1:
			return V_UI1(this) == V_UI1(pSrc);

		default:
			_com_issue_error(E_INVALIDARG);
			// fall through
	}

	return false;
}

// Compare a _variant_t against a const VARIANT& for in-equality
//
inline bool _variant_t::operator!=(const VARIANT& varSrc) const 
{
	return !(*this == &varSrc);
}

// Compare a _variant_t against a const VARIANT* for in-equality
//
inline bool _variant_t::operator!=(const VARIANT* pSrc) const 
{
	return !(*this == pSrc);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Low-level operations
//
//////////////////////////////////////////////////////////////////////////////////////////

// Clear the _variant_t
//
inline void _variant_t::Clear() 
{
	_com_utilBPP::CheckError(::VariantClear(this));
}

inline void _variant_t::Attach(VARIANT& varSrc) 
{
	//
	// Free up previous VARIANT
	//
	Clear();

	//
	// Give control of data to _variant_t
	//
	memcpy(this, &varSrc, sizeof(varSrc));
	V_VT(&varSrc) = VT_EMPTY;
}

inline VARIANT _variant_t::Detach() 
{
	VARIANT varResult = *this;
	V_VT(this) = VT_EMPTY;

	return varResult;
}

// Change the type and contents of this _variant_t to the type vartype and
// contents of pSrc
//
inline void _variant_t::ChangeType(VARTYPE vartype, const _variant_t* pSrc) 
{
	//
	// If pDest is NULL, convert type in place
	//
	if (pSrc == NULL) {
		pSrc = this;
	}

	if ((this != pSrc) || (vartype != V_VT(this))) {
		_com_utilBPP::CheckError(::VariantChangeType(static_cast<VARIANT*>(this),
												  const_cast<VARIANT*>(static_cast<const VARIANT*>(pSrc)),
												  0, vartype));
	}
}

inline void _variant_t::SetString(const char* pSrc) 
{
	//
	// Free up previous VARIANT
	//
	Clear();

	V_VT(this) = VT_BSTR;
	V_BSTR(this) = _com_utilBPP::ConvertAnsiStrToBStr(pSrc);

	if (V_BSTR(this) == NULL && pSrc != NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
//////////////////////////////////////////////////////////////////////////////////////////

inline _variant_t::~_variant_t() 
{
	_com_utilBPP::CheckError(::VariantClear(this));
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Mutually-dependent definitions
//
//////////////////////////////////////////////////////////////////////////////////////////

// Construct a _bstr_t from a const _variant_t&
//
/*
inline _bstr_t::_bstr_t(const _variant_t &var) 
	: m_Data(NULL)
{
	if (V_VT(&var) == VT_BSTR) {
		*this = V_BSTR(&var);
		return;
	}

	_variant_t varDest;

	varDest.ChangeType(VT_BSTR, &var);

	*this = V_BSTR(&varDest);
}
*/
// Assign a const _variant_t& to a _bstr_t
//
inline _bstr_t& _bstr_t::operator=(const _variant_t &var)
{
	if (V_VT(&var) == VT_BSTR) {
		*this = V_BSTR(&var);
		return *this;
	}

	_variant_t varDest;

	varDest.ChangeType(VT_BSTR, &var);

	*this = V_BSTR(&varDest);

	return *this;
}

extern _variant_t vtMissing;

#ifndef _USE_RAW
#define bstr_t _bstr_t
#define variant_t _variant_t
#endif

#pragma warning(pop)

#pragma option pop /*P_O_Pop*/
#endif	// _INC_COMUTIL
