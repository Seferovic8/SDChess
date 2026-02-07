#pragma once
#include <iostream>
#include <string>
#include "Board/Board.h"
#include "Position/Position.h"
#include "json.hpp"

#include "httplib.h" // Make sure this file is in your project directory
 using json = nlohmann::json;

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

        // basic extraction logic
        try {

        json j = json::parse(body);
        valFrom = j["fen"];
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