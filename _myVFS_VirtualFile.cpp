#include "_myVFS_VirtualFile.h"

using namespace TestTask;

Data_Fragment::Data_Fragment(size_t& d_position, size_t& d_size)
{
	data_position = d_position;
	data_size = d_size;
}


/*
Constructor for virtual file
*/
File::File(fs::path fp)
{
	path = fp;
}

File::File(const File& vf)
{
	path = vf.GetPath();
	size = vf.GetSize();
	ReadOnly = vf.IsReadOnly();
	Opened = vf.IsOpened();
	data_stored = vf.Read();
	data_pfile = vf.GetPFile();
}

/*
	Get path of a File
*/
fs::path File::GetPath() const
{
	return path;
}

/*
	Get size of a virtual file
*/
size_t File::GetSize() const
{
	return size;
}

/*
	Add pointer to corresponding data file
*/
void File::AddDataFile(fs::path path)
{
	//vf_mutex.lock();

	data_pfile = path;

	//vf_mutex.unlock();
}

/*
	Check current handling mode
*/
bool File::IsReadOnly() const
{
	return ReadOnly;
}

/*
	Check if file is already opened
*/
bool File::IsOpened() const
{
	return Opened;
}

/*
	Get pointer to corresponding data file
*/
fs::path File::GetPFile() const
{
	return data_pfile;
};

/*
	Open virtual file in corresponding mode
*/
bool File::Open(bool ReadMode)
{
	//vf_mutex.lock();

	if (Opened)
		return ReadOnly == ReadMode;

	Opened = true;
	ReadOnly = ReadMode;

	return true;

	//vf_mutex.unlock();
}

/*
	Get stored data positioning for reading
*/
std::vector<Data_Fragment> File::Read() const
{
	return data_stored;
}

/*
	Change stored data positioning according to data written
*/
void File::Write(size_t& d_position, size_t& d_size)
{
	//vf_mutex.lock();

	size += d_size;
	if (data_stored.size() > 0)
	{
		size_t last_position = data_stored.back().data_position + data_stored.back().data_size;

		if (last_position == d_position)
			data_stored.back().data_size += d_size;
		else
			data_stored.emplace_back(d_position, d_size);
	}
	else
		data_stored.emplace_back(d_position, d_size);

	//vf_mutex.unlock();
}


/*
	Close virtual file
*/
void File::Close()
{
	//std::lock_guard<std::mutex> lock(vf_mutex);

	Opened = false;
}
