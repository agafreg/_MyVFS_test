#pragma once


#include "_myVFS_PhysicalFile.h"

namespace TestTask
{
	/*
	Information on stored data positioning
	*/
	struct Data_Fragment
	{
		size_t						data_position;
		size_t						data_size;

		Data_Fragment(size_t& d_position, size_t& d_size);
	};

	/*
	Virtual file
	*/
	class File
	{
	private:
		//std::mutex					vf_mutex;

		fs::path					path;
		size_t						size = 0;

		bool						ReadOnly = false;
		bool						Opened = false;

		std::vector<Data_Fragment>	data_stored;
		fs::path					data_pfile;

	public:

		File(fs::path get_path);
		File(const File& vf);

		fs::path GetPath() const;
		size_t GetSize() const;
		void AddDataFile(fs::path path);

		bool IsReadOnly() const;
		bool IsOpened() const;
		fs::path GetPFile() const;

		bool Open(bool ReadMode);
		std::vector<Data_Fragment> Read() const;
		void Write(size_t& d_position, size_t& d_size);
		void Close();
	};
}