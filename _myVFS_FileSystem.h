#pragma once

#include "_myVFS_VirtualFile.h"

namespace TestTask
{
	struct IVFS
	{
	private:
		//std::mutex									fs_mutex;

		fs::path									MountPoint;
		bool										IsInitialized = false;
		size_t										max_data_size = 1024;
		int											max_data_files = 4;

		//std::unordered_map<File*, P_File*>			Mapped_Files;
		
		std::vector<P_File>							AllPhysFiles;
		std::vector<File>							AllVirtFiles;

		/*
		File system state management
		*/
	private:
		//bool GetState(fs::path old_path);
		P_File* SelectPFile();
		P_File* GetPFile(fs::path path);
		File* GetVFile(fs::path path);
		File* CreateVFile(fs::path path);

	public:
		IVFS(size_t max_file_size, int max_files_created);
		bool Mount(const char* mount_path);
		void Defragment(); // ----
		//void SetState(fs::path new_path);

		/*
		Interface elements management
		*/
	public:
		File* Create(const char* name);
		File* Open(const char* name);
		size_t Read(File* f, char* buff, size_t len);
		size_t Write(File* f, char* buff, size_t len);
		void Close(File* f);

	};
};
