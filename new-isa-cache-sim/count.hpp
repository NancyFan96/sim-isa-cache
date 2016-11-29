//
//  count.hpp
//  new-isa-cache-sim
//
//  Created by Nancy Fan on 11/28/16.
//  Copyright © 2016 Nancy Fan. All rights reserved.
//

#ifndef count_hpp
#define count_hpp

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

using namespace std;


void print_ins_cnt_init();
void print_ins_cnt();

extern std::map<std::string,int> COUNTS;


#endif /* count_hpp */
