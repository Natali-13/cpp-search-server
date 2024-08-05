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
    return s; } 

int ReadLineWithNumber() { 
    int result = 0; 
    cin >> result; 
    ReadLine(); 
    return result; } 

 vector<string> SplitIntoWords(const string& text) { 
    vector<string> words; 
    string word; 
    for (const char c : text) { 
        if (c == ' ') { 
            if (!word.empty()) { 
                words.push_back(word); 
                word.clear(); 
            } 
        } 
        else {     word += c; } } 
    if (!word.empty()) { 
        words.push_back(word); }
    return words; }  

struct Document { 
    int id; 
    double relevance; }; 

 

class SearchServer { 
public: 
    void SetStopWords(const string& text) { 
        for (const string& word : SplitIntoWords(text)) { 
            stop_words_.insert(word);  }     }  

    void AddDocument(int document_id, const string& document) { 
        ++document_count_; 
        vector<string> words = SplitIntoWordsNoStop(document); 
        double counter = 1.0 / words.size(); 
        for (const string& word : words) { 
            word_to_document_freqs_[word][document_id] += counter; }  }  

    vector<Document> FindTopDocuments(const string& raw_query) const { 
        const auto [query_words, minus_words] = ParseQuery(raw_query); 
        auto matched_documents = FindAllDocuments(query_words, minus_words); 
        sort(matched_documents.begin(), matched_documents.end(), 
            [](const Document& lhs, const Document& rhs) { 
                return lhs.relevance > rhs.relevance;      }); 
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) { 
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT); 
        } 
        return matched_documents;  } 
 private: 
    map<string, map<int, double>> word_to_document_freqs_; 
    set<string> stop_words_; 
    int document_count_ = 0;  

    bool IsStopWord(const string& word) const { 
        return stop_words_.count(word) > 0; } 
    
     vector<string> SplitIntoWordsNoStop(const string& text) const { 
        vector<string> words; 
        for (const string& word : SplitIntoWords(text)) { 
            if (!IsStopWord(word)) { 
                words.push_back(word); }     } 
        return words;  }  

    pair<set<string>, set<string>> ParseQuery(const string& text) const { 
        set<string> query_words; 
        set<string> minus_words; 
        for (const string& word : SplitIntoWordsNoStop(text)) { 
            if (word[0] != '-') { 
               query_words.insert(word); 
            } 
            else { 
                string word_without_minus = word.substr(1); 
                minus_words.insert(word_without_minus); 
            } 
        } 
        return { query_words, minus_words }; 
    } 

    double IDFCounter(const string& word) const{
          if (word_to_document_freqs_.count(word) == 0) {
            return 0;        }
        return log(document_count_ * 1.0 / word_to_document_freqs_.at(word).size());
    }
 
    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const { 
        map<int, double> document_to_relevance; 
        for (const string& word : query_words) { 
            if (word_to_document_freqs_.count(word) == 0) { 
                continue;            } 
            const double idf = IDFCounter(word); 
            for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) { 
                document_to_relevance[document_id] += idf * term_freq ; }  } 
        for (const string& word : minus_words) { 
            if (word_to_document_freqs_.count(word) == 0) { 
                continue;} 
            for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) { 
                document_to_relevance.erase(document_id);}} 
         vector<Document> matched_documents; 
        for (const auto& [document_id, relevance] : document_to_relevance) { 
            matched_documents.push_back({document_id, relevance});} 
        return matched_documents; } 
}; 
 
SearchServer CreateSearchServer() { 
    SearchServer search_server; 
    search_server.SetStopWords(ReadLine()); 
    const int document_count = ReadLineWithNumber(); 
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());  } 
    return search_server; } 

 

int main() { 
    const SearchServer search_server = CreateSearchServer(); 
     const string query = ReadLine(); 
   for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) { 
        cout << "{ document_id = "s << document_id << ", "  << "relevance = "s << relevance << " }"s << endl; }} 
