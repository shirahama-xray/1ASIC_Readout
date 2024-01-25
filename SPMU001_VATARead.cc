#include <iostream>
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

#include <anlnext/CLIUtility.hh>
#include "SPMU001_VATARead.hh"

using namespace anlnext;

SPMU001_VATARead::SPMU001_VATARead()
{}

SPMU001_VATARead::~SPMU001_VATARead() = default;

ANLStatus SPMU001_VATARead::mod_define()
{
    spwaddress_offset = 0x44a20000;
    spwaddress_dacmod = 0x44a30000;
    spwaddress_dram = 0x00000000;
    
    readframe_ptr = 0;
    writeframe_ptr = 0;
    read_ptr_address = 0x00000000;
    write_ptr_address = 0x00000000;
  
    numofeventsperfile = 100000;
    read_seq_no = 0;
    fileseq =0;
    filewriteid = 0;
    nevent = 0;
    
    time_from_start = 0;
    qlnloop = 0;
    for(int i=0;i<60;i++) lastnevent60[i] = 0;
    
    writeTimeoutDuration = 1000.;
    readTimeoutDuration = 1000.;
    
    writedata.clear();

    define_parameter("config_filename",       &mod_class::config_filename);

    return AS_OK;
}

ANLStatus SPMU001_VATARead::mod_initialize()
{
    boost::property_tree::read_json(config_filename, pt);

    if(boost::optional<std::string> hostname_tmp = pt.get_optional<string>("connect.hostname")){
        hostname = hostname_tmp.get();
    }   else{
        std::cout << "No host name" << std::endl;
    } 

    if(boost::optional<unsigned int> tcp_portnumber_tmp = pt.get_optional<unsigned int>("connect.tcp_portnumber")){
        tcp_portnumber = tcp_portnumber_tmp.get();
    }   else{
        std::cout << "No TCP port number" << std::endl;
    } 

    if(boost::optional<std::string> spwboardname_tmp = pt.get_optional<string>("connect.spwboardname")){
        spwboardname = spwboardname_tmp.get();
    }   else{
        std::cout << "No spw board name" << std::endl;
    } 

    if(boost::optional<std::string> filenamebase_tmp = pt.get_optional<string>("connect.filenamebase")){
        filenamebase = filenamebase_tmp.get();
    }   else{
        std::cout << "No file name base" << std::endl;
    } 

    slotnumber = 1;
    
    if(spwboardname == "spmu001" || spwboardname == "spmu" )
    {
      logicaladdress = 0xfe;
      destinationkey = 0x02;
      crcversion = "draftF";
      spwaddress_offset = 0x44a20000;
    }
    
    spwaddress_eventdata   = spwaddress0_eventdata   + spwaddress_offset;
    spwaddress_statusdata1 = spwaddress0_statusdata1 + spwaddress_offset;
    spwaddress_statusdata2 = spwaddress0_statusdata2 + spwaddress_offset;
    spwaddress_statusdata3 = spwaddress0_statusdata3 + spwaddress_offset;
    
    //Target path
    if(boost::optional<std::string> target_boost_tmp = pt.get_optional<std::string>("connect.path_to_target")){
        std::string target_boost_tmp_str = target_boost_tmp.get();
        path_to_target_tmp = return_path(target_boost_tmp_str);
    }   else{
        std::cout <<  "No path to target" << std::endl;
        return AS_QUIT;
    } 

    for(int i=0;i<path_to_target_tmp.size();i++){
        int path = path_to_target_tmp[i];
        path_to_target.push_back((unsigned char)(0xff&path));
    }

    //Source path
    if(boost::optional<std::string> source_boost_tmp = pt.get_optional<std::string>("connect.path_to_source")){
        std::string source_boost_tmp_str = source_boost_tmp.get();
        path_to_source_tmp = return_path(source_boost_tmp_str);
    }   else{
        std::cout << "No path to source" << std::endl;
        return AS_QUIT;
    } 

    path_to_source.clear();

    for(int i=0;i<path_to_source_tmp.size();i++){
        int path = path_to_source_tmp[i];
        path_to_source.push_back((unsigned char)(0xff&path));
    }

    //Space wire
    spacewireif = new SpaceWireIFOverIPClient(hostname,tcp_portnumber);
    try {
        spacewireif->open();
    } catch (...) {
        cerr << "Connection timed out." << endl;
        exit(-1);
    }


    rmapengine=new RMAPEngine(spacewireif);
    rmapengine->start();

    rmaptargetnode1 =  new RMAPTargetNode();
    rmaptargetnode1->setTargetLogicalAddress(logicaladdress);
    rmaptargetnode1->setDefaultKey(destinationkey);

    rmaptargetnode1->setTargetSpaceWireAddress(path_to_target);

    rmaptargetnode1->setReplyAddress(path_to_source);

    rmapinitiator = new RMAPInitiator(rmapengine);
    rmapinitiator->setInitiatorLogicalAddress(0xfe);

    cout << rmaptargetnode1->toString() << endl;
    

    cout << "################################################" << endl;
    cout << "### SPMU 001 VATA Readout                    ###" << endl;
    cout << "###          VATA READ                       ###" << endl;
    cout << "################################################" << endl << endl;
    
    fout_ql.open("quicklook_log.txt", ios::app);

  
    return AS_OK;
}


ANLStatus SPMU001_VATARead::mod_begin_run()
{
    while(1)
    {
        cout << "Select : " << endl;
        cout << "  RMAP Write                [RW]" << endl;
        cout << "  RMAP Read                 [RR]" << endl;
        cout << "  Proceed                   [Q] " << endl;
      
        string selection;
        cli_read((std::string)"selection", &selection);

        if(selection == "Q" || selection == "q" )
        {
            break;
        }
        else if(selection == "RW" || selection == "rw")
        {
            int address=0;
            int val=0;

            cli_read((std::string)"Address(hex)", &address);
            cli_read((std::string)"value(dec)",   &val);

            unsigned int addressUW;
            addressUW = (unsigned int) address;
            writedata.clear();
            writedata.resize(4);
            writedata[3]=(unsigned char) ((val & 0x000000ff) >> 0);
            writedata[2]=(unsigned char) ((val & 0x0000ff00) >> 8);
            writedata[1]=(unsigned char) ((val & 0x00ff0000) >> 16);
            writedata[0]=(unsigned char) ((val & 0xff000000) >> 24);
            
            rmapwrite32bits(addressUW, &(writedata[0]));
        }
        else if(selection == "RR" || selection == "rr")
        {
            int address=0;
            int val=0;

            cli_read((std::string)"Address(hex)", &address);
            unsigned int addressUW;
            unsigned int readdata32bits;
            addressUW = (unsigned int) address;
            readdata32bits = rmapread32bits(addressUW);
            cout << "data : "
            << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0xff000000)>>24) << " "
            << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0x00ff0000)>>16) << " "
            << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0x0000ff00)>> 8) << " "
            << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0x000000ff)>> 0) << endl;
            cout << dec << endl;
        }
    }
    
    read_seq_no++;
    sprintf(charseq, "%.5d", read_seq_no);
    fileseq = 0;
    filewriteid = 0;
    
    send_current_status();

    return AS_OK;
}


ANLStatus SPMU001_VATARead::mod_analyze()
{

    cout << "Select : " << endl;
    cout << "  Config FPGA               [1]" << endl;
    cout << "  Config VA Register        [2]" << endl;
    cout << "  Set FPGA Reg. for VA Reg. [3]" << endl;
    cout << "  Load/Readback VA Register [4]" << endl;
    cout << "  Read (exposure)   (TA)    [A]" << endl;
    cout << "  Send Status               [C]" << endl;
    cout << "  File Change               [F]" << endl;
    cout << "  Sleep                     [SLEEP]" << endl;
    cout << "  RMAP Write                [7]" << endl;
    cout << "  RMAP Read                 [E]" << endl;
    cout << "  Show Status               [8]" << endl;
    cout << "  Quit                      [9]" << endl;
    
    string selection;

    cli_read((std::string)"selection", &selection);
    
    if(selection == "9")
    {
        return AS_QUIT;
    }
    else if(selection == "8")
    {
        show_status();
    }
    else if(selection == "1")
    {
        int notend = 1;

        while(notend==1){
            cout << "Select : " << endl;
            cout << "  Set all                  [1]"  << endl;
            cout << "  peaking time             [2]"  << endl;
            cout << "  conversion clock rate    [3]"  << endl;
            cout << "  readout out clock rate   [4]"  << endl;
            cout << "  enable flags             [5]"  << endl;
            cout << "  pseudo setting           [6]"  << endl;
            cout << "  Pulse injection          [7]"  << endl;
            cout << "  Disable period           [8]"  << endl;
            cout << "  latch timing             [9]" << endl;
            cout << "  DAC setting             [20]"  << endl;
            cout << "  return to main           [0]"  << endl;

            string selection_set;
            cli_read((std::string)"selection_set", &selection_set);

            if(selection_set == "1")
            {
                config_fpga();
            }   
            //Peaking time
            else if(selection_set == "2")
            {
                int peakingtime_p;
                int peakingtime_n;
                cout << "P-side (Pt-side) value ? [peaking time = X/100MHz] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"peakingtime_p", &peakingtime_p);
                cout << "N-side (Al-side) value ? [peaking time = X/100MHz] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"peakingtime_n", &peakingtime_n);
                cout << "peaking time P " << dec << peakingtime_p << endl;
                cout << "peaking time N " << dec << peakingtime_n << endl;

                unsigned int addressUW = 0x00000040;
                writedata[3]=(unsigned char) (peakingtime_p % 0x0100);
                writedata[2]=(unsigned char) (peakingtime_p / 0x0100);
                writedata[1]=(unsigned char) (peakingtime_n % 0x0100);
                writedata[0]=(unsigned char) (peakingtime_n / 0x0100);
                rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);
            }
            //conversion clock rate
            else if(selection_set == "3"){
                int convclkrate;
                cout << "value ? [conversion clk rate ] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"conversion_clk_rate", &convclkrate);
                cout << "conversion clk rate " << dec << convclkrate << endl;
                        
                unsigned int addressUW = 0x00000044;
                writedata[3]=(unsigned char) (convclkrate % 0x0100);
                writedata[2]=(unsigned char) (convclkrate / 0x0100);
                writedata[1]=0x00;
                writedata[0]=0x00;
                rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);
            }
            //readout clock rate
            else if(selection_set == "4"){
                int readclkrate;
                int readdelay;
                cout << "value ? [readout clk rate ] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"readout_clk_rate", &readclkrate);
                cout << "value ? [readout delay ] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"readout_delay", &readdelay);
                cout << "readout clk rate " << dec << readclkrate << endl;
                cout << "readout delay "    << dec << readdelay   << endl;

                unsigned int addressUW = 0x00000048;
                writedata[3]=(unsigned char) (readclkrate%0x0100);
                writedata[2]=(unsigned char) (readdelay%0x0100);
                writedata[1]=0x00;
                writedata[0]=0x00;
                rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);
            }
            //enable flags
            else if(selection_set == "5"){
            }
            else if(selection_set == "6"){
                int pseudo_onoff;
                int pseudo_rate;
                cout << "Pseudo on off" << endl;
                cout << ">>";
                cli_read((std::string)"pseudo_onoff", &pseudo_onoff);
                cout << "Pseudo rate [1:2Hz, 2:4Hz, -- 10:20Hz, 100:211Hz, 4096: 2110Hz, else 20Hz]: " << endl;
                cout << ">>";
                cli_read((std::string)"pseudo_rate", &pseudo_rate);

                unsigned int addressUW = 0x000000c0;
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
            }
            else if(selection_set == "7"){
                int caldtrigreq;
                int pulsewidth;
                int pulsewidthveto;
                cout << "Inject cal pulse ? (0/1)" << endl;
                cout << ">>";
                cli_read((std::string)"caldtrigreq",    &caldtrigreq);
                cout << "Pulse width value ? [pulse width =X/100MHz] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"pulsewidth",     &pulsewidth);
                cout << "Pulse width veto value ? [pulse width veto =X/100MHz] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"pulsewidthveto", &pulsewidthveto);

                cout << "caldtrigreq "    << dec << caldtrigreq    << endl;
                cout << "pulsewidth "     << dec << pulsewidth     << endl;
                cout << "pulsewidthveto " << dec << pulsewidthveto << endl;

                unsigned int addressUW = 0x00000070;
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
            }
            else if(selection_set == "8"){
                int disable_period = 10000;
                cout << "Disable period ? [disable_period =X/100MHz] (Dec)" << endl;
                cout << ">>";
                cli_read((std::string)"disable_period",    &disable_period);

                unsigned int addressUW = 0x0000007c;
                writedata[3]=(unsigned char) ((disable_period & 0x000000ff) >> 0);
                writedata[2]=(unsigned char) ((disable_period & 0x0000ff00) >> 8);
                writedata[1]=(unsigned char) ((disable_period & 0x00ff0000) >> 16);
                writedata[0]=(unsigned char) ((disable_period & 0xff000000) >> 24);
                rmapwrite32bits(addressUW + spwaddress_offset, &writedata[0]);
            }
            else if(selection_set == "9"){
                //		CLintrd((char*)"fastbgo_latch_timing", &fastbgo_latch_timing);
                //		CLintrd((char*)"hitpat_latch_timing", &hitpat_latch_timing);
                int trigpat_latch_timing = 50;
                cout << "trig_pat_latch_timing ? " << endl;
                cout << ">>";
                cli_read((std::string)"trig_pat_latch_timing",    &trigpat_latch_timing);

                int resetwait_time = 3000;
                cout << "resetwait_time ? " << endl;
                cout << ">>";
                cli_read((std::string)"resetwait_time",    &resetwait_time);

                int resetwait_time2 = 500;
                cout << "resetwait_time2 ?" << endl;
                cout << ">>";
                cli_read((std::string)"resetwait_time2",    &resetwait_time2);

                //addressUW = 0x0000004c;
                //writedata[3]=(unsigned char) (fastbgo_latch_timing%0x0100);
                //writedata[2]=(unsigned char) (fastbgo_latch_timing/0x0100);
                //writedata[1]=0x00;
                //writedata[0]=0x00;
                //		rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
                
                //addressUW = 0x00000050;
                //writedata[3]=(unsigned char) (hitpat_latch_timing%0x0100);
                //writedata[2]=(unsigned char) (hitpat_latch_timing/0x0100);
                //writedata[1]=0x00;
                //writedata[0]=0x00;
                //		rmapwrite32bits(addressUW + spwaddress_offset,&writedata[0]);
                
                unsigned int addressUW = 0x00000054;
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

                break;
            }
            else if(selection_set == "20")
            {
                int dac_exec    = 0;
                int dac_set_val = 0;
                cli_read((std::string)"DAC exec[1:on, 0:only set]", &dac_exec);

                unsigned int addressUW;

                if(dac_exec == 0)
                {
                    cli_read((std::string)"DAC val : [12bit: 0-4095]", &dac_set_val);
                    addressUW = 0x00000004;
                    writedata[3]=(unsigned char)(dac_set_val%0x0100);
                    writedata[2]=(unsigned char)(dac_set_val/0x0100);
                    writedata[1]=0x00;
                    writedata[0]=0x00;
                    rmapwrite32bits(addressUW + spwaddress_dacmod, &writedata[0]);
                }
                else if(dac_exec == 1)
                {
                    addressUW = 0x00000000;
                    writedata[3]=0x01;
                    writedata[2]=0x00;
                    writedata[1]=0x00;
                    writedata[0]=0x00;
                    rmapwrite32bits(addressUW + spwaddress_dacmod, &writedata[0]);
                }
            }
            else if(selection_set == "0"){
                notend = 0;
            }
        }
    }
    else if(selection == "2")
    {
        config_vareg();
    }
    else if(selection == "3")
    {
        set_vareg();
    }
    else if(selection == "4")
    {
        load_readback_vareg();
    }
    else if(selection == "5")
    {}
    else if(selection == "6")
    {}
    else if(selection == "A" || selection == "a")
    {
        read_events_ta_time();
    }
    else if(selection == "B" || selection == "b")
    {}
    else if(selection == "C" || selection == "c")
    {
        send_current_status();
    }
    else if(selection == "F" || selection == "f")
    {
        mod_end_run();
        read_seq_no++;
        sprintf(charseq, "%.5d", read_seq_no);
        fileseq = 0;
        filewriteid = 0;
        
        send_current_status();
    }
    else if(selection == "7")
    {
        string address;
        cli_read((std::string)"Address(hex)", &address);

        int val=0;
        cli_read((std::string)"value (dec)", &val);
        
        cout << "TEST ADDRESS: " << address << endl;
        cout << "TEST VAL: " << val << endl;
        
        std::istringstream iss(address);
        unsigned int addressUW;
        iss >> std::hex >> addressUW; 
        cout << "TTEESSTT: " << hex << addressUW << endl;
        writedata.clear();
        writedata.resize(4);

        vector<unsigned int> writedata2(4);
        vector<unsigned char> writedata3(4);
        cout << "TTTEEESSSTTT000: " <<         ((unsigned char)(val & 0x000000ff) >> 0) 
                                    << ", " << ((unsigned char)(val & 0x0000ff00) >> 8)  
                                    << ", " << ((unsigned char)(val & 0x00ff0000) >> 16 ) 
                                    << ", " << ((unsigned char)(val & 0xff000000) >> 24) 
                                    << endl;
        unsigned int writedata01 = (((unsigned char)(val & 0x000000ff)) >> 0 );
        int writedata02 = (((unsigned char)(val & 0x0000ff00)) >> 8 );
        unsigned int writedata03 = (((unsigned char)(val & 0x00ff0000)) >> 16);

        writedata2[3]=((unsigned int) ((val & 0x000000ff) >> 0) );
        writedata2[2]=((unsigned int) ((val & 0x0000ff00) >> 8) );
        writedata2[1]=((unsigned int) ((val & 0x00ff0000) >> 16));
        writedata2[0]=((unsigned int) ((val & 0xff000000) >> 24));

        writedata3[3]=static_cast<unsigned int> (writedata2[3]);
        writedata3[2]=static_cast<unsigned int> (writedata2[2]);
        writedata3[1]=static_cast<unsigned int> (writedata2[1]);
        writedata3[0]=static_cast<unsigned int> (writedata2[0]);

        writedata[3]=(unsigned char) ((val & 0x000000ff) >> 0);
        writedata[2]=(unsigned char) ((val & 0x0000ff00) >> 8);
        writedata[1]=(unsigned char) ((val & 0x00ff0000) >> 16);
        writedata[0]=(unsigned char) ((val & 0xff000000) >> 24);
        cout << "TTTEEESSSTTT: " << writedata01 << ", " << writedata02 << ", " <<  writedata03  << endl;

        cout << "TTTEEESSSTTT: " << hex << writedata2[3] << ", " << writedata2[2] << ", " <<  writedata2[1] << ", " << writedata2[0] << endl;
        cout << "TTTEEESSSTTT: " << hex << writedata3[3] << ", " << writedata3[2] << ", " <<  writedata3[1] << ", " << writedata3[0] << endl;

        cout << "TTTEEESSSTTT: " << hex << writedata[3] << ", " << writedata[2] << ", " <<  writedata[1] << ", " << writedata[0] << endl;
        cout << "TTTEEESSSTTT: " << (val & 0x000000ff) << ", " << (val & 0x0000ff00)  << ", " << (val & 0x00ff0000)  << ", " << (val & 0xff000000)  << endl;
        cout << "TTTEEESSSTTT: " << ((val & 0x000000ff) >> 0) << ", " << ((val & 0x0000ff00) >> 8)  << ", " << ((val & 0x00ff0000) >> 16 ) << ", " << ((val & 0xff000000) >> 24) << endl;
        
        rmapwrite32bits(addressUW, &writedata[0]);
        /*
                unsigned int addressUW = 0x00000070;
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
        */
    }
    else if(selection == "E" || selection == "e")
    {
        // edit by shirahma
        // int address=0;
        string address;
        int val=0;
        cli_read((std::string)"Address(hex)", &address);

        std::istringstream iss(address);
        unsigned int addressUW;
        cout << "GOOD: " << address << endl;
        iss >> std::hex >> addressUW; 

        cout << "TEST: " << addressUW << ", " << spwaddress_offset << endl;
        unsigned int readdata32bits;
        readdata32bits = rmapread32bits(addressUW);
        cout << readdata32bits << endl;
        cout << "data : "
        << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0xff000000)>>24) << " "
        << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0x00ff0000)>>16) << " "
        << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0x0000ff00)>> 8) << " "
        << setfill('0') << setw(2)  << hex << (unsigned int)((readdata32bits & 0x000000ff)>> 0) << endl;
        cout << dec << endl;
    }
    else if(selection =="SLEEP" || selection =="sleep")
    {
        sleep(5);
    }
    
    return AS_OK;
}

ANLStatus SPMU001_VATARead::mod_end_run()
{
    if(fout) delete fout;

    return AS_OK;
}

ANLStatus SPMU001_VATARead::mod_finalize()
{
    delete rmapengine;
    delete rmapinitiator;
    delete spacewireif;

    return AS_OK;
}

void SPMU001_VATARead::mod_display()
{
    int tmpnevent;
    int usedsize;
    int rate = tmpnevent - lastnevent;
    int rate60 = tmpnevent - lastnevent60[qlnloop%60];
    
    fout_ql << setw(16) << filename <<" , "<< setw(12) << unixtime <<" , " << setw(6) << nevent << " , " << setw(6) << time_from_start << " sec," << " rate: " << setw(4) << rate << " , r60: " << setw(5) << rate60 << " , rfptr: " << setw(6) << readframe_ptr << " , wfptr: " << setw(6) << writeframe_ptr <<endl;
    
    lastnevent = tmpnevent;
    lastnevent60[qlnloop%60] = tmpnevent;
    qlnloop++;
}



void SPMU001_VATARead::sdram_reset()
{
    unsigned int address = 0x00000000;
    writedata.clear();
    writedata.resize(4);
    
    writedata[3]=(unsigned char)0x00;
    writedata[2]=(unsigned char)0x02;
    writedata[1]=(unsigned char)0x02;
    writedata[0]=(unsigned char)0xa5;
    
    rmapwrite32bits(address, &(writedata[0]));
    
    writedata.clear();
    writedata.resize(4);
    
    writedata[3]=(unsigned char)0xff;
    writedata[2]=(unsigned char)0xff;
    writedata[1]=(unsigned char)0xff;
    writedata[0]=(unsigned char)0xa5;
    
    rmapwrite32bits(address, &(writedata[0]));
    
    writedata.clear();
    writedata.resize(4);
    address = 0x000000ac;
    
    writedata[3]=(unsigned char)0x01;
    writedata[2]=(unsigned char)0x00;
    writedata[1]=(unsigned char)0x00;
    writedata[0]=(unsigned char)0x00;
    
    rmapwrite32bits(address, &(writedata[0]));
    sleep(8);
    
    address = 0x000000ac;
    
    unsigned int flag;
    int i;
    unsigned int readdata32bits;
    
    
    for(i=0;i<10;i++)
    {
        flag = rmapread32bits(address);
        cout << "data : "
        << setfill('0') << setw(8) << hex << address << " "   << flag << dec << endl;
        if(flag & 0x00000002)
        {
            return;
        }
        else
        {
            sleep(5);
        }
    }
}

void SPMU001_VATARead::userfpga_init()
{
    unsigned int address = 0x00000000;
    
    writedata.clear();
    writedata.resize(4);
    
    writedata[3]=(unsigned char)0xff;
    writedata[2]=(unsigned char)0x02;
    writedata[1]=(unsigned char)0x02;
    writedata[0]=(unsigned char)0xa5;
    
    rmapwrite32bits(address, &(writedata[0]));
}



unsigned int SPMU001_VATARead::rmapread32bits(unsigned int add){
    unsigned char readdata_raw[4];
    for(int ijk=0;ijk<20;ijk++)
    {
        try
        {
            rmapinitiator->read(rmaptargetnode1, add, (unsigned int)4, readdata_raw, readTimeoutDuration);
            break;
        }
        catch(RMAPInitiatorException e)
        {
            cerr << "RMAPInitiatorException " << e.toString() << endl;
            cerr << " rmapread32bits() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPReplyException e)
        {
            cerr << "RMAPReplyException " << e.toString() << endl;
            cerr << " rmapread32bits() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPEngineException e)
        {
            cerr << "RMAPEngineException " << e.toString() << endl;
            cerr << " rmapread32bits() " <<  ijk+1 << " trial " << endl;
        }
        catch(...)
        {
            cerr << "rmapread32bits():: unknown error " << endl;
            exit(-1);
        }
    }
    
    return ((unsigned int)(readdata_raw[0])*0x01000000 + (unsigned int)(readdata_raw[1])*0x00010000
            + (unsigned int)(readdata_raw[2])*0x00000100 + (unsigned int)(readdata_raw[3])*0x00000001);
}

unsigned short SPMU001_VATARead::rmapread32bitsupper(unsigned int add)
{
    return (unsigned short)((rmapread32bits(add) & 0xffff0000) >> 16);
}

unsigned short SPMU001_VATARead::rmapread32bitslower(unsigned int add)
{
    return (unsigned short)(rmapread32bits(add) & 0x0000ffff);
}

void SPMU001_VATARead::rmapwrite32bits(unsigned int add, unsigned char* writedata_raw)
{
    for(int ijk=0;ijk<20;ijk++)
    {
        try
        {
            rmapinitiator->write(rmaptargetnode1, add, writedata_raw, (unsigned int)4, writeTimeoutDuration);
            
            break;
        }
        catch(RMAPInitiatorException e)
        {
            cerr << "RMAPInitiatorException " << e.toString() << endl;
            cerr << " rmapwrite32bits() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPReplyException e)
        {
            cerr << "RMAPReplyException " << e.toString() << endl;
            cerr << " rmapwrite32bits() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPEngineException e)
        {
            cerr << "RMAPEngineException " << e.toString() << endl;
            cerr << " rmapwrite32bits() " <<  ijk+1 << " trial " << endl;
        }
        catch(...)
        {
            cerr << "rmapwrite32bits():: unknown error " << endl;
            exit(-1);
        }
    }
}



void SPMU001_VATARead::rmapread(unsigned int add, unsigned int size, unsigned char* readdata_raw)
{
    for(int ijk=0;ijk<20;ijk++)
    {
        try
        {
            rmapinitiator->read(rmaptargetnode1, add, size, readdata_raw, readTimeoutDuration);
            
            break;
        }
        catch(RMAPInitiatorException e)
        {
            cerr << "RMAPInitiatorException " << e.toString() << endl;
            cerr << " rmapread() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPReplyException e)
        {
            cerr << "RMAPReplyException " << e.toString() << endl;
            cerr << " rmapread() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPEngineException e)
        {
            cerr << "RMAPEngineException " << e.toString() << endl;
            cerr << " rmapread() " <<  ijk+1 << " trial " << endl;
        }
        catch(...)
        {
            cerr << "rmapread():: unknown error " << endl;
            exit(-1);
        }
    }
}

void SPMU001_VATARead::rmapwrite(unsigned int add, unsigned int size, unsigned char* writedata_raw)
{
    for(int ijk=0;ijk<20;ijk++)
    {
        try
        {
            rmapinitiator->write(rmaptargetnode1, add, writedata_raw, size, writeTimeoutDuration);
            break;
        }
        catch(RMAPInitiatorException e)
        {
            cerr << "RMAPInitiatorException " << e.toString() << endl;
            cerr << " rmapwrite() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPReplyException e)
        {
            cerr << "RMAPReplyException " << e.toString() << endl;
            cerr << " rmapwrite() " <<  ijk+1 << " trial " << endl;
        }
        catch(RMAPEngineException e)
        {
            cerr << "RMAPEngineException " << e.toString() << endl;
            cerr << " rmapwrite() " <<  ijk+1 << " trial " << endl;
        }
        catch(...)
        {
            cerr << "rmapwrite():: unknown error " << endl;
            exit(-1);
        }
    }
}
