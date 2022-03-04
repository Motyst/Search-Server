#include "../src/search_server.h"

using namespace std::string_literals;

SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(string_processing::SplitIntoWords(stop_words_text))
{
}

void SearchServer::AddDocument(int document_id, const std::string& document,
                               DocumentStatus status, const std::vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw std::invalid_argument("Document number negative or alredy exists!"s);
    }

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inverse_word_count = 1.0 / words.size();
    std::set<std::string> unique_words;

    for (const std::string& word : words) {
        document_to_word_frequency_[document_id][word] += inverse_word_count;
        word_to_document_frequency_[word][document_id] += inverse_word_count;
        unique_words.insert(word);
    }

    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    document_ids_.insert(document_id);
}

int SearchServer::GetDocumentCount() const {
    return static_cast<int>(documents_.size());
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);
    std::vector<std::string> matched_words;

    for (const std::string& word : query.plus_words) {
        if (!document_to_word_frequency_.count(document_id)) {
            continue;
        }
        if (document_to_word_frequency_.at(document_id).count(word)) {
            matched_words.push_back(word);
        }
    }

    for (const std::string& word : query.minus_words) {
        if (!document_to_word_frequency_.count(document_id)) {
            continue;
        }
        if (document_to_word_frequency_.at(document_id).count(word)) {
            matched_words.clear();
            break;
        }
    }
    return {matched_words, documents_.at(document_id).status};
}

const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static const std::map<std::string, double> empty_map;

    if(!document_to_word_frequency_.count(document_id)){
        return empty_map;
    }
    return document_to_word_frequency_.at(document_id);
}

void SearchServer::RemoveDocument(int document_id){
    if(!documents_.count(document_id)){
        return;
    }

    for(const auto& [word, frequency] : document_to_word_frequency_.at(document_id)){
        word_to_document_frequency_.at(word).erase(document_id);

        if(word_to_document_frequency_.at(word).empty()){
            word_to_document_frequency_.erase(word);
        }
    }

    documents_.erase(document_id);
    document_to_word_frequency_.erase(document_id);
    document_ids_.erase(document_id);
}

bool SearchServer::IsValidWord(const std::string& word) const {
    return none_of(word.begin(), word.end(), [](char symbol) {
        return symbol >= '\0' && symbol < ' ';
    });
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;

    for (const std::string& word : string_processing::SplitIntoWords(text)) {
        if(!IsValidWord(word)){
            throw std::invalid_argument("Input ["s + word + "] contains wrong symbols!"s);
        }

        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    if (text.empty()) {
        throw std::invalid_argument("No text!"s);
    }

    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }

    if (text.empty()) {
        throw std::invalid_argument("Empty text!"s);
    }
    if (text[0] == '-') {
        throw std::invalid_argument("Only one symbol [-] allowed before the word!"s);
    }

    if(!IsValidWord(text)){
        throw std::invalid_argument("Input ["s + text + "] contains wrong symbols!"s);
    }

    return {text, is_minus, IsStopWord(text)};
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;
    for (const std::string& word : string_processing::SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);

        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

double SearchServer::ComputeWordInverseDocumentFrequency(const std::string& word) const {
    // Check for existance of the word
    assert(word_to_document_frequency_.count(word));
    const size_t entry_size = word_to_document_frequency_.at(word).size();

    // Check for possible division by zero
    assert(entry_size != 0);

    return log(GetDocumentCount() * 1.0 / entry_size);
}
