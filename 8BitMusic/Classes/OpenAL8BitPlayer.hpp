//
//  OpenAL8BitPlayer.hpp
//  8BitMusic
//
//  Created by SuXinDe on 2020/12/3.
//  Copyright © 2020 Skyprayer Studio. All rights reserved.
//

#ifndef OpenAlEightBitPlayer_hpp
#define OpenAlEightBitPlayer_hpp

#include <stdio.h>
#include <cstdlib>

using namespace std;

class OpenAL8BitPlayer {
public:
    int play(const char *filePath);
};

#endif /* OpenAlEightBitPlayer_hpp */

/*
 乐谱文件解析说明
 
 每一行表示一个音节
 同时可以有多个音节播放
 
 第一行 定义乐谱类型 简谱或五线谱
 第二行 调号， 五线谱的话多个#或者b
 第三行 速度， 每秒的节拍数
 
 每个音节 第一个参数012分别表示低中高音，第二个参数1-7表示音名， 第三个参数表示节拍，1表示整拍， 0.5表示半拍， 0.25表示四分之一拍，最后有一个可选参数，-1和1表示降调和升调，也就是#和b
  
 
 
 
 
 */
