#include "../src/request_queue.h"

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus document_status) {
    return RequestQueue::AddFindRequest(raw_query,
            [document_status](int document_id, DocumentStatus status, int rating){
                return status == document_status;
            });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    return RequestQueue::AddFindRequest(raw_query, DocumentStatus::kActual);
}

int RequestQueue::GetNoResultRequests() const {
    return empty_results_counter_;
}

void RequestQueue::RequestUpdate(const std::string& raw_query, const std::vector<Document>& found_documents){
    if(requests_.size() >= kMinutesInDay){
        RemoveRequest();
    }
    AddRequest(raw_query, found_documents);
}

void RequestQueue::AddRequest(const std::string& raw_query, const std::vector<Document>& found_documents){
    QueryResult result = {raw_query, found_documents.size()};
    if(!result.document_count){
        ++empty_results_counter_;
    }
    requests_.push_back(result);
}

void RequestQueue::RemoveRequest(){
    if(!requests_.front().document_count){
        --empty_results_counter_;
    }
    requests_.pop_front();
}
