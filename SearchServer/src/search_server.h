#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../src/document.h"
#include "../src/log_duration.h"
#include "../src/string_processing.h"

class SearchServer {

public:
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
    }

    explicit SearchServer(const std::string& stop_words_text);

    void AddDocument(int document_id, const std::string& document,
            DocumentStatus status, const std::vector<int>& ratings);

public:
    template <typename Predicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, Predicate predicate) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, predicate);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& left, const Document& right) {
                if (std::abs(left.relevance - right.relevance) < kRelevanceConstant) {
                    return left.rating > right.rating;
                } else {
                    return left.relevance > right.relevance;
                }
             });

        if (matched_documents.size() > static_cast<int>(kMaxResultDocumentCount)) {
            matched_documents.resize(kMaxResultDocumentCount);
        }
        return matched_documents;
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query,
                                           DocumentStatus document_status = DocumentStatus::kActual) const {
        return FindTopDocuments(raw_query,
                                [document_status](int document_id, DocumentStatus status, int rating){
                                    return status == document_status;
                                });
    }

    int GetDocumentCount() const;

    auto begin() const {
        return document_ids_.begin();
    }

    auto end() const {
        return document_ids_.end();
    }

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;

    const std::map<std::string, double>& GetWordFrequencies(int document_id) const;

    void RemoveDocument(int document_id);

private:
    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    struct QueryWord {
        std::string data;
        bool is_minus = false;
        bool is_stop = false;
    };

    struct DocumentData {
        int rating = 0;
        DocumentStatus status = DocumentStatus::kActual;
    };

private:
    static constexpr int kMaxResultDocumentCount = 5;
    static constexpr double kRelevanceConstant = 1e-6;

private:
    static int ComputeAverageRating(const std::vector<int>& ratings) {
    int rating_sum = 0;

    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

private:
    template <typename StringContainer>
    std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
        using namespace std::string_literals;
        std::set<std::string> non_empty_strings;

        for (const std::string& word : strings) {
            if(!IsValidWord(word)){
                throw std::invalid_argument("Input ["s + word + "] contains wrong symbols!"s);
            } else if (!word.empty()){
                non_empty_strings.insert(word);
            }
        }
        return non_empty_strings;
    }

private:
    bool IsValidWord(const std::string& word) const;

    bool IsStopWord(const std::string& word) const;

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    QueryWord ParseQueryWord(std::string text) const;

    Query ParseQuery(const std::string& text) const;

    double ComputeWordInverseDocumentFrequency(const std::string& word) const;

    template <typename Predicate>
    std::vector<Document> FindAllDocuments(const Query& query, Predicate predicate) const {
        std::map<int, double> document_to_relevance;

        for (const std::string& word : query.plus_words) {
            if (!word_to_document_frequency_.count(word)) {
                continue;
            }
            const double inverse_document_frequency = ComputeWordInverseDocumentFrequency(word);

            for (const auto [document_id, term_frequency] : word_to_document_frequency_.at(word)) {
                if (predicate(document_id, documents_.at(document_id).status, documents_.at(document_id).rating)) {
                    document_to_relevance[document_id] += term_frequency * inverse_document_frequency;
                }
            }
        }

        for (const std::string& word : query.minus_words) {
            if (!word_to_document_frequency_.count(word)) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_frequency_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({
                document_id,
                relevance,
                documents_.at(document_id).rating
            });
        }
        return matched_documents;
    }

private:
    std::set<std::string> stop_words_;
    std::map<int, std::map<std::string, double>> document_to_word_frequency_;
    std::map<std::string, std::map<int, double>> word_to_document_frequency_;

    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;
};
