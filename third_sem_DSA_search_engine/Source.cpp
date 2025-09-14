#include "crow.h"
#include "crow/middlewares/cors.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <functional>
#include "loading.h"
#include "D:\\3rd Sem\\DSA\\Project\\to_send\\tf_idf2.h"

// Helper function to make error responses
crow::response make_error_response(int code, const std::string& message) {
    crow::response res(code, message);
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
    return res;
}

int main() {

    crow::App<crow::CORSHandler> app;
    const std::string file_path = "D:\\3rd Sem\\DSA\\Project\\to_send\\final_output.json";
    load_all_json_datas();

    // Route to handle POST request
    CROW_ROUTE(app, "/get_json")
        .methods("POST"_method)
        ([file_path](const crow::request& req) {
        try {
            std::cout << "Hello\n";

            // Parse the request body to extract the search text
            auto parsed_body = nlohmann::json::parse(req.body);
            std::string searchText = parsed_body["searchText"];
            std::cout << "Request Body: " << req.body << "\n";
            std::cout << "Extracted searchText: " << searchText << "\n";

            // Process the search text (TF-IDF can be applied here)
            TF_IDF(searchText);

            // Check if the file exists
            if (!std::filesystem::exists(file_path)) {
                return make_error_response(500, "File does not exist");
            }

            // Open the file and read its contents
            std::ifstream file(file_path);
            if (!file.is_open()) {
                return make_error_response(500, "Could not open file");
            }

            std::stringstream buffer_stream;
            buffer_stream << file.rdbuf();
            file.close();

            // Parse the file contents as JSON
            nlohmann::json file_json = nlohmann::json::parse(buffer_stream.str());

            // Create the response JSON object
            nlohmann::json response;
            response["received_data"] = parsed_body;
            response["file_data"] = file_json;  // Send the content of final_output.json

            // Create the HTTP response with the JSON data
            crow::response res(200, response.dump());
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }
        catch (const nlohmann::json::exception& e) {
            std::cerr << "JSON Error: " << e.what() << "\n";
            return make_error_response(400, "Invalid JSON format");
        }
            });

    std::cout << "Server is running on http://localhost:18080\n";
    app.port(18080).multithreaded().run();
    return 0;
}
