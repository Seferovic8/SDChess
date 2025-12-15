#pragma once
#include <iostream>
#include <string>
#include "Board/Board.h"
#include "Position/Position.h"

#include "httplib.h" // Make sure this file is in your project directory

int server() {
    int i = 0;
    // create server on port 8000
    httplib::Server svr;

    std::cout << "Server listening on http://127.0.0.1:8000..." << std::endl;

    // Define the POST endpoint
    svr.Post("/calculate", [](const httplib::Request& req, httplib::Response& res) {

        // 1. Simple manual parsing of the body (e.g., { "from": 10, "to": 20 })
        // In a real app, use nlohmann/json, but here we do it manually to match your style
        std::string body = req.body;

        std::string valFrom = "";
        int valTo = 0;

        // basic extraction logic
        try {
            size_t posFen = body.find("\"fen\":");

            if (posFen != std::string::npos) {
                // 2. Find the FIRST quote after the key (skipping "fen": itself)
                size_t startQuote = body.find("\"", posFen + 6);

                // 3. Find the SECOND quote after the first one
                size_t endQuote = body.find("\"", startQuote + 1);

                // 4. Extract only what is between the quotes
                if (startQuote != std::string::npos && endQuote != std::string::npos) {
                    valFrom = body.substr(startQuote + 1, endQuote - startQuote - 1);
                }
            }
        }
        catch (...) {
            std::cout << "Error parsing JSON" << std::endl;
        }

        std::cout <<  valFrom << std::endl;

        chess::Board board = chess::Board(valFrom);
        std::cout << board.evaluate() << std::endl;
 
        auto mv = board.findBestMove(5);
        std::cout << mv.getMoveText() << std::endl;
        std::cout << "end" << std::endl;

        int fromMv = chess::Position::positionToNum(mv.getFromPos());
        int toMv = chess::Position::positionToNum(mv.getToPos());
    //    fromMv = chess::Position::flipPosition(fromMv);
     //   toMv = chess::Position::flipPosition(toMv);
        // 3. Construct JSON response
        // MUST match the format your client expects exactly
        std::string json_response = "{ \"from\": " + std::to_string(fromMv) + ", " +
            "\"to\": " + std::to_string(toMv) + " }";

        // 4. Send back
        res.set_content(json_response, "application/json");
        });

    svr.listen("127.0.0.1", 8000);
    return 0;
}