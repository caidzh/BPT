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
    const int maxlen=50;
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
        file.open(file_name, std::ios::out);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        file.open(file_name,std::ios::in);
        file.seekg((n-1)*sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp),sizeof(int));
        file.close();
    }
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.open(file_name,std::ios::in|std::ios::out);
        file.seekp((n-1)*sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp),sizeof(int));
        file.close();
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
        delete tail;
        tail=prv;
    }
    // void check(){
    //     link *ptr=head;
    //     while(ptr){
    //         std::cout<<ptr->pos<<" ";
    //         ptr=ptr->nxt;
    //     }
    //     std::cout<<"\n";
    // }
    int write(T &t){
        len++;
        link *ptr=new link;
        ptr->val=t;
        file.open(file_name,std::ios::in|std::ios::out);
        file.seekp(0,std::ios::end);
        int lst=file.tellp();
        ptr->pos=lst;
        shift_to_head(ptr);
        if(len>maxlen)
            pop_back(),len--;
        file.write(reinterpret_cast<char *>(&t),sizeofT);
        file.close();
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
        file.open(file_name,std::ios::in|std::ios::out);
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&t),sizeofT);
        file.close();
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
            file.open(file_name,std::ios::in);
            file.seekg(index);
            file.read(reinterpret_cast<char *>(&t),sizeofT);
            file.close();
            ptr->val=t;
            shift_to_head(ptr);
            if(len>maxlen)
                pop_back(),len--;
        }
    }
};


#endif