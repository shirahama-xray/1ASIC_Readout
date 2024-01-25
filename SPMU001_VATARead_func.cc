#include <anlnext/CLIUtility.hh>
#include "SPMU001_VATARead.hh"

using namespace anlnext;

//size: byte
void SPMU001_VATARead::printReadData(unsigned int address, vector<unsigned char>* data, unsigned int size)
{
    unsigned int upperAddress,lowerAddress;
    for(unsigned int i=0; i< size/4; i++)
    {
        unsigned int ii=i*4;
        upperAddress=(address+ii)/0x00010000;
        lowerAddress=(address+ii)%0x00010000;
        cout << setfill('0') << setw(4) << hex << (unsigned int)upperAddress;
        cout << "-" << setfill('0') << setw(4) << hex << (unsigned int)lowerAddress;
        cout << "  ";
        cout << "0x" << setfill('0') << setw(8) << hex << ((unsigned int)(data->at(ii))*0x01000000+(unsigned int)(data->at(ii+1))*0x00010000 + (unsigned int)(data->at(ii+2))*0x0100+(unsigned int)(data->at(ii+3))) << dec << endl;
    }
}


void SPMU001_VATARead::show_status()
{
    unsigned char tmp;
    unsigned int readdata32bits;
    unsigned int address;
    
    address = 0x00000090 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "TI upper 32bits : " << dec << readdata32bits << endl;
    address = 0x00000094 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "TI lower 32bits : " << dec << readdata32bits << endl;
    address = 0x00000098 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "TI upper 32bits next: " << dec << readdata32bits << endl;
    address = 0x0000009c + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Timecode: " << dec << readdata32bits << endl;
    
    address = 0x000000a0 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ext1 TI upper 32bits : " << dec << readdata32bits << endl;
    address = 0x000000a4 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ext1 TI lower 32bits : " << dec << readdata32bits << endl;
    
    address = 0x000000b0 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ext2 TI upper 32bits : " << dec << readdata32bits << endl;
    address = 0x000000b4 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ext2 TI lower 32bits : " << dec << readdata32bits << endl;
    
    address = 0x00000020 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    tmp = readdata32bits&0x07;
    cout << "VaStatus :" << "0x" << hex << (unsigned int)tmp << endl;
    
    address = 0x00000024 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ModuleStatus :" << "0x" << hex << (unsigned int)readdata32bits << endl;

    // edit by shirahama
    address = 0x00000028 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "VaFlags :" << dec << readdata32bits << endl;

    address = 0x0000002c + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "SetUpMode_Flags :" << dec << readdata32bits << endl;

    address = 0x00000030 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ObsMode_Flags :" << dec << readdata32bits << endl;
    // upto here
    
    address = 0x00000040 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Peaking Time : " << dec << readdata32bits << " (" << "0x" << hex << readdata32bits << ")" << endl;
    int p_side_peakt = readdata32bits&0xffff;
    int n_side_peakt = (readdata32bits&0xffff0000)>>16;
    cout << " p-side : " << dec << p_side_peakt << endl;
    cout << " n-side : " << dec << n_side_peakt << endl;
    
    
    address = 0x00000044 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ADCClockPeriod :" << dec << readdata32bits << endl;
    
    address = 0x00000048 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "ReadOutPeriod :" << dec << (0x00ff & readdata32bits) << endl;
    cout << "ReadOutDelay :" << dec << ((0xff00 & readdata32bits)/0x0100) << endl;
    
    address = 0x00000038 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Enable Flag :" << dec << readdata32bits << endl;
    
    address = 0x0000003c + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "External Input Mode :" << dec << readdata32bits << endl;
    
    address = 0x00000054 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "TRIGPAT Latch timing :" << dec << readdata32bits << endl;
    
    address = 0x00000058 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Reset wait time :" << dec << readdata32bits << endl;
    
    address = 0x0000005c + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Reset wait 2 time :" << dec << readdata32bits << endl;

    address = 0x00000060 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "TI: " << dec << readdata32bits << endl;
    
    address = 0x00000064 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Integral Livetime : " << dec << readdata32bits << endl;
    
    address = 0x00000068 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Dead time : " << dec << readdata32bits << endl;
    
    address = 0x0000006c + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Test Data : " << "0x" << hex << readdata32bits << endl;
    
    address = 0x00000070 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "CaldTrigReq : " << "0x" << hex << readdata32bits << endl;
    
    address = 0x00000074 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "PulseWidth : " << "0x" << hex << readdata32bits << endl;
    
    address = 0x00000078 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "PulseWidthVeto : " << "0x" << hex << readdata32bits << endl;

    address = 0x0000007c + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "disable period : " << dec << readdata32bits << endl;

    address = 0x000000c0 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Pseudo ONOFF : " << "0x" << hex << readdata32bits << endl;
    address = 0x000000c4 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Pseudo Rate : " << "0x" << hex << readdata32bits << endl;
    address = 0x000000c8 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Pseudo Counter : " << dec << readdata32bits << endl;
    
    address = 0x00000080 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "trigon time1 : " << dec << readdata32bits << endl;
    address = 0x00000084 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "max wait time of electron : " << dec << readdata32bits << endl;

    address = 0x000000d0 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Write Ptr : " << "0x" << hex << readdata32bits << endl;
    address = 0x000000d4 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "Write Ptr Reset Rq. : " << dec << readdata32bits << endl;

    // edit by shirahama
    // TRIG_IN_counter setting
    address = 0x000000e0 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "TRIG_photon_counter : " << dec << readdata32bits << endl;
    // nodata_forced_trigger_counter setting
    address = 0x000000e4 + spwaddress_offset;
    readdata32bits= rmapread32bits(address);
    cout << "nodata_forced_trigger_counter : " << dec << readdata32bits << endl; 
    // upto here

    address = 0x00000000 + spwaddress_dacmod;
    readdata32bits= rmapread32bits(address);
    cout << "DAC EXEC : " << dec << readdata32bits << endl;
    address = 0x00000004 + spwaddress_dacmod;
    readdata32bits= rmapread32bits(address);
    cout << "DAC SET : " << dec << readdata32bits << endl;

    cout << dec << endl;
    
    return;
}


void SPMU001_VATARead::config_fpga()
{  
    unsigned int addressUW;
    int peakingtime   = 40;
    int peakingtime_p = 250;
    int peakingtime_n = 250;
    int convclkrate   = 1;
    int readclkrate   = 5;
    int readdelay     = 15;
    int enableflag    = 1;
    int resetmode     = 0;
    int resetwait_time;
    int resetwait_time2;

    int caldtrigreq;
    int pulsewidth;
    int pulsewidthveto;
    
    int hitpat_latch_timing  = 1500;
    int trigpat_latch_timing = 25;
    int fastbgo_latch_timing = 250;

    int disable_period = 100;
    
    int pseudo_onoff = 0;
    int pseudo_rate  = 1;

    // edit by shirahama
    int trigon_time1;
    int trigon_time2;
    // up to here
    
    writedata.clear();
    writedata.resize(4,0x00);

    //Peaking time
    peakingtime_p = (pt.get_optional<int>("fpga_config.peakingtime_p")).get();
    peakingtime_n = (pt.get_optional<int>("fpga_config.peakingtime_n")).get();
    cout << "peaking time P " << dec << peakingtime_p << endl;
    cout << "peaking time N " << dec << peakingtime_n << endl;

    addressUW = 0x00000040;
    writedata[3]=(unsigned char) (peakingtime_p % 0x0100);
    writedata[2]=(unsigned char) (peakingtime_p / 0x0100);
    writedata[1]=(unsigned char) (peakingtime_n % 0x0100);
    writedata[0]=(unsigned char) (peakingtime_n / 0x0100);
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);


    //Conversion clock time
    convclkrate = (pt.get_optional<int>("fpga_config.convclkrate")).get();
    cout << "conversion clk rate " << dec << convclkrate << endl;
            
    addressUW = 0x00000044;
    writedata[3]=(unsigned char) (convclkrate % 0x0100);
    writedata[2]=(unsigned char) (convclkrate / 0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);

    //Readout
    readclkrate = (pt.get_optional<int>("fpga_config.readclkrate")).get();
    readdelay   = (pt.get_optional<int>("fpga_config.readdelay")).get();
    cout << "readout clk rate " << dec << readclkrate << endl;
    cout << "readout delay "    << dec << readdelay   << endl;

    addressUW = 0x00000048;
    writedata[3]=(unsigned char) (readclkrate%0x0100);
    writedata[2]=(unsigned char) (readdelay%0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);

    //Enable flags
    enableflag = (pt.get_optional<int>("fpga_config.enableflag")).get();
    cout << "enable flags  " << dec << enableflag << endl;

    addressUW = 0x00000038;
    writedata[3]=(unsigned char) (enableflag%0x0100);
    writedata[2]=(unsigned char) (enableflag/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);


    //Reset mode
    resetmode = (pt.get_optional<int>("fpga_config.resetmode")).get();
    cout << "external input " << dec << resetmode << endl;
    
    addressUW = 0x0000003c;
    writedata[3]=(unsigned char) (resetmode%0x0100);
    writedata[2]=(unsigned char) (resetmode/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);

    //Reset wait time
    trigpat_latch_timing = (pt.get_optional<int>("fpga_config.trigpat_latch_timing")).get();
    resetwait_time       = (pt.get_optional<int>("fpga_config.resetwait_time")).get();
    resetwait_time2      = (pt.get_optional<int>("fpga_config.resetwait_time2")).get();

    addressUW = 0x00000050;
    writedata[3]=(unsigned char) (hitpat_latch_timing%0x0100);
    writedata[2]=(unsigned char) (hitpat_latch_timing/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    //		rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    
    addressUW = 0x00000054;
    writedata[3]=(unsigned char) (trigpat_latch_timing%0x0100);
    writedata[2]=(unsigned char) (trigpat_latch_timing/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    
    addressUW = 0x00000058;
    writedata[3]=(unsigned char) ((resetwait_time & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((resetwait_time & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((resetwait_time & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((resetwait_time & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    
    addressUW = 0x0000005C;
    writedata[3]=(unsigned char) ((resetwait_time2 & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((resetwait_time2 & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((resetwait_time2 & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((resetwait_time2 & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);

    //Charge injection
    caldtrigreq = (pt.get_optional<int>("fpga_config.caldtrigreq")).get();
    pulsewidth  = (pt.get_optional<int>("fpga_config.pulsewidth")).get();
    pulsewidthveto = (pt.get_optional<int>("fpga_config.pulsewidthveto")).get();

    addressUW = 0x00000070;
    writedata[3]=(unsigned char)(caldtrigreq%0x0100);
    writedata[2]=(unsigned char)(caldtrigreq/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    addressUW = 0x00000074;
    writedata[3]=(unsigned char) ((pulsewidth & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((pulsewidth & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((pulsewidth & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((pulsewidth & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);
    addressUW = 0x00000078;
    writedata[3]=(unsigned char) ((pulsewidthveto & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((pulsewidthveto & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((pulsewidthveto & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((pulsewidthveto & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);

    //Disable period
    disable_period = (pt.get_optional<int>("fpga_config.disable_period")).get();
    addressUW = 0x0000007C;
    writedata[3]=(unsigned char) ((disable_period & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((disable_period & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((disable_period & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((disable_period & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);

    //Pseudo
    pseudo_onoff = (pt.get_optional<int>("fpga_config.pseudo_onoff")).get();
    pseudo_rate  = (pt.get_optional<int>("fpga_config.pseudo_rate")).get();

    addressUW = 0x000000c0;
    writedata[3]=0x00;
    writedata[2]=0x00;
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    addressUW = 0x000000c4;
    writedata[3]=(unsigned char)(pseudo_rate%0x0100);
    writedata[2]=(unsigned char)(pseudo_rate/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    addressUW = 0x000000c0;
    writedata[3]=(unsigned char)(pseudo_onoff%0x0100);
    writedata[2]=(unsigned char)(pseudo_onoff/0x0100);
    writedata[1]=0x00;
    writedata[0]=0x00;
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);

    // edit by shirahama
    // trigon_time
    trigon_time1 = (pt.get_optional<int>("fpga_config.trigon_time1")).get();
    trigon_time2 = (pt.get_optional<int>("fpga_config.trigon_time2")).get();
    
    addressUW = 0x00000080;
    writedata[3]=(unsigned char) ((trigon_time1 & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((trigon_time1 & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((trigon_time1 & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((trigon_time1 & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    addressUW = 0x00000084;
    writedata[3]=(unsigned char) ((trigon_time2 & 0x000000ff) >> 0);
    writedata[2]=(unsigned char) ((trigon_time2 & 0x0000ff00) >> 8);
    writedata[1]=(unsigned char) ((trigon_time2 & 0x00ff0000) >> 16);
    writedata[0]=(unsigned char) ((trigon_time2 & 0xff000000) >> 24);
    rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
    // up to here

    return;
}

void SPMU001_VATARead::config_vareg()
{
    int selection = 0;
    int notend = 1;

    while(notend==1)
    {
        cout << "Select : " << endl;
        cout << "  One VATA45X chip [1]" << endl;
        cout << "  One VATA46X chip [2]" << endl;
        cout << "  HXImini DSD      [4]" << endl;
        cout << "  ALL              [7]" << endl;
        cout << "  Gantry DSD       [8]" << endl;
        cout << "  return to main   [9]" << endl;

        cli_read((std::string)"selection", &selection);
        
        if(selection == 7)
        {
            config_vareg_all();
        }
        if(selection == 8)
        {
            config_vareg_part(8, 32);
        }
        else if(selection == 4)
        {
            config_vareg_part(4, 32);
        }
        else if(selection == 1)
        {
            config_vareg_part(1, 32);
        }
        else if(selection == 2)
        {
            config_vareg_part(1, 16);
        }
        else if(selection == 9)
        {
            notend = 0;
        }
        else
        {
            notend  = 0;
        }
    }
}


void SPMU001_VATARead::config_vareg_part(int chips, int words)
{
    int i,j;
    int base_address = spwaddress_statusdata2;
    unsigned int tmpval;

    std::string regfilename;
    cli_read((std::string)"ASIC regster file", (std::string*)&regfilename);
    ifstream ifs(regfilename);

    std::string str;
    int str_i;
    
    for(i=0;i<REG_BUF_SIZE/4;i++)
    {
        reg_buf[i] = 0;
        if(i%words == 0)
        {
            cout << dec << i/words << " chip " << endl;
            j = i/words;
        }
        
        tmpval = 0x00000000;
        if(j < chips)
        {
            cout << hex << base_address+i*4 << " ";

            //upper 16bits
            getline(ifs, str);
            std::istringstream iss_up(str);
            iss_up >> std::hex >> tmpval;
            reg_buf[i] += tmpval * 0x00010000;

            //lower 16bits
            getline(ifs, str);
            std::istringstream iss_low(str);
            iss_low >> std::hex >> tmpval;
            reg_buf[i] += tmpval;
            cout << hex << setfill('0') << setw(8) << reg_buf[i] << endl;
        }
        else
        {
            reg_buf[i] = 0;
        }
    }

    return;
}


void SPMU001_VATARead::config_vareg_all()
{
    config_vareg_part(32, 32);
    return;
}


void SPMU001_VATARead::set_vareg()
{
    int i;
    
    writedata.clear();
    writedata.resize(REG_BUF_SIZE);
    readdata.clear();
    readdata.resize(REG_BUF_SIZE);
    
    for(i=0;i<REG_BUF_SIZE/4;i++)
    {
        writedata[i*4+3]=(unsigned char) ((reg_buf[i] & 0x000000ff) >> 0);
        writedata[i*4+2]=(unsigned char) ((reg_buf[i] & 0x0000ff00) >> 8);
        writedata[i*4+1]=(unsigned char) ((reg_buf[i] & 0x00ff0000) >> 16);
        writedata[i*4+0]=(unsigned char) ((reg_buf[i] & 0xff000000) >> 24);
        rmapwrite(spwaddress_statusdata2+i*4, 4, &writedata[i*4]);
        rmapread(spwaddress_statusdata2+i*4, 4, &readdata[i*4]);
    }
    
    printReadData(spwaddress_statusdata2, &readdata, REG_BUF_SIZE);
}

void SPMU001_VATARead::load_readback_vareg()
{
    unsigned int spwaddress_vaflag        = 0x00000028 + spwaddress_offset;
    unsigned int spwaddress_setupmodeflag = 0x0000002c + spwaddress_offset;
    
    unsigned int spwaddress_modstatus     = 0x00000024 + spwaddress_offset;
    vector<unsigned char> vaflag;
    vector<unsigned char> setupmode_flag;
    vaflag.resize(4, 0x00);
    setupmode_flag.resize(4,0x00);
    
    vaflag[3] = 0x01;
    rmapwrite32bits(spwaddress_vaflag, &vaflag[0]);
    
    setupmode_flag[3] = 0x03;
    rmapwrite32bits(spwaddress_setupmodeflag, &setupmode_flag[0]);
    
    unsigned int readstatus;

    do
    {
        usleep2(100000);
        readstatus= rmapread32bits(spwaddress_modstatus);
        
        if(( (readstatus & 0xff) == 0x06))
        {
            setupmode_flag[3] = 0x00;
            rmapwrite32bits(spwaddress_setupmodeflag, &setupmode_flag[0]);
            break;
        }
    }
    while(1);

    unsigned int reg_addressUW = spwaddress_statusdata2;
    unsigned int rb_addressUW =  spwaddress_statusdata3;
    vector<unsigned char> reg_data;
    vector<unsigned char> rb_data;
    reg_data.resize(REG_BUF_SIZE);
    rb_data.resize(REG_BUF_SIZE);
    
    rmapread(reg_addressUW, REG_BUF_SIZE, &reg_data[0]);
    rmapread(rb_addressUW, REG_BUF_SIZE, &rb_data[0]);
    
    
    cout << "send data" << endl;
    printReadData(reg_addressUW, &reg_data, REG_BUF_SIZE);
    
    cout << "read back data" << endl;
    printReadData(rb_addressUW, &rb_data, REG_BUF_SIZE);
    
    unsigned int error = 0;
    for(unsigned int i=0; i<REG_BUF_SIZE; i++)
    {
        if(reg_data[i] != rb_data[i])
        {
            cout << dec << i << " : " << "NG!! Set VA Regisiter again. " << endl;
            cout << "load     " << setfill('0') << setw(8) << hex << reg_addressUW + i << " " << setfill('0') << setw(2) << hex << (int)reg_data[i] << endl;
            cout << "readback " << setfill('0') << setw(8) << hex << rb_addressUW +  i << " " << setfill('0') << setw(2) << hex << (int)rb_data[i] << endl;
            
            error++;
        }
    }

    if(error==0){
        cout << dec << "OK" << endl;
    } else {
        cout<< dec << error << " errors found" << "NG!! Set VA Regisiter again. " << endl;
    }
    
    vaflag[3] = 0x00;
    rmapwrite32bits(spwaddress_vaflag, &vaflag[0]);
}

void SPMU001_VATARead::read_singleevent_forced()
{
    read_events(1, 1);
    send_current_status();
}

void SPMU001_VATARead::read_events_forced()
{
    int nevents =0;
    cli_read((std::string)"How many events:", &nevents);

    read_events(nevents, 1);
    send_current_status();
}

void SPMU001_VATARead::read_events_ta()
{
    int nevents =0;
    cli_read((std::string)"How many events:", &nevents);
    
    send_current_status();
    read_events(nevents, 0);
    send_current_status();
}


void SPMU001_VATARead::read_events_ta_time()
{
    int exposure =1;
    cli_read((std::string)"How long (min):", &exposure);
    
    exposure *= 60;
    send_current_status();
    read_events_time(exposure, 0);
    send_current_status();
}


void SPMU001_VATARead::read_events(int nevents, int forced_trig)
{
    unsigned int spwaddress_obsmodeflag  = 0x00000030 + spwaddress_offset;
    unsigned int spwaddress_forcetrigflag= 0x00000034 + spwaddress_offset;
    unsigned int spwaddress_vaflag       = 0x00000028 + spwaddress_offset;
    unsigned int spwaddress_modstatus    = 0x00000024 + spwaddress_offset;
    
    vector<unsigned char> vaflag;
    vector<unsigned char> obsmodeflag;
    vector<unsigned char> readstatus;
    vector<unsigned char> forcetrigflag;
    vaflag.resize(4, 0x00);
    readstatus.resize(4, 0x00);
    obsmodeflag.resize(4, 0x00);
    forcetrigflag.resize(4, 0x00);
    
    readdata.clear();
    readdata.resize(eventdatasize1);
    
    unsigned char eventdata[eventdatasize];
    eventdata[0] = 0xab;
    eventdata[1] = 0xcd;
    eventdata[2] = 0xef;
    eventdata[3] = 0x02;
    eventdata[eventdatasize-4] = 0xff;
    eventdata[eventdatasize-3] = 0xff;
    eventdata[eventdatasize-2] = 0x01;
    eventdata[eventdatasize-1] = 0x23;
    
    forcetrigflag[3] = 0x00;
    rmapwrite32bits(spwaddress_forcetrigflag, &forcetrigflag[0]);
    
    vaflag[3] = 0x03;
    rmapwrite32bits(spwaddress_vaflag, &vaflag[0]);
    
    int i;
    unsigned int cnt, flag;

    for(i=0;i<nevents; i++)
    {
        cnt = 0;
        flag = 0;
        obsmodeflag[3] = 0x01;
        rmapwrite32bits(spwaddress_obsmodeflag, &obsmodeflag[0]);
        do
        {
            rmapread(spwaddress_modstatus, (unsigned int)4, &readstatus[0]);
            
            if((readstatus[2] & 0x06))
            {
                flag = 1;
                break;
            }
            else if(forced_trig==1)
            {
                forcetrigflag[3] = 0x01;
                rmapwrite32bits(spwaddress_forcetrigflag, &forcetrigflag[0]);
            }
            cnt++;
            usleep2(10);
        }
        while(cnt < 100000);
        
        if(flag)
        {
            unixtime = time(NULL);
            memcpy(eventdata+eventdatasize1+4, &unixtime,  4);
            
            rmapread(spwaddress_eventdata+0*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+0*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+1*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+1*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+2*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+2*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+3*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+3*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+4*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+4*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+5*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+5*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+6*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+6*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+7*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+7*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+8*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+8*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+9*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+9*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+10*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+10*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+11*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+11*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+12*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+12*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+13*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+13*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+14*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+14*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(spwaddress_eventdata+15*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+15*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            
            
            filewrite(eventdata, eventdatasize);
            
            obsmodeflag[2] = 0x01;
            rmapwrite32bits(spwaddress_obsmodeflag, &obsmodeflag[0]);
            obsmodeflag[2] = 0x00;
            rmapwrite32bits(spwaddress_obsmodeflag, &obsmodeflag[0]);
            
        }
        else
        {
            cout << "Not Triggered!" << endl;
        }
        
        if(!(i%100)) cout << "Event : " << dec << i << " / " << nevents << endl;
    }
    
    obsmodeflag[3] = 0x00;
    rmapwrite32bits(spwaddress_obsmodeflag, &obsmodeflag[0]);
    vaflag[3] = 0x00;
    rmapwrite32bits(spwaddress_vaflag, &vaflag[0]);
}


void SPMU001_VATARead::read_events_time(int exposure, int forced_trig)
{
    unsigned int spwaddress_obsmodeflag  = 0x00000030 + spwaddress_offset;
    unsigned int spwaddress_forcetrigflag= 0x00000034 + spwaddress_offset;
    unsigned int spwaddress_vaflag       = 0x00000028 + spwaddress_offset;
    unsigned int spwaddress_modstatus    = 0x00000024 + spwaddress_offset;
    unsigned int spwaddress_wrtprt       = 0x000000d0 + spwaddress_offset;
    unsigned int spwaddress_wr_ptr_reset = 0x000000d4 + spwaddress_offset;
    
    vector<unsigned char> vaflag;
    vector<unsigned char> obsmodeflag;
    vector<unsigned char> readstatus;
    vector<unsigned char> forcetrigflag;
    vector<unsigned char> wr_ptr_reset;
    vaflag.resize(4, 0x00);
    readstatus.resize(4, 0x00);
    obsmodeflag.resize(4, 0x00);
    forcetrigflag.resize(4, 0x00);
    wr_ptr_reset.resize(4, 0x00);
    
    readdata.clear();
    readdata.resize(eventdatasize1);
    
    unsigned char eventdata[eventdatasize];
    eventdata[0] = 0xab;
    eventdata[1] = 0xcd;
    eventdata[2] = 0xef;
    eventdata[3] = 0x02;
    eventdata[eventdatasize-4] = 0xff;
    eventdata[eventdatasize-3] = 0xff;
    eventdata[eventdatasize-2] = 0x01;
    eventdata[eventdatasize-1] = 0x23;

    // edit by shirahama
    wr_ptr_reset[3] = 0x01;
    rmapwrite32bits(spwaddress_wr_ptr_reset, &wr_ptr_reset[0]);
    wr_ptr_reset[3] = 0x00;
    rmapwrite32bits(spwaddress_wr_ptr_reset, &wr_ptr_reset[0]);
    // upto here
    
    forcetrigflag[3] = 0x00;
    rmapwrite32bits(spwaddress_forcetrigflag, &forcetrigflag[0]);
    
    vaflag[3] = 0x03;
    rmapwrite32bits(spwaddress_vaflag, &vaflag[0]);
    
    int i;
    
    unsigned int cnt, flag;
    unsigned int flag2 = 0;
    unsigned int iframe, nframe;

    time_t starttime = time(NULL);
    
    unsigned int writeframe_ptr_last;

    writeframe_ptr = 0;
    write_ptr_address = rmapread32bits(spwaddress_wrtprt);
    writeframe_ptr = (writeframe_ptr & 0xFFFFF800) + (write_ptr_address >> 15); //( / 0x8000 32kbyte)
    readframe_ptr = writeframe_ptr;
    read_ptr_address = (readframe_ptr << 15) & 0x3FFFFFF ;

    if(forced_trig==1)
    {
        forcetrigflag[3] = 0x01;
        rmapwrite32bits(spwaddress_forcetrigflag, &forcetrigflag[0]);
    }
    
    obsmodeflag[3] = 0x01;
    rmapwrite32bits(spwaddress_obsmodeflag, &obsmodeflag[0]);

    while((starttime+exposure) > time(NULL))
    {
        time_from_start = time(NULL) -starttime;
        cnt = 0;
        nframe = 0;
        do
        {
            usleep2(100000);
            write_ptr_address = rmapread32bits(spwaddress_wrtprt);
            writeframe_ptr_last = writeframe_ptr;
            writeframe_ptr = (writeframe_ptr & 0xFFFFF800) + (write_ptr_address >> 15); //( / 0x8000 32kbyte)
            if(writeframe_ptr < writeframe_ptr_last)  writeframe_ptr += 2048;
            
            nframe = writeframe_ptr - readframe_ptr;

            if(nframe > 0) {
                cout << "roughly event rate : " << nframe*eventdatasize1/220/(cnt+1) << " events/s" << endl << endl;
                break;
            }

            cnt++;
        }
        while(cnt < 30);

        if(nframe ==0) cout << "Not stored frame data !" << endl;
        
        for(iframe=0;iframe<nframe;iframe++)
        {
            unixtime = time(NULL);
            memcpy(eventdata+eventdatasize1+4, &unixtime,  4);
            
            read_ptr_address = (readframe_ptr << 15) & 0x3FFFFFF ;
            
            rmapread(read_ptr_address+0*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+0*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+1*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+1*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+2*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+2*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+3*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+3*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+4*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+4*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+5*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+5*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+6*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+6*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+7*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+7*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+8*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+8*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+9*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+9*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+10*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+10*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+11*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+11*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+12*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+12*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+13*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+13*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+14*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+14*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            rmapread(read_ptr_address+15*eventdatasize1/16,eventdatasize1/16, &readdata[0]);
            memcpy(eventdata+4+15*eventdatasize1/16, &readdata[0], eventdatasize1/16);
            
            
            filewrite(eventdata, eventdatasize);
            
            readframe_ptr++;
        }

        if( (unixtime-starttime)%100 == 1 ) flag2 = 1;
        if( (unixtime-starttime)%100 == 0 && flag2 == 1)
        {
            flag2 = 0;
            cout << dec << (unixtime-starttime) << " / " << exposure << " sec " << endl;
        }
        
        
    }
    
    obsmodeflag[3] = 0x00;
    rmapwrite32bits(spwaddress_obsmodeflag, &obsmodeflag[0]);
    vaflag[3] = 0x00;
    rmapwrite32bits(spwaddress_vaflag, &vaflag[0]);
}





void SPMU001_VATARead::send_current_status()
{
    unsigned char statusdata[statusdatasize];
    statusdata[0] = 0xab;
    statusdata[1] = 0xcd;
    statusdata[2] = 0xef;
    statusdata[3] = 0x03;
    
    readdata.clear();
    readdata.resize(statusdatasize1);
    
    rmapread(spwaddress_statusdata1, statusdatasize1, &readdata[0]);
    memcpy(statusdata+4, &readdata[0], statusdatasize1);
    
    readdata.clear();
    readdata.resize(statusdatasize2/4);
    rmapread(spwaddress_statusdata2+0*statusdatasize2/4,statusdatasize2/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+0*statusdatasize2/4, &readdata[0], statusdatasize2/4);
    
    rmapread(spwaddress_statusdata2+1*statusdatasize2/4,statusdatasize2/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+1*statusdatasize2/4, &readdata[0], statusdatasize2/4);
    
    rmapread(spwaddress_statusdata2+2*statusdatasize2/4,statusdatasize2/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+2*statusdatasize2/4, &readdata[0], statusdatasize2/4);
    
    rmapread(spwaddress_statusdata2+3*statusdatasize2/4,statusdatasize2/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+3*statusdatasize2/4, &readdata[0], statusdatasize2/4);
    
    readdata.clear();
    readdata.resize(statusdatasize3/4);
    rmapread(spwaddress_statusdata3+0*statusdatasize3/4,statusdatasize3/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+statusdatasize2+0*statusdatasize3/4, &readdata[0], statusdatasize3/4);
    
    rmapread(spwaddress_statusdata3+1*statusdatasize3/4,statusdatasize3/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+statusdatasize2+1*statusdatasize3/4, &readdata[0], statusdatasize3/4);
    
    rmapread(spwaddress_statusdata3+2*statusdatasize3/4,statusdatasize3/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+statusdatasize2+2*statusdatasize3/4, &readdata[0], statusdatasize3/4);
    
    rmapread(spwaddress_statusdata3+3*statusdatasize3/4,statusdatasize3/4, &readdata[0]);
    memcpy(statusdata+4+statusdatasize1+statusdatasize2+3*statusdatasize3/4, &readdata[0], statusdatasize3/4);
    
    statusdata[statusdatasize-4] = 0xff;
    statusdata[statusdatasize-3] = 0xff;
    statusdata[statusdatasize-2] = 0x01;
    statusdata[statusdatasize-1] = 0x23;
    
    filewrite(statusdata, statusdatasize);
}


void SPMU001_VATARead::filewrite(unsigned char* buf, long size)
{
    if(!(filewriteid % numofeventsperfile))
    {
        if(fileseq != 0) delete fout;
        fileseq++;
        sprintf(charfileseq, "_%.3d", fileseq);
        filename = filenamebase + string(charseq) + string(charfileseq);

        fout = new ofstream(filename.c_str(), ios::out | ios::binary );
    }

    fout->write((const char*)buf, size);
    filewriteid++;
    nevent++;
}

std::vector<std::string> split(std::string str, char del) {
    int first = 0;
    int last = str.find_first_of(del);

    std::vector<std::string> result;

    while (first < str.size()) {
        std::string subStr(str, first, last - first);

        result.push_back(subStr);

        first = last + 1;
        last = str.find_first_of(del, first);

        if (last == std::string::npos) {
            last = str.size();
        }
    }

    return result;
}

std::vector<int> SPMU001_VATARead::return_path(std::string path_boost_tmp_str){

    while(path_boost_tmp_str.find(" ") != std::string::npos){
        int index_space = path_boost_tmp_str.find(" ");
        path_boost_tmp_str.erase(index_space,1);
    }

    path_boost_tmp_str.pop_back();
    path_boost_tmp_str.erase(path_boost_tmp_str.begin());

    std::vector<std::string> path_boost_tmp_str_vector;

    if(path_boost_tmp_str.find(",") == std::string::npos){
        path_boost_tmp_str_vector.push_back(path_boost_tmp_str);
    }   else{
        path_boost_tmp_str_vector = split(path_boost_tmp_str, ',');
    }

    std::vector<int> path_to_internal;

    for (const auto subStr : path_boost_tmp_str_vector) {
        path_to_internal.push_back(stoi(subStr));
    }
    
    return path_to_internal;
}

