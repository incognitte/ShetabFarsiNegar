#pragma once

//void InitLock(void)
//{
//	CString strPlace;
//	//1
//	strPlace = CString("file=%1; section=MSUCE; encryptor=190;") + CString("entry=sfndtt;");
//	strPlace.Replace(_T("%1"), m::GetWindowsDirectory() + _T("\\win.ini"));
//	GetLockTime()->AddPlace(strPlace);
//	//2
//	strPlace = CString("file=%1; section=VbPlug-inx; encryptor=890;") + CString("entry=value3;");
//	strPlace.Replace(_T("%1"), m::GetWindowsDirectory() + _T("\\vb.ini"));
//	GetLockTime()->AddPlace(strPlace);
//	//3
//	strPlace = CString("Subkey=") + CString("SYSTEM\\") + CString("Setup; entry=") + CString("SetupTyp3") + Convert::ToString(10) + CString(";encryptor=320;");
//	GetLockTime()->AddPlace(strPlace);
//	//4
//	strPlace = CString("Subkey=") + CString("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\") + CString("MCD\\") + CString("Swapnc;");
//	strPlace += CString("encryptor=440;");
//	GetLockTime()->AddPlace(strPlace);
//	//5
//	strPlace = CString("Subkey=") + CString("SOFTWARE\\Microsoft\\Windows\\CurrentVersion;");
//	strPlace += _T("entry=SF_") + Lock_GetComputerId().Left(4) + _T(";");
//	strPlace += _T("encryptor=443;");
//	GetLockTime()->AddPlace(strPlace);
//}


class CLockTime
{
public:
	__forceinline void RemoveAllPlaces(void)
	{
		m_places.RemoveAll();
	}

	//Initialize locktime
	__forceinline void Init(int nMaxDays)
	{
		m_nMaxDays = nMaxDays;
	}

	//@param string file=;section=;entry=;encryptor=;
	//@param string subkey=;entry=;encryptor=;
	__forceinline void AddPlace(CString str)
	{
		m_places.Add(str);
	}

	void Update()
	{
		//get all day from seprated registery and files
		int nDay = GetPassedDay();
		for (int i=0; i<(int)m_places.GetCount(); i++)
		{
			CString strSubKey = Param::Get(_T("Subkey"), _T(""), m_places[i]);
			Path fileName(Param::Get(_T("File"), _T(""), m_places[i]));
			CString strEntry = Param::Get(_T("Entry"), _T(""), m_places[i]);
			CString strSection = Param::Get(_T("Section"), _T(""), m_places[i]);
			int nEncryptor = Param::Get(_T("Encryptor"), 0, m_places[i]);

			//create value
			int nCurTime = CTime::GetCurrentTime().GetDay();
			int nValue = nCurTime<<24 | nDay;

			if ( !strSubKey.IsEmpty() )
			{
				m::Reg::Write(HKEY_LOCAL_MACHINE, strSubKey, strEntry, nValue ^ nEncryptor);
			}
			else if ( !fileName.IsEmpty() )
			{
				PrivateProfile::Write(strSection, strEntry, nValue ^ nEncryptor, fileName);
			}
		}
	}

	//@return the number of passed days
	int GetPassedDay()
	{
		//return 0;
		UINT nPassedDay=0;
		
		//get all day from seprated registery and files
		for (int i=0; i<(int)m_places.GetCount(); i++)
		{
			CString strSubKey = Param::Get(_T("Subkey"), _T(""), m_places[i]);
			Path fileName(Param::Get(_T("File"), _T(""), m_places[i]));
			CString strEntry = Param::Get(_T("Entry"), _T(""), m_places[i]);
			CString strSection = Param::Get(_T("Section"), _T(""), m_places[i]);
			int nEncryptor = Param::Get(_T("Encryptor"), 0, m_places[i]);

			int nDay=0;
			if ( !strSubKey.IsEmpty() )
			{
				nDay = m::Reg::Get(HKEY_LOCAL_MACHINE, strSubKey, strEntry, 0);
			}
			else if ( !fileName.IsEmpty() )
			{
				nDay = PrivateProfile::Get(strSection, strEntry, 0, fileName);
			}

			if ( nDay!=0)
				nDay = nDay ^ nEncryptor;

			//find last date in high character
			int nLastTime = HIBYTE(HIWORD(nDay));
			nDay &= 0x00FFFFFF;
			if ( nLastTime!=CTime::GetCurrentTime().GetDay() )
				nDay++;

			nPassedDay = max(nPassedDay, (UINT)nDay);
		}

		return nPassedDay;
	}

	//@return the number of remain days
	__forceinline int GetRemainDay()
	{
		return max(0, m_nMaxDays - GetPassedDay());
	}

	__forceinline CLockTime(void)
	{
		int a = 5;
		m_nMaxDays = a*2;
	}

	~CLockTime(void)
	{
	}

private:
	int m_nMaxDays;
	CStringArray m_places;
};
