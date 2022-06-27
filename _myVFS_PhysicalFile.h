#pragma once


#include "_myVFS_base.h"

namespace TestTask
{
	/*
	Physical data file
	*/
	class P_File
	{
	private:
		//std::mutex		pf_mutex;

		fs::path		path;
		size_t			size = 0;

		size_t			Opened = 0;
		bool			ReadOnly = false;

		std::fstream	m_stream;

		size_t Seek(size_t offset);

	public:
		P_File(fs::path get_path);
		P_File(const P_File& pf);

		fs::path GetPath() const;
		size_t GetSize() const;
		size_t IsOpened() const;
		bool IsReadOnly() const;

		bool Open(bool ReadOnly);
		size_t HandleData(char* buff, size_t d_size, size_t d_pos, bool ReadMode);
		void Close();
	};
}