#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    int document_count_ = 0;
    
    struct Quaery {
    set<string> minus_words_;
    set<string> plus_words_;
    };
    
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(const int& document_id, const string& document) {
     
        const vector<string> words = SplitIntoWordsNoStop(document);
        const int count_of_words_in_doc = words.size();
        
       
        for(const string word : words){   
            double TF = static_cast<double>(count(words.begin(), words.end(), word))               /count_of_words_in_doc; 

        word_to_document_freqs_[word].insert({document_id, TF});
       
        } 
    }
   
     vector<Document> FindTopDocuments(const string& raw_query) const  { 
       
        Quaery query_words = ParseQuery(raw_query);
         
        auto matched_documents = FindAllDocuments(query_words,  word_to_document_freqs_,        document_count_); 
           
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
         
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        // if(matched_documents[matched_documents.size()-1]==0.0){matched_documents.pop_back();}
        return matched_documents;
    }

private:
    
   
    map<string, map<int, double>> word_to_document_freqs_;
    
    map<int, int> relevance_; // doc id, relevance
    set<string> stop_words_;
    
     vector<Document> FindAllDocuments(const Quaery& query_words, const map<string, map<int,      double>>& word_to_document_freqs_,const int& document_count_) const { 
        
        set<int> doc_with_minus_words; 
        vector<Document> result; // <id,rel>
        map<int, double> document_to_relevance; //<id,rel>
    
        for(string PQw : query_words.plus_words_){
             if(word_to_document_freqs_.count(PQw)){
                 auto id_TF = word_to_document_freqs_.at(PQw);  
                    double IDF = log(static_cast<double>(document_count_) / id_TF.size());
                         double result_w = 0.0;
                           
            for(const auto&[id, TF]:id_TF){
                result_w = IDF*TF;
                document_to_relevance[id] += result_w;
                          } } } 
        
        for(const auto& [doc_id, doc_rel] : document_to_relevance){
            if(!doc_with_minus_words.count(doc_id)){
                result.push_back({doc_id, doc_rel});}}
         
  return  result; }  // result = vector <id, rel>

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Quaery ParseQuery(const string& text) const {
        Quaery PQ; 
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if(word[0] == '-'){ 
             PQ.minus_words_.insert( word.substr(1)); 
            }
            else{
            PQ.plus_words_.insert(word);}
        }
        return PQ;
    }
};

const SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    search_server.document_count_ = document_count;
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
     SearchServer search_server = CreateSearchServer();
    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}