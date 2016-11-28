//
//  count.cpp
//  new-isa-cache-sim
//
//  Created by Nancy Fan on 11/28/16.
//  Copyright © 2016 Nancy Fan. All rights reserved.
//

#include "count.hpp"


//initialize map
std::map<std::string,int> COUNTS;
long int COUNT_INS = 0;

void print_ins_cnt_init(){
    COUNTS["FMADD_D"] = 0;
    COUNTS["ADD"] = 0;
    COUNTS["SLL"] = 0;
    COUNTS["SLT"] = 0;
    COUNTS["SLTU"] = 0;
    COUNTS["XOR"] = 0;
    COUNTS["SRL"] = 0;
    COUNTS["OR"] = 0;
    COUNTS["AND"] = 0;
    COUNTS["SUB"] = 0;
    COUNTS["SRA"] = 0;
    COUNTS["MUL"] = 0;
    COUNTS["MULH"] = 0;
    COUNTS["MULHSH"] = 0;
    COUNTS["MULHU"] = 0;
    COUNTS["DIV"] = 0;
    COUNTS["DIVU"] = 0;
    COUNTS["REM"] = 0;
    COUNTS["REMU"] = 0;
    COUNTS["ADDW"] = 0;
    COUNTS["SLLW"] = 0;
    COUNTS["SRLW"] = 0;
    COUNTS["SUBW"] = 0;
    COUNTS["SRAW"] = 0;
    COUNTS["MULW"] = 0;
    COUNTS["DIVW"] = 0;
    COUNTS["DIVUW"] = 0;
    COUNTS["REMW"] = 0;
    COUNTS["REMUW"] = 0;
    COUNTS["FADD_S"] = 0;
    COUNTS["FADD_D"] = 0;
    COUNTS["FSUB_S"] = 0;
    COUNTS["FSUB_D"] = 0;
    COUNTS["FMUL_S"] = 0;
    COUNTS["FSUB_D"] = 0;
    COUNTS["FMUL_S"] = 0;
    COUNTS["FMUL_D"] = 0;
    COUNTS["FEQ_S"] = 0;
    COUNTS["FLT_S"] = 0;
    COUNTS["FLE_S"] = 0;
    COUNTS["FEQ_D"] = 0;
    COUNTS["FLT_D"] = 0;
    COUNTS["FLE_D"] = 0;
    COUNTS["JALR"] = 0;
    COUNTS["LB"] = 0;
    COUNTS["LH"] = 0;
    COUNTS["LW"] = 0;
    COUNTS["LBU"] = 0;
    COUNTS["LHU"] = 0;
    COUNTS["LD"] = 0;
    COUNTS["ADDI"] = 0;
    COUNTS["SLTI"] = 0;
    COUNTS["SLTIU"] = 0;
    COUNTS["XORI"] = 0;
    COUNTS["ORI"] = 0;
    COUNTS["ANDI"] = 0;
    COUNTS["SLLI"] = 0;
    COUNTS["SRLI"] = 0;
    COUNTS["SRAI"] = 0;
    COUNTS["ADDIW"] = 0;
    COUNTS["SLLIW"] = 0;
    COUNTS["SRLIW"] = 0;
    COUNTS["SRAIW"] = 0;
    COUNTS["SB"] = 0;
    COUNTS["SH"] = 0;
    COUNTS["SW"] = 0;
    COUNTS["SD"] = 0;
    COUNTS["BEQ"] = 0;
    COUNTS["BNE"] = 0;
    COUNTS["BLT"] = 0;
    COUNTS["BGE"] = 0;
    COUNTS["BLTU"] = 0;
    COUNTS["BGEU"] = 0;
    COUNTS["FLW"] = 0;
    COUNTS["FLD"] = 0;
    COUNTS["FSW"] = 0;
    COUNTS["FSD"] = 0;
    COUNTS["SFGNJ_D"] = 0;
    COUNTS["FSGNJN_D"] = 0;
    COUNTS["FCVT_S_D"] = 0;
    COUNTS["FCVT_D_S"] = 0;
    COUNTS["FCVT_W_S"] = 0;
    COUNTS["FCVT_WU_S"] = 0;
    COUNTS["FCVT_W_D"] = 0;
    COUNTS["FCVT_WU_D"] = 0;
    COUNTS["FCVT_S_W"] = 0;
    COUNTS["FCVT_S_WU"] = 0;
    COUNTS["FCVT_D_W"] = 0;
    COUNTS["FCVT_D_WU"] = 0;
    COUNTS["FCVT_L_S"] = 0;
    COUNTS["FCVT_L_D"] = 0;
    COUNTS["FCVT_S_L"] = 0;
    COUNTS["FMV_X_D"] = 0;
    COUNTS["FMV_D_X"] = 0;
    COUNTS["LUI"] = 0;
    COUNTS["AUIPC"] = 0;
    COUNTS["JAL"] = 0;
    COUNTS["MULHSU"] = 0;
    COUNTS["ECALL"] = 0;
}

void print_ins_cnt(){
    double ins_percent;
    //double check = 0;
    cout<<"\n\nINSTRUCTION PROFILE TABLE\n"<<setw(20)<<"INS NAME"<<setw(20)<<"COUNT"<<setw(20)<<"Percent(%)"<<endl;
    map<string, int>::iterator i_ins;
    for (i_ins = COUNTS.begin(); i_ins!=COUNTS.end(); i_ins++) {
        if (i_ins->second!=0) {
            cout<<setw(20)<<i_ins->first;
            cout<<setw(20)<<i_ins->second;
            ins_percent = (double)i_ins->second/COUNT_INS;
            cout<<setw(20)<<setiosflags(ios::fixed)<<setprecision(4)<<ins_percent*100<<endl;
            //check+=i_ins->second;
        }
    }
    cout<<"total instructions: "<<COUNT_INS<<endl;
    //cout << check << endl;
    
}
