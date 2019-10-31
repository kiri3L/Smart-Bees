#include "HTTPHandler.h"
#include "HTTPServer.h"
#include "HTTPClient.h"
#include "Logger.h"
#include "nlohmann/json.hpp"

#include <iostream>

#define SUB_SERVICE_HOST "localhost"
#define ID_SERVICE_HOST "localhost"
#define AC_SERVICE_HOST "localhost"

#define SUB_SERVICE_PORT 3000
#define ID_SERVICE_PORT 2000
#define AC_SERVICE_PORT 2525

using namespace std;
using json = nlohmann::json;

void set_bad_request(HTTPHandler::Answer& answer) {
	answer.status_code = 403;
	answer.status_description = "Bad Request";
}

void set_ok(HTTPHandler::Answer& answer) {
	answer.status_code = 200;
	answer.status_description = "OK";
}

HTTPHandler::Request build_request(HTTPHandler::Method method, string uri, string body="") {
	HTTPHandler::Request request;
	request.method = method;
	request.uri = uri;
	request.body = body;
}

void check_sub(json req_body) {
	HTTPClient client;
	HTTPHandler::Answer answer;
	json ans_body;
	string user_id = req_body["user_id"];
	client.connect_to_server(SUB_SERVICE_HOST, SUB_SERVICE_PORT);
	if(client.is_connected()) {
		client.send_request(build_request(HTTPHandler::Method::GET, "/sub_status?user_id=" + user_id));
		answer = client.read_answer();
		ans_body = json::parse(answer.body);
		if(ans_body["sub_status"] == nullptr)
			throw runtime_error(ans_body["exception"]);
		else if(ans_body["sub_status"] == false)
			throw runtime_error("Subscribe is invalid");
	}
	else
		throw runtime_error("Subscribe sevice is not responding");
}

string get_rasp_id(json req_body) {
	HTTPClient client;
	HTTPHandler::Answer answer;
	json ans_body;
	string user_id = req_body["user_id"];
	client.connect_to_server(ID_SERVICE_HOST, ID_SERVICE_PORT);
	if(client.is_connected()) {
		client.send_request(build_request(HTTPHandler::Method::GET, "/get_gate?user_id=" + user_id));
		answer = client.read_answer();
		ans_body = json::parse(answer.body);
		if(ans_body["gate_id"] == nullptr)
			throw runtime_error(ans_body["exception"]);
		else
			return ans_body["gate_id"];
	}
	else
		throw runtime_error("ID sevice is not responding");
}

void ac_send(json req_body) {
	HTTPClient client;
	HTTPHandler::Answer answer;
	json ans_body;
	client.connect_to_server(AC_SERVICE_HOST, AC_SERVICE_PORT);
	if(client.is_connected()) {
		client.send_request(build_request(HTTPHandler::Method::POST, "/", req_body.dump(4)));
		answer = client.read_answer();
		ans_body = json::parse(answer.body);
		if(ans_body["status"] == "fail")
			throw runtime_error(ans_body["exception"]);
	}
	else
		throw runtime_error("AC sevice is not responding");

}

HTTPHandler::Answer check_all(HTTPHandler::Request request) {
	HTTPHandler::Answer answer;
	json answer_json;
	string rasp_id;
	
	try {
		json req_body = json::parse(request.body);
		check_sub(req_body);
		rasp_id = get_rasp_id(req_body);
		req_body["rasp_id"] = rasp_id;
		ac_send(req_body);
		answer_json["status"] = "ok";
    } catch (exception& e) {
		set_bad_request(answer);
		answer_json["status"] = "fail";
	   	answer_json["exception"] = e.what();
    }
	
	answer.body = answer_json.dump(4);
	return answer;
}


int main() {
	const int port = 2000;

	try {
		logger << "Starting manager service..." << endl;
		HTTPServer server;
		logger << "Starting server..." << endl;
		server.start_server(port);
		server.turn_to_listen(10);
		while(true) {
			logger << "Awaiting for connection..." << endl;
			int client_id = server.connect_client();
			logger << "Client connected" << endl;
			logger << "Getting request..." << endl;
			HTTPHandler::Request request = server.get_request(client_id);
			logger << "Checking..." << endl;
			HTTPHandler::Answer answer = check_all(request);
			logger << "Sending answer" << endl;
			server.send_answer(client_id, answer);
			server.close_con(client_id);
			logger << "Connection closed" << endl;

		}
    } catch (exception& e) {
        logger << "[EXCEPTION] " << e.what() << endl;
    }

}
