//
//  DateDefs.h
//  8BitMusic
//
//  Created by SuXinDe on 2020/12/3.
//  Copyright © 2020 Skyprayer Studio. All rights reserved.
//

#ifndef DateDefs_h
#define DateDefs_h

#define hm_count 12

//音阶八度基准频率
float hzList[12] = {
    261.632f,
    277.189f,
    293.672f,
    311.135f,
    329.636f,
    349.237f,
    370.003f,
    392.005f,
    415.315f,
    440.010f,
    466.175f,
    493.895f
};

//谐振倍频振幅
float hm[26] = {
    0.9878f,
    0.3686f,
    0.6202f,
    0.4839f,
    0.1567f,
    0.08362f,
    0.1201f,
    0.07073f,
    0.005348f,
    0.02441f,
    0.02735f,
    0.0213f,
    0.01031f,
    0.006477f,
    0.01591f,
    0.003495f,
    0.002546f,
    0.0004751f,
    0.0008858f,
    0.0003792f,
    0.0006012f,
    0.0004224f,
    0.0001538f,
    0.0001454f,
    0.0002032f,
    0.0000483f
};


#define TRUE 1
#define FALSE 0

#endif /* DateDefs_h */
