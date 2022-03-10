#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../src/document.h"
#include "../src/paginator.h"
#include "../src/read_input_functions.h"
#include "../src/remove_duplicates.h"
#include "../src/request_queue.h"
#include "../src/search_server.h"

using namespace std::string_literals;
using namespace read_input;

void BasicFunctionality(){
    // Creating server with "minus words"
    SearchServer search_server("and in on"s);

    // Adding documents
    AddDocument(search_server, 1, "furry cat furry tail"s, DocumentStatus::kActual, {7, 2, 7});
    AddDocument(search_server, 2, "furry dog and cool belt"s, DocumentStatus::kActual, {1, 2});
    AddDocument(search_server, 3, "big dog bird evgeny"s, DocumentStatus::kActual, {1, 3, 2});
    AddDocument(search_server, 4, "big dog bird dog is not real"s, DocumentStatus::kActual, {1, 1, 1});
    AddDocument(search_server, 5, "cartoon dogs wasn't there"s, DocumentStatus::kActual, {1, 2});

    // Looking for documents that contain requested words
    FindTopDocuments(search_server, "furry dog"s);
}

int main() {
    BasicFunctionality();

    return 0;
}
