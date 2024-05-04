#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
public:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    //读出第 n 个 int 的值赋给 tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        /* your code here */
        file.open(file_name,std::ios::in);
        file.seekg((n-1)*sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp),sizeof(int));
        file.close();
    }

    //将 tmp 写入第 n 个 int 的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        /* your code here */
        file.open(file_name,std::ios::in|std::ios::out);
        file.seekp((n-1)*sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp),sizeof(int));
        file.close();
    }

    //在文件合适位置写入类对象 t，并返回写入的位置索引 index
    //位置索引意味着当输入正确的位置索引 index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引 index 可以取为对象写入的起始位置
    int write(T &t) {
        /* your code here */
        file.open(file_name,std::ios::in|std::ios::out);
        file.seekp(0,std::ios::end);
        int lst=file.tellp();
        file.write(reinterpret_cast<char *>(&t),sizeofT);
        file.close();
        return lst;
    }

    //用 t 的值更新位置索引 index 对应的对象，保证调用的 index 都是由 write 函数产生
    void update(T &t, const int index) {
        /* your code here */
        file.open(file_name,std::ios::in|std::ios::out);
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&t),sizeofT);
        file.close();
    }

    //读出位置索引 index 对应的 T 对象的值并赋值给 t，保证调用的 index 都是由 write 函数产生
    void read(T &t, const int index) {
        /* your code here */
        file.open(file_name,std::ios::in);
        file.seekg(index);
        file.read(reinterpret_cast<char *>(&t),sizeofT);
        file.close();
    }
};


#endif //BPT_MEMORYRIVER_HPP