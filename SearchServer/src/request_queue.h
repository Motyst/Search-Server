#pragma once

#include <stack>

#include "../src/document.h"
#include "../src/search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) : search_server_(search_server)
    {
    }

public:
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        auto found_documents = search_server_.FindTopDocuments(raw_query, document_predicate);
        RequestUpdate(raw_query, found_documents);
        return found_documents;
    }

public:
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        std::string raw_query;
        size_t document_count = 0;
    };

private:
    static const int kMinutesInDay = 1440;

private:
    void RequestUpdate(const std::string& raw_query, const std::vector<Document>& found_documents);

    void AddRequest(const std::string& raw_query, const std::vector<Document>& found_documents);

    void RemoveRequest();

private:
    std::deque<QueryResult> requests_;
    int empty_results_counter_ = 0;
    const SearchServer& search_server_;
};
