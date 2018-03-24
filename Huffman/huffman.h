//
//  huffman.h
//  Huffman
//
//  Created by lotu on 2018/3/23.
//  Copyright © 2018年 lotu. All rights reserved.
//

#ifndef huffman_h
#define huffman_h

#include "struct.h"
#include "heap.h"
class Huffman{
public:
    Huffman();
    Huffman(Node** nodes,long len);
    ~Huffman();
    
    void in_traverse_tree(Node* root, long level=0);
    void test_huffman_tree();
    Node* get_root(){
        return root;
    }
private:
    Node* root;
    void free_memory(Node* root);
};

Huffman::Huffman(){
    root=nullptr;
}
Huffman::Huffman(Node** nodes,long len){
    if(nodes == nullptr || len < 1 || len > 256){
        error("init_tree:argument error");
    }
    if(len == 1){
        error("init_tree:\nnodes len == 1, too small");
    }
    long len_heap = len,i;
    Node* parent_node = new Node[len];
    init_heap(nodes,len);
    Node *min1,*min2;
    for(i=1;i<=len-1;i++){
        get2min_heap(nodes,&len_heap,&min1,&min2);
        parent_node[i].left=min1;
        parent_node[i].right=min2;
        parent_node[i].count=min1->count+min2->count;
        min1->parent = &(parent_node[i]);
        min2->parent = &(parent_node[i]);
        len_heap++;
        nodes[len_heap] = &(parent_node[i]);
        init_heap(nodes,len_heap);
    }
    if(parent_node[len-1].parent == nullptr)this->root=&parent_node[len-1];
    else error("init tree:failed");
}

void Huffman::test_huffman_tree(){
    const long data_num=16;
    long data_max=100;
    long data_array[data_num+1];
    long heap_size=data_num;
    srand((long)time(0));
    Node * huffman_data[data_num+1];
    long level=0;
    
    for(long index=1;index<=data_num;index++){
        huffman_data[index]=(Node *)malloc(sizeof(Node));
        huffman_data[index]->count=rand()%data_max;
        data_array[index]=huffman_data[index]->count;
        huffman_data[index]->left=NULL;
        huffman_data[index]->right=NULL;
    }
    printf("orginal data\n");
    for(long i=1;i<=data_num;i++){
        if(i%9==0) printf("\n");
        printf("%-3ld, ",data_array[i]);
    }
    printf("\n");
    Node *huffman_tree_root=NULL;
    Huffman h = * new Huffman(huffman_data,heap_size);
//    huffman_tree(&huffman_tree_root,huffman_data,heap_size);
    h.in_traverse_tree(h.get_root());
//    hin_traverse_tree(huffman_tree_root,level);
    
}


Huffman::~Huffman(){
    if(root != nullptr){
        free_memory(root);
        root = nullptr;
    }
}

void Huffman::in_traverse_tree(Node* root , long level){
    if(root){
        in_traverse_tree(root->left,level+1);
        if(root->left==nullptr && root->right==nullptr){
            if(root->parent==nullptr){
                printf("error, leaf has no parent\n");
            }
            printf("leaf-1:%-4lld, ",root->count);
        }
//        printf("[level:%d,value:%lld] ",level,root->count);
        in_traverse_tree(root->right,level+1);
        
    }
}

void Huffman::free_memory(Node* root){
    
}


#endif /* huffman_h */
