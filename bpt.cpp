#include<iostream>
#include<array>
#include<string>
#include "MemoryRiver.hpp"
const int M=5;
class node;
class full_node;
class Arr;
MemoryRiver<full_node>file_node("1");
MemoryRiver<Arr>file_Arr("2");
MemoryRiver<int>file_bpt("3");
class node{
public:
    std::array<char,65>index;
    int val;
    bool operator <(const node &x)const{
        if(index!=x.index)
            return index<x.index;
        return val<x.val;
    }
    // node(node &x):index(x.index),val(x.val){}
    // node(std::array<char,65>&ind,int &val):index(ind),val(val){}
};
class full_node{
public:
    node a[M+1];
};
class Arr{
public:
    int size;
    bool is_leaf;
    int fa;//father pos in file_Arr
    int my;//my pos in file_Arr
    int arr;//full_node pos in file_node
    int son[M+1];//son pos in file_Arr
    int prev;//leaf pointer
    int next;//leaf pointer
    Arr(){
        size=my=arr=0;
        is_leaf=false;
        for(int i=0;i<=M;i++)
            son[i]=-1;
        prev=next=-1;
    };
};
int insert_to_Arr(node &x,Arr &cur);
void insert(node &x);
void split(node &x,Arr &fa,Arr &cur);
void insert_upper(node &x,Arr &cur,Arr &lson,Arr &rson);
bool find(node &x);
void display();
int root;
//if bpt doesn't have root,the first element in file_bpt is -1
int insert_to_Arr(node &x,Arr &cur){
    int i=0;
    full_node A;
    file_node.read(A,cur.arr);
    while(i<cur.size&&A.a[i]<x)
        i++;
    for(int j=cur.size;j>i;j--)
        A.a[j]=A.a[j-1];
    A.a[i]=x;
    cur.size++;
    file_node.update(A,cur.arr);
    return i;
}
void insert(node &x){
    if(root==-1){
        full_node A;
        Arr B;
        A.a[0]=x;
        B.arr=file_node.write(A);
        B.my=file_Arr.write(B);
        B.is_leaf=true;
        B.fa=-1;
        B.size=1;
        file_Arr.update(B,B.my);
        root=B.my;
    }
    else{
        Arr cur,fa;
        file_Arr.read(cur,root);
        while(!cur.is_leaf){
            fa=cur;
            full_node A;
            file_node.read(A,cur.arr);
            bool flg=false;
            for(int i=0;i<cur.size;i++)
                if(x<A.a[i]){
                    file_Arr.read(cur,cur.son[i]);
                    flg=true;
                    break;
                }
            if(!flg)
                file_Arr.read(cur,cur.son[cur.size]);
        }
        if(cur.size<M){
            insert_to_Arr(x,cur);
            cur.fa=fa.my;
            cur.son[cur.size]=cur.son[cur.size-1];
            cur.son[cur.size-1]=-1;
            file_Arr.update(cur,cur.my);
        }
        else split(x,fa,cur);
    }
}
void split(node &x,Arr &fa,Arr &cur){
    Arr lson,rson;
    lson.my=file_Arr.write(lson);
    rson.my=file_Arr.write(rson);
    insert_to_Arr(x,cur);
    lson.is_leaf=rson.is_leaf=true;
    lson.size=(M+1)/2;
    rson.size=(M+1)-lson.size;
    lson.next=rson.my;
    rson.prev=lson.my;
    lson.prev=cur.prev;
    rson.next=cur.next;
    if(lson.prev!=-1){
        Arr prev;
        file_Arr.read(prev,lson.prev);
        prev.next=lson.my;
        file_Arr.update(prev,prev.my);
    }
    if(rson.next!=-1){
        Arr next;
        file_Arr.read(next,rson.next);
        next.prev=rson.my;
        file_Arr.update(next,next.my);
    }
    full_node A,B,C;
    file_node.read(C,cur.arr);
    for(int i=0;i<lson.size;i++)
        A.a[i]=C.a[i];
    for(int i=0;i<rson.size;i++)
        B.a[i]=C.a[i+lson.size];
    lson.arr=file_node.write(A);
    rson.arr=file_node.write(B);
    lson.fa=fa.my;
    rson.fa=fa.my;
    file_Arr.update(lson,lson.my);
    file_Arr.update(rson,rson.my);
    if(cur.my==root){
        Arr cur_rt;
        full_node D;
        D.a[0]=B.a[0];
        cur_rt.my=file_Arr.write(cur_rt);
        cur_rt.arr=file_node.write(D);
        cur_rt.son[0]=lson.my;
        cur_rt.son[1]=rson.my;
        cur_rt.is_leaf=false;
        cur_rt.size=1;
        file_Arr.update(cur_rt,cur_rt.my);
        root=cur_rt.my;
        lson.fa=rson.fa=root;
        file_Arr.update(lson,lson.my);
        file_Arr.update(rson,rson.my);
    }
    else insert_upper(B.a[0],fa,lson,rson);
}
void insert_upper(node &x,Arr &cur,Arr &lson,Arr &rson){
    if(cur.size<M){
        int i=insert_to_Arr(x,cur);
        for(int j=cur.size;j>i+1;j--)
            cur.son[j]=cur.son[j-1];
        cur.son[i]=lson.my;
        cur.son[i+1]=rson.my;
        file_Arr.update(cur,cur.my);
    }
    else{
        Arr cur_lson,cur_rson,fa;
        if(cur.my!=root)
            file_Arr.read(fa,cur.fa);
        cur_lson.my=file_Arr.write(cur_lson);
        cur_rson.my=file_Arr.write(cur_rson);
        int son[M+2];
        for(int j=0;j<M+1;j++)
            son[j]=cur.son[j];
        int i=insert_to_Arr(x,cur);
        for(int j=M+1;j>i+1;j--)
            son[j]=son[j-1];
        son[i]=lson.my;
        son[i+1]=rson.my;
        cur_lson.is_leaf=cur_rson.is_leaf=false;
        cur_lson.size=(M+1)/2;
        cur_rson.size=M-(M+1)/2;
        full_node A,B,C;
        file_node.read(C,cur.arr);
        for(int j=0;j<cur_lson.size;j++)
            A.a[j]=C.a[j];
        for(int j=0;j<cur_rson.size;j++)
            B.a[j]=C.a[j+cur_lson.size+1];
        for(int j=0;j<lson.size+1;j++){
            cur_lson.son[j]=son[j];
            Arr s;
            file_Arr.read(s,son[j]);
            s.fa=cur_lson.my;
            file_Arr.update(s,s.my);
        }
        for(int j=0;j<rson.size+1;j++){
            cur_rson.son[j]=son[j+lson.size+1];
            Arr s;
            file_Arr.read(s,son[j+lson.size+1]);
            s.fa=cur_rson.my;
            file_Arr.update(s,s.my);
        }
        cur_lson.arr=file_node.write(A);
        cur_rson.arr=file_node.write(B);
        if(cur.my!=root){
            cur_lson.fa=fa.my;
            cur_rson.fa=fa.my;
        }
        else{
            cur_lson.fa=-1;
            cur_rson.fa=-1;
        }
        file_Arr.update(cur_lson,cur_lson.my);
        file_Arr.update(cur_rson,cur_rson.my);
        if(cur.my==root){
            Arr cur_rt;
            full_node D;
            D.a[0]=C.a[cur_lson.size];
            cur_rt.my=file_Arr.write(cur_rt);
            cur_rt.arr=file_node.write(D);
            cur_rt.son[0]=cur_lson.my;
            cur_rt.son[1]=cur_rson.my;
            cur_rt.is_leaf=false;
            cur_rt.size=1;
            file_Arr.update(cur_rt,cur_rt.my);
            root=cur_rt.my;
            cur_lson.fa=cur_rson.fa=root;
            file_Arr.update(cur_lson,cur_lson.my);
            file_Arr.update(cur_rson,cur_rson.my);
        }
        else
            insert_upper(C.a[cur_lson.size],fa,cur_lson,cur_rson);
    }
}
bool find(node &x){
    Arr cur;
    full_node A;
    file_Arr.read(cur,root);
    while(!cur.is_leaf){
        file_node.read(A,cur.arr);
        bool flg=false;
        for(int i=0;i<cur.size;i++)
            if(x.index<=A.a[i].index){
                file_Arr.read(cur,cur.son[i]);
                flg=true;
                break;
            }
        if(!flg)
            file_Arr.read(cur,cur.son[cur.size]);
    }
    bool flg=false;
    do{
        file_node.read(A,cur.arr);
        // if(x.index[0]=='8'&&x.index[1]=='0')
        //     for(int i=0;i<cur.size;i++)
        //         std::cout<<A.a[i].index[0]<<A.a[i].index[1]<<" ";std::cout<<std::endl;
        if(A.a[0].index>x.index)
            break;
        for(int i=0;i<cur.size;i++)
            if(A.a[i].index==x.index){
                flg=1;
                std::cout<<A.a[i].val<<" ";
            }
        // std::cout<<"\n";
        if(cur.next!=-1)
            file_Arr.read(cur,cur.next);
        else
            break;
    }while(A.a[0].index<=x.index);
    return flg;
}
#include<queue>
void display(){
    std::queue<int>que;
    que.push(root);
    while(!que.empty()){
        int size=que.size();
        while(size--){
            int u=que.front();
            que.pop();
            Arr a;
            file_Arr.read(a,u);
            full_node b;
            file_node.read(b,a.arr);
            std::cout<<"[";
            for(int i=0;i<a.size;i++){
                for(int j=0;b.a[i].index[j];j++)
                    std::cout<<b.a[i].index[j];
                std::cout<<" ";
                std::cout<<b.a[i].val<<",";
            }
            std::cout<<"] ";
            if(!a.is_leaf)
                for(int i=0;i<=a.size;i++){
                    que.push(a.son[i]);
                    Arr son;
                    file_Arr.read(son,a.son[i]);
                    if(son.fa!=u)
                        std::cout<<"holy shit!!!"<<std::endl;
                }
        }
        std::cout<<"\n";
    }
}
void init(){
    file_bpt.file.open("1",std::ios::in|std::ios::out|std::ios::binary);
    if(!file_bpt.file.good()){
        file_node.initialise("1");
        file_Arr.initialise("2");
        file_bpt.initialise("3");
        file_bpt.write_info(-1,1);
    }
    else
        file_bpt.file.close();
}
void fsta(std::array<char,65>&arr,string &s){
    for(std::string::size_type i=0;i<s.length();i++)
        arr[i]=s[i];
    for(std::string::size_type i=s.length();i<65;i++)
        arr[i]='\0';
}
int main(){
    freopen("data","r",stdin);
    freopen("out","w",stdout);
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    std::cout.tie(0);
    //init();
    file_node.initialise("1");
    file_Arr.initialise("2");
    file_bpt.initialise("3");
    file_bpt.write_info(-1,1);
    file_node.sizeofT=sizeof(full_node);
    file_Arr.sizeofT=sizeof(Arr);
    file_bpt.sizeofT=sizeof(int);
    file_bpt.get_info(root,1);
    int T;
    std::cin>>T;
    int cnt=0;
    while(T--){
        string opt,ind;
        std::cin>>opt>>ind;
        node x;
        fsta(x.index,ind);
        if(opt[0]=='i'){
            std::cin>>x.val;
            insert(x);
        }
        if(opt[0]=='d'){
            std::cin>>x.val;

        }
        if(opt[0]=='f'){
            cnt++;
            if(!find(x))
                std::cout<<"null";
            std::cout<<"\n";
        }
        // std::cout<<"after "<<cnt<<"th query\n";
        // display();
        // std::cout<<"\n";
    }
    file_bpt.write_info(root,1);
    return 0;
}
/*
8
insert  1966
insert CppPrimer 2012
insert Dune 2021
insert CppPrimer 2001
find CppPrimer
find Dune
find FlowersForAlgernon
find Java

*/