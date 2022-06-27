#include "_myVFS_FileSystem.h"

using namespace TestTask;

/*
	FileSystem constructor, defines maximum size of a physical file and maximum possible physical files created
*/
IVFS::IVFS(size_t max_file_size, int max_files_created)
{
	max_data_size = max_file_size;
	max_data_files = max_files_created;
};

/*
Create mount point and header file or check if previous version of File System exists in the directory
*/
bool IVFS::Mount(const char* mount_path)
{
	//std::lock_guard<std::mutex> lock(fs_mutex);

	MountPoint = mount_path;
	fs::path p_mount = MountPoint;
	if (!p_mount.has_filename())
		MountPoint += "default_header";

	if (!fs::exists(p_mount.parent_path()))
		if (!fs::create_directories(p_mount.parent_path()))
		{
			std::cout << "File system failed to mount" << std::endl;
			return false;
		};;

	std::cout << "File system is being mounted to " << p_mount.parent_path() << std::endl;
	std::cout << "Header path: " << MountPoint << std::endl;

	/*if (fs::exists(MountPoint))
		if (!GetState(MountPoint))
			return false;*/

	std::cout << "Successfuly mounted" << "\n" << std::endl;
	return true;
}

/*
Get File System state from existing header file
*/
//bool IVFS::GetState(fs::path old_Header)
//{
//	std::cout << "\nLoading previous File system state ..." << old_Header << std::endl;
//
//	// If new state is not avalible
//	std::ifstream HeaderFileData(old_Header);
//	if (!HeaderFileData.is_open())
//	{
//		std::cout << "Can not open Header File" << old_Header << "\n"
//			<< "Changes discarded" << std::endl;
//		return false;
//	}
//
//	// Save previous state
//	fs::path tmp_path = MountPoint.parent_path();
//	tmp_path /= "_tmp_header";
//	SetState(tmp_path);
//	bool save_previous_state = false;
//
//	Mapped_Files.clear();
//	AllPhysFiles.clear();
//	AllVirtFiles.clear();
//
//	// Read Header line-per-line and map files
//	for (std::string g_line; std::getline(HeaderFileData, g_line); )
//	{
//		fs::path fp, dp;
//		std::stringstream line;
//		line << g_line;
//		line >> fp >> dp;
//
//		fs::path parent_path = fp.parent_path();
//
//		File* new_file = CreateVFile(fp);
//		P_File* new_data = GetPFile(dp);
//		if (new_file == nullptr || new_data == nullptr)
//		{
//			std::cout << "Failed to map virtual file " << fp << " with data file " << dp << std::endl;
//			save_previous_state = true;
//		}
//		else
//		{
//			Mapped_Files.insert({ new_file, new_data });
//			new_file->AddDataFile(dp);
//			while (line.tellg() != -1)
//			{
//				size_t d_pos, d_size;
//				line >> d_pos >> d_size;
//				new_file->Write(d_pos, d_size);
//			}
//		}
//	}
//
//	if (save_previous_state)
//	{
//		std::cout << "\n" << "Problems during loading file system saved state.\n"
//			<< "Previous succesful state saved in" << tmp_path << "\n" << std::endl;
//		return false;
//	}
//
//	fs::remove(tmp_path);
//	std::cout << "Successfuly loaded saved state, previous state is deleted\n" << std::endl;
//	return true;
//}
//
///*
//Save current File System state into header file
//*/
//void IVFS::SetState(fs::path new_Header)
//{
//	fs::path tmp_p = new_Header;
//	fs::path tmp_filename = tmp_p.filename();
//	tmp_filename += "_tmp";
//	fs::path tmp_path = tmp_p.parent_path();
//	tmp_path /= tmp_filename;
//	tmp_path += tmp_p.extension();
//
//	std::ofstream Header(tmp_path);
//
//	// Save virtual files mappen vith physical data files data
//	for (const std::pair<File*, P_File*> &it : Mapped_Files)
//	{
//		
//		Header
//			<< it.first->GetPath()
//			<< it.second->GetPath();
//
//		for (Data_Fragment i : it.first->Read())
//			Header
//			<< " " << i.data_position
//			<< " " << i.data_size;
//		Header << std::endl;
//	};
//
//	Header.close();
//
//	fs::rename(tmp_path, new_Header);
//
//	std::cout << "Previous state of file system is saved to " << new_Header << std::endl;
//}

/*
	Defragmentize data files
*/
void IVFS::Defragment()
{
}

/*
	Get pointer to a physical file if euther of them (pointer or file) exists
*/
P_File* IVFS::GetPFile(fs::path path)
{
	for (int it = 0; it < AllPhysFiles.size(); ++it)
	{
		if (AllPhysFiles[it].GetPath() == path)
			return &AllPhysFiles[it];
	};

	return nullptr;
}
/*
	Select physical file for writing virtual data, create new file if needed

	Different algorythms of selection can be used (based on statistics, file size, game directories etc)
	but i just return the file with the least data written or create new file if possible
*/
P_File* IVFS::SelectPFile()
{
	{
		//std::lock_guard<std::mutex> lock(fs_mutex);

		if (AllPhysFiles.size() < max_data_files)
		{
			fs::path p_file_path = MountPoint.parent_path();
			p_file_path /= "Data_file_";
			p_file_path += std::to_string(AllPhysFiles.size() + 1);
			p_file_path += ".txt";

			AllPhysFiles.emplace_back(p_file_path);
			return &AllPhysFiles.back();
		}
	}

	P_File* selected_pfile = nullptr;
	size_t min_file = max_data_size;

	for (int it = 0; it < AllPhysFiles.size(); ++it)
	{
		size_t p_size = AllPhysFiles[it].GetSize();
		if (p_size < min_file)
		{
			selected_pfile = &AllPhysFiles[it];
			min_file = p_size;
		}
	};
	return selected_pfile;
}

/*
	Get virtual file pointer from path
*/
File* IVFS::GetVFile(fs::path path)
{
	for (int it = 0; it < AllVirtFiles.size(); ++it)
	{
		if (AllVirtFiles[it].GetPath() == path)
			return &AllVirtFiles[it];
	}

	return nullptr;
}

/*
	Create new virtual file from path
*/
File* IVFS::CreateVFile(fs::path path)
{
	File* result = GetVFile(path);

	if (result == nullptr)
	{
		//std::lock_guard<std::mutex> lock(fs_mutex);

		AllVirtFiles.emplace_back(path);
		return &AllVirtFiles.back();
	}

	return result;
}