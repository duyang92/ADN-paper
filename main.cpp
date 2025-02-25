#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <string.h>
#include <ctime>
#include <time.h>
#include <iterator>
#include <math.h>
#include <vector>
#include <map>

#include "./Algorithms/CMSketch.h"
#include "./Algorithms/CSketch.h"
#include "./Algorithms/ASketch.h"
#include "./Algorithms/ElasticSketch.h"
#include "./Algorithms/BitMatcher.h"
#include "./Algorithms/adn.h"
#include "./Algorithms/madn.h"

using namespace std;


char * filename_stream = "../data/1.dat";

char insert[40000000][20];
char query[40000000][20];

unordered_map<string, int> unmp;

#define testcycles 10


int main(int argc, char** argv)
{
    double memory = 0.1;	//MB
    if(argc >= 2)
    {
        filename_stream = argv[1];
    }
    if (argc >= 3)
    {
    	memory = stod(argv[2]);
    }
    

    unmp.clear();
    int val;

    int memory_ = memory * 1000;//KB
    int word_size = 64;


    int w = memory * 1024 * 1024 * 8.0 / COUNTER_SIZE;	//how many counter;
    int w_p = memory * 1024 * 1024 * 8.0 / (word_size * 2);
    int m1 = memory * 1024 * 1024 * 1.0/4 / 8 / 12;
    int m2 = memory * 1024 * 1024 * 3.0/4 / 2 / 1;
    int m2_mv = memory * 1024 * 1024 / 4 / 4;
    int w_dhs = memory * 1000 * 1024 / 16;
    int w_salsa = memory * 1024 * 1024;

    printf("\n******************************************************************************\n");
    printf("Evaluation starts!\n\n");

    
    CMSketch *cmsketch;
    CSketch *csketch;
    ASketch *asketch;
    BitMatcher *bmatcher;
    Elasticsketch *elasticsketch;
    NRCSketch *adn;
    MNRCSketch *madn;

    char _temp[200], temp2[200];
    int t = 0;

    int package_num = 0;

    char timestamp[8];

    FILE *file_stream = fopen(filename_stream, "r");

    char t1mp[20];
    while( fread(t1mp, 1, KEY_LEN, file_stream)==KEY_LEN )
    {
        memcpy(insert[package_num], t1mp, 4);
        string str = string(insert[package_num]);
        unmp[str]++;
        package_num++;

        if(package_num == MAX_INSERT_PACKAGE)
            break;
    }
    fclose(file_stream);

    printf("memory = %dKB\n", memory_);
    printf("dataset name: %s\n", filename_stream);
    printf("total stream size = %d\n", package_num);
    printf("distinct item number = %d\n", unmp.size());
  
    int max_freq = 0;
    unordered_map<string, int>::iterator it = unmp.begin();

    for(int i = 0; i < unmp.size(); i++, it++)
    {
        strcpy(query[i], it->first.c_str());

        int temp2 = it->second;
        max_freq = max_freq > temp2 ? max_freq : temp2;
    }
    printf("max_freq = %d\n", max_freq);
    
    printf("*************************************\n");



/********************************insert*********************************/

    timespec time1, time2;
    long long resns;

    std::ofstream outfile;
    outfile.open("throughput_memory=" + to_string(memory_) + ".txt");

    outfile << "Insert: " << "\n";

    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < testcycles; t++)
    {
        cmsketch = new CMSketch(w / LOW_HASH_NUM, LOW_HASH_NUM);
        for(int i = 0; i < package_num; i++)
        {
            cmsketch->Insert(insert[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    double throughput_cm = (double)1000.0 * testcycles * package_num / resns;
    printf("throughput of CM (insert): %.6lf Mips\n", throughput_cm);
    // outfile << "CM   " << throughput_cm << "\n";
    outfile << throughput_cm << " ";

    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < testcycles; t++)
    {
        csketch = new CSketch(w / LOW_HASH_NUM, LOW_HASH_NUM);
        for(int i = 0; i < package_num; i++)
        {
            cmsketch->Insert(insert[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    double throughput_cs = (double)1000.0 * testcycles * package_num / resns;
    printf("throughput of CS (insert): %.6lf Mips\n", throughput_cs);
    // outfile << "CS   " << throughput_cs << "\n";
    outfile << throughput_cs << " ";


    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < testcycles; t++)
    {
        asketch = new ASketch(w / LOW_HASH_NUM, LOW_HASH_NUM);
        for(int i = 0; i < package_num; i++)
        {
            asketch->Insert(insert[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    double throughput_a = (double)1000.0 * testcycles * package_num / resns;
    printf("throughput of A (insert): %.6lf Mips\n", throughput_a);
    // outfile << "A    " << throughput_a << "\n";
    outfile << throughput_a << " ";

 	unordered_map<string, int> tmp;
    int flag[100]={0};	
	clock_gettime(CLOCK_MONOTONIC, &time1);
    for (int t = 0; t < testcycles; t++)
    {
            bmatcher = new BitMatcher(memory * 1000 * 1024/8/2);
		    for (int i = 0; i < package_num; i++)
            {
                bmatcher->Insert(insert[i]);
            }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    double throughput_bmatcher = (double)1000.0 * testcycles * package_num / resns;
    printf("throughput of BM (insert): %.6lf Mips\n", throughput_bmatcher);
    // outfile << "BM   " << throughput_bmatcher << "\n";
    outfile << throughput_bmatcher << " ";


	clock_gettime(CLOCK_MONOTONIC, &time1);
	for (int t = 0; t < testcycles; t++)
	{
		elasticsketch = new Elasticsketch(m1, m2);
		for (int i = 0; i < package_num; i++)
		{
			elasticsketch->Insert(insert[i]);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double throughput_elastic = (double)1000.0 * testcycles * package_num / resns;
	printf("throughput of EL (insert): %.6lf Mips\n", throughput_elastic);
    outfile << throughput_elastic << " ";

    clock_gettime(CLOCK_MONOTONIC, &time1);
	for (int t = 0; t < testcycles; t++)
	{
		adn = new NRCSketch(memory * 1000 * 1024 * 8 / 2 / 32, 2, 20, 0.02);
		for (int i = 0; i < package_num; i++)
		{
			adn->Insert(insert[i]);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double throughput_adn = (double)1000.0 * testcycles * package_num / resns;
	printf("throughput of ADN (insert): %.6lf Mips\n", throughput_adn);
    // outfile << "ADN  " << throughput_adn << "\n";
    outfile << throughput_adn << " ";

    clock_gettime(CLOCK_MONOTONIC, &time1);
	for (int t = 0; t < testcycles; t++)
	{
		madn = new MNRCSketch(memory * 1000 * 1024 * 8 / 2 / (7 * 4 + 4), 2, 10, 0.02, 4, 7);
		for (int i = 0; i < package_num; i++)
		{
			madn->Insert(insert[i]);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double throughput_madn = (double)1000.0 * testcycles * package_num / resns;
	printf("throughput of mADN (insert): %.6lf Mips\n", throughput_madn);
    // outfile << "mADN " << throughput_madn << "\n";
    outfile << throughput_madn << "\n";


/********************************************************************************************/

    printf("*************************************\n");

    outfile << "Query: " << "\n";

/********************************query*********************************/

    double res_tmp=0;
	//double query_temp = 0;
    int flow_num = unmp.size();

    double sum = 0;

    cout << res_tmp << endl;


    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < testcycles; t++)
    {
        for(int i = 0; i < flow_num; i++)
        {
            res_tmp = cmsketch->Query(query[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    throughput_cm = (double)1000.0 * testcycles * flow_num / resns;
    printf("throughput of CM (query): %.6lf Mips\n", throughput_cm);
//     outfile << "CM   " << throughput_cm << "\n";
    outfile << throughput_cm << " ";
    sum += res_tmp;
    cout << res_tmp << endl;

    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < testcycles; t++)
    {
        for(int i = 0; i < flow_num; i++)
        {
            res_tmp = csketch->Query(query[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    throughput_cs = (double)1000.0 * testcycles * flow_num / resns;
    printf("throughput of CS (query): %.6lf Mips\n", throughput_cs);
//     outfile << "CS   " << throughput_cs << "\n";
    outfile << throughput_cs << " ";
    sum += res_tmp;
    cout << res_tmp << endl;


    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < testcycles; t++)
    {
        for(int i = 0; i < flow_num; i++)
        {
            res_tmp = asketch->Query(query[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    throughput_a = (double)1000.0 * testcycles * flow_num / resns;
    printf("throughput of A (query): %.6lf Mips\n", throughput_a);
    // outfile << "A    " << throughput_a << "\n";
    outfile << throughput_a << " ";
    sum += res_tmp;
    cout << res_tmp << endl;

    clock_gettime(CLOCK_MONOTONIC, &time1);
    for (int t = 0; t < testcycles; t++)
    {
        for (int i = 0; i < flow_num; i++)
        {
            res_tmp = bmatcher->Query(query[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    throughput_bmatcher = (double)1000.0 * testcycles * flow_num / resns;
    printf("throughput of BM (query): %.6lf Mips\n", throughput_bmatcher);
    // outfile << "BM   " << throughput_bmatcher << "\n";
    outfile << throughput_bmatcher << " ";
    sum += res_tmp;
    cout << res_tmp << endl;

    clock_gettime(CLOCK_MONOTONIC, &time1);
	for (int t = 0; t < testcycles; t++)
	{
		for (int i = 0; i < flow_num; i++)
		{
			res_tmp = elasticsketch->Query(query[i]);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	throughput_elastic = (double)1000.0 * testcycles * flow_num / resns;
	printf("throughput of EL (query): %.6lf Mips\n", throughput_elastic);
    // outfile << "EL   " << throughput_elastic << "\n";
    outfile << throughput_elastic << " ";
	sum += res_tmp;
    cout << res_tmp << endl;

    clock_gettime(CLOCK_MONOTONIC, &time1);
    for (int t = 0; t < testcycles; t++)
    {
        for (int i = 0; i < flow_num; i++)
        {
            res_tmp = adn->Query(query[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    throughput_adn = (double)1000.0 * testcycles * flow_num / resns;
    printf("throughput of ADN (query): %.6lf Mips\n", throughput_adn);
    // outfile << "ADN  " << throughput_adn << "\n";
    outfile << throughput_adn << " ";
    sum += res_tmp;
    cout << res_tmp << endl;

    clock_gettime(CLOCK_MONOTONIC, &time1);
    for (int t = 0; t < testcycles; t++)
    {
        for (int i = 0; i < flow_num; i++)
        {
            res_tmp = madn->Query(query[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    throughput_madn = (double)1000.0 * testcycles * flow_num / resns;
    printf("throughput of mADN (query): %.6lf Mips\n", throughput_madn);
    // outfile << "mADN " << throughput_madn << "\n";
    outfile << throughput_madn << "\n";
    sum += res_tmp;
    cout << res_tmp << endl;

    outfile.close();


    cout << sum << " " << res_tmp << endl;

    

/********************************************************************************************/
    printf("*************************************\n");

    //avoid the over-optimize of the compiler! 
    if(sum == (1 << 30))
        return 0;

    return 0;

    char temp[20];

    double re_cm = 0.0, re_cs = 0.0,  re_a = 0.0,  re_pcssketch = 0.0, re_pcsketch = 0.0, re_bmatcher = 0.0, re_elastic=0.0, re_nitro=0.0, re_mvsketch=0.0, re_adn = 0.0, re_madn = 0.0;
    double re_cm_sum = 0.0, re_cs_sum = 0.0,  re_a_sum = 0.0,  re_pcssketch_sum = 0.0, re_bmatcher_sum = 0.0, re_elastic_sum=0.0, re_nitro_sum=0.0, re_mvsketch_sum=0.0, re_adn_sum = 0.0, re_madn_sum = 0;
    
    double ae_cm = 0.0, ae_cs = 0.0,  ae_a = 0.0,  ae_pcssketch = 0.0, ae_bmatcher = 0.0, ae_elastic=0.0, ae_nitro=0.0, ae_mvsketch=0.0, ae_adn = 0.0, ae_madn = 0;
    double ae_cm_sum = 0.0, ae_cs_sum = 0.0,  ae_a_sum = 0.0,  ae_pcssketch_sum = 0.0, ae_bmatcher_sum = 0.0, ae_elastic_sum=0.0, ae_nitro_sum=0.0, ae_mvsketch_sum=0.0, ae_adn_sum = 0.0, ae_madn_sum = 0;

    double val_cm = 0.0, val_cs = 0.0,  val_a = 0.0,  val_pcssketch = 0.0, val_bmatcher = 0.0, val_elastic=0.0, val_nitro=0.0, val_mvsketch=0.0, val_adn = 0, val_madn = 0;
    double erro_cm = 0.0, erro_cs = 0.0, erro_a = 0.0,  erro_pcssketch = 0.0, erro_bmatcher = 0.0, erro_elastic=0.0, erro_nitro=0.0, erro_mvsketch=0.0, erro_adn = 0, erro_madn = 0;
    double mem_cc = 0.0, mem_cc_sum = 0.0;

    //double mark_cm=0, mark_el=0, mark_cc=0, mark_pcs=0, mark_nitro=0, mark_mv=0;

    for(unordered_map<string, int>::iterator it = unmp.begin(); it != unmp.end(); it++)
    {
        strcpy(temp, (it->first).c_str());
        val = it->second;
        

        val_cm = cmsketch->Query(temp);
        val_cs = csketch->Query(temp);    
        val_a = asketch->Query(temp);      
	    val_bmatcher = bmatcher->Query(temp);
        val_elastic = elasticsketch->Query(temp);
	    val_adn = adn->Query(temp);
	    val_madn = madn->Query(temp);

        re_cm = fabs(val_cm - val) / (val * 1.0);
        re_cs = fabs(val_cs - val) / (val * 1.0);
        re_a = fabs(val_a - val) / (val * 1.0);
	    re_bmatcher = fabs(val_bmatcher - val) / (val * 1.0);
	    re_elastic = fabs(val_elastic - val) / (val * 1.0);
        re_adn = fabs(val_adn - val) / (val * 1.0);	
        re_madn = fabs(val_madn - val) / (val * 1.0);	

        ae_cm = fabs(val_cm - val);
        ae_cs = fabs(val_cs - val);       
        ae_a = fabs(val_a - val);      
	    ae_bmatcher = fabs(val_bmatcher - val);
	    ae_elastic = fabs(val_elastic - val);
	    ae_adn = fabs(val_adn - val);
	    ae_madn = fabs(val_madn - val);

        re_cm_sum += re_cm;
        re_cs_sum += re_cs;        
        re_a_sum += re_a;              
	    re_bmatcher_sum += re_bmatcher;
	    re_elastic_sum += re_elastic;
	    re_adn_sum += re_adn;
	    re_madn_sum += re_madn;

        ae_cm_sum += ae_cm;
        ae_cs_sum += ae_cs;      
        ae_a_sum += ae_a;           
	    ae_bmatcher_sum += ae_bmatcher;
        ae_elastic_sum += ae_elastic;
        ae_adn_sum += ae_adn;
        ae_madn_sum += ae_madn;
    }

    double a = package_num * 1.0;
    double b = unmp.size() * 1.0;

	printf("*************************************\n");
    printf("aae_cm = %lf\n", ae_cm_sum / b);
    printf("aae_cs = %lf\n", ae_cs_sum / b);
	printf("aae_a = %lf\n", ae_a_sum / b);
	printf("aae_BM = %lf\n", ae_bmatcher_sum / b);
	printf("aae_EL = %lf\n", ae_elastic_sum / b);
	printf("aae_ADN = %lf\n", ae_adn_sum / b);
	printf("aae_mADN = %lf\n", ae_madn_sum / b);
    printf("*************************************\n");
    printf("are_cm = %lf\n", re_cm_sum / b);
    printf("are_cs = %lf\n", re_cs_sum / b);
	printf("are_a = %lf\n", re_a_sum / b);
    printf("are_BM = %lf\n", re_bmatcher_sum / b);
    printf("are_elastic = %lf\n", re_elastic_sum / b); 
    printf("are_ADN = %lf\n", re_adn_sum / b);
    printf("are_mADN = %lf\n", re_madn_sum / b);
    printf("**************************************\n");
	printf("Evaluation Ends!\n\n");

    return 0;
}