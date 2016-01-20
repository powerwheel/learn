#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <fstream>

void print_space(int w){
    for (int i = 0; i < w; ++i){
        std::cout << "\t";
    }
}
std::queue<std::string> parse(const std::string &html)
{
    std::queue<std::string> root;
    int pos = 0;
    while(pos < html.length()){
        std::size_t found = html.find('<', pos);
        if (found == std::string::npos) 
            break;
        if (found != html.length() - 1){
            if (html[ found + 1] == '/'){
                std::size_t nfound = html.find('>', found + 1);
                if (nfound != std::string::npos ){
                    std::string tag = html.substr(found, nfound - found + 1);
                    root.push(tag);
                    pos = nfound + 1;
                }else{
                    pos = found + 1;
                    std::cout << "format error" << std::endl;
                }
            } else {
                std::size_t nfound = html.find('>', found + 1);
                if (nfound != std::string::npos){
                    std::string tag = html.substr(found, nfound - found + 1);
                    std::size_t nspace = tag.find(' ');
                    if (nspace != std::string::npos){
                        tag = tag.substr(0, nspace);
                        if (html[ nfound - 1 ] == '/'){
                            tag = tag + "/>";
                        }else{
                            tag = tag + ">";
                        }
                    }

                
                    root.push(tag);
                    pos = nfound + 1;
                }else{
                    pos = found + 1;
                    std::cout << "format error" << std::endl;
                }
            }
        }else{
            std::cout << "html format error" << std::endl;
            break;
        }
    }
    return root;   
}

int main(int argc , char *argv[])
{
    if (argc != 2){
        std::cout << argv[0] << " html file" << std::endl;
        return 0;
    }
    std::ifstream infile(argv[1]);
    if (!infile){
        std::cout << argv[1] << " open failed!" << std::endl;
        return 0;
    }
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    infile.seekg(0, infile.beg);

    std::string html;
    html.resize(length, ' ' );
    char *begin = &*html.begin();

    infile.read(begin, length);

    infile.close();

    std::queue<std::string> root = parse(html);
    std::stack<std::string> temp;
    int deep = 0;
    while( !root.empty() ){
        std::string tag = root.front();
        root.pop();
        if (tag[tag.length() - 2] == '/'){
            print_space(temp.size());
            std::cout << tag << std::endl;
        }else{
            if (tag[1] != '/'){
                temp.push(tag);
                print_space(temp.size() ? temp.size() - 1 : 0);
                std::cout << tag << std::endl;
            }else{
                print_space(temp.size() ? temp.size() - 1 : 0);
                std::cout << tag << std::endl;
                temp.pop();
            }
        }

    } 

}
