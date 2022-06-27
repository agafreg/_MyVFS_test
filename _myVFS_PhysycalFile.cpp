#include "_myVFS_PhysicalFile.h"

using namespace TestTask;

/*
	Constructor for data file
*/
P_File::P_File(fs::path get_path)
{
	path = get_path;
	if (fs::exists(path))
		size = fs::file_size(path);
	else
	{
		//std::cout << "Creating physical file in " << path << std::endl;
		std::ofstream new_file(path);
		new_file.close();
	};
}

P_File::P_File(const P_File& pf)
{
	path = pf.GetPath();
	size = pf.GetSize();
	Opened = pf.IsOpened();
	ReadOnly = pf.IsReadOnly();
}

/*
	Get path of a File
*/
fs::path P_File::GetPath() const
{
	return path;
}

/*
	Get size of a File
*/
size_t P_File::GetSize() const
{
	return size;
}

/*
	Checks if file already been opened
*/
size_t P_File::IsOpened() const
{
	return Opened;
}
/*
	Check ReaadOnly status
*/
bool P_File::IsReadOnly() const
{
	return  ReadOnly;
}

/*
	Open data file
*/
bool P_File::Open(bool ReadMode)
{
	bool result = false;

	//pf_mutex.lock();

	if (Opened > 0 && m_stream)
	{
		if (ReadMode == ReadOnly && m_stream)
		{
			Opened++;
			result = true;
		}
	}
	else
	{
		std::ios_base::openmode open_mode;
		if (ReadMode == true)
			open_mode = std::fstream::in;

		else
			open_mode = std::fstream::app;

		Opened++;
		m_stream.open(path, open_mode);
		result = (bool)m_stream;
	}

	//pf_mutex.unlock();

	return result;
}

/*
	Read/Write data from/to physical file
*/
size_t P_File::HandleData(char* buff, size_t d_size, size_t d_pos, bool ReadMode)
{
	//std::cout << "Handling physical file " << this->GetPath() << std::endl;

	if (Opened == 0 || !(m_stream))
		return 0;

	size_t result = 0;

	//pf_mutex.lock();

	if (ReadMode == true)
	{
		if (Seek(d_pos) == d_pos)
		{
			m_stream.read(buff, d_size);
			if (m_stream)
				result = d_size;
			else
				result = m_stream.gcount();
		}
	}
	else
	{
		if (Seek(d_pos) == d_pos)
		{
			m_stream.write(buff, d_size);
			if (m_stream)
			{
				size += d_size;
				result = d_size;
			}
			else
			{
				size = m_stream.gcount();
				result = size - d_pos;
			}
		}
	}

	//pf_mutex.unlock();

	return result;
}

/*
	Set position indicator of streambuff object
*/
size_t P_File::Seek(size_t offset)
{
	if (Opened==0)
		return 0;

	m_stream.seekg(offset, std::ios_base::beg);
	m_stream.seekp(offset, std::ios_base::beg);

	return m_stream.tellg();
}

/*
	Close data file
*/
void P_File::Close()
{
	//pf_mutex.lock();

	if (Opened > 0)
		Opened--;
	if (Opened == 0 && (m_stream))
		m_stream.close();

	//pf_mutex.unlock();
}