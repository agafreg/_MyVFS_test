#include "_myVFS_FileSystem.h"

using namespace TestTask;
TestTask::IVFS* FS = 0;

void f1(TestTask::IVFS* FS, bool read, const char* name, char* buffer, size_t length, int times)
{
    if (read)
    {
        File* f = FS->Open(name);
        size_t res = FS->Read(f, buffer, length);
        std::cout << "Read from " << name << ": " << res << std::endl;
        FS->Close(f);
    }
    else
    {
        File* f = FS->Create(name);
        for (int it = 0; it < times;++it)
        {
            size_t res = FS->Write(f, buffer, length);
            std::cout << "Wrote to " << name << ": " << res << std::endl;
        }
        FS->Close(f);
    }
}



int main()
{
    FS = new TestTask::IVFS(1024, 5);

    const char* mount_path = "sandbox/newFS";
    FS->Mount(mount_path);


    std::string s_buffer1 = "Write this very imortant information to file 1 ";
    char* buffer1 = s_buffer1.data();
    size_t length1 = s_buffer1.size();
    f1(FS, false, "new_file_1.a", buffer1, length1, 6);
    //std::thread t1(f1, FS, false, "new_file_1.a", buffer1, length1, 6);


    std::string s_buffer2 = "Write this imortant information to file 2 ";
    char* buffer2 = s_buffer2.data();
    size_t length2 = s_buffer2.size();
    f1(FS, false, "new_2.b", buffer2, length2, 12);
    //std::thread t2(f1, FS, false, "new_2.b", buffer2, length2, 12);


    const size_t length3 = 90;
    char* buffer3 = new char[length3];
    f1(FS, true, "new_file_1.a", buffer3, length3, 0);
    //std::thread t3(f1, FS, true, "new_file_1.a", buffer3, length3, 0);


    const size_t length4 = 700;
    char* buffer4 = new char[length4];
    f1(FS, true, "new_2.b", buffer4, length4, 0);
    //std::thread t4(f1, FS, true, "new_2.b", buffer4, length4, 0);

    std::string s_buffer5 = "Multiinfo to multiple files";
    char* buffer5 = s_buffer5.data();
    size_t length5 = s_buffer5.size();
    for (int i = 0; i < 20; ++i)
    {
        std::string multiple_failnames = "multi_";
        multiple_failnames += std::to_string(i);
        multiple_failnames += ".file";
        const char* fname = multiple_failnames.c_str();
        f1(FS, false, fname, buffer5, length5, 1);
        //std::thread t5(f1, FS, false, "new_file_1.a", buffer1, length1, 6);
    }
    
    /*t1.join();
    t2.join();
    t3.join();
    t4.join();*/

    delete FS;
}