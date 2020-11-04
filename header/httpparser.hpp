
#include <cstring>
#include <string>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

static void printHeader(const vector< pair<string, string> >& header){
    for(auto itr=header.begin();itr!= header.end();itr++){
        cout<<itr->first<<" : "<<itr->second<<endl;
    }
}

class http_request{
public:

    vector< pair<string, string> > header;
    vector<char> body;
    int parseHeaderLine(char* input, int n);

    http_request(){}
};

class http_response{
public:

    vector< pair<string, string>> header;
    vector<char> body;
    
    http_response(){}
    void addHeader(string title, string value);
    void addBody(const char* filename);
    int to_c(char* buffer);
};
