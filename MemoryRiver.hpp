#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

// #include <iostream>
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
    const int maxlen=100;
    int len;
    struct link{
        T val;
        int pos;
        link *nxt,*prv;
        link(){
            nxt=prv=nullptr;
        }
    };
    link *head,*tail;
    MemoryRiver() = default;
    MemoryRiver(const string& file_name) : file_name(file_name),len(0),head(nullptr),tail(nullptr) {}
    ~MemoryRiver(){
        link *ptr=head;
        while(ptr){
            link *nxt=ptr->nxt;
            delete ptr;
            ptr=nxt;
        }
    }
    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out|std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        file.seekg((n-1)*sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp),sizeof(int));
    }
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.seekp((n-1)*sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp),sizeof(int));
    }
    void shift_to_head(link *ptr){
        if(!head)
            head=tail=ptr;
        if(ptr==head)
            return;
        if(ptr==tail)
            tail=ptr->prv;
        link *nxt=ptr->nxt,*prv=ptr->prv;
        if(nxt)
            nxt->prv=prv;
        if(prv)
            prv->nxt=nxt;
        ptr->nxt=head;
        head->prv=ptr;
        ptr->prv=nullptr;
        head=ptr;
    }
    void pop_back(){
        link *prv=tail->prv;
        prv->nxt=nullptr;
        file.seekp(tail->pos);
        file.write(reinterpret_cast<char *>(&tail->pos),sizeofT);
        delete tail;
        tail=prv;
    }
    int write(T &t){
        len++;
        link *ptr=new link;
        ptr->val=t;
        file.seekp(0,std::ios::end);
        int lst=file.tellp();
        ptr->pos=lst;
        shift_to_head(ptr);
        if(len>maxlen)
            pop_back(),len--;
        file.write(reinterpret_cast<char *>(&t),sizeofT);
        return lst;
    }
    void update(T &t, const int index){
        link *ptr=head;
        while(ptr!=nullptr&&ptr->pos!=index)
            ptr=ptr->nxt;
        if(ptr){
            ptr->val=t;
            shift_to_head(ptr);
        }
        else{
            len++;
            link *ptr=new link;
            ptr->val=t;
            ptr->pos=index;
            shift_to_head(ptr);
            if(len>maxlen)
                pop_back(),len--;
        }
    }
    void read(T &t, const int index) {
        link *ptr=head;
        while(ptr!=nullptr&&ptr->pos!=index)
            ptr=ptr->nxt;
        if(ptr){
            t=ptr->val;
            shift_to_head(ptr);
            return;
        }
        else{
            len++;
            link *ptr=new link;
            ptr->pos=index;
            file.seekg(index);
            file.read(reinterpret_cast<char *>(&t),sizeofT);
            ptr->val=t;
            shift_to_head(ptr);
            if(len>maxlen)
                pop_back(),len--;
        }
    }
    bool exist(){
        file.open(file_name, std::ios::in);
        bool res = file.is_open();
        file.close();
        return res;
    }
    void open(string FN = ""){
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
};


#endif