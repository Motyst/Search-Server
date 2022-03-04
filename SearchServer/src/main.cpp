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

int main() {
    SearchServer search_server("and with"s);

    search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::kActual, {7, 2, 7});

    search_server.AddDocument(2, "funny pet with curly hair"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(3, "funny pet with curly hair"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(4, "funny pet and curly hair"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(5, "funny funny pet and nasty nasty rat"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(6, "funny pet and not very nasty rat"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(7, "very nasty rat and not very funny pet"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(8, "pet with rat and rat and rat"s, DocumentStatus::kActual, {1, 2});

    search_server.AddDocument(9, "nasty rat with curly hair"s, DocumentStatus::kActual, {1, 2});

    std::cout << "Before duplicates removed: "s << search_server.GetDocumentCount() << std::endl;
    RemoveDuplicates(search_server);
    std::cout << "After duplicates removed: "s << search_server.GetDocumentCount() << std::endl;

    return 0;
}
