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
    Huffman(Encode* nodes);
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
    else error("init_tree:failed");
}
Huffman::Huffman(Encode* encodes){
    if(encodes == nullptr)error("Huffman:argument error");
    long num = 0 ,i , j , index;
    for(i=0;i<256;i++)
        if(encodes[i].len > 0)
           num++;
    
    Node* nodes = new Node[num*2-1];
    if(nodes == nullptr)error("Huffman:new nodes error");
    
    index = 0;
    root = &(nodes[index++]);
    Node* pt=nullptr;
    for(i=0;i<256;i++){
        if(encodes[i].len > 0){
            pt = root;
            for(j=0 ; j<encodes[i].len ;j++){
                if(encodes[i].code[j] == '0'){
                    if(pt->left != nullptr)pt = pt->left;
                    else{
                        pt->left = &(nodes[index++]);
                        pt->left->parent = pt;
                        pt = pt->left;
                    }
                }
                else if (encodes[i].code[j] == '1'){
                    if(pt->right != nullptr) pt = pt->right;
                    else{
                        pt->right = &nodes[index++];
                        pt->right->parent = pt;
                        pt = pt->right;
                    }
                }
                else error("Huffman:encodes error");
            }
            pt->data = (unsigned char)i;
        }
    }
}
Huffman::~Huffman(){
    if(root != nullptr){
        free_memory(root);
        root = nullptr;
    }
}

void Huffman::in_traverse_tree(Node* root , long level){
    if(root != nullptr){
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
