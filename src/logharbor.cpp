#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>
namespace fs=std::filesystem;
std::string json(const std::string&s){std::string r="\"";const char*h="0123456789abcdef";for(unsigned char c:s){if(c=='"'||c=='\\'){r+='\\';r+=c;}else if(c<32){r+="\\u00";r+=h[c>>4];r+=h[c&15];}else r+=c;}return r+'"';}
std::string lower(std::string s){for(auto&c:s)if(c>='A'&&c<='Z')c+=32;return s;}
int ranklevel(std::string s){s=lower(s);if(s=="debug"||s=="trace")return 0;if(s=="info"||s=="notice")return 1;if(s=="warn"||s=="warning")return 2;if(s=="error"||s=="err")return 3;if(s=="critical"||s=="fatal"||s=="alert"||s=="emerg")return 4;return -1;}
std::string levelname(int r){return std::vector<std::string>{"DEBUG","INFO","WARNING","ERROR","CRITICAL"}[r];}
int month(const std::string&s){std::string m="JanFebMarAprMayJunJulAugSepOctNovDec";auto p=m.find(s);return p==std::string::npos||p%3?-1:static_cast<int>(p/3+1);}
int64_t days(int y,unsigned m,unsigned d){y-=m<=2;int era=(y>=0?y:y-399)/400;unsigned yo=static_cast<unsigned>(y-era*400);unsigned doy=(153*(m>2?m-3:m+9)+2)/5+d-1;unsigned doe=yo*365+yo/4-yo/100+doy;return era*146097+static_cast<int>(doe)-719468;}
bool valid(int y,int m,int d,int h,int n,int s){if(y<1970||y>2099||m<1||m>12||d<1||h<0||h>23||n<0||n>59||s<0||s>59)return false;int ds[]={31,28,31,30,31,30,31,31,30,31,30,31};if(y%4==0&&(y%100!=0||y%400==0))ds[1]=29;return d<=ds[m-1];}
std::string stamp(int y,int m,int d,int h,int n,int s,std::string zone){if(!valid(y,m,d,h,n,s))return "";int offset=0;if(zone!=""&&zone!="Z"){char sign=zone[0];zone.erase(std::remove(zone.begin(),zone.end(),':'),zone.end());if(zone.size()!=5)return "";int hh=std::stoi(zone.substr(1,2)),mm=std::stoi(zone.substr(3,2));if(hh>23||mm>59)return "";offset=(hh*60+mm)*60*(sign=='-'?-1:1);}int64_t sec=days(y,m,d)*86400+h*3600+n*60+s-offset;int64_t z=sec/86400;if(sec<0&&sec%86400)--z;int rem=static_cast<int>(sec-z*86400);z+=719468;int era=static_cast<int>((z>=0?z:z-146096)/146097);unsigned doe=static_cast<unsigned>(z-era*146097),yoe=(doe-doe/1460+doe/36524-doe/146096)/365;int yy=static_cast<int>(yoe)+era*400;unsigned doy=doe-(365*yoe+yoe/4-yoe/100),mp=(5*doy+2)/153,dd=doy-(153*mp+2)/5+1,mm=mp<10?mp+3:mp-9;yy+=mm<=2;char out[32];std::snprintf(out,sizeof(out),"%04d-%02u-%02uT%02d:%02d:%02dZ",yy,mm,dd,rem/3600,(rem%3600)/60,rem%60);return out;}
struct Record{size_t line;std::string time,level,format,message;};
bool parse(const std::string&line,int year,Record&r){
 static const std::regex iso(R"(^\[?(\d{4})-(\d{2})-(\d{2})[T ](\d{2}):(\d{2}):(\d{2})(?:\.\d+)?(Z|[+-]\d{2}:?\d{2})?\]?\s+\[?([A-Za-z]+)\]?\s*[: -]?\s*(.*)$)");
 static const std::regex apache(R"rx(^.*\[(\d{2})/([A-Za-z]{3})/(\d{4}):(\d{2}):(\d{2}):(\d{2}) ([+-]\d{4})\] "([^"]*)" (\d{3})(?: .*)?$)rx");
 static const std::regex syslog(R"(^([A-Za-z]{3})\s+(\d{1,2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\S+)\s+(.*)$)");
 static const std::regex severity(R"(\b(CRITICAL|FATAL|ALERT|EMERG|ERROR|ERR|WARNING|WARN|INFO|NOTICE|DEBUG|TRACE)\b)",std::regex::icase);
 std::smatch a;int lv=-1;
 if(std::regex_match(line,a,iso)){r.time=stamp(std::stoi(a[1]),std::stoi(a[2]),std::stoi(a[3]),std::stoi(a[4]),std::stoi(a[5]),std::stoi(a[6]),a[7]);lv=ranklevel(a[8]);r.format="ISO application";r.message=a[9];}
 else if(std::regex_match(line,a,apache)){r.time=stamp(std::stoi(a[3]),month(a[2]),std::stoi(a[1]),std::stoi(a[4]),std::stoi(a[5]),std::stoi(a[6]),a[7]);int status=std::stoi(a[9]);if(status<100||status>599)return false;lv=status>=500?3:(status>=400?2:1);r.format="Apache access";r.message=std::string(a[8])+" · HTTP "+std::string(a[9]);}
 else if(std::regex_match(line,a,syslog)){r.time=stamp(year,month(a[1]),std::stoi(a[2]),std::stoi(a[3]),std::stoi(a[4]),std::stoi(a[5]),"Z");r.format="Syslog (UTC/year supplied)";r.message=std::string(a[6])+" "+std::string(a[7]);std::smatch severityMatch;lv=std::regex_search(r.message,severityMatch,severity)?ranklevel(severityMatch[1]):1;}
 else return false;
 if(r.time.empty()||lv<0)return false;r.level=levelname(lv);return true;
}
int program_main(int argc,char**argv){
 if(argc<2||argc>5){std::cerr<<"Usage: logharbor LOG_FILE [MIN_LEVEL] [TEXT_QUERY] [SYSLOG_YEAR]\n";return 2;}
 std::string minimum=argc>2?argv[2]:"DEBUG",query=argc>3?lower(argv[3]):"";int minrank=ranklevel(minimum),year=2026;
 try{if(argc>4){std::string yy=argv[4];size_t pos=0;year=std::stoi(yy,&pos);if(pos!=yy.size())throw std::invalid_argument("year");}}catch(...){std::cerr<<"Invalid year.\n";return 2;}
 if(minrank<0||year<1970||year>2099||query.size()>256){std::cerr<<"Invalid filters.\n";return 2;}
 fs::path p=fs::u8path(argv[1]);std::error_code ec;if(!fs::is_regular_file(p,ec)||fs::file_size(p,ec)>268435456){std::cerr<<"Use a readable regular log file of at most 256 MiB.\n";return 2;}std::ifstream in(p,std::ios::binary);if(!in){std::cerr<<"Cannot read log file.\n";return 2;}
 size_t lines=0,parsed=0,matched=0,malformed=0;std::vector<Record> records;std::vector<std::pair<size_t,std::string>> diagnostics;std::map<std::string,size_t> levels,formats;std::string earliest,latest,line;bool limit=false;
 while(in.peek()!=EOF&&lines<1000000){line.clear();bool overflow=false;char c;while(in.get(c)){if(c=='\n')break;if(line.size()<65536)line+=c;else overflow=true;}if(!line.empty()&&line.back()=='\r')line.pop_back();++lines;Record r;r.line=lines;
 if(overflow||!parse(line,year,r)){++malformed;if(diagnostics.size()<50)diagnostics.push_back({lines,overflow?"Line exceeds 64 KiB; skipped safely.":"Unrecognized format, severity or invalid calendar date."});continue;}
 ++parsed;++levels[r.level];++formats[r.format];if(earliest.empty()||r.time<earliest)earliest=r.time;if(latest.empty()||r.time>latest)latest=r.time;
 if(ranklevel(r.level)>=minrank&&(query.empty()||lower(r.message).find(query)!=std::string::npos)){++matched;if(records.size()<500)records.push_back(r);}
 }
 if(in.peek()!=EOF)limit=true;if(in.bad()){std::cerr<<"Read error while processing log.\n";return 3;}
 std::cout<<"{\"file\":"<<json(fs::absolute(p).u8string())<<",\"lines\":"<<lines<<",\"parsed\":"<<parsed<<",\"matched\":"<<matched<<",\"malformed\":"<<malformed<<",\"display_limit\":500,\"line_limit_reached\":"<<(limit?"true":"false")<<",\"earliest\":"<<json(earliest)<<",\"latest\":"<<json(latest)<<",\"minimum\":"<<json(levelname(minrank))<<",\"query\":"<<json(query)<<",\"syslog_year\":"<<year<<",\"levels\":{";
 bool first=true;for(auto&[k,v]:levels){if(!first)std::cout<<',';first=false;std::cout<<json(k)<<':'<<v;}std::cout<<"},\"formats\":{";first=true;for(auto&[k,v]:formats){if(!first)std::cout<<',';first=false;std::cout<<json(k)<<':'<<v;}std::cout<<"},\"records\":[";first=true;for(auto&r:records){if(!first)std::cout<<',';first=false;std::cout<<"{\"line\":"<<r.line<<",\"timestamp\":"<<json(r.time)<<",\"level\":"<<json(r.level)<<",\"format\":"<<json(r.format)<<",\"message\":"<<json(r.message)<<'}';}std::cout<<"],\"diagnostics\":[";first=true;for(auto&[number,reason]:diagnostics){if(!first)std::cout<<',';first=false;std::cout<<"{\"line\":"<<number<<",\"reason\":"<<json(reason)<<'}';}std::cout<<"]}\n";
 return 0;
}

#ifdef _WIN32
int wmain(int argc,wchar_t**wargv){
 std::vector<std::string> args; for(int i=0;i<argc;++i) args.push_back(fs::path(wargv[i]).u8string());
 std::vector<char*> ptrs; for(auto&arg:args) ptrs.push_back(arg.data());
 return program_main(argc,ptrs.data());
}
#else
int main(int argc,char**argv){return program_main(argc,argv);}
#endif
