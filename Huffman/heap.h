//
//  heap.h
//  Huffman
//
//  Created by lotu on 2018/3/23.
//  Copyright © 2018年 lotu. All rights reserved.
//

#ifndef heap_h
#define heap_h

#include"struct.h"
//调整最小堆
void adjust_heap(Node** nodes,long s,long e){
    if(nodes==nullptr || s < 0  || e < 0 || e < s){
        error("adjust_heap:argument error");
    }
    if(s==e)return;
    Node* cur=nodes[s];
    long cur_i = s,i;
    for(i=2*s;i<=e;i*=2){
        if(i<e && nodes[i]->count > nodes[i+1]->count){
            i++;
        }
        if(cur->count <= nodes[i]->count)break;
        nodes[cur_i]=nodes[i];
        cur_i=i;
    }
    nodes[cur_i]=cur;
}
//初始化最小堆
void init_heap(Node** nodes,long len){
    if(nodes == nullptr || len <=0 || len > 256){
        error("init_heap:argument error");
    }
    
    for(long root = len/2;root >= 1;root--){
        adjust_heap(nodes,root,len);
    }
}
//获得当前堆最小的两个数
void get2min_heap(Node** nodes,long* len,Node** min1,Node** min2){
    if(nodes == nullptr){
        error("get2min_heap:argument error");
    }
    *min1 = nodes[1];
    nodes[1]=nodes[*len ];
    (*len)-=1;
    
    adjust_heap(nodes,1,*len);
    *min2 = nodes[1];
    nodes[1] = nodes[*len ];
    if(*len > 0){
        adjust_heap(nodes,1,*len);
    }
}

#endif /* heap_h */
