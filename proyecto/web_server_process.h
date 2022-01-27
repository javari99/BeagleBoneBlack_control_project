#ifndef WEBSERVERPROCESS_H
#define WEBSERVERPROCESS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <atomic>
//#include <thread>
//#include <mutex>
//#include <queue>
//#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>

#include <stdexcept>
#include <future>
#include <map>

#include "../mylibs/libarm/mysocket.h"
#include "sync.h"

#define MAX_BUFFER_LENGTH 1024

namespace WebServer{
    const char header[] = "HTTP/1.0 200 OK\r\nPBM Server\r\n\r\n";

    std::map<std::string, std::string> filesMap;

    //This function finds the desired substring, partitions the string in two, inserts the filling
    //and builds the new string.
    std::string replaceTemplate(std::string text, std::string templ, std::string filling){
        std::string start, end;
        start = text.substr(0, text.find(templ));
        text.erase(0,text.find(templ)+templ.length());
        end = text;
        return start + filling + end;
    }
    //Function that iterates over a replacements map, will replace each key with the replacement string if found
    //on the string
    std::string renderFullTemplate(std::string text, std::map<std::string, std::string> replacementMap){
        for(auto it = replacementMap.begin(); it!=replacementMap.end(); it++){
            if(text.find(it->first)){
               text = replaceTemplate(text, it->first, it->second);
            }
        }
        return text;
    }

    std::string readFileToString(std::string path){
        std::ifstream indexfile(path);
        std::stringstream strbuffer;
        strbuffer << indexfile.rdbuf();
        return strbuffer.str();
    }

    char* GetMsg(char* token, int n){
        for (int i=0;i<n;i++){
            if ( token[i]=='H' && token[i+1]=='T'){ //busca HTTP
                token[i-1]=0; break;
            }
        }  
        //ahora busca el "/"  GET /Led=1 sin nada GET / HTTP....
        for (int i=0;i<strlen(token);i++){
            if ( token[i] == '/' ) return &token[i+1];  //salta el /
        }
        return token;
    }

    void StartServer(int portno){
        //Build the paths map
        //Map will be <route, file>
        filesMap[""] = readFileToString("./views/index.html");
        filesMap["style.css"] = readFileToString("./views/style.css");
        //filesMap["logo.png"] = readFileToString("./views/logo.png");


        mysocket server(portno);

        if ( server.Open() == 0){  
            std::cout << "ERROR opening socket\n"; 
            throw std::runtime_error("ERROR opening socket \n");
        }

        try{
            std::cout << "Webserver thread running..." << std::endl;
            while(shouldRun){
                mysocket newClient;
                server.Accept(newClient);
                std::cout << "New client accepted!" << std::endl;
                std::future<void> result(std::async(std::launch::async, [](mysocket client){
                    std::cout << "Handling client..." << std::endl;
                    char buffer[MAX_BUFFER_LENGTH];
                    int n = client.Read(buffer, MAX_BUFFER_LENGTH);
                    if (n > 0){
                        std::string msg = GetMsg(buffer, n);
                        cout << "Msg: " << msg << endl;

                        std::string latestCommand = "";

                        std::string response = filesMap.at("");
                        auto search = filesMap.find(msg);
                        if(search != filesMap.end() && msg !=""){
                            response = filesMap.at(msg);
                            int nh = client.Send(header);
                            client.Send(response.c_str());
                            client.Close();
                            return;
                        }else if(powerMode != EMERGENCIA){ //While on emergency mode, no changes shall be made
                            if(msg == "manual"){
                                powerMode = POTENCIOMETRO;
                                latestCommand = "Potentiometer mode";
                            }else if(msg == "web"){
                                powerMode = SERVER;
                                latestCommand = "Server mode";
                            }else if(msg == "power_0" && powerMode == SERVER){
                                powerLevel = POWER_NULL;
                                latestCommand = "Power set to 0";
                            }else if(msg == "power_1" && powerMode == SERVER){
                                powerLevel = LEVEL1;
                                latestCommand = "Power set to 1";
                            }else if(msg == "power_2" && powerMode == SERVER){
                                powerLevel = LEVEL2;
                                latestCommand = "Power set to 2";
                            }else if(msg == "power_3" && powerMode == SERVER){
                                powerLevel = LEVEL3;
                                latestCommand = "Power set to 3";
                            }
                        }
                        
                        std::map<std::string, std::string> replacementMap;
                        replacementMap["{{x_val}}"] = std::to_string(g_x_val);
                        replacementMap["{{y_val}}"] = std::to_string(g_y_val);
                        replacementMap["{{z_val}}"] = std::to_string(g_z_val);
                        replacementMap["{{light_val}}"] = std::to_string(g_light_val);
                        replacementMap["{{temp_val}}"] = std::to_string(g_temp_val);
                        replacementMap["{{powerLevel}}"] = std::to_string(powerLevel);
                        replacementMap["{{x_avg}}"] = std::to_string(g_x_mean);
                        replacementMap["{{x_var}}"] = std::to_string(g_x_variance);
                        replacementMap["{{y_avg}}"] = std::to_string(g_y_mean);
                        replacementMap["{{y_var}}"] = std::to_string(g_y_variance);
                        replacementMap["{{z_avg}}"] = std::to_string(g_z_mean);
                        replacementMap["{{z_var}}"] = std::to_string(g_z_variance);
                        replacementMap["{{temp_avg}}"] = std::to_string(g_temp_mean);
                        replacementMap["{{temp_var}}"] = std::to_string(g_temp_variance);
                        replacementMap["{{light_avg}}"] = std::to_string(g_light_mean);
                        replacementMap["{{light_var}}"] = std::to_string(g_light_variance);

                        response = renderFullTemplate(response, replacementMap);

                        int nh = client.Send(header);
                        client.Send(response.c_str());
                        client.Close();
                    }
                }, newClient));
                result.get();
            }
            server.Close();

        }catch(std::exception& e){
            std::cout << e.what() << "\n";
            server.Close();
        }
    }

}

#endif
