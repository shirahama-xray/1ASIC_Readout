#ifndef SPMU001_VATARead_HH
#define SPMU001_VATARead_HH

#include <anlnext/BasicModule.hh>

#include "SpaceWire.hh"
#include "RMAP.hh"

#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include <unistd.h>
#include <sys/time.h>
#include <boost/property_tree/ptree.hpp>

using namespace std;

const unsigned int REG_BUF_SIZE = 4096;

std::vector<std::string> split(std::string, char);


class SPMU001_VATARead : public anlnext::BasicModule
{
    DEFINE_ANL_MODULE(SPMU001_VATARead, "5.0_DRAM");
public:
    SPMU001_VATARead();
    virtual ~SPMU001_VATARead();

    anlnext::ANLStatus mod_define() override;
    anlnext::ANLStatus mod_initialize() override;
    anlnext::ANLStatus mod_begin_run() override;
    anlnext::ANLStatus mod_analyze() override;
    anlnext::ANLStatus mod_end_run() override;
    anlnext::ANLStatus mod_finalize() override;
  
    void mod_display();

    long get_microsectime()
    {
        struct timeval tp;
        gettimeofday(&tp,NULL);
        return ((long)(tp.tv_sec) * 1000000 + tp.tv_usec);
    }
    void usleep2(useconds_t usleeptime)
    {
        struct timespec ts;
        ts.tv_sec = usleeptime/1000000;
        ts.tv_nsec = (usleeptime%1000000)*1000;
        nanosleep(&ts, NULL);
    }

    std::vector<int> return_path(std::string);
    
    
private:
    
    SpaceWireIF* spacewireif;
    RMAPEngine* rmapengine;
    RMAPInitiator* rmapinitiator;
    RMAPTargetNode* rmaptargetnode1;
    
    int slotnumber;
    string hostname;
    unsigned int tcp_portnumber;
    
    string spwboardname;
    unsigned char logicaladdress;
    unsigned char destinationkey;
    string crcversion;
    
    vector<unsigned char> path_to_target;
    vector<unsigned char> path_to_source;
    vector<int>           path_to_target_tmp;
    vector<int>           path_to_source_tmp;
    
    unsigned int rmapread32bits(unsigned int add);
    unsigned short rmapread32bitsupper(unsigned int add);
    unsigned short rmapread32bitslower(unsigned int add);
    
    void rmapwrite32bits(unsigned int add, unsigned char* writedata);
    
    void rmapread(unsigned int add, unsigned int size, unsigned char* readdata);
    void rmapwrite(unsigned int add, unsigned int size, unsigned char* writedata);
    
    double writeTimeoutDuration;
    double readTimeoutDuration;
    
    
    void printReadData(unsigned int address, vector<unsigned char>* data, unsigned int size);
    void config_fpga();
    void config_vareg_all();
    void config_vareg_part(int chips, int words);
    void config_vareg();
    void set_vareg();
    void load_readback_vareg();
    void read_singleevent_forced();
    void read_events_forced();
    void read_events_ta();
    void read_events_ta_time();
    
    void read_events(int nevents, int forced_trig);
    void read_events_time(int exposure, int forced_trig);
    
    void show_status();
    void send_current_status();
    void userfpga_init();
    void sdram_reset();
    
    vector<unsigned char> writedata;
    vector<unsigned char> readdata;
    
    unsigned int reg_buf[REG_BUF_SIZE];

    boost::property_tree::ptree pt;
    
    ofstream* fout;
    string config_filename;
    string filenamebase;
    int fileseq;
    int filewriteid;
    int read_seq_no;
    int numofeventsperfile;
    char charseq[16];
    char charfileseq[16];
    void filewrite(unsigned char* buf, long size);
    std::string filename;
    
    static const long eventdatasize1=32768; //16384;
    static const long eventdatasize=32780; // eventdatasize1+8+4(unixtime)
    
    unsigned int spwaddress_eventdata;
    unsigned int spwaddress_statusdata1;
    unsigned int spwaddress_statusdata2;
    unsigned int spwaddress_statusdata3;
    
    static const unsigned int statusdatasize1=224;
    static const unsigned int statusdatasize2=4096;
    static const unsigned int statusdatasize3=4096;
    static const long statusdatasize=8424; // statusdatasize1+statusdatasize2+statusdatasize3+8
    
    unsigned int spwaddress_offset;// = 0x00000000  or 0x00008000;
    static const unsigned int spwaddress0_statusdata1=0x00000020;
    static const unsigned int spwaddress0_statusdata2=0x00001000;
    static const unsigned int spwaddress0_statusdata3=0x00002000;
    static const unsigned int spwaddress0_eventdata=0x00003000;
    unsigned int spwaddress_dacmod;
    unsigned int spwaddress_dram;
    
    unsigned int readframe_ptr;
    unsigned int writeframe_ptr;
    unsigned int read_ptr_address;
    unsigned int write_ptr_address;

    ofstream fout_ql;
    int lastnevent;
    int eventid;
    int nevent;
    int lastnevent60[60];
    int qlnloop;
    int time_from_start;
    time_t unixtime;

    int path;

};




#endif //SPMU001_VATARead_HH
