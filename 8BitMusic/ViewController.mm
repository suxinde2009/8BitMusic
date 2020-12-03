//
//  ViewController.m
//  8BitMusic
//
//  Created by SuXinDe on 2020/12/3.
//  Copyright © 2020 Skyprayer Studio. All rights reserved.
//

#import "ViewController.h"

#import "OpenAL8BitPlayer.hpp"


@interface ViewController () {
    OpenAL8BitPlayer *player;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}


- (IBAction)click:(id)sender {
    
    
    
    player = new OpenAL8BitPlayer();
    
    NSString *fileName =
    @"Time after time";
    //@"幻化成风";
    //@"钢铁洪流进行曲";
    
    NSString *filePath = [[NSBundle mainBundle] pathForResource:fileName
                                                         ofType:@"txt"];
    const char *filePathCstr = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        player->play(filePathCstr);
    });
    
}


@end
