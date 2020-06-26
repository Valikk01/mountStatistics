#include <iostream>
#include <sys/statvfs.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <sys/statvfs.h>
#include <map>
#include <fstream>
#include <sys/stat.h>


using namespace std;

map <string, double> fileStats = {{"Documents", 0}, {"Libraries", 0}, {"Others", 0}};

struct Mount {
    std::string device;
    std::string destination;
    std::string fstype;
    std::string options;
    int dump;
    int pass;
};

std::ostream& operator<<(std::ostream& stream, const Mount& mount) {
    return stream << mount.fstype <<" device \""<<mount.device<<"\", mounted on \""<<mount.destination<<"\". Options: "<<mount.options<<". Dump:"<<mount.dump<<" Pass:"<<mount.pass;
}


void fileExt (string name, double size){ // group files by extension
    
    string ext = name.substr(name.find_last_of(".") + 1);
    if(ext == "txt" || ext == "docx"){
        auto a = fileStats.find("Documents");
        a->second += size;
    }
    if(ext == "a" || ext == "so"){
        auto a = fileStats.find("Libraries");
        a->second += size;
    }else{
        auto a = fileStats.find("Others");
        a->second += size;
    }

}

int listDir(string path) {          //recursively run through all folders  and get file sizes
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir (path.data())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
                listDir(path + '/' + ent->d_name);
            }
            else if (ent->d_type == DT_REG){
                 struct stat statbuf;
                 string sizePath = path + '/' + ent->d_name;

                 if (stat(sizePath.data(), &statbuf) == -1) {
                      cout << "SizeError" << endl;
                 }
                 fileExt(ent->d_name,(double) (statbuf.st_size/1000));
            }
        }
        closedir (dir);
    }
    else
    {
        perror ("");
        return EXIT_FAILURE;
    }
    return 0;
}



int mountInfo() {       //get all mounted file systems
    std::ifstream mountInfo("/proc/mounts");

    while( !mountInfo.eof() ) {
        Mount each;
        mountInfo >> each.device >> each.destination >> each.fstype >> each.options >> each.dump >> each.pass;
        if( each.device != "" ){
            std::cout << each << std::endl;
            
            listDir(each.destination);
            long total = 0;
    
          for (auto i = fileStats.begin(); i != fileStats.end(); ++i) {
              total += i->second;
          }
    
          for (auto i = fileStats.begin(); i != fileStats.end(); ++i) {
              cout << i->first << " - " << i->second << "kb; Percents - "<< (total == 0 ? 0 :((float)(i->second*100)/total)) <<"%"<< endl;
          }
         
         for (auto i = fileStats.begin(); i != fileStats.end(); ++i) {
             i->second = 0;
          }
        }
    }

    return 0;
}


int main(int argc, const char * argv[]) {
   
    mountInfo();

    return 0;
}
