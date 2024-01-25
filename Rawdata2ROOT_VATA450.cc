#include <iostream>
#include <fstream>
#include <anlnext/CLIUtility.hh>

#include "Rawdata2ROOT_VATA450.hh"

using namespace anlnext;


Rawdata2ROOT_VATA450::Rawdata2ROOT_VATA450()
{}

Rawdata2ROOT_VATA450::~Rawdata2ROOT_VATA450() = default;

ANLStatus Rawdata2ROOT_VATA450::mod_define()
{
    eventid = 0;
    infilename = "test";

    numofdaisychains = 1;
    numofasicperdaisychains = 1;
    int i;
    for(i=0;i<sizeofeventdatabit;i++)
    {
        asicdatabits[i] = 0;
    }

    define_parameter("infilename",  &mod_class::infilename);
    //register_parameter(&infilename, "infilename");

    define_parameter("num_daisy_chains",          &mod_class::numofdaisychains);
    define_parameter("num_asic_per_daisy_chains", &mod_class::numofasicperdaisychains);

    return AS_OK;
}


ANLStatus Rawdata2ROOT_VATA450::mod_initialize()
{
    numofallch = noofch * numofasicperdaisychains * numofdaisychains;
    numofalladc = numofdaisychains * numofasicperdaisychains;

    outfilename = infilename + std::string(".root");
    outfile   = new TFile(outfilename.c_str(), "recreate");
    eventtree = new TTree("eventtree", "eventtree");
    
    char name1_chflag[32];
    char name1_cmn[32];
    char name1_adc[32];
    char name1_index[32];
    char name1_ref[32];
    char name1_hitnum[32];
    char name1_cmn_ex[32];
    char name2_chflag[32];
    char name2_cmn[32];
    char name2_adc[32];
    char name2_index[32];
    char name2_ref[32];
    char name2_hitnum[32];
    char name2_cmn_ex[32];
    
    for (int i = 0; i < numofalladc; i++) {
        
        sprintf(name1_chflag, "chflag%d", i);
        sprintf(name1_cmn,    "cmn%d", i);
        sprintf(name1_adc,    "adc%d", i);
        sprintf(name1_index,  "index%d", i);
        sprintf(name1_ref,    "ref%d", i);
        sprintf(name1_hitnum, "hitnum%d", i);
        sprintf(name1_cmn_ex, "cmn%d_ex", i);
        
        sprintf(name2_chflag, "chflag%d[3]/i", i);
        sprintf(name2_cmn,    "cmn%d/s", i);
        sprintf(name2_adc,    "adc%d[hitnum%d]/s", i, i);
        sprintf(name2_index,  "index%d[hitnum%d]/s", i, i);
        sprintf(name2_ref,    "ref%d/s", i);
        sprintf(name2_hitnum, "hitnum%d/s", i);
        sprintf(name2_cmn_ex, "cmn%d_ex/D", i);
        
        eventtree->Branch(name1_hitnum, &array_hitnum[i], name2_hitnum);
        eventtree->Branch(name1_chflag, array_chflag[i], name2_chflag);
        eventtree->Branch(name1_cmn, &array_cmn[i], name2_cmn);
        eventtree->Branch(name1_adc, array_adc[i], name2_adc);
        eventtree->Branch(name1_index, array_index[i], name2_index);
        eventtree->Branch(name1_ref, &array_ref[i], name2_ref);
        eventtree->Branch(name1_cmn_ex, &array_cmn_ex[i], name2_cmn_ex);
    }
    
    
    eventtree->Branch("ti",                   &ti,                   "ti/i");
    eventtree->Branch("livetime",             &livetime,             "livetime/i");
    eventtree->Branch("hit_cnt",              &hit_cnt,              "hit_cnt/i");
    eventtree->Branch("integral_livetime",    &integral_livetime,    "integral_livetime/i");
    eventtree->Branch("trighitpat",           &trighitpat,           "trighitpat/i");
    eventtree->Branch("time_from_exttrigger", &time_from_exttrigger, "time_from_exttrigger/i");
    eventtree->Branch("hitid",                &hitid,                "hitid/i");
    eventtree->Branch("trig_flag",            &trig_flag,            "trig_flag/s");
    eventtree->Branch("unixtime",             &unixtime,             "unixtime/i");
    eventtree->Branch("adcclkcnt",            &adcclkcnt,            "adcclkcnt/s");
    
    eventtree->Branch("ti_upper",       &ti_upper,       "ti_upper/i");
    eventtree->Branch("ti_lower",       &ti_lower,       "ti_lower/i");
    eventtree->Branch("ext1ti_upper",   &ext1ti_upper,   "ext1ti_upper/i");
    eventtree->Branch("ext1ti_lower",   &ext1ti_lower,   "ext1ti_lower/i");
    eventtree->Branch("ext2ti_upper",   &ext2ti_upper,   "ext2ti_upper/i");
    eventtree->Branch("ext2ti_lower",   &ext2ti_lower,   "ext2ti_lower/i");
    eventtree->Branch("pseudo_counter", &pseudo_counter, "pseudo_counter/i");
    eventtree->Branch("flag_pseudo",    &flag_pseudo,    "flag_pseudo/s");
    eventtree->Branch("flag_forcetrig", &flag_forcetrig, "flag_forcetrig/s");
    eventtree->Branch("flag_bgo",       &flag_bgo,       "flag_bgo/s");
    eventtree->Branch("flag_trig",      &flag_trig,      "flag_trig/s");

    
    hktree = new TTree("hktree","hktree");
    hktree->Branch("eventid",     &eventid,     "eventid/i");
    hktree->Branch("hk_readsize", &hk_readsize, "hk_readsize/i");
    hktree->Branch("hkraw",       hkraw,        "hkraw[hk_readsize]/i");
    
    
    char name[32];
    int i;
    for(i=0;i<numofallch;i++)
    {
        sprintf(name,"hist%.3d",i);
        hist[i] = new TH1D(name, name, 1024, -0.5, 1023.5);
        sprintf(name,"hist_cmn%.3d",i);
        hist_cmn[i] = new TH1D(name, name, 1024, -50.5, 1024.-50.5);
        hist_cmn[i]->SetLineColor(2);
        sprintf(name,"hist_cmn_ex%.3d",i);
        hist_cmn_ex[i] = new TH1D(name, name, 1024, -50.5, 1024.-50.5);
        hist_cmn_ex[i]->SetLineColor(3);
    }
    
    histref = new TH1D("histref", "histref", 1024, -0.5, 1023.5);
    histall = new TH2D("histall","histall", numofallch, -0.5, -0.5+(double)numofallch, 1024, -0.5, 1023.5);
    histall_cmn = new TH2D("histall_cmn","histall_cmn", numofallch, -0.5, -0.5+(double)numofallch, 1024, -50.5, 1024.0-50.5);
    histall_cmn_ex = new TH2D("histall_cmn_ex","histall_cmn_ex", numofallch, -0.5, -0.5+(double)numofallch, 1024, -50.5, 1024.0-50.5);
    histall2 = new TH2D("histall2","histall2", numofallch/2, -0.5, -0.5+(double)(numofallch/2), 1024, -0.5, 1023.5);
    histall2_cmn = new TH2D("histall2_cmn","histall2_cmn", numofallch/2, -0.5, -0.5+(double)(numofallch/2), 1024, -50.5, 1024.0-50.5);
    histall2_cmn_ex = new TH2D("histall2_cmn_ex","histall2_cmn_ex", numofallch/2, -0.5, -0.5+(double)(numofallch/2), 1024, -50.5, 1024.0-50.5);
    
    return AS_OK;
}

ANLStatus Rawdata2ROOT_VATA450::mod_begin_run()
{
    return AS_OK;
}

ANLStatus Rawdata2ROOT_VATA450::mod_analyze()
{
    std::ifstream inputfile(infilename.c_str(), std::ios::in|std::ios::binary);
    
    int nword;
    int amari;
    int i,j, k;
    int ibit;
    int l;
    int offset = 0;
    int isize = 0;
    int iasic;
    int idchain;
    int framecount = 0;
    
    while(!inputfile.eof())
    {
        framedataok = false;
        unixtime = 0;
        inputfile.read((char*)&tmpintdata, sizeof(int));
        if( (tmpintdata & 0x00FFFFFF) != 0x00efcdab ) continue;
        tmpdata = (tmpintdata & 0xFF000000) >> 24;
        
        //status data
        if(tmpdata == 0x03)
        {
            i=0;
            while(!inputfile.eof())
            {
                inputfile.read((char*)data, sizeof(int));
                if(data[0] == 0x2301FFFF)
                {
                    break;
                }
                else
                {
                    hkraw[i] = ((data[0]&0xff000000)>>24)
                    + ((data[0]&0x00ff0000)>>8)
                    + ((data[0]&0x0000ff00)<<8)
                    + ((data[0]&0x000000ff)<<24);
                    i++;
                }
            }
            
            std::cout << "read hk packet" << std::endl;
            std::cout << std::dec << "size: " << i << " word" << std::endl;
            std::cout << std::dec << std::endl;
            
            hk_readsize = i;
            hktree->Fill();
            framecount++;
            
        }
        //event data
        else if(tmpdata == 0x02)
        {
            inputfile.read((char*)framedata, framesize);
            if( !((framedata[framewordsize-1] == 0x2301FFFF)
                  ))
            {
                std::cout << "error in reading frame data packet" << std::endl;
                std::cout << std::hex << framedata[framewordsize-1] << std::endl;
                std::cout << std::dec << std::endl;
            }
            else
            {
                unixtime =  framedata[framewordsize-2] ;
                framedataok = true;
            }
        }
        
        if(framedataok)
        {
            j=0;
            framecount++;
            do
            {
                while( (framedata[j] & 0x0000FFFF) != 0x00003c3c && j<framewordsize)
                {
                    j++;
                }
                i=0;
                offset = j;
                
                while(framedata[j] != 0x77770000 && i<eventsize && j<framewordsize)
                {
                    eventdata[i] = ((framedata[j]&0xff000000)>>24)
                    + ((framedata[j]&0x00ff0000)>>8)
                    + ((framedata[j]&0x0000ff00)<<8)
                    + ((framedata[j]&0x000000ff)<<24);
                    j++;
                    i++;
                }
                isize = i;
                
                if(i >= eventsize)
                {
                    j = offset +1;
                }
                else if(j<framewordsize)
                {
                    ti = eventdata[1];
                    livetime = eventdata[2];
                    //hit_cnt = eventdata[2];
                    integral_livetime = (eventdata[3] & 0xffff0000) >>16;
                    flag_pseudo = (eventdata[3] & 0x00000003);
                    flag_forcetrig = (eventdata[3] & 0x00000004) >> 2;
                    flag_bgo = (eventdata[3] & 0x0000003c) >> 2;
                    flag_trig = (eventdata[3] & 0x00000078) >> 3;
                    //trighitpat = eventdata[4];
                    //time_from_exttrigger = eventdata[4];
                    hitid     = eventdata[4];
                    trig_flag = (eventdata[5] & 0x0001);
                    adcclkcnt = 0;
                    
                    pseudo_counter = eventdata[5];
                    ti_upper = 0;
                    ti_lower = 0;
                    ext1ti_upper = 0;
                    ext1ti_lower = 0;
                    ext2ti_upper = 0;
                    ext2ti_lower = 0;
                    
                    
                    if(eventid ==1)
                    {
                        std::cout << "ti : " << ti << std::endl;
                        std::cout << "livetime : " << livetime << std::endl;
                        std::cout << "integral_livetime : " << integral_livetime << std::endl;
                        //std::cout << "trighitpat : " << trighitpat << std::endl;
                    }
                    
                    for(i=0;i<sizeofeventdatabit;i++)
                    {
                        asicdatabits[i] = 0;
                    }
                    
                    for(ibit=0;ibit< 32*(isize-6);ibit++)
                    {
                        amari = ibit%32;
                        nword = 6+(ibit-amari)/32;
                        asicdatabits[ibit] = (eventdata[nword] >> (31-amari)) & 0x00000001;
                    }
                    
                    int bitoffset = 0;
                    for(idchain=0; idchain<numofdaisychains; idchain++)
                    {
                        for(iasic=0;iasic<numofasicperdaisychains;iasic++)
                        {
                            unsigned short adc[noofch]={0};
                            unsigned short ref=0;
                            unsigned short cmn=0;
                            unsigned int chflag[3]={0,0,0};
                            unsigned short index[noofch] = {0};
                            int hitnum = 0;
                            chflag[0] = (asicdatabits[0+bitoffset] << 31)
                            + (asicdatabits[1+bitoffset]<<30)
                            + (asicdatabits[2+bitoffset]<<29)
                            + (asicdatabits[3+bitoffset]<<28);
                            
                            if(asicdatabits[1+bitoffset] == 0)
                            {
                                chflag[0] += (asicdatabits[4+bitoffset]<< 27);
                                bitoffset += 5;
                            }
                            else
                            {
                                chflag[0] += (asicdatabits[4+bitoffset]<<1);
                                chflag[1] =0; chflag[2] =0;
                                for(l=0;l<noofch;l++)
                                {
                                    if(l<32)
                                    {
                                        chflag[1] += (asicdatabits[5+l+bitoffset] << (31-l));
                                    }
                                    else
                                    {
                                        chflag[2] += (asicdatabits[5+l+bitoffset] << (63-l));
                                    }
                                    
                                    if(asicdatabits[5+l+bitoffset]==1)
                                    {
                                        index[hitnum] = l;
                                        hitnum++;
                                    }
                                }
                                
                                chflag[0] += (asicdatabits[69+bitoffset]);
                                for(l=0;l<10;l++)
                                {
                                    ref += (asicdatabits[70+l+bitoffset] << l);
                                    for(i=0;i<hitnum;i++)
                                    {
                                        adc[i] += (asicdatabits[80+10*i+l+bitoffset] << l);
                                    }
                                    cmn += (asicdatabits[80+10*hitnum+l+bitoffset] << l);
                                }
                                chflag[0] += (asicdatabits[80+10*hitnum+10+bitoffset]<< 27);
                                bitoffset += (80+10*hitnum+10+1);
                            }
                            
                            k= iasic + idchain * numofasicperdaisychains;
                            
                            if( !(eventid % 1000) && k==0)
                            {
                                std::cout << "eventid :" << std::dec << eventid << std::endl;
                            }
                            
                            
                            array_chflag[k][0] = chflag[0];
                            array_chflag[k][1] = chflag[1];
                            array_chflag[k][2] = chflag[2];
                            array_cmn_ex[k] = TMath::Median((Long64_t) hitnum, (const short*) adc);
                            
                            for (i = 0; i < noofch; i++) {
                                array_adc[k][i] = adc[i];
                                array_index[k][i] = index[i];
                                
                                if (eventid == 1) {
                                    std::cout << "iasic:" << iasic << " idchain:" << idchain << " k:" << k << " i:" << i << " index[i]:"
                                    << index[i] << " adc[i]:" << adc[i] << std::endl;
                                }
                                
                                if((i==0 && hitnum != 0)|| index[i]!=0){
                                    hist[index[i] + k * noofch]->Fill(adc[i]);
                                    hist_cmn[index[i] + k * noofch]->Fill(adc[i] - cmn);
                                    histall->Fill(index[i] + k * noofch, adc[i]);
                                    histall_cmn->Fill(index[i] + k * noofch, adc[i] - cmn);
                                    hist_cmn_ex[index[i] + k * noofch]->Fill(adc[i] - array_cmn_ex[k]);
                                    histall_cmn_ex->Fill(index[i] + k * noofch, adc[i] - array_cmn_ex[k]);
                                    
                                    if(index[i]%2==0){
                                        histall2->Fill((index[i]+k*noofch)/2, adc[i]);
                                        histall2_cmn->Fill((index[i]+k*noofch)/2, adc[i]-cmn);
                                        histall2_cmn_ex->Fill((index[i]+k*noofch)/2, adc[i]-array_cmn_ex[k]);
                                    }
                                }
                            }
                            histref->Fill(ref);
                            array_ref[k] = ref;
                            array_cmn[k] = cmn;
                            array_hitnum[k] = hitnum;
                            
                        }
                        if (bitoffset % 32 != 0) {
                            bitoffset += (32 - bitoffset % 32);
                        }
                        bitoffset += 32;
                    }
                    
                    eventtree->Fill();
                    eventid++;
                    
                }
            } while(j<framewordsize);
            
        }
    }

    std::cout << "frame counts : " << framecount << std::endl;

    return AS_QUIT;
}

ANLStatus Rawdata2ROOT_VATA450::mod_end_run()
{
    outfile->Write();
    outfile->Close();
    
   return AS_OK; 
}

ANLStatus Rawdata2ROOT_VATA450::mod_finalize()
{
   return AS_OK; 
}
