#include "_myVFS_FileSystem.h"

using namespace TestTask;



/*
	Open virtual file in readonly mode
*/
File* IVFS::Open(const char* name)
{
	fs::path path = name;
	//std::cout << "\nOpening file " << path << std::endl;
	File* file = GetVFile(path);

	if (file == nullptr)
		return nullptr;

	fs::path d_path = file->GetPFile();
	P_File* data = GetPFile(d_path);

	bool mode = true;

	if (data != nullptr && file->Open(mode))
		if (data->Open(mode))
			return file;
		else
		{
			file->Close();
			data->Close();
		}
	return nullptr;
}

/*
	Open/create virtual file in writeonly state
*/
File* IVFS::Create(const char* name)
{
	fs::path path = name;
	//std::cout << "\nCreating file " << path << std::endl;
	File* file = CreateVFile(path);

	fs::path d_path = file->GetPFile();
	P_File* data = nullptr;

	if (d_path.has_filename())
		data = GetPFile(d_path);
	else
	{
		data = SelectPFile();
		{
			//std::lock_guard<std::mutex> lock(fs_mutex);
			//Mapped_Files.insert({ file, data });
		}
		file->AddDataFile(data->GetPath());
	}
		
	bool mode = false;

	if (data != nullptr && file->Open(mode))
		if (data->Open(mode))
			return file;
		else
			data->Close();

	return nullptr;
}

/*
	Read data from virtual file
*/
size_t IVFS::Read(File* f, char* buff, size_t len)
{
	File* file(f);
	fs::path d_path = file->GetPFile();
	P_File* data = GetPFile(d_path);

	if (data == nullptr || !file->IsOpened() || !data->IsOpened())
		return 0;

	std::vector<Data_Fragment> data_vector = file->Read();
	std::stringstream tmp_stream;

	size_t bytes_read = 0;
	size_t bytes_to_read = len;

	// Loop throug data fragments and read from them to temporary buffer
	for (Data_Fragment it : data_vector)
	{
		size_t bytes_from_fragment = bytes_to_read;
		if (it.data_size <= bytes_to_read)
			bytes_from_fragment = it.data_size;

		char* buffer = new char[bytes_from_fragment + 1];
		size_t data_read = data->HandleData(buffer, bytes_from_fragment, it.data_position, true);

		tmp_stream << buffer;
		bytes_read += data_read;

		//std::cout << buffer << std::endl;

		if (data_read < bytes_from_fragment)
			break;
	}
	tmp_stream.str().copy(buff, bytes_read);
	return bytes_read;
}

/*
	Write data into virtual file
*/
size_t IVFS::Write(File* f, char* buff, size_t len)
{
	File* file(f);
	fs::path d_path = file->GetPFile();
	P_File* data = GetPFile(d_path);

	if (data == nullptr || !file->IsOpened() || !data->IsOpened())
		return 0;

	//std::cout << "Writing to file " << f->GetPath() << std::endl;

	size_t d_pos = data->GetSize();
	size_t bytes_to_write = len;
	size_t possible_size = max_data_size - d_pos;

	if (possible_size < bytes_to_write)
		bytes_to_write = possible_size;
	if (bytes_to_write == 0)
		return 0;

	size_t bytes_written = data->HandleData(buff, bytes_to_write, d_pos, false);
	file->Write(d_pos, bytes_written);

	return bytes_written;
}

/*
	Close virtual file and all corresponding data files
*/
void IVFS::Close(File* f)
{
	File* file(f);
	fs::path d_path = file->GetPFile();
	P_File* data = GetPFile(d_path);

	if (data == nullptr || !file->IsOpened() || !data->IsOpened())
		return;

	//std::cout << "Closig file " << file->GetPath() << std::endl;

	data->Close();
	file->Close();
}



