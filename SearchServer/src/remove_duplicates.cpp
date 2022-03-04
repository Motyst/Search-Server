#include "../src/remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server){
    using namespace std::string_literals;

    std::set<int> documents_to_remove;
    std::map<std::set<std::string>, int> existing_word_sets;

    for(int document_id : search_server){
        const auto words_in_document = search_server.GetWordFrequencies(document_id);
        std::set<std::string> temporary;

        for(auto [word, _] : words_in_document){
            temporary.insert(word);
        }

        auto [_, emplaced] = existing_word_sets.try_emplace(temporary, document_id);
        if(!emplaced){
            documents_to_remove.insert(document_id);
        }
    }

    for(int document_id : documents_to_remove){
        std::cout << "Found duplicate document id " << document_id << std::endl;
        search_server.RemoveDocument(document_id);
    }
}
